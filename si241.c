#include "main.h"
#include "si241.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/include/port.h"
#include "mcc_generated_files/include/spi0.h"

extern uint8_t SPI_Bout[16];
extern uint8_t SPI_Bin[16];
extern uint8_t RX_Payload[16];
extern uint8_t TX_Payload[16];
extern uint8_t RX_PL_Len;
extern uint8_t TX_PL_Len;
extern uint8_t TX_Address[6];
extern uint8_t RX_Address[6];
extern uint8_t Device_ID[5];
extern uint8_t tx_pipe;
extern uint16_t si24_on_timer;
extern uint8_t go_tx;

extern volatile TmrDelay TimerD;
extern volatile KEYstateControl Key;
extern volatile uint8_t Si24_Status;

const unsigned char Broadcast_ID[] = DEFAULT_FOB_ID;
uint8_t tx_payload_size;

void SI241_PwrOn(void)
{
    uint8_t work;
    if(!IO_PA5_PWR_GetValue())
    {
        PORTA.PIN4CTRL = 0x00;
        IO_PA5_PWR_SetHigh();
        PORTC_set_pin_level(1, true);      // TP3

        IO_PA6_CSN_SetHigh();
        IO_PA7_CE_SetLow();
        TimerD._delaycnt = 0;
        TimerD._delaytarget = 0x0a;     // Set to 100mSec delay
        TimerD._finished = 0;
        TimerD._active = 1;
        while (!TimerD._finished);
        PORTA.DIR = 0xEA;               // Set MISO as INPUT
        PORTMUX.SPIROUTEA = 0x00;       // Map SPI standard mappings to PORTA functions 
        SPI0_Initialize();              // Enable SPI after power up        
        work = SPI0.INTFLAGS;
        if(work && 0x80)
        {
            work = SPI0.DATA;
        }
        SPI0_Enable();
        SPI0.INTCTRL = 0x01;            // enable SPI Interrupts 
        PORTA.PIN4CTRL = 0x02;          // enable IRQ interrupts
    }
    si24_on_timer = 500;            // set to 500 * 0.01 = 5 Seconds
}

void SI241_PwrOff(void)
{
    if(IO_PA5_PWR_GetValue())
    {
        PORTA.DIR = 0xEE;               // Set MISO as OUTPUT
        PORTMUX.SPIROUTEA = 0x03;       // Un-map SPI standard mappings to PORTA functions 
        PORTA.OUT = 0x00;               // All lines Low
        SPI0_Disable();                 // Disable SPI after power dn       
    }
    IO_PA5_PWR_SetLow(); 
    IO_PA6_CSN_SetLow();
    IO_PA7_CE_SetLow();
}

void SI241_SetupTx(void)
{
    SI241_PwrOn();
    TX_Payload[0] = W_TX_PAYLOAD_NOACK;
    if(tx_pipe == 0)
    {
        TX_Payload[1] = 0xC0;
        TX_Payload[2] = 0x3F;         
        TX_Payload[3] = Device_ID[0];
        TX_Payload[4] = Device_ID[1];
        TX_Payload[5] = Device_ID[2];
        TX_Payload[6] = Device_ID[3];
        TX_Payload[7] = Device_ID[4];
        tx_payload_size = 7;
    }
    else
    {
        if(go_tx & 0x02)
        {
            TX_Payload[1] = ~Key._cmd;
            TX_Payload[2] = Key._cmd;                        
        }
        else
        {
            TX_Payload[1] = Key._cmd;
            TX_Payload[2] = ~Key._cmd;            
        }
        tx_payload_size = 2;
    }
    
    SI241_LoadTxAddress();
    IO_PA6_CSN_SetLow();    
    SPI0_WriteBlock(&TX_Address, 6);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();    

    SI241_LoadRxAddress();
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&RX_Address, 6);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();    

    SPI_Bout[0] = FEATURE;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0x01;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();        

    IO_PA6_CSN_SetLow();
    tx_payload_size++;
    SPI0_WriteBlock(&TX_Payload, tx_payload_size);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();        

    SPI_Bout[0] = SETUP_AW;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0x03;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();        

    SPI_Bout[0] = RF_CH;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;    
    SPI_Bout[1] = 0x40;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();        

    SPI_Bout[0] = RF_SETUP;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;    
    SPI_Bout[1] = 0x0e;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();        

    SPI_Bout[0] = CONFIG;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;    
    SPI_Bout[1] = 0x0E;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh(); 
    
    IO_PA6_CSN_SetLow();
    SPI0_ReadBlockCmd(&SPI_Bin, 6, 0x10);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();
    asm ("nop");
    asm ("nop");
    asm ("nop");       
}


void SI241_SetTx(void)
{
    Si24_Status = 0x80;
    IO_PA7_CE_SetHigh();
}

void SI241_SetStandby(void)
{
    Si24_Status = 0x40;
    IO_PA7_CE_SetLow();
    SPI_Bout[0] = STATUS;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0b01110000;       // clear interrupt flags
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();            
    
}
uint8_t SI241_Status(void)
{
    IO_PA6_CSN_SetLow();
    SPI0_ReadBlockCmd(&SPI_Bin, 2, 0x00);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();
    return SPI_Bin[0];
}

void SI241_LoadTxAddress(void)
{
    TX_Address[0] = TX_ADDR;
    TX_Address[0] = TX_Address[0] | W_REGISTER;
    if(tx_pipe == 0)
    {
        TX_Address[1] = Broadcast_ID[0];
        TX_Address[2] = Broadcast_ID[1];
        TX_Address[3] = Broadcast_ID[2];
        TX_Address[4] = Broadcast_ID[3];
        TX_Address[5] = Broadcast_ID[4];
        
    }
    else
    {
//        TX_Address[1] = Device_ID[0];
//        TX_Address[2] = Device_ID[1];
//        TX_Address[3] = Device_ID[2];
//        TX_Address[4] = Device_ID[3];
//        TX_Address[5] = Device_ID[4];
        TX_Address[1] = 0x01;
        TX_Address[2] = 0x00;
        TX_Address[3] = 0x00;
        TX_Address[4] = 0x00;
        TX_Address[5] = 0x01;
    }    
}

void SI241_LoadRxAddress(void)
{
    RX_Address[0] = RX_ADDR_P0;    
    RX_Address[0] = RX_Address[0] | W_REGISTER;
    RX_Address[1] = 0x00;
    RX_Address[2] = 0x01;
    RX_Address[3] = 0x01;
    RX_Address[4] = 0x01;
    RX_Address[5] = 0x00;    
}
