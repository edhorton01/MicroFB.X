
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

typedef struct {
    uint8_t _chan;
    uint8_t _address[5];
} micro_id;

typedef union {
    struct {
        uint8_t _flags;
    };
    struct {
        unsigned _front:1;
        unsigned _rear:1;
        unsigned _top:1;
        unsigned _sos:1;
        unsigned _ga:1;
        unsigned _gahold:1;
        unsigned _work:1;
        unsigned _state:1;        
    };
} ButtonState;

enum function_map {
    FRONT_M, 
    REAR_M, 
    TOP_M,
    SOS_M,
    GA_M,
    GAHOLD_M,
    WORK_M        
};