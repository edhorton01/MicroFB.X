
#include "mcc_generated_files/utils/compiler.h"
    
typedef union {
    struct {
        uint8_t _flags;
    };
    struct {
        unsigned _scan_st:1;
        unsigned _scan_end:1;
        unsigned _pressed:1;        
        unsigned _new_cmd:1;
        unsigned _interrupt:1;
        unsigned _startCycle:1;
        unsigned _hold_req:1;
        unsigned _hold_ack:1;        
    };
} KEYstateBITS;

typedef struct {
    uint8_t _DB_cnt;
    uint8_t _bounce;
    uint8_t _cmd;
    uint8_t _kb;
    uint16_t _hold_cnt;    
} KEYstateControl;

typedef union {
    struct {
        uint8_t _flags;
    };
    struct {
        unsigned _tc0:1;
        unsigned _spi_tx:1;
        unsigned _spi_rx:1;        
    };
} INTstateBITS;

typedef struct {
    uint8_t _delaycnt;    
    uint8_t _delaytarget;
    union
    {
        struct {
            uint8_t _flags;
        };
        struct {
            unsigned _active:1;
            unsigned _finished:1;        
        };
    };
} TmrDelay;
