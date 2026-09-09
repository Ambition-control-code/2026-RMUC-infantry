#ifndef _NEW_RC_H
#define _NEW_RC_H
#include  "main.h"
#include "struct_typedef.h"

#pragma pack(push, 1)
typedef struct
{
    uint8_t sof_1;
    uint8_t sof_2;
    uint64_t ch_0:11;
    uint64_t ch_1:11;
    uint64_t ch_2:11;
    uint64_t ch_3:11;
    uint64_t mode_sw:2;
    uint64_t pause:1;
    uint64_t fn_1:1;
    uint64_t fn_2:1;
    uint64_t wheel:11;
    uint64_t trigger:1;

    int16_t mouse_x;
    int16_t mouse_y;
    int16_t mouse_z;
    uint8_t mouse_left:2;
    uint8_t mouse_right:2;
    uint8_t mouse_middle:2;
    uint8_t without:2;
    uint16_t W:1;
    uint16_t S:1;
    uint16_t A:1;
    uint16_t D:1;
    uint16_t SHIFT:1;
    uint16_t CTRL:1;
    uint16_t Q:1;
    uint16_t E:1;
    uint16_t R:1;
    uint16_t F:1;
    uint16_t G:1;
    uint16_t Z:1;
    uint16_t X:1;
    uint16_t C:1;
    uint16_t V:1;
    uint16_t B:1;
    uint16_t crc16;
}VT13_data;
#pragma pack(pop)

typedef union {
	VT13_data use_data;
	uint8_t temp_data[21];
} VT13_USE_t;

extern VT13_USE_t VT13_USE;
void tele_link_init(void);
void work_out_tele_data(VT13_data *data);
void work_out_init(void);
#endif
