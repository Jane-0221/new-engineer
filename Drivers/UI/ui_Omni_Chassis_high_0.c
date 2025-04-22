//
// Created by RM UI Designer
//

#include "ui_Omni_Chassis_high_0.h"

#define FRAME_ID 1
#define GROUP_ID 1
#define START_ID 0
#define OBJ_NUM 5
#define FRAME_OBJ_NUM 5

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_Omni_Chassis_high_0;
ui_interface_arc_t *ui_Omni_Chassis_high_Chassis_arc = (ui_interface_arc_t *)&(ui_Omni_Chassis_high_0.data[0]);
ui_interface_rect_t *ui_Omni_Chassis_high_Supercap_rect = (ui_interface_rect_t *)&(ui_Omni_Chassis_high_0.data[1]);
ui_interface_line_t *ui_Omni_Chassis_high_Supercap_Line = (ui_interface_line_t *)&(ui_Omni_Chassis_high_0.data[2]);
ui_interface_round_t *ui_Omni_Chassis_high_NewRound = (ui_interface_round_t *)&(ui_Omni_Chassis_high_0.data[3]);
ui_interface_ellipse_t *ui_Omni_Chassis_high_NewEllipse = (ui_interface_ellipse_t *)&(ui_Omni_Chassis_high_0.data[4]);

void _ui_init_Omni_Chassis_high_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_Omni_Chassis_high_0.data[i].figure_name[0] = FRAME_ID;
        ui_Omni_Chassis_high_0.data[i].figure_name[1] = GROUP_ID;
        ui_Omni_Chassis_high_0.data[i].figure_name[2] = i + START_ID;
        ui_Omni_Chassis_high_0.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_Omni_Chassis_high_0.data[i].operate_tpyel = 0;
    }

    ui_Omni_Chassis_high_Chassis_arc->figure_tpye = 4;
    ui_Omni_Chassis_high_Chassis_arc->layer = 0;
    ui_Omni_Chassis_high_Chassis_arc->rx = 330;
    ui_Omni_Chassis_high_Chassis_arc->ry = 330;
    ui_Omni_Chassis_high_Chassis_arc->start_x = 960;
    ui_Omni_Chassis_high_Chassis_arc->start_y = 540;
    ui_Omni_Chassis_high_Chassis_arc->color = 1;
    ui_Omni_Chassis_high_Chassis_arc->width = 5;
    ui_Omni_Chassis_high_Chassis_arc->start_angle = 150;
    ui_Omni_Chassis_high_Chassis_arc->end_angle = 210;

    ui_Omni_Chassis_high_Supercap_rect->figure_tpye = 1;
    ui_Omni_Chassis_high_Supercap_rect->layer = 0;
    ui_Omni_Chassis_high_Supercap_rect->start_x = 445;
    ui_Omni_Chassis_high_Supercap_rect->start_y = 159;
    ui_Omni_Chassis_high_Supercap_rect->color = 6;
    ui_Omni_Chassis_high_Supercap_rect->width = 3;
    ui_Omni_Chassis_high_Supercap_rect->end_x = 1436;
    ui_Omni_Chassis_high_Supercap_rect->end_y = 201;

    ui_Omni_Chassis_high_Supercap_Line->figure_tpye = 0;
    ui_Omni_Chassis_high_Supercap_Line->layer = 0;
    ui_Omni_Chassis_high_Supercap_Line->start_x = 450;
    ui_Omni_Chassis_high_Supercap_Line->start_y = 166;
    ui_Omni_Chassis_high_Supercap_Line->end_x = 1431;
    ui_Omni_Chassis_high_Supercap_Line->end_y = 166;
    ui_Omni_Chassis_high_Supercap_Line->color = 2;
    ui_Omni_Chassis_high_Supercap_Line->width = 30;

    ui_Omni_Chassis_high_NewRound->figure_tpye = 2;
    ui_Omni_Chassis_high_NewRound->layer = 0;
    ui_Omni_Chassis_high_NewRound->r = 76;
    ui_Omni_Chassis_high_NewRound->start_x = 126;
    ui_Omni_Chassis_high_NewRound->start_y = 126;
    ui_Omni_Chassis_high_NewRound->color = 0;
    ui_Omni_Chassis_high_NewRound->width = 1;

    ui_Omni_Chassis_high_NewEllipse->figure_tpye = 3;
    ui_Omni_Chassis_high_NewEllipse->layer = 0;
    ui_Omni_Chassis_high_NewEllipse->rx = 50;
    ui_Omni_Chassis_high_NewEllipse->ry = 50;
    ui_Omni_Chassis_high_NewEllipse->start_x = 100;
    ui_Omni_Chassis_high_NewEllipse->start_y = 100;
    ui_Omni_Chassis_high_NewEllipse->color = 0;
    ui_Omni_Chassis_high_NewEllipse->width = 1;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_Omni_Chassis_high_0);
    SEND_MESSAGE((uint8_t *) &ui_Omni_Chassis_high_0, sizeof(ui_Omni_Chassis_high_0));
}

void _ui_update_Omni_Chassis_high_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_Omni_Chassis_high_0.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_Omni_Chassis_high_0);
    SEND_MESSAGE((uint8_t *) &ui_Omni_Chassis_high_0, sizeof(ui_Omni_Chassis_high_0));
}

void _ui_remove_Omni_Chassis_high_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_Omni_Chassis_high_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_Omni_Chassis_high_0);
    SEND_MESSAGE((uint8_t *) &ui_Omni_Chassis_high_0, sizeof(ui_Omni_Chassis_high_0));
}
