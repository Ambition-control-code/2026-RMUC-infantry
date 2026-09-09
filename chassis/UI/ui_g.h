//
// Created by RM UI Designer
// Static Edition
//

#ifndef UI_g_H
#define UI_g_H

#include "ui_interface.h"

extern ui_interface_round_t *ui_g_INFORMATION_ZX;
extern ui_interface_rect_t *ui_g_INFORMATION_CAPCITY;
extern ui_interface_number_t *ui_g_INFORMATION_MCLSP;
extern ui_interface_number_t *ui_g_INFORMATION_FDL;
extern ui_interface_number_t *ui_g_INFORMATION_ZM_ON;
extern ui_interface_string_t *ui_g_INFORMATION_ZM;
extern ui_interface_string_t *ui_g_INFORMATION_LEVEL;

void ui_init_g_INFORMATION();
void ui_update_g_INFORMATION();
void ui_remove_g_INFORMATION();

extern ui_interface_number_t *ui_g_INFOTMATION_PITCHAN;
extern ui_interface_string_t *ui_g_INFOTMATION_MODE;
extern ui_interface_string_t *ui_g_INFOTMATION_COUNT;

void ui_init_g_INFOTMATION();
void ui_update_g_INFOTMATION();
void ui_remove_g_INFOTMATION();

extern ui_interface_rect_t *ui_g_MIAOBIAN_QUAN;
extern ui_interface_line_t *ui_g_MIAOBIAN_SB;
extern ui_interface_line_t *ui_g_MIAOBIAN_SB2;
extern ui_interface_round_t *ui_g_MIAOBIAN_SBROUND;
extern ui_interface_round_t *ui_g_MIAOBIAN_SBROUND2;
extern ui_interface_string_t *ui_g_MIAOBIAN_NL;

void ui_init_g_MIAOBIAN();
void ui_update_g_MIAOBIAN();
void ui_remove_g_MIAOBIAN();

extern ui_interface_string_t *ui_g_MOTOR_YAW;
extern ui_interface_string_t *ui_g_MOTOR_PIT2;
extern ui_interface_string_t *ui_g_MOTOR_WH1;
extern ui_interface_string_t *ui_g_MOTOR_WH2;
extern ui_interface_string_t *ui_g_MOTOR_WH3;
extern ui_interface_string_t *ui_g_MOTOR_WH4;
extern ui_interface_string_t *ui_g_MOTOR_SHT;
extern ui_interface_string_t *ui_g_MOTOR_CAP;
extern ui_interface_string_t *ui_g_MOTOR_C;

void ui_init_g_MOTOR();
void ui_update_g_MOTOR();
void ui_remove_g_MOTOR();


#endif // UI_g_H
