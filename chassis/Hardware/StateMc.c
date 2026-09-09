#include "StateMC.h"
#include "main.h"
#include "ui.h"

enum aim_status_t aim_status;

static uint8_t ui_group = 0;   // 当剝更新组索�???
static uint8_t ui_group2 = 0;  // 当剝初�?�化组索�???
static uint8_t ui_init_cnt = 0;  // 初�?�化计数，毝10次循�??执�?�一�??init

// 坄组初�?�化函数指针�???
typedef void (*ui_func_t)(void);
static const ui_func_t ui_init_table[] = {
    ui_init_g_INFORMATION,
    ui_init_g_INFOTMATION,
    ui_init_g_MIAOBIAN,
    ui_init_g_MOTOR,
};
static const ui_func_t ui_update_table[] = {
    ui_update_g_INFORMATION,
    ui_update_g_INFOTMATION,
    ui_update_g_MOTOR,
};
#define UI_GROUP_COUNT        (sizeof(ui_init_table)   / sizeof(ui_init_table[0]))
#define UI_UPDATE_GROUP_COUNT (sizeof(ui_update_table) / sizeof(ui_update_table[0]))

void draw_UI(void const * argument)
{
    ui_set_self_id();

    for (;;)
    {
        // ?�?�??�?�?�??10??�???�?�???????????�??
        if (++ui_init_cnt >= 10) {
            ui_init_cnt = 0;
            ui_init_table[ui_group2]();
            ui_group2 = (ui_group2 + 1) % UI_GROUP_COUNT;
        }
        ui_g_INFORMATION_MCLSP->number = (int)(Data_C.shoot_speed*1000);
        ui_g_MOTOR_WH1->color = Online.Cwhe1;
        ui_g_MOTOR_WH2->color = Online.Cwhe2;
        ui_g_MOTOR_WH3->color = Online.Cwhe3;
        ui_g_MOTOR_WH4->color = Online.Cwhe4;
        ui_g_MOTOR_CAP->color = Online.Ccapcity;
        ui_g_MOTOR_C->color = Online.Cc_board;
        ui_g_MOTOR_PIT2->color = Online.Cpitch2;
        ui_g_MOTOR_SHT->color = Online.Cshoot_2006;
        ui_g_INFORMATION_CAPCITY->color = 0;
        // ???????????: CAP_U_TX_DATA 1100~2300 ? end_x 537~925
            float cap_v = chassis_task.D_R.CAP_U_TX_DATA;
            if (cap_v >= 2300.0f)
                ui_g_INFORMATION_CAPCITY->end_x = 925;
            else if (cap_v <= 1100.0f)
                ui_g_INFORMATION_CAPCITY->end_x = 537;
            else
                ui_g_INFORMATION_CAPCITY->end_x = (int)(537.0f + ((cap_v - 1100.0f) / 1200.0f) * 388.0f);
        ui_g_INFORMATION_ZM_ON->number = robot.fortress;
        ui_update_table[ui_group]();
        ui_group = (ui_group + 1) % UI_UPDATE_GROUP_COUNT;
    }
}
