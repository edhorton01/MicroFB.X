/**
  @Company
    Microchip Technology Inc.

  @Description
    This Source file provides APIs.
    Generation Information :
    Driver Version    :   1.0.0
*/
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


#include "../include/pin_manager.h"
#include "../../key_handler.h"

extern volatile uint8_t PORTA_img;

static void (*PORTB_IO_PB3_SW3_InterruptHandler)(void);
static void (*PORTA_PA2_InterruptHandler)(void);
static void (*PORTB_IO_PB2_SW2_InterruptHandler)(void);
static void (*PORTC_IO_PC3_TP3_InterruptHandler)(void);
static void (*PORTA_PA1_InterruptHandler)(void);
static void (*PORTB_IO_PB5_SW5_InterruptHandler)(void);
static void (*PORTA_IO_PA4_IRQ_InterruptHandler)(void);
static void (*PORTB_IO_PB4_SW4_InterruptHandler)(void);
static void (*PORTA_PA3_InterruptHandler)(void);
static void (*PORTA_IO_PA6_CSN_InterruptHandler)(void);
static void (*PORTA_IO_PA5_PWR_InterruptHandler)(void);
static void (*PORTA_IO_PA7_CE_InterruptHandler)(void);
static void (*PORTC_IO_PC0_TP0_InterruptHandler)(void);
static void (*PORTB_IO_PB1_SW1_InterruptHandler)(void);
static void (*PORTC_IO_PC2_TP2_InterruptHandler)(void);
static void (*PORTB_IO_PB0_InterruptHandler)(void);
static void (*PORTC_IO_PC1_TP1_InterruptHandler)(void);

void PORT_Initialize(void);

void PIN_MANAGER_Initialize()
{
    PORT_Initialize();

    /* DIR Registers Initialization */
    PORTA.DIR = 0xEA;
    PORTB.DIR = 0x00;
    PORTC.DIR = 0x0F;

    /* OUT Registers Initialization */
    PORTA.OUT = 0x00;
    PORTB.OUT = 0x00;
    PORTC.OUT = 0x00;

    /* PINxCTRL registers Initialization */
    PORTA.PIN0CTRL = 0x00;
    PORTA.PIN1CTRL = 0x00;
    PORTA.PIN2CTRL = 0x00;
    PORTA.PIN3CTRL = 0x00;
    PORTA.PIN4CTRL = 0x02;
    PORTA.PIN5CTRL = 0x00;
    PORTA.PIN6CTRL = 0x00;
    PORTA.PIN7CTRL = 0x00;
    PORTB.PIN0CTRL = 0x09;
    PORTB.PIN1CTRL = 0x09;
    PORTB.PIN2CTRL = 0x09;
    PORTB.PIN3CTRL = 0x09;
    PORTB.PIN4CTRL = 0x09;
    PORTB.PIN5CTRL = 0x09;
    PORTB.PIN6CTRL = 0x00;
    PORTB.PIN7CTRL = 0x00;
    PORTC.PIN0CTRL = 0x00;
    PORTC.PIN1CTRL = 0x00;
    PORTC.PIN2CTRL = 0x00;
    PORTC.PIN3CTRL = 0x00;
    PORTC.PIN4CTRL = 0x00;
    PORTC.PIN5CTRL = 0x00;
    PORTC.PIN6CTRL = 0x00;
    PORTC.PIN7CTRL = 0x00;

    /* PORTMUX Initialization */
    PORTMUX.CCLROUTEA = 0x08;
    PORTMUX.EVSYSROUTEA = 0x00;
    PORTMUX.SPIROUTEA = 0x00;
    PORTMUX.TCAROUTEA = 0x00;
    PORTMUX.TCBROUTEA = 0x00;
    PORTMUX.USARTROUTEA = 0x00;

    // register default ISC callback functions at runtime; use these methods to register a custom function
    PORTB_IO_PB3_SW3_SetInterruptHandler(PORTB_IO_PB3_SW3_DefaultInterruptHandler);
    PORTA_PA2_SetInterruptHandler(PORTA_PA2_DefaultInterruptHandler);
    PORTB_IO_PB2_SW2_SetInterruptHandler(PORTB_IO_PB2_SW2_DefaultInterruptHandler);
    PORTC_IO_PC3_TP3_SetInterruptHandler(PORTC_IO_PC3_TP3_DefaultInterruptHandler);
    PORTA_PA1_SetInterruptHandler(PORTA_PA1_DefaultInterruptHandler);
    PORTB_IO_PB5_SW5_SetInterruptHandler(PORTB_IO_PB5_SW5_DefaultInterruptHandler);
    PORTA_IO_PA4_IRQ_SetInterruptHandler(PORTA_IO_PA4_IRQ_DefaultInterruptHandler);
    PORTB_IO_PB4_SW4_SetInterruptHandler(PORTB_IO_PB4_SW4_DefaultInterruptHandler);
    PORTA_PA3_SetInterruptHandler(PORTA_PA3_DefaultInterruptHandler);
    PORTA_IO_PA6_CSN_SetInterruptHandler(PORTA_IO_PA6_CSN_DefaultInterruptHandler);
    PORTA_IO_PA5_PWR_SetInterruptHandler(PORTA_IO_PA5_PWR_DefaultInterruptHandler);
    PORTA_IO_PA7_CE_SetInterruptHandler(PORTA_IO_PA7_CE_DefaultInterruptHandler);
    PORTC_IO_PC0_TP0_SetInterruptHandler(PORTC_IO_PC0_TP0_DefaultInterruptHandler);
    PORTB_IO_PB1_SW1_SetInterruptHandler(PORTB_IO_PB1_SW1_DefaultInterruptHandler);
    PORTC_IO_PC2_TP2_SetInterruptHandler(PORTC_IO_PC2_TP2_DefaultInterruptHandler);
    PORTB_IO_PB0_SetInterruptHandler(PORTB_IO_PB0_DefaultInterruptHandler);
    PORTC_IO_PC1_TP1_SetInterruptHandler(PORTC_IO_PC1_TP1_DefaultInterruptHandler);
}

void PORT_Initialize(void)
{
    /* On AVR devices all peripherals are enable from power on reset, this
     * disables all peripherals to save power. Driver shall enable
     * peripheral if used */

    /* Set all pins to low power mode */
//    for (uint8_t i = 0; i < 8; i++) {
//        *((uint8_t *)&PORTA + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
//    }
    
    for (uint8_t i = 0; i < 8; i++) {
        *((uint8_t *)&PORTB + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
    }
    
//    for (uint8_t i = 0; i < 8; i++) {
//        *((uint8_t *)&PORTC + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
//    }
    
}

/**
  Allows selecting an interrupt handler for PORTB_IO_PB3_SW3 at application runtime
*/
void PORTB_IO_PB3_SW3_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTB_IO_PB3_SW3_InterruptHandler = interruptHandler;
}

void PORTB_IO_PB3_SW3_DefaultInterruptHandler(void)
{
    ServiceKeyPressInt();
    // add your PORTB_IO_PB3_SW3 interrupt custom code
    // or set custom function using PORTB_IO_PB3_SW3_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTA_PA2 at application runtime
*/
void PORTA_PA2_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTA_PA2_InterruptHandler = interruptHandler;
}

void PORTA_PA2_DefaultInterruptHandler(void)
{
    // add your PORTA_PA2 interrupt custom code
    // or set custom function using PORTA_PA2_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTB_IO_PB2_SW2 at application runtime
*/
void PORTB_IO_PB2_SW2_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTB_IO_PB2_SW2_InterruptHandler = interruptHandler;
}

void PORTB_IO_PB2_SW2_DefaultInterruptHandler(void)
{
    ServiceKeyPressInt();
    // add your PORTB_IO_PB2_SW2 interrupt custom code
    // or set custom function using PORTB_IO_PB2_SW2_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTC_IO_PC3_TP3 at application runtime
*/
void PORTC_IO_PC3_TP3_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTC_IO_PC3_TP3_InterruptHandler = interruptHandler;
}

void PORTC_IO_PC3_TP3_DefaultInterruptHandler(void)
{
    // add your PORTC_IO_PC3_TP3 interrupt custom code
    // or set custom function using PORTC_IO_PC3_TP3_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTA_PA1 at application runtime
*/
void PORTA_PA1_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTA_PA1_InterruptHandler = interruptHandler;
}

void PORTA_PA1_DefaultInterruptHandler(void)
{
    // add your PORTA_PA1 interrupt custom code
    // or set custom function using PORTA_PA1_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTB_IO_PB5_SW5 at application runtime
*/
void PORTB_IO_PB5_SW5_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTB_IO_PB5_SW5_InterruptHandler = interruptHandler;
}

void PORTB_IO_PB5_SW5_DefaultInterruptHandler(void)
{
    ServiceKeyPressInt();
    // add your PORTB_IO_PB5_SW5 interrupt custom code
    // or set custom function using PORTB_IO_PB5_SW5_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTA_IO_PA4_IRQ at application runtime
*/
void PORTA_IO_PA4_IRQ_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTA_IO_PA4_IRQ_InterruptHandler = interruptHandler;
}

void PORTA_IO_PA4_IRQ_DefaultInterruptHandler(void)
{
        PORTA_img = PORTA.IN;
        if(!(PORTA_img & 0x10))
        {
            asm ("nop");
            asm ("nop");
            asm ("nop");            
        }
    // add your PORTA_IO_PA4_IRQ interrupt custom code
    // or set custom function using PORTA_IO_PA4_IRQ_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTB_IO_PB4_SW4 at application runtime
*/
void PORTB_IO_PB4_SW4_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTB_IO_PB4_SW4_InterruptHandler = interruptHandler;
}

void PORTB_IO_PB4_SW4_DefaultInterruptHandler(void)
{
    ServiceKeyPressInt();
    // add your PORTB_IO_PB4_SW4 interrupt custom code
    // or set custom function using PORTB_IO_PB4_SW4_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTA_PA3 at application runtime
*/
void PORTA_PA3_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTA_PA3_InterruptHandler = interruptHandler;
}

void PORTA_PA3_DefaultInterruptHandler(void)
{
    // add your PORTA_PA3 interrupt custom code
    // or set custom function using PORTA_PA3_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTA_IO_PA6_CSN at application runtime
*/
void PORTA_IO_PA6_CSN_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTA_IO_PA6_CSN_InterruptHandler = interruptHandler;
}

void PORTA_IO_PA6_CSN_DefaultInterruptHandler(void)
{
    ServiceKeyPressInt();
    // add your PORTA_IO_PA6_CSN interrupt custom code
    // or set custom function using PORTA_IO_PA6_CSN_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTA_IO_PA5_PWR at application runtime
*/
void PORTA_IO_PA5_PWR_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTA_IO_PA5_PWR_InterruptHandler = interruptHandler;
}

void PORTA_IO_PA5_PWR_DefaultInterruptHandler(void)
{
    // add your PORTA_IO_PA5_PWR interrupt custom code
    // or set custom function using PORTA_IO_PA5_PWR_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTA_IO_PA7_CE at application runtime
*/
void PORTA_IO_PA7_CE_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTA_IO_PA7_CE_InterruptHandler = interruptHandler;
}

void PORTA_IO_PA7_CE_DefaultInterruptHandler(void)
{
    // add your PORTA_IO_PA7_CE interrupt custom code
    // or set custom function using PORTA_IO_PA7_CE_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTC_IO_PC0_TP0 at application runtime
*/
void PORTC_IO_PC0_TP0_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTC_IO_PC0_TP0_InterruptHandler = interruptHandler;
}

void PORTC_IO_PC0_TP0_DefaultInterruptHandler(void)
{
    // add your PORTC_IO_PC0_TP0 interrupt custom code
    // or set custom function using PORTC_IO_PC0_TP0_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTB_IO_PB1_SW1 at application runtime
*/
void PORTB_IO_PB1_SW1_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTB_IO_PB1_SW1_InterruptHandler = interruptHandler;
}

void PORTB_IO_PB1_SW1_DefaultInterruptHandler(void)
{
    ServiceKeyPressInt();
    // add your PORTB_IO_PB1_SW1 interrupt custom code
    // or set custom function using PORTB_IO_PB1_SW1_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTC_IO_PC2_TP2 at application runtime
*/
void PORTC_IO_PC2_TP2_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTC_IO_PC2_TP2_InterruptHandler = interruptHandler;
}

void PORTC_IO_PC2_TP2_DefaultInterruptHandler(void)
{
    // add your PORTC_IO_PC2_TP2 interrupt custom code
    // or set custom function using PORTC_IO_PC2_TP2_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTB_IO_PB0 at application runtime
*/
void PORTB_IO_PB0_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTB_IO_PB0_InterruptHandler = interruptHandler;
}

void PORTB_IO_PB0_DefaultInterruptHandler(void)
{
    ServiceKeyPressInt();
    // add your PORTB_IO_PB0 interrupt custom code
    // or set custom function using PORTB_IO_PB0_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for PORTC_IO_PC1_TP1 at application runtime
*/
void PORTC_IO_PC1_TP1_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    PORTC_IO_PC1_TP1_InterruptHandler = interruptHandler;
}

void PORTC_IO_PC1_TP1_DefaultInterruptHandler(void)
{
    // add your PORTC_IO_PC1_TP1 interrupt custom code
    // or set custom function using PORTC_IO_PC1_TP1_SetInterruptHandler()
}
ISR(PORTA_PORT_vect)
{  
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTA.INTFLAGS & PORT_INT2_bm)
    {
       PORTA_PA2_InterruptHandler();
    }
    if(VPORTA.INTFLAGS & PORT_INT1_bm)
    {
       PORTA_PA1_InterruptHandler();
    }
    if(VPORTA.INTFLAGS & PORT_INT4_bm)
    {
       PORTA_IO_PA4_IRQ_InterruptHandler();
    }
    if(VPORTA.INTFLAGS & PORT_INT3_bm)
    {
       PORTA_PA3_InterruptHandler();
    }
    if(VPORTA.INTFLAGS & PORT_INT6_bm)
    {
       PORTA_IO_PA6_CSN_InterruptHandler();
    }
    if(VPORTA.INTFLAGS & PORT_INT5_bm)
    {
       PORTA_IO_PA5_PWR_InterruptHandler();
    }
    if(VPORTA.INTFLAGS & PORT_INT7_bm)
    {
       PORTA_IO_PA7_CE_InterruptHandler();
    }

    /* Clear interrupt flags */
    VPORTA.INTFLAGS = 0xff;
}

ISR(PORTB_PORT_vect)
{  
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTB.INTFLAGS & PORT_INT3_bm)
    {
       PORTB_IO_PB3_SW3_InterruptHandler();
    }
    if(VPORTB.INTFLAGS & PORT_INT2_bm)
    {
       PORTB_IO_PB2_SW2_InterruptHandler();
    }
    if(VPORTB.INTFLAGS & PORT_INT5_bm)
    {
       PORTB_IO_PB5_SW5_InterruptHandler();
    }
    if(VPORTB.INTFLAGS & PORT_INT4_bm)
    {
       PORTB_IO_PB4_SW4_InterruptHandler();
    }
    if(VPORTB.INTFLAGS & PORT_INT1_bm)
    {
       PORTB_IO_PB1_SW1_InterruptHandler();
    }
    if(VPORTB.INTFLAGS & PORT_INT0_bm)
    {
       PORTB_IO_PB0_InterruptHandler();
    }

    /* Clear interrupt flags */
    VPORTB.INTFLAGS = 0xff;
}

