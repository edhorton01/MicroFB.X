/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "main.h"
#include "key_handler.h"
#include "si241.h"

uint16_t timer0_val;
uint16_t si24_on_timer;

volatile KEYstateBITS KeyStatus;
volatile KEYstateControl Key;
volatile INTstateBITS IntStatus;
volatile TmrDelay TimerD;
volatile uint8_t Si24_Status;
volatile uint8_t PORTA_img;

uint8_t work1;
uint8_t go_tx;
uint8_t tx_pipe;
uint8_t SPI_Bout[16];
uint8_t SPI_Bin[16];
uint8_t RX_Payload[16];
uint8_t TX_Payload[16];
uint8_t RX_PL_Len;
uint8_t TX_PL_Len;
uint8_t TX_Address[6];
uint8_t RX_Address[6];
uint8_t e2_buf[16];

uint8_t Device_ID[5];
uint8_t active_channel;
uint8_t active_device;
uint8_t get_resp;
uint8_t stat;

uint8_t rx_bc;
uint8_t rssi;

micro_id remotes[2];
ButtonState function;
Multiple dev_ctl;


/*
    Main application
*/
int main(void)
{
    /* Initializes MCU, drivers and middleware */
    SYSTEM_Initialize();
    TimerD._flags = 0;
    go_tx = 0;
    tx_pipe = 1;
    active_channel = 0x40;
    active_device = 0;
    function._flags = 0;
    
    Device_ID[0] = (SIGROW.SERNUM0 & 0x7f);
    Device_ID[1] = SIGROW.SERNUM1;
    Device_ID[2] = SIGROW.SERNUM2;
    Device_ID[3] = SIGROW.SERNUM3;
    Device_ID[4] = SIGROW.SERNUM4;
    
    TCA0_ClearOverflowInterruptFlag();
    TCA0_EnableInterrupt();
    
    SPI0_Disable();                     // Disable SPI at startup
    SPI0.INTCTRL = 0x00;                // disable SPI Interrupts
    IO_PA5_PWR_SetLow();                // Make sure voltage regulator to RF chip is off
    PORTC_set_pin_level(0, false);      // TP4
    PORTA.DIR = 0xEE;                   // Set MISO as output when SPI0 is disabled
    PORTMUX.SPIROUTEA = 0x03;           // Un-map SPI standard mappings to PORTA functions 
    
    KeyStatus._flags = 0;
    KeyStatus._scan_end = 1;
    Key._bounce = 0x3f;
    Key._cmd = Key._bounce;
    IntStatus._flags = 0;
    PORTC_set_pin_level(1, false);      // TP3
    get_resp = 0;
    
    remotes[0] = SI241_ReadRxAddress(0);    
    remotes[1] = SI241_ReadRxAddress(0x20);    
    sei();
    
    tx_pipe = 0;
    SI241_SetupTx();
    tx_pipe = 1;
    
    asm ("nop");
    asm ("nop");
    asm ("nop");   
    SI241_SetTx();
            
    while (1)
    {
        asm ("nop");
        asm ("nop");
        asm ("nop");
        if(IntStatus._tc0)
        {
            IntStatus._tc0 = 0;
            if(Si24_Status == 0x80)
            {
                PORTA_img = PORTA.IN;
                if(!(PORTA_img & 0x10))
                {
                    stat = SI241_Status();
                    asm ("nop");
                    asm ("nop");
                    asm ("nop");
                    SI241_SetStandby();
                    if(get_resp != 0)
                    {
                        asm ("nop");
                        asm ("nop");
                        asm ("nop");
                        get_resp = 0;
                        si24_on_timer = 500;            // set to 500 * 0.01 = 5 Seconds
                        SI241_SetupRxResp();
                        SI241_SetRxResp();
                    }
                    else if(dev_ctl._both_devices)
                    {
                        if(!active_device)
                        {
                            active_device = 0x80;
                            dev_ctl._both_devices = 0;
                            dev_ctl._both_devices_go = 1;
                        }
                    }
                }
            }
            
            else if(Si24_Status == 0x40)
            {
                PORTA_img = PORTA.IN;
                if(!(PORTA_img & 0x10))
                {
                    asm ("nop");
                    asm ("nop");
                    asm ("nop");
                    rx_bc = SI241_RX0_BC();
                    asm ("nop");
                    asm ("nop");
                    asm ("nop");
                    SI241_RX0_Payload(rx_bc);
                    rssi = SI241_RSSI();
                    asm ("nop");
                    asm ("nop");
                    asm ("nop");                    
                    SI241_RX0_ClearInt();                                        
                    SI241_SetStandby();
                    asm ("nop");
                    asm ("nop");
                    asm ("nop");                    
                    SI241_SaveRxAddress();
                    asm ("nop");
                    asm ("nop");
                    asm ("nop");                                
                    SI241_ReadRxAddress(0);
                    asm ("nop");
                    asm ("nop");
                    asm ("nop");                                
                    
                }
                else
                {
                    if(si24_on_timer != 0)
                    {
                        si24_on_timer--;
                        if(si24_on_timer == 0)
                        {
                            Si24_Status = 0;
                            SI241_PwrOff();
                        }
                    }                        

                }
            }   

            else if(Si24_Status == 0x20)
            {
                work1 = SI241_Status();
                if(work1 & 0x20)
                {
                    asm ("nop");
                    asm ("nop");
                    asm ("nop");                
                }                    
                if(si24_on_timer != 0)
                {
                    si24_on_timer--;
                    if(si24_on_timer == 0)
                    {
                        Si24_Status = 0;
                        SI241_PwrOff();
                    }
                }
            }
            if(!KeyStatus._scan_end)
            {
                KeyStatus._interrupt = 0;
                ServiceKeyPress();
            }
            else if(KeyStatus._pressed)
            {
                ServiceKeyPress();
            }
        }
        if(KeyStatus._new_cmd || dev_ctl._both_devices_go || (KeyStatus._hold_req && !KeyStatus._hold_ack))
        {
            ServiceCmd();
            if(go_tx || dev_ctl._both_devices_go)
            {
                dev_ctl._both_devices_go = 0;
                SI241_SetupTx();
                go_tx = 0;
                asm ("nop");
                asm ("nop");
                asm ("nop");   
                SI241_SetTx();
            }            
        }
//        timer0_val = TCA0_ReadTimer();
    }
}
/**
    End of File
*/