#include "main.h"
#include "key_handler.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/include/port.h"

#define debounce 20
#define hold_len 200 

extern volatile KEYstateBITS KeyStatus;
extern volatile KEYstateControl Key;
extern uint8_t go_tx;
extern uint8_t tx_pipe;
extern uint8_t get_resp;

void ServiceKeyPressInt(void)
{

    Key._kb = PORTB_get_port_level();
    Key._kb = Key._kb & 0x3f; // mask off buttons
    if(!KeyStatus._scan_st)
    {
        KeyStatus._interrupt = 1; // flag this interrupt happened
        Key._bounce = Key._kb;
        KeyStatus._scan_st = 1;
        KeyStatus._scan_end = 0;
        Key._DB_cnt = debounce; 
        KeyStatus._pressed = 1;
        Key._hold_cnt = 0;
        KeyStatus._hold_req = 0;
        KeyStatus._hold_ack = 0;
        
        if(Key._kb == 0x3f)
        {
            KeyStatus._pressed = 0;
        }
    }
}

void ServiceKeyPress(void)
{

    Key._kb = PORTB_get_port_level();
    Key._kb = Key._kb & 0x3f; // mask off buttons
    if (Key._kb == Key._bounce)
    {
        if (Key._DB_cnt != 0)
        {
            Key._DB_cnt--;
            if (Key._DB_cnt == 0)
            {
                KeyStatus._scan_end = 1;
                KeyStatus._scan_st = 0;
                if (Key._cmd != Key._bounce)
                {
                    Key._cmd = Key._bounce;
                    KeyStatus._new_cmd = 1;
                    if(Key._kb == 0x3f)
                    {
                        Key._hold_cnt = 0;
                        KeyStatus._pressed = 0;
                    }
                    else
                    {
                        Key._hold_cnt = hold_len;
                        KeyStatus._pressed = 1;                        
                    }
                }
            }
        }
        else if(KeyStatus._scan_end && !KeyStatus._scan_st && KeyStatus._pressed)
        {
            if(Key._hold_cnt != 0)
            {
                Key._hold_cnt--;
                if(Key._hold_cnt == 0)
                {
                    KeyStatus._hold_req = 1;
                    KeyStatus._hold_ack = 0;
                }
            }
        }
    }
    else
    {
        Key._bounce = Key._kb;
        Key._DB_cnt = debounce; 
        Key._hold_cnt = 0;
        KeyStatus._hold_req = 0;
        KeyStatus._hold_ack = 0;
    }
}    

void ServiceCmd(void)
{
    go_tx = 0;
    if (KeyStatus._new_cmd)
    {
        KeyStatus._new_cmd = 0;
        KeyStatus._scan_st = 0;        
        tx_pipe = 1;
        
        switch (Key._cmd)
        {
            case 0x3e:
            {
                go_tx = 1;
                break;
            }

            case 0x3d:
            {
                go_tx = 1;
                break;
            }

            case 0x3b:
            {
                go_tx = 1;
                break;
            }

            case 0x37:
            {
                go_tx = 1;
                break;
            }

            case 0x2f:
            {
                go_tx = 1;
                break;
            }

            case 0x1f:
            {
                go_tx = 1;
                break;
            }                        
        }
    }
    else if(KeyStatus._hold_req && !KeyStatus._hold_ack)
    {
        KeyStatus._hold_ack = 1;
        switch (Key._cmd)
        {
            case 0x3b:
            {
                go_tx = 3;
                tx_pipe = 1;
                get_resp = 0;
                break;
            }

            case 0x3d:
            {
                go_tx = 3;
                tx_pipe = 0;
                get_resp = 1;
                break;
            }

            case 0x1f:
            {
                go_tx = 3;
                tx_pipe = 0;
                get_resp = 1;
                break;
            }
            
        }
    }
}