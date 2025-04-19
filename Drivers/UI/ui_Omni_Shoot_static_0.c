//
// Created by RM UI Designer
//

#include "ui_Omni_Shoot_static_0.h"

#define FRAME_ID 1
#define GROUP_ID 2
#define START_ID 0
#define OBJ_NUM 1
#define FRAME_OBJ_NUM 1

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_Omni_Shoot_static_0;
ui_interface_line_t *ui_Omni_Shoot_static_Shoot_Line = (ui_interface_line_t *)&(ui_Omni_Shoot_static_0.data[0]);

void _ui_init_Omni_Shoot_static_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_Omni_Shoot_static_0.data[i].figure_name[0] = FRAME_ID;
        ui_Omni_Shoot_static_0.data[i].figure_name[1] = GROUP_ID;
        ui_Omni_Shoot_static_0.data[i].figure_name[2] = i + START_ID;
        ui_Omni_Shoot_static_0.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_Omni_Shoot_static_0.data[i].operate_tpyel = 0;
    }

    ui_Omni_Shoot_static_Shoot_Line->figure_tpye = 0;
    ui_Omni_Shoot_static_Shoot_Line->layer = 0;
    ui_Omni_Shoot_static_Shoot_Line->start_x = 960;
    ui_Omni_Shoot_static_Shoot_Line->start_y = 541;
    ui_Omni_Shoot_static_Shoot_Line->end_x = 960;
    ui_Omni_Shoot_static_Shoot_Line->end_y = 400;
    ui_Omni_Shoot_static_Shoot_Line->color = 0;
    ui_Omni_Shoot_static_Shoot_Line->width = 2;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_Omni_Shoot_static_0);
    SEND_MESSAGE((uint8_t *) &ui_Omni_Shoot_static_0, sizeof(ui_Omni_Shoot_static_0));
}

void _ui_update_Omni_Shoot_static_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_Omni_Shoot_static_0.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_Omni_Shoot_static_0);
    SEND_MESSAGE((uint8_t *) &ui_Omni_Shoot_static_0, sizeof(ui_Omni_Shoot_static_0));
}

void _ui_remove_Omni_Shoot_static_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_Omni_Shoot_static_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_Omni_Shoot_static_0);
    SEND_MESSAGE((uint8_t *) &ui_Omni_Shoot_static_0, sizeof(ui_Omni_Shoot_static_0));
}
