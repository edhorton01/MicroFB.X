#include "mcc_generated_files/utils/compiler.h"

/* Si241 register definations*/

#define CONFIG          0x00
#define EE_AA           0x01
#define EN_RXADDR       0x02
#define SETUP_AW        0x03
#define SETUP_RETR      0x04
#define RF_CH           0x05
#define RF_SETUP        0x06
#define STATUS          0x07
#define OBSERVE_TX      0x08
#define RSSI            0x09
#define RX_ADDR_P0      0x0A
#define RX_ADDR_P1      0x0B
#define RX_ADDR_P2      0x0C
#define RX_ADDR_P3      0x0D
#define RX_ADDR_P4      0x0E
#define RX_ADDR_P5      0x0F
#define TX_ADDR         0x10
#define EX_PW_P0        0x11
#define EX_PW_P1        0x12
#define EX_PW_P2        0x13
#define EX_PW_P3        0x14
#define EX_PW_P4        0x15
#define EX_PW_P5        0x16
#define FIFO_STATUS     0x17
#define DYNPD           0x1c
#define FEATURE         0x1d

#define R_REGISTER      0x00
#define W_REGISTER      0x20
#define R_RX_PAYLOAD    0x61
#define W_TX_PAYLOAD    0xA0
#define FLUSH_TX        0xE1
#define FLUSH_RX        0xE2
#define REUSE_TX_PL     0xE3
#define R_RX_PL_WID     0x60
#define W_ACK_PAYLOAD   0xA8
#define W_TX_PAYLOAD_NOACK   0xB0

#define DEFAULT_FOB_ID      { 'M', 'I', 'C', 'R', 'O' }

void SI241_PwrOn(void);
void SI241_PwrOff(void);
void SI241_SetupTx(void);
void SI241_SetTx(void);
void SI241_LoadTxAddress(void);
void SI241_LoadRxAddress(void);
void SI241_SetStandby(void);
uint8_t SI241_Status(void);
