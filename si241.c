#include "main.h"
#include "si241.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/include/port.h"
#include "mcc_generated_files/include/spi0.h"
#include "mcc_generated_files/include/nvmctrl.h"

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
extern uint8_t active_channel;
extern uint8_t rx_bc;
extern uint8_t e2_buf[6];
extern micro_id remotes[2];
extern uint8_t active_device;
extern ButtonState function;
extern Multiple dev_ctl;

extern volatile TmrDelay TimerD;
extern volatile KEYstateControl Key;
extern volatile uint8_t Si24_Status;

const unsigned char Broadcast_ID[] = DEFAULT_FOB_ID;
uint8_t tx_payload_size;
uint8_t ee_data, ee_data_n;
eeprom_adr_t ee_addr, ee_addr_n, ee_addr_e;
nvmctrl_status_t eerom_ret;

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
    uint8_t work;
    SI241_PwrOn();
    if(tx_pipe == 0)
    {
        TX_Payload[0] = W_TX_PAYLOAD_NOACK;
        TX_Payload[1] = ~Key._cmd;
        TX_Payload[2] = Key._cmd;                        
        TX_Payload[3] = Device_ID[0];
        TX_Payload[4] = Device_ID[1];
        TX_Payload[5] = Device_ID[2];
        TX_Payload[6] = Device_ID[3];
        TX_Payload[7] = Device_ID[4];
        tx_payload_size = 7;
    }
    else
    {
        TX_Payload[0] = W_TX_PAYLOAD;
//        TX_Payload[0] = W_TX_PAYLOAD_NOACK;
        
        work = Key._cmd | (function._flags & 0x80);
        if(dev_ctl._invert)
        {
            TX_Payload[1] = ~work;
            TX_Payload[2] = work;                        
        }
        else
        {
            TX_Payload[1] = work;
            TX_Payload[2] = ~work;            
        }
        tx_payload_size = 2;
    }
    
    SI241_LoadTxAddress();
    IO_PA6_CSN_SetLow();    
    SPI0_WriteBlock(&TX_Address, 6);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();    

    SI241_LoadRxAddress(0);
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
    if(tx_pipe == 0)
    {
        SPI_Bout[1] = 0x40;
    }
    else
    {
        SPI_Bout[1] = active_channel;
    }
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

    if(tx_pipe != 0)
    {
        SPI_Bout[0] = EN_RXADDR;
        SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;    
        SPI_Bout[1] = 0x01;
        IO_PA6_CSN_SetLow();        
        SPI0_WriteBlock(&SPI_Bout, 2);
        while(!SPI0_StatusDone());
        IO_PA6_CSN_SetHigh();                
        SPI_Bout[0] = SETUP_RETR;
        SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;    
        SPI_Bout[1] = 0xf1;
        IO_PA6_CSN_SetLow();        
        SPI0_WriteBlock(&SPI_Bout, 2);
        while(!SPI0_StatusDone());
        IO_PA6_CSN_SetHigh();                
    }
    
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

void SI241_SetupRxResp(void)
{
    SPI_Bout[0] = SETUP_AW;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0x03;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();        
    
    SPI_Bout[0] = EN_RXADDR;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0x01;
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

    IO_PA6_CSN_SetLow();        
    SPI_Bout[0] = RX_PW_P0;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;    
    SPI_Bout[1] = 7;
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
    SPI_Bout[1] = 0x0F;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh(); 
}

void SI241_SetRxResp(void)
{
    Si24_Status = 0x40;
    IO_PA7_CE_SetHigh();
}

void SI241_SetStandby(void)
{
    Si24_Status = 0x20;
    IO_PA7_CE_SetLow();
    SPI_Bout[0] = STATUS;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0b01110000;       // clear interrupt flags
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh(); 
    
    IO_PA7_CE_SetLow();
    SPI_Bout[0] = FLUSH_TX;
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 1);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();                    
}

uint8_t SI241_Status(void)
{
    IO_PA6_CSN_SetLow();
    SPI0_ReadBlockCmd(&SPI_Bin, 1, 0xff);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();
    return SPI_Bin[0];
}

uint8_t SI241_RX0_BC(void)
{
    IO_PA6_CSN_SetLow();
    SPI0_ReadBlockCmd(&SPI_Bin, 2, 0x11);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();
    return SPI_Bin[1];
}

uint8_t SI241_RSSI(void)
{
    IO_PA6_CSN_SetLow();
    SPI0_ReadBlockCmd(&SPI_Bin, 2, 0x09);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();
    return SPI_Bin[1];
}

void SI241_RX0_Payload(uint8_t rx_bc)
{
    if(rx_bc > Payload_len)
    {
        rx_bc = Payload_len;
    }
    rx_bc++;
    IO_PA6_CSN_SetLow();
    SPI0_ReadBlockCmd(&RX_Payload, rx_bc, 0x61);       // R_RX_Payload
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();
}

void SI241_RX0_ClearInt(void)
{
    SPI_Bout[0] = STATUS;
    SPI_Bout[0] = SPI_Bout[0] | W_REGISTER;
    SPI_Bout[1] = 0b01000000;       // clear RX interrupt flags
    IO_PA6_CSN_SetLow();        
    SPI0_WriteBlock(&SPI_Bout, 2);
    while(!SPI0_StatusDone());
    IO_PA6_CSN_SetHigh();
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
        TX_Address[1] = (Device_ID[0] | active_device);
        TX_Address[2] = Device_ID[1];
        TX_Address[3] = Device_ID[2];
        TX_Address[4] = Device_ID[3];
        TX_Address[5] = Device_ID[4];
//        TX_Address[1] = 0x01;
//        TX_Address[2] = 0x00;
//        TX_Address[3] = 0x00;
//        TX_Address[4] = 0x00;
//        TX_Address[5] = 0x01;
    }    
}

void SI241_LoadRxAddress(uint8_t chan)
{
    RX_Address[0] = RX_ADDR_P0;    
    RX_Address[0] = RX_Address[0] | W_REGISTER;
    if(tx_pipe == 0)
    {
        RX_Address[1] = 0x00;
        RX_Address[2] = 0x01;
        RX_Address[3] = 0x01;
        RX_Address[4] = 0x01;
        RX_Address[5] = 0x00;
    }
    else
    {
        RX_Address[1] = (Device_ID[0] | active_device);
        RX_Address[2] = Device_ID[1];
        RX_Address[3] = Device_ID[2];
        RX_Address[4] = Device_ID[3];
        RX_Address[5] = Device_ID[4];
        
//        RX_Address[1] = remotes[chan]._address[0];
//        RX_Address[2] = remotes[chan]._address[1];
//        RX_Address[3] = remotes[chan]._address[2];
//        RX_Address[4] = remotes[chan]._address[3];
//        RX_Address[5] = remotes[chan]._address[4];        
    }
}

void SI241_SaveRxAddress(void)
{
    uint8_t k;
    uint16_t il, ip;
    cli();    
    FLASH_Initialize();
    if(RX_Payload[1] == 0x11)
    {
        ee_addr = 0x0;
    }
    else
    {
        ee_addr = 0x20;
    }
    
    il = ee_addr + 8;
    ip = ee_addr + 5;
    k = 3;
    
    while (ee_addr < il)
    {
        ee_addr_n = ee_addr + 8;
        if (ee_addr < ip)
        {
            ee_data = RX_Payload[k++];
        }
        else if (ee_addr == ip)
        {
            ee_data = 0x40;
        }
        else
        {
            ee_data = 0xff;
        }
        ee_data_n = ~ee_data;

        while(!FLASH_IsEepromReady())
        {
            asm ("nop");
            asm ("nop");
            asm ("nop");       
        }
        eerom_ret = FLASH_WriteEepromByte(ee_addr, ee_data);
        ee_addr_e = ee_addr + 0x10;
        while(!FLASH_IsEepromReady())
        {
            asm ("nop");
            asm ("nop");
            asm ("nop");       
        }            
        eerom_ret = FLASH_WriteEepromByte(ee_addr_e, ee_data);

        while(!FLASH_IsEepromReady())
        {
            asm ("nop");
            asm ("nop");
            asm ("nop");       
        }
        eerom_ret = FLASH_WriteEepromByte(ee_addr_n, ee_data_n);
        ee_addr_e = ee_addr_n + 0x10;
        while(!FLASH_IsEepromReady())
        {
            asm ("nop");
            asm ("nop");
            asm ("nop");       
        }            
        eerom_ret = FLASH_WriteEepromByte(ee_addr_e, ee_data_n);
        ee_addr++;
    }
    sei();
}

micro_id SI241_ReadRxAddress(uint8_t offset)
{
    uint8_t k, c1, c2;
    uint16_t ip;
    micro_id remote;
    
        ee_addr = (uint16_t) offset;
        c1 = 0;
        c2 = 0;
        ip = ee_addr + 6; 
        k = 0;
        while (k < 5)
        {
            remote._address[k++] = 0;
        }
        k = 0;
        while (ee_addr < ip)
        {
            ee_addr_n = ee_addr + 8;
            ee_data = FLASH_ReadEepromByte(ee_addr);
            ee_data_n = FLASH_ReadEepromByte(ee_addr_n);
            ee_data_n = ~ee_data_n;
            if (ee_data == ee_data_n)
            {
                if (ee_addr != (ip - 1))
                {
                    remote._address[k++] = ee_data;
                }
                else
                {
                    remote._chan = ee_data;
                }
            }
            else
            {
                c1 = 1;
                break;
            }
            ee_addr++;
        }

        if (c1)
        {
            ee_addr = (uint16_t) offset + 16;
            ip = ee_addr + 6;
            k = 0;
            while (ee_addr < ip)
            {
                ee_addr_n = ee_addr + 8;
                ee_data = FLASH_ReadEepromByte(ee_addr);
                ee_data_n = FLASH_ReadEepromByte(ee_addr_n);
                ee_data_n = ~ee_data_n;
                if (ee_data == ee_data_n)
                {
                    if (ee_addr != (ip - 1))
                    {
                        remote._address[k++] = ee_data;
                    }
                    else
                    {
                        remote._chan = ee_data;
                    }
                }
                else
                {
                    c2 = 1;
                    break;
                }
                ee_addr++;
            }
        }
        if (c1 && c2)
        {
            remote._address[0] = 0x01;
            remote._address[1] = 0x00;
            remote._address[2] = 0x00;
            remote._address[3] = 0x00;
            remote._address[4] = 0x01;
            remote._chan = 0x40;
        }
        return remote;
    }
