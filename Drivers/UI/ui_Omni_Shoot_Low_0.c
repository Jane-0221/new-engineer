//
// Created by RM UI Designer
//

#include "ui_Omni_Shoot_Low_0.h"

#define FRAME_ID 1
#define GROUP_ID 0
#define START_ID 0
#define OBJ_NUM 5
#define FRAME_OBJ_NUM 5

CAT(ui_, CAT(FRAME_OBJ_NUM, _frame_t)) ui_Omni_Shoot_Low_0;
ui_interface_rect_t *ui_Omni_Shoot_Low_Auto_rect = (ui_interface_rect_t *)&(ui_Omni_Shoot_Low_0.data[0]);
ui_interface_round_t *ui_Omni_Shoot_Low_Shoot_Round = (ui_interface_round_t *)&(ui_Omni_Shoot_Low_0.data[1]);
ui_interface_round_t *ui_Omni_Shoot_Low_Shootend_Round = (ui_interface_round_t *)&(ui_Omni_Shoot_Low_0.data[2]);
ui_interface_round_t *ui_Omni_Shoot_Low_NewRound = (ui_interface_round_t *)&(ui_Omni_Shoot_Low_0.data[3]);
ui_interface_ellipse_t *ui_Omni_Shoot_Low_NewEllipse = (ui_interface_ellipse_t *)&(ui_Omni_Shoot_Low_0.data[4]);

void _ui_init_Omni_Shoot_Low_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_Omni_Shoot_Low_0.data[i].figure_name[0] = FRAME_ID;
        ui_Omni_Shoot_Low_0.data[i].figure_name[1] = GROUP_ID;
        ui_Omni_Shoot_Low_0.data[i].figure_name[2] = i + START_ID;
        ui_Omni_Shoot_Low_0.data[i].operate_tpyel = 1;
    }
    for (int i = OBJ_NUM; i < FRAME_OBJ_NUM; i++) {
        ui_Omni_Shoot_Low_0.data[i].operate_tpyel = 0;
    }

    ui_Omni_Shoot_Low_Auto_rect->figure_tpye = 1;
    ui_Omni_Shoot_Low_Auto_rect->layer = 0;
    ui_Omni_Shoot_Low_Auto_rect->start_x = 615;
    ui_Omni_Shoot_Low_Auto_rect->start_y = 246;
    ui_Omni_Shoot_Low_Auto_rect->color = 8;
    ui_Omni_Shoot_Low_Auto_rect->width = 5;
    ui_Omni_Shoot_Low_Auto_rect->end_x = 1247;
    ui_Omni_Shoot_Low_Auto_rect->end_y = 808;

    ui_Omni_Shoot_Low_Shoot_Round->figure_tpye = 2;
    ui_Omni_Shoot_Low_Shoot_Round->layer = 0;
    ui_Omni_Shoot_Low_Shoot_Round->r = 13;
    ui_Omni_Shoot_Low_Shoot_Round->start_x = 960;
    ui_Omni_Shoot_Low_Shoot_Round->start_y = 540;
    ui_Omni_Shoot_Low_Shoot_Round->color = 4;
    ui_Omni_Shoot_Low_Shoot_Round->width = 2;

    ui_Omni_Shoot_Low_Shootend_Round->figure_tpye = 2;
    ui_Omni_Shoot_Low_Shootend_Round->layer = 0;
    ui_Omni_Shoot_Low_Shootend_Round->r = 15;
    ui_Omni_Shoot_Low_Shootend_Round->start_x = 920;
    ui_Omni_Shoot_Low_Shootend_Round->start_y = 460;
    ui_Omni_Shoot_Low_Shootend_Round->color = 0;
    ui_Omni_Shoot_Low_Shootend_Round->width = 3;

    ui_Omni_Shoot_Low_NewRound->figure_tpye = 2;
    ui_Omni_Shoot_Low_NewRound->layer = 0;
    ui_Omni_Shoot_Low_NewRound->r = 76;
    ui_Omni_Shoot_Low_NewRound->start_x = 126;
    ui_Omni_Shoot_Low_NewRound->start_y = 126;
    ui_Omni_Shoot_Low_NewRound->color = 0;
    ui_Omni_Shoot_Low_NewRound->width = 1;

    ui_Omni_Shoot_Low_NewEllipse->figure_tpye = 3;
    ui_Omni_Shoot_Low_NewEllipse->layer = 0;
    ui_Omni_Shoot_Low_NewEllipse->rx = 50;
    ui_Omni_Shoot_Low_NewEllipse->ry = 50;
    ui_Omni_Shoot_Low_NewEllipse->start_x = 100;
    ui_Omni_Shoot_Low_NewEllipse->start_y = 100;
    ui_Omni_Shoot_Low_NewEllipse->color = 0;
    ui_Omni_Shoot_Low_NewEllipse->width = 1;


    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_Omni_Shoot_Low_0);
    SEND_MESSAGE((uint8_t *) &ui_Omni_Shoot_Low_0, sizeof(ui_Omni_Shoot_Low_0));
}

void _ui_update_Omni_Shoot_Low_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_Omni_Shoot_Low_0.data[i].operate_tpyel = 2;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_Omni_Shoot_Low_0);
    SEND_MESSAGE((uint8_t *) &ui_Omni_Shoot_Low_0, sizeof(ui_Omni_Shoot_Low_0));
}

void _ui_remove_Omni_Shoot_Low_0() {
    for (int i = 0; i < OBJ_NUM; i++) {
        ui_Omni_Shoot_Low_0.data[i].operate_tpyel = 3;
    }

    CAT(ui_proc_, CAT(FRAME_OBJ_NUM, _frame))(&ui_Omni_Shoot_Low_0);
    SEND_MESSAGE((uint8_t *) &ui_Omni_Shoot_Low_0, sizeof(ui_Omni_Shoot_Low_0));
}
