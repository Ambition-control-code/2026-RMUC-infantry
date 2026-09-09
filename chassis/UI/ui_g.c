//
// Created by RM UI Designer
// Static Edition
//

#include <string.h>
#include <main.h>
#include "can.h"
#include "ui_interface.h"

ui_5_frame_t ui_g_INFORMATION_0;

ui_interface_round_t *ui_g_INFORMATION_ZX = (ui_interface_round_t*)&(ui_g_INFORMATION_0.data[0]);
ui_interface_rect_t *ui_g_INFORMATION_CAPCITY = (ui_interface_rect_t*)&(ui_g_INFORMATION_0.data[1]);
ui_interface_number_t *ui_g_INFORMATION_MCLSP = (ui_interface_number_t*)&(ui_g_INFORMATION_0.data[2]);
ui_interface_number_t *ui_g_INFORMATION_FDL = (ui_interface_number_t*)&(ui_g_INFORMATION_0.data[3]);
ui_interface_number_t *ui_g_INFORMATION_ZM_ON = (ui_interface_number_t*)&(ui_g_INFORMATION_0.data[4]);

void _ui_init_g_INFORMATION_0() {
    for (int i = 0; i < 5; i++) {
        ui_g_INFORMATION_0.data[i].figure_name[0] = 0;
        ui_g_INFORMATION_0.data[i].figure_name[1] = 0;
        ui_g_INFORMATION_0.data[i].figure_name[2] = i + 0;
        ui_g_INFORMATION_0.data[i].operate_type = 1;
    }
    for (int i = 5; i < 5; i++) {
        ui_g_INFORMATION_0.data[i].operate_type = 0;
    }

    ui_g_INFORMATION_ZX->figure_type = 2;
    ui_g_INFORMATION_ZX->operate_type = 1;
    ui_g_INFORMATION_ZX->layer = 0;
    ui_g_INFORMATION_ZX->color = 2;
    ui_g_INFORMATION_ZX->start_x = 964;
    ui_g_INFORMATION_ZX->start_y = 532;
    ui_g_INFORMATION_ZX->width = 4;
    ui_g_INFORMATION_ZX->r = 3;

    ui_g_INFORMATION_CAPCITY->figure_type = 1;
    ui_g_INFORMATION_CAPCITY->operate_type = 1;
    ui_g_INFORMATION_CAPCITY->layer = 0;
    ui_g_INFORMATION_CAPCITY->color = Data_C.spin;
    ui_g_INFORMATION_CAPCITY->start_x = 537;
    ui_g_INFORMATION_CAPCITY->start_y = 119;
    ui_g_INFORMATION_CAPCITY->width = 17;
    ui_g_INFORMATION_CAPCITY->end_x = 925;
    ui_g_INFORMATION_CAPCITY->end_y = 126;

    ui_g_INFORMATION_MCLSP->figure_type = 5;
    ui_g_INFORMATION_MCLSP->operate_type = 1;
    ui_g_INFORMATION_MCLSP->layer = 0;
    ui_g_INFORMATION_MCLSP->color = 0;
    ui_g_INFORMATION_MCLSP->start_x = 466;
    ui_g_INFORMATION_MCLSP->start_y = 787;
    ui_g_INFORMATION_MCLSP->width = 1;
    ui_g_INFORMATION_MCLSP->font_size = 15;
    ui_g_INFORMATION_MCLSP->number = (int)(Data_C.shoot_speed*1000);

    ui_g_INFORMATION_FDL->figure_type = 6;
    ui_g_INFORMATION_FDL->operate_type = 1;
    ui_g_INFORMATION_FDL->layer = 0;
    ui_g_INFORMATION_FDL->color = 8;
    ui_g_INFORMATION_FDL->start_x = 721;
    ui_g_INFORMATION_FDL->start_y = 779;
    ui_g_INFORMATION_FDL->width = 1;
    ui_g_INFORMATION_FDL->font_size = 15;
    ui_g_INFORMATION_FDL->number = 500;

    ui_g_INFORMATION_ZM_ON->figure_type = 6;
    ui_g_INFORMATION_ZM_ON->operate_type = 1;
    ui_g_INFORMATION_ZM_ON->layer = 0;
    ui_g_INFORMATION_ZM_ON->color = 0;
    ui_g_INFORMATION_ZM_ON->start_x = 1398;
    ui_g_INFORMATION_ZM_ON->start_y = 828;
    ui_g_INFORMATION_ZM_ON->width = 2;
    ui_g_INFORMATION_ZM_ON->font_size = 23;
    ui_g_INFORMATION_ZM_ON->number = 0;


    ui_proc_5_frame(&ui_g_INFORMATION_0);
    SEND_MESSAGE((uint8_t *) &ui_g_INFORMATION_0, sizeof(ui_g_INFORMATION_0));
}

void _ui_update_g_INFORMATION_0() {
    for (int i = 0; i < 5; i++) {
        ui_g_INFORMATION_0.data[i].operate_type = 2;
    }

    ui_proc_5_frame(&ui_g_INFORMATION_0);
    SEND_MESSAGE((uint8_t *) &ui_g_INFORMATION_0, sizeof(ui_g_INFORMATION_0));
}

void _ui_remove_g_INFORMATION_0() {
    for (int i = 0; i < 5; i++) {
        ui_g_INFORMATION_0.data[i].operate_type = 3;
    }

    ui_proc_5_frame(&ui_g_INFORMATION_0);
    SEND_MESSAGE((uint8_t *) &ui_g_INFORMATION_0, sizeof(ui_g_INFORMATION_0));
}

ui_string_frame_t ui_g_INFORMATION_1;
ui_interface_string_t* ui_g_INFORMATION_ZM = &(ui_g_INFORMATION_1.option);

static void _ui_set_g_INFORMATION_gesture_text(void) {
    const uint8_t is_goudong = (Data_C.Shift != 0);
    const char *gesture_text = is_goudong ? "GD" : "NM";
    ui_g_INFORMATION_ZM->color = is_goudong ? 2 : 1;
    ui_g_INFORMATION_ZM->str_length = 2;
    strcpy(ui_g_INFORMATION_ZM->string, gesture_text);
}

void _ui_init_g_INFORMATION_1() {
    ui_g_INFORMATION_1.option.figure_name[0] = 0;
    ui_g_INFORMATION_1.option.figure_name[1] = 0;
    ui_g_INFORMATION_1.option.figure_name[2] = 5;
    ui_g_INFORMATION_1.option.operate_type = 1;

    ui_g_INFORMATION_ZM->figure_type = 7;
    ui_g_INFORMATION_ZM->operate_type = 1;
    ui_g_INFORMATION_ZM->layer = 0;
    ui_g_INFORMATION_ZM->start_x = 900;
    ui_g_INFORMATION_ZM->start_y = 428;
    ui_g_INFORMATION_ZM->width = 2;
    ui_g_INFORMATION_ZM->font_size = 23;
    _ui_set_g_INFORMATION_gesture_text();


    ui_proc_string_frame(&ui_g_INFORMATION_1);
    SEND_MESSAGE((uint8_t *) &ui_g_INFORMATION_1, sizeof(ui_g_INFORMATION_1));
}

void _ui_update_g_INFORMATION_1() {
    ui_g_INFORMATION_1.option.operate_type = 2;
    _ui_set_g_INFORMATION_gesture_text();

    ui_proc_string_frame(&ui_g_INFORMATION_1);
    SEND_MESSAGE((uint8_t *) &ui_g_INFORMATION_1, sizeof(ui_g_INFORMATION_1));
}

void _ui_remove_g_INFORMATION_1() {
    ui_g_INFORMATION_1.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_INFORMATION_1);
    SEND_MESSAGE((uint8_t *) &ui_g_INFORMATION_1, sizeof(ui_g_INFORMATION_1));
}
ui_string_frame_t ui_g_INFORMATION_2;
ui_interface_string_t* ui_g_INFORMATION_LEVEL = &(ui_g_INFORMATION_2.option);

void _ui_init_g_INFORMATION_2() {
    ui_g_INFORMATION_2.option.figure_name[0] = 0;
    ui_g_INFORMATION_2.option.figure_name[1] = 0;
    ui_g_INFORMATION_2.option.figure_name[2] = 6;
    ui_g_INFORMATION_2.option.operate_type = 1;

    ui_g_INFORMATION_LEVEL->figure_type = 7;
    ui_g_INFORMATION_LEVEL->operate_type = 1;
    ui_g_INFORMATION_LEVEL->layer = 0;
    ui_g_INFORMATION_LEVEL->color = 0;
    ui_g_INFORMATION_LEVEL->start_x = 862;
    ui_g_INFORMATION_LEVEL->start_y = 698;
    ui_g_INFORMATION_LEVEL->width = 2;
    ui_g_INFORMATION_LEVEL->font_size = 1;
    ui_g_INFORMATION_LEVEL->str_length = 3;
    strcpy(ui_g_INFORMATION_LEVEL->string, "LV");


    ui_proc_string_frame(&ui_g_INFORMATION_2);
    SEND_MESSAGE((uint8_t *) &ui_g_INFORMATION_2, sizeof(ui_g_INFORMATION_2));
}

void _ui_update_g_INFORMATION_2() {
    ui_g_INFORMATION_2.option.operate_type = 2;

    ui_proc_string_frame(&ui_g_INFORMATION_2);
    SEND_MESSAGE((uint8_t *) &ui_g_INFORMATION_2, sizeof(ui_g_INFORMATION_2));
}

void _ui_remove_g_INFORMATION_2() {
    ui_g_INFORMATION_2.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_INFORMATION_2);
    SEND_MESSAGE((uint8_t *) &ui_g_INFORMATION_2, sizeof(ui_g_INFORMATION_2));
}

void ui_init_g_INFORMATION() {
    _ui_init_g_INFORMATION_0();
    _ui_init_g_INFORMATION_1();
    _ui_init_g_INFORMATION_2();
}

void ui_update_g_INFORMATION() {
    _ui_update_g_INFORMATION_0();
    _ui_update_g_INFORMATION_1();
    _ui_update_g_INFORMATION_2();
}

void ui_remove_g_INFORMATION() {
    _ui_remove_g_INFORMATION_0();
    _ui_remove_g_INFORMATION_1();
    _ui_remove_g_INFORMATION_2();
}

ui_1_frame_t ui_g_INFOTMATION_0;

ui_interface_number_t *ui_g_INFOTMATION_PITCHAN = (ui_interface_number_t*)&(ui_g_INFOTMATION_0.data[0]);

void _ui_init_g_INFOTMATION_0() {
    for (int i = 0; i < 1; i++) {
        ui_g_INFOTMATION_0.data[i].figure_name[0] = 0;
        ui_g_INFOTMATION_0.data[i].figure_name[1] = 1;
        ui_g_INFOTMATION_0.data[i].figure_name[2] = i + 0;
        ui_g_INFOTMATION_0.data[i].operate_type = 1;
    }
    for (int i = 1; i < 1; i++) {
        ui_g_INFOTMATION_0.data[i].operate_type = 0;
    }

    ui_g_INFOTMATION_PITCHAN->figure_type = 6;
    ui_g_INFOTMATION_PITCHAN->operate_type = 1;
    ui_g_INFOTMATION_PITCHAN->layer = 0;
    ui_g_INFOTMATION_PITCHAN->color = 0;
    ui_g_INFOTMATION_PITCHAN->start_x = 470;
    ui_g_INFOTMATION_PITCHAN->start_y = 574;
    ui_g_INFOTMATION_PITCHAN->width = 2;
    ui_g_INFOTMATION_PITCHAN->font_size = 20;
    ui_g_INFOTMATION_PITCHAN->number = 12;


    ui_proc_1_frame(&ui_g_INFOTMATION_0);
    SEND_MESSAGE((uint8_t *) &ui_g_INFOTMATION_0, sizeof(ui_g_INFOTMATION_0));
}

void _ui_update_g_INFOTMATION_0() {
    for (int i = 0; i < 1; i++) {
        ui_g_INFOTMATION_0.data[i].operate_type = 2;
    }

    ui_proc_1_frame(&ui_g_INFOTMATION_0);
    SEND_MESSAGE((uint8_t *) &ui_g_INFOTMATION_0, sizeof(ui_g_INFOTMATION_0));
}

void _ui_remove_g_INFOTMATION_0() {
    for (int i = 0; i < 1; i++) {
        ui_g_INFOTMATION_0.data[i].operate_type = 3;
    }

    ui_proc_1_frame(&ui_g_INFOTMATION_0);
    SEND_MESSAGE((uint8_t *) &ui_g_INFOTMATION_0, sizeof(ui_g_INFOTMATION_0));
}

ui_string_frame_t ui_g_INFOTMATION_1;
ui_interface_string_t* ui_g_INFOTMATION_MODE = &(ui_g_INFOTMATION_1.option);

static void ui_update_chassis_mode_display(void) {
    switch (chassis_task.chassis_mode_e) {
        case CHASSIS_SPIN:
            ui_g_INFOTMATION_MODE->color = ((HAL_GetTick() / 300U) % 2U) ? 8U : 2U;
            strcpy(ui_g_INFOTMATION_MODE->string, "SPIN");
            break;

        case CHASSIS_FOLLOW:
            ui_g_INFOTMATION_MODE->color = 0U;
            strcpy(ui_g_INFOTMATION_MODE->string, "NOML");
            break;

        default:
            ui_g_INFOTMATION_MODE->color = 0U;
            strcpy(ui_g_INFOTMATION_MODE->string, "MODE");
            break;
    }
}

void _ui_init_g_INFOTMATION_1() {
    ui_g_INFOTMATION_1.option.figure_name[0] = 0;
    ui_g_INFOTMATION_1.option.figure_name[1] = 1;
    ui_g_INFOTMATION_1.option.figure_name[2] = 1;
    ui_g_INFOTMATION_1.option.operate_type = 1;

    ui_g_INFOTMATION_MODE->figure_type = 7;
    ui_g_INFOTMATION_MODE->operate_type = 1;
    ui_g_INFOTMATION_MODE->layer = 0;
    ui_g_INFOTMATION_MODE->start_x = 665;
    ui_g_INFOTMATION_MODE->start_y = 810;
    ui_g_INFOTMATION_MODE->width = 2;
    ui_g_INFOTMATION_MODE->font_size = 20;
    ui_g_INFOTMATION_MODE->str_length = 4;
    ui_update_chassis_mode_display();


    ui_proc_string_frame(&ui_g_INFOTMATION_1);
    SEND_MESSAGE((uint8_t *) &ui_g_INFOTMATION_1, sizeof(ui_g_INFOTMATION_1));
}

void _ui_update_g_INFOTMATION_1() {
    ui_g_INFOTMATION_1.option.operate_type = 2;
    ui_update_chassis_mode_display();

    ui_proc_string_frame(&ui_g_INFOTMATION_1);
    SEND_MESSAGE((uint8_t *) &ui_g_INFOTMATION_1, sizeof(ui_g_INFOTMATION_1));
}

void _ui_remove_g_INFOTMATION_1() {
    ui_g_INFOTMATION_1.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_INFOTMATION_1);
    SEND_MESSAGE((uint8_t *) &ui_g_INFOTMATION_1, sizeof(ui_g_INFOTMATION_1));
}
ui_string_frame_t ui_g_INFOTMATION_2;
ui_interface_string_t* ui_g_INFOTMATION_COUNT = &(ui_g_INFOTMATION_2.option);

static void ui_update_tunnel_mode_display(void) {
    switch (s_tunnel_mode) {
        case TUNNEL_WIREPOWER:
            strcpy(ui_g_INFOTMATION_COUNT->string, "CHARG");
            ui_g_INFOTMATION_COUNT->font_size = 17;
            break;

        case TUNNEL_SUPERPOWER:
            strcpy(ui_g_INFOTMATION_COUNT->string, "SUPER");
            ui_g_INFOTMATION_COUNT->font_size = 1;
            break;

        default:
            strcpy(ui_g_INFOTMATION_COUNT->string, "ERROR");
            ui_g_INFOTMATION_COUNT->font_size = 1;
            break;
    }
}

void _ui_init_g_INFOTMATION_2() {
    ui_g_INFOTMATION_2.option.figure_name[0] = 0;
    ui_g_INFOTMATION_2.option.figure_name[1] = 1;
    ui_g_INFOTMATION_2.option.figure_name[2] = 2;
    ui_g_INFOTMATION_2.option.operate_type = 1;

    ui_g_INFOTMATION_COUNT->figure_type = 7;
    ui_g_INFOTMATION_COUNT->operate_type = 1;
    ui_g_INFOTMATION_COUNT->layer = 0;
    ui_g_INFOTMATION_COUNT->color = 0;
    ui_g_INFOTMATION_COUNT->start_x = 859;
    ui_g_INFOTMATION_COUNT->start_y = 629;
    ui_g_INFOTMATION_COUNT->width = 2;
    ui_g_INFOTMATION_COUNT->font_size = 17;
    ui_g_INFOTMATION_COUNT->str_length = 5;
    ui_update_tunnel_mode_display();


    ui_proc_string_frame(&ui_g_INFOTMATION_2);
    SEND_MESSAGE((uint8_t *) &ui_g_INFOTMATION_2, sizeof(ui_g_INFOTMATION_2));
}

void _ui_update_g_INFOTMATION_2() {
    ui_g_INFOTMATION_2.option.operate_type = 2;
    ui_update_tunnel_mode_display();

    ui_proc_string_frame(&ui_g_INFOTMATION_2);
    SEND_MESSAGE((uint8_t *) &ui_g_INFOTMATION_2, sizeof(ui_g_INFOTMATION_2));
}

void _ui_remove_g_INFOTMATION_2() {
    ui_g_INFOTMATION_2.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_INFOTMATION_2);
    SEND_MESSAGE((uint8_t *) &ui_g_INFOTMATION_2, sizeof(ui_g_INFOTMATION_2));
}

void ui_init_g_INFOTMATION() {
    _ui_init_g_INFOTMATION_0();
    _ui_init_g_INFOTMATION_1();
    _ui_init_g_INFOTMATION_2();
}

void ui_update_g_INFOTMATION() {
    _ui_update_g_INFOTMATION_0();
    _ui_update_g_INFOTMATION_1();
    _ui_update_g_INFOTMATION_2();
}

void ui_remove_g_INFOTMATION() {
    _ui_remove_g_INFOTMATION_0();
    _ui_remove_g_INFOTMATION_1();
    _ui_remove_g_INFOTMATION_2();
}

ui_5_frame_t ui_g_MIAOBIAN_0;

ui_interface_rect_t *ui_g_MIAOBIAN_QUAN = (ui_interface_rect_t*)&(ui_g_MIAOBIAN_0.data[0]);
ui_interface_line_t *ui_g_MIAOBIAN_SB = (ui_interface_line_t*)&(ui_g_MIAOBIAN_0.data[1]);
ui_interface_line_t *ui_g_MIAOBIAN_SB2 = (ui_interface_line_t*)&(ui_g_MIAOBIAN_0.data[2]);
ui_interface_round_t *ui_g_MIAOBIAN_SBROUND = (ui_interface_round_t*)&(ui_g_MIAOBIAN_0.data[3]);
ui_interface_round_t *ui_g_MIAOBIAN_SBROUND2 = (ui_interface_round_t*)&(ui_g_MIAOBIAN_0.data[4]);

void _ui_init_g_MIAOBIAN_0() {
    for (int i = 0; i < 5; i++) {
        ui_g_MIAOBIAN_0.data[i].figure_name[0] = 0;
        ui_g_MIAOBIAN_0.data[i].figure_name[1] = 2;
        ui_g_MIAOBIAN_0.data[i].figure_name[2] = i + 0;
        ui_g_MIAOBIAN_0.data[i].operate_type = 1;
    }
    for (int i = 5; i < 5; i++) {
        ui_g_MIAOBIAN_0.data[i].operate_type = 0;
    }

    ui_g_MIAOBIAN_QUAN->figure_type = 1;
    ui_g_MIAOBIAN_QUAN->operate_type = 1;
    ui_g_MIAOBIAN_QUAN->layer = 0;
    ui_g_MIAOBIAN_QUAN->color = 8;
    ui_g_MIAOBIAN_QUAN->start_x = 1360;
    ui_g_MIAOBIAN_QUAN->start_y = 489;
    ui_g_MIAOBIAN_QUAN->width = 3;
    ui_g_MIAOBIAN_QUAN->end_x = 1461;
    ui_g_MIAOBIAN_QUAN->end_y = 590;

    ui_g_MIAOBIAN_SB->figure_type = 0;
    ui_g_MIAOBIAN_SB->operate_type = 1;
    ui_g_MIAOBIAN_SB->layer = 0;
    ui_g_MIAOBIAN_SB->color = 0;
    ui_g_MIAOBIAN_SB->start_x = 211;
    ui_g_MIAOBIAN_SB->start_y = 864;
    ui_g_MIAOBIAN_SB->width = 5;
    ui_g_MIAOBIAN_SB->end_x = 213;
    ui_g_MIAOBIAN_SB->end_y = 992;

    ui_g_MIAOBIAN_SB2->figure_type = 0;
    ui_g_MIAOBIAN_SB2->operate_type = 1;
    ui_g_MIAOBIAN_SB2->layer = 0;
    ui_g_MIAOBIAN_SB2->color = 6;
    ui_g_MIAOBIAN_SB2->start_x = 1705;
    ui_g_MIAOBIAN_SB2->start_y = 889;
    ui_g_MIAOBIAN_SB2->width = 4;
    ui_g_MIAOBIAN_SB2->end_x = 1705;
    ui_g_MIAOBIAN_SB2->end_y = 994;

    ui_g_MIAOBIAN_SBROUND->figure_type = 2;
    ui_g_MIAOBIAN_SBROUND->operate_type = 1;
    ui_g_MIAOBIAN_SBROUND->layer = 0;
    ui_g_MIAOBIAN_SBROUND->color = 0;
    ui_g_MIAOBIAN_SBROUND->start_x = 213;
    ui_g_MIAOBIAN_SBROUND->start_y = 847;
    ui_g_MIAOBIAN_SBROUND->width = 4;
    ui_g_MIAOBIAN_SBROUND->r = 18;

    ui_g_MIAOBIAN_SBROUND2->figure_type = 2;
    ui_g_MIAOBIAN_SBROUND2->operate_type = 1;
    ui_g_MIAOBIAN_SBROUND2->layer = 0;
    ui_g_MIAOBIAN_SBROUND2->color = 6;
    ui_g_MIAOBIAN_SBROUND2->start_x = 1705;
    ui_g_MIAOBIAN_SBROUND2->start_y = 867;
    ui_g_MIAOBIAN_SBROUND2->width = 4;
    ui_g_MIAOBIAN_SBROUND2->r = 18;


    ui_proc_5_frame(&ui_g_MIAOBIAN_0);
    SEND_MESSAGE((uint8_t *) &ui_g_MIAOBIAN_0, sizeof(ui_g_MIAOBIAN_0));
}

void _ui_update_g_MIAOBIAN_0() {
    for (int i = 0; i < 5; i++) {
        ui_g_MIAOBIAN_0.data[i].operate_type = 2;
    }

    ui_proc_5_frame(&ui_g_MIAOBIAN_0);
    SEND_MESSAGE((uint8_t *) &ui_g_MIAOBIAN_0, sizeof(ui_g_MIAOBIAN_0));
}

void _ui_remove_g_MIAOBIAN_0() {
    for (int i = 0; i < 5; i++) {
        ui_g_MIAOBIAN_0.data[i].operate_type = 3;
    }

    ui_proc_5_frame(&ui_g_MIAOBIAN_0);
    SEND_MESSAGE((uint8_t *) &ui_g_MIAOBIAN_0, sizeof(ui_g_MIAOBIAN_0));
}

ui_string_frame_t ui_g_MIAOBIAN_1;
ui_interface_string_t* ui_g_MIAOBIAN_NL = &(ui_g_MIAOBIAN_1.option);

void _ui_init_g_MIAOBIAN_1() {
    ui_g_MIAOBIAN_1.option.figure_name[0] = 0;
    ui_g_MIAOBIAN_1.option.figure_name[1] = 2;
    ui_g_MIAOBIAN_1.option.figure_name[2] = 5;
    ui_g_MIAOBIAN_1.option.operate_type = 1;

    ui_g_MIAOBIAN_NL->figure_type = 7;
    ui_g_MIAOBIAN_NL->operate_type = 1;
    ui_g_MIAOBIAN_NL->layer = 0;
    ui_g_MIAOBIAN_NL->color = 0;
    ui_g_MIAOBIAN_NL->start_x = 1489;
    ui_g_MIAOBIAN_NL->start_y = 321;
    ui_g_MIAOBIAN_NL->width = 2;
    ui_g_MIAOBIAN_NL->font_size = 20;
    ui_g_MIAOBIAN_NL->str_length = 6;
    strcpy(ui_g_MIAOBIAN_NL->string, "PINKYO");


    ui_proc_string_frame(&ui_g_MIAOBIAN_1);
    SEND_MESSAGE((uint8_t *) &ui_g_MIAOBIAN_1, sizeof(ui_g_MIAOBIAN_1));
}

void _ui_update_g_MIAOBIAN_1() {
    ui_g_MIAOBIAN_1.option.operate_type = 2;

    ui_proc_string_frame(&ui_g_MIAOBIAN_1);
    SEND_MESSAGE((uint8_t *) &ui_g_MIAOBIAN_1, sizeof(ui_g_MIAOBIAN_1));
}

void _ui_remove_g_MIAOBIAN_1() {
    ui_g_MIAOBIAN_1.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_MIAOBIAN_1);
    SEND_MESSAGE((uint8_t *) &ui_g_MIAOBIAN_1, sizeof(ui_g_MIAOBIAN_1));
}

void ui_init_g_MIAOBIAN() {
    _ui_init_g_MIAOBIAN_0();
    _ui_init_g_MIAOBIAN_1();
}

void ui_update_g_MIAOBIAN() {
    _ui_update_g_MIAOBIAN_0();
    _ui_update_g_MIAOBIAN_1();
}

void ui_remove_g_MIAOBIAN() {
    _ui_remove_g_MIAOBIAN_0();
    _ui_remove_g_MIAOBIAN_1();
}


ui_string_frame_t ui_g_MOTOR_0;
ui_interface_string_t* ui_g_MOTOR_YAW = &(ui_g_MOTOR_0.option);

void _ui_init_g_MOTOR_0() {
    ui_g_MOTOR_0.option.figure_name[0] = 0;
    ui_g_MOTOR_0.option.figure_name[1] = 3;
    ui_g_MOTOR_0.option.figure_name[2] = 0;
    ui_g_MOTOR_0.option.operate_type = 1;

    ui_g_MOTOR_YAW->figure_type = 7;
    ui_g_MOTOR_YAW->operate_type = 1;
    ui_g_MOTOR_YAW->layer = 0;
    ui_g_MOTOR_YAW->color = 8;
    ui_g_MOTOR_YAW->start_x = 4;
    ui_g_MOTOR_YAW->start_y = 874;
    ui_g_MOTOR_YAW->width = 1;
    ui_g_MOTOR_YAW->font_size = 14;
    ui_g_MOTOR_YAW->str_length = 3;
    strcpy(ui_g_MOTOR_YAW->string, "YAW");


    ui_proc_string_frame(&ui_g_MOTOR_0);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_0, sizeof(ui_g_MOTOR_0));
}

void _ui_update_g_MOTOR_0() {
    ui_g_MOTOR_0.option.operate_type = 2;

    ui_proc_string_frame(&ui_g_MOTOR_0);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_0, sizeof(ui_g_MOTOR_0));
}

void _ui_remove_g_MOTOR_0() {
    ui_g_MOTOR_0.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_MOTOR_0);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_0, sizeof(ui_g_MOTOR_0));
}
ui_string_frame_t ui_g_MOTOR_1;
ui_interface_string_t* ui_g_MOTOR_PIT2 = &(ui_g_MOTOR_1.option);

void _ui_init_g_MOTOR_1() {
    ui_g_MOTOR_1.option.figure_name[0] = 0;
    ui_g_MOTOR_1.option.figure_name[1] = 3;
    ui_g_MOTOR_1.option.figure_name[2] = 1;
    ui_g_MOTOR_1.option.operate_type = 1;

    ui_g_MOTOR_PIT2->figure_type = 7;
    ui_g_MOTOR_PIT2->operate_type = 1;
    ui_g_MOTOR_PIT2->layer = 0;
    ui_g_MOTOR_PIT2->color = 8;
    ui_g_MOTOR_PIT2->start_x = 6;
    ui_g_MOTOR_PIT2->start_y = 847;
    ui_g_MOTOR_PIT2->width = 1;
    ui_g_MOTOR_PIT2->font_size = 14;
    ui_g_MOTOR_PIT2->str_length = 4;
    strcpy(ui_g_MOTOR_PIT2->string, "PIT2");


    ui_proc_string_frame(&ui_g_MOTOR_1);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_1, sizeof(ui_g_MOTOR_1));
}

void _ui_update_g_MOTOR_1() {
    ui_g_MOTOR_1.option.operate_type = 2;

    ui_proc_string_frame(&ui_g_MOTOR_1);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_1, sizeof(ui_g_MOTOR_1));
}

void _ui_remove_g_MOTOR_1() {
    ui_g_MOTOR_1.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_MOTOR_1);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_1, sizeof(ui_g_MOTOR_1));
}
ui_string_frame_t ui_g_MOTOR_2;
ui_interface_string_t* ui_g_MOTOR_WH1 = &(ui_g_MOTOR_2.option);

void _ui_init_g_MOTOR_2() {
    ui_g_MOTOR_2.option.figure_name[0] = 0;
    ui_g_MOTOR_2.option.figure_name[1] = 3;
    ui_g_MOTOR_2.option.figure_name[2] = 2;
    ui_g_MOTOR_2.option.operate_type = 1;

    ui_g_MOTOR_WH1->figure_type = 7;
    ui_g_MOTOR_WH1->operate_type = 1;
    ui_g_MOTOR_WH1->layer = 0;
    ui_g_MOTOR_WH1->color = Online.Cwhe1;
    ui_g_MOTOR_WH1->start_x = 7;
    ui_g_MOTOR_WH1->start_y = 822;
    ui_g_MOTOR_WH1->width = 1;
    ui_g_MOTOR_WH1->font_size = 14;
    ui_g_MOTOR_WH1->str_length = 3;
    strcpy(ui_g_MOTOR_WH1->string, "WH1");


    ui_proc_string_frame(&ui_g_MOTOR_2);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_2, sizeof(ui_g_MOTOR_2));
}

void _ui_update_g_MOTOR_2() {
    ui_g_MOTOR_2.option.operate_type = 2;

    ui_proc_string_frame(&ui_g_MOTOR_2);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_2, sizeof(ui_g_MOTOR_2));
}

void _ui_remove_g_MOTOR_2() {
    ui_g_MOTOR_2.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_MOTOR_2);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_2, sizeof(ui_g_MOTOR_2));
}
ui_string_frame_t ui_g_MOTOR_3;
ui_interface_string_t* ui_g_MOTOR_WH2 = &(ui_g_MOTOR_3.option);

void _ui_init_g_MOTOR_3() {
    ui_g_MOTOR_3.option.figure_name[0] = 0;
    ui_g_MOTOR_3.option.figure_name[1] = 3;
    ui_g_MOTOR_3.option.figure_name[2] = 3;
    ui_g_MOTOR_3.option.operate_type = 1;

    ui_g_MOTOR_WH2->figure_type = 7;
    ui_g_MOTOR_WH2->operate_type = 1;
    ui_g_MOTOR_WH2->layer = 0;
    ui_g_MOTOR_WH2->color = Online.Cwhe2;
    ui_g_MOTOR_WH2->start_x = 7;
    ui_g_MOTOR_WH2->start_y = 799;
    ui_g_MOTOR_WH2->width = 1;
    ui_g_MOTOR_WH2->font_size = 14;
    ui_g_MOTOR_WH2->str_length = 3;
    strcpy(ui_g_MOTOR_WH2->string, "WH2");


    ui_proc_string_frame(&ui_g_MOTOR_3);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_3, sizeof(ui_g_MOTOR_3));
}

void _ui_update_g_MOTOR_3() {
    ui_g_MOTOR_3.option.operate_type = 2;

    ui_proc_string_frame(&ui_g_MOTOR_3);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_3, sizeof(ui_g_MOTOR_3));
}

void _ui_remove_g_MOTOR_3() {
    ui_g_MOTOR_3.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_MOTOR_3);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_3, sizeof(ui_g_MOTOR_3));
}
ui_string_frame_t ui_g_MOTOR_4;
ui_interface_string_t* ui_g_MOTOR_WH3 = &(ui_g_MOTOR_4.option);

void _ui_init_g_MOTOR_4() {
    ui_g_MOTOR_4.option.figure_name[0] = 0;
    ui_g_MOTOR_4.option.figure_name[1] = 3;
    ui_g_MOTOR_4.option.figure_name[2] = 4;
    ui_g_MOTOR_4.option.operate_type = 1;

    ui_g_MOTOR_WH3->figure_type = 7;
    ui_g_MOTOR_WH3->operate_type = 1;
    ui_g_MOTOR_WH3->layer = 0;
    ui_g_MOTOR_WH3->color = Online.Cwhe3;
    ui_g_MOTOR_WH3->start_x = 7;
    ui_g_MOTOR_WH3->start_y = 777;
    ui_g_MOTOR_WH3->width = 1;
    ui_g_MOTOR_WH3->font_size = 14;
    ui_g_MOTOR_WH3->str_length = 3;
    strcpy(ui_g_MOTOR_WH3->string, "WH3");


    ui_proc_string_frame(&ui_g_MOTOR_4);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_4, sizeof(ui_g_MOTOR_4));
}

void _ui_update_g_MOTOR_4() {
    ui_g_MOTOR_4.option.operate_type = 2;

    ui_proc_string_frame(&ui_g_MOTOR_4);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_4, sizeof(ui_g_MOTOR_4));
}

void _ui_remove_g_MOTOR_4() {
    ui_g_MOTOR_4.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_MOTOR_4);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_4, sizeof(ui_g_MOTOR_4));
}
ui_string_frame_t ui_g_MOTOR_5;
ui_interface_string_t* ui_g_MOTOR_WH4 = &(ui_g_MOTOR_5.option);

void _ui_init_g_MOTOR_5() {
    ui_g_MOTOR_5.option.figure_name[0] = 0;
    ui_g_MOTOR_5.option.figure_name[1] = 3;
    ui_g_MOTOR_5.option.figure_name[2] = 5;
    ui_g_MOTOR_5.option.operate_type = 1;

    ui_g_MOTOR_WH4->figure_type = 7;
    ui_g_MOTOR_WH4->operate_type = 1;
    ui_g_MOTOR_WH4->layer = 0;
    ui_g_MOTOR_WH4->color = Online.Cwhe4;
    ui_g_MOTOR_WH4->start_x = 7;
    ui_g_MOTOR_WH4->start_y = 756;
    ui_g_MOTOR_WH4->width = 1;
    ui_g_MOTOR_WH4->font_size = 14;
    ui_g_MOTOR_WH4->str_length = 3;
    strcpy(ui_g_MOTOR_WH4->string, "WH4");


    ui_proc_string_frame(&ui_g_MOTOR_5);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_5, sizeof(ui_g_MOTOR_5));
}

void _ui_update_g_MOTOR_5() {
    ui_g_MOTOR_5.option.operate_type = 2;

    ui_proc_string_frame(&ui_g_MOTOR_5);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_5, sizeof(ui_g_MOTOR_5));
}

void _ui_remove_g_MOTOR_5() {
    ui_g_MOTOR_5.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_MOTOR_5);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_5, sizeof(ui_g_MOTOR_5));
}
ui_string_frame_t ui_g_MOTOR_6;
ui_interface_string_t* ui_g_MOTOR_SHT = &(ui_g_MOTOR_6.option);

void _ui_init_g_MOTOR_6() {
    ui_g_MOTOR_6.option.figure_name[0] = 0;
    ui_g_MOTOR_6.option.figure_name[1] = 3;
    ui_g_MOTOR_6.option.figure_name[2] = 6;
    ui_g_MOTOR_6.option.operate_type = 1;

    ui_g_MOTOR_SHT->figure_type = 7;
    ui_g_MOTOR_SHT->operate_type = 1;
    ui_g_MOTOR_SHT->layer = 0;
    ui_g_MOTOR_SHT->color = 8;
    ui_g_MOTOR_SHT->start_x = 7;
    ui_g_MOTOR_SHT->start_y = 731;
    ui_g_MOTOR_SHT->width = 1;
    ui_g_MOTOR_SHT->font_size = 14;
    ui_g_MOTOR_SHT->str_length = 3;
    strcpy(ui_g_MOTOR_SHT->string, "SHT");


    ui_proc_string_frame(&ui_g_MOTOR_6);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_6, sizeof(ui_g_MOTOR_6));
}

void _ui_update_g_MOTOR_6() {
    ui_g_MOTOR_6.option.operate_type = 2;

    ui_proc_string_frame(&ui_g_MOTOR_6);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_6, sizeof(ui_g_MOTOR_6));
}

void _ui_remove_g_MOTOR_6() {
    ui_g_MOTOR_6.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_MOTOR_6);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_6, sizeof(ui_g_MOTOR_6));
}
ui_string_frame_t ui_g_MOTOR_7;
ui_interface_string_t* ui_g_MOTOR_CAP = &(ui_g_MOTOR_7.option);

void _ui_init_g_MOTOR_7() {
    ui_g_MOTOR_7.option.figure_name[0] = 0;
    ui_g_MOTOR_7.option.figure_name[1] = 3;
    ui_g_MOTOR_7.option.figure_name[2] = 7;
    ui_g_MOTOR_7.option.operate_type = 1;

    ui_g_MOTOR_CAP->figure_type = 7;
    ui_g_MOTOR_CAP->operate_type = 1;
    ui_g_MOTOR_CAP->layer = 0;
    ui_g_MOTOR_CAP->color = Online.Ccapcity;
    ui_g_MOTOR_CAP->start_x = 7;
    ui_g_MOTOR_CAP->start_y = 708;
    ui_g_MOTOR_CAP->width = 1;
    ui_g_MOTOR_CAP->font_size = 14;
    ui_g_MOTOR_CAP->str_length = 3;
    strcpy(ui_g_MOTOR_CAP->string, "CAP");


    ui_proc_string_frame(&ui_g_MOTOR_7);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_7, sizeof(ui_g_MOTOR_7));
}

void _ui_update_g_MOTOR_7() {
    ui_g_MOTOR_7.option.operate_type = 2;

    ui_proc_string_frame(&ui_g_MOTOR_7);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_7, sizeof(ui_g_MOTOR_7));
}

void _ui_remove_g_MOTOR_7() {
    ui_g_MOTOR_7.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_MOTOR_7);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_7, sizeof(ui_g_MOTOR_7));
}
ui_string_frame_t ui_g_MOTOR_8;
ui_interface_string_t* ui_g_MOTOR_C = &(ui_g_MOTOR_8.option);

void _ui_init_g_MOTOR_8() {
    ui_g_MOTOR_8.option.figure_name[0] = 0;
    ui_g_MOTOR_8.option.figure_name[1] = 3;
    ui_g_MOTOR_8.option.figure_name[2] = 8;
    ui_g_MOTOR_8.option.operate_type = 1;

    ui_g_MOTOR_C->figure_type = 7;
    ui_g_MOTOR_C->operate_type = 1;
    ui_g_MOTOR_C->layer = 0;
    ui_g_MOTOR_C->color = 8;
    ui_g_MOTOR_C->start_x = 6;
    ui_g_MOTOR_C->start_y = 684;
    ui_g_MOTOR_C->width = 1;
    ui_g_MOTOR_C->font_size = 14;
    ui_g_MOTOR_C->str_length = 1;
    strcpy(ui_g_MOTOR_C->string, "C");


    ui_proc_string_frame(&ui_g_MOTOR_8);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_8, sizeof(ui_g_MOTOR_8));
}

void _ui_update_g_MOTOR_8() {
    ui_g_MOTOR_8.option.operate_type = 2;

    ui_proc_string_frame(&ui_g_MOTOR_8);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_8, sizeof(ui_g_MOTOR_8));
}

void _ui_remove_g_MOTOR_8() {
    ui_g_MOTOR_8.option.operate_type = 3;

    ui_proc_string_frame(&ui_g_MOTOR_8);
    SEND_MESSAGE((uint8_t *) &ui_g_MOTOR_8, sizeof(ui_g_MOTOR_8));
}

void ui_init_g_MOTOR() {
    _ui_init_g_MOTOR_0();
    _ui_init_g_MOTOR_1();
    _ui_init_g_MOTOR_2();
    _ui_init_g_MOTOR_3();
    _ui_init_g_MOTOR_4();
    _ui_init_g_MOTOR_5();
    _ui_init_g_MOTOR_6();
    _ui_init_g_MOTOR_7();
    _ui_init_g_MOTOR_8();
}

void ui_update_g_MOTOR() {
    _ui_update_g_MOTOR_0();
    _ui_update_g_MOTOR_1();
    _ui_update_g_MOTOR_2();
    _ui_update_g_MOTOR_3();
    _ui_update_g_MOTOR_4();
    _ui_update_g_MOTOR_5();
    _ui_update_g_MOTOR_6();
    _ui_update_g_MOTOR_7();
    _ui_update_g_MOTOR_8();
}

void ui_remove_g_MOTOR() {
    _ui_remove_g_MOTOR_0();
    _ui_remove_g_MOTOR_1();
    _ui_remove_g_MOTOR_2();
    _ui_remove_g_MOTOR_3();
    _ui_remove_g_MOTOR_4();
    _ui_remove_g_MOTOR_5();
    _ui_remove_g_MOTOR_6();
    _ui_remove_g_MOTOR_7();
    _ui_remove_g_MOTOR_8();
}
