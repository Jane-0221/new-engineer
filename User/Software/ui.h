//
// Created by RM UI Designer
//

#ifndef UI_H
#define UI_H
#ifdef __cplusplus
extern "C" {
#endif

#include "ui_interface.h"

#include "ui_Omni_Shoot_Low_0.h"

#define ui_init_Omni_Shoot_Low() \
_ui_init_Omni_Shoot_Low_0()

#define ui_update_Omni_Shoot_Low() \
_ui_update_Omni_Shoot_Low_0()

#define ui_remove_Omni_Shoot_Low() \
_ui_remove_Omni_Shoot_Low_0()
    

#include "ui_Omni_Chassis_high_0.h"

#define ui_init_Omni_Chassis_high() \
_ui_init_Omni_Chassis_high_0()

#define ui_update_Omni_Chassis_high() \
_ui_update_Omni_Chassis_high_0()

#define ui_remove_Omni_Chassis_high() \
_ui_remove_Omni_Chassis_high_0()
    

#include "ui_Omni_Shoot_static_0.h"

#define ui_init_Omni_Shoot_static() \
_ui_init_Omni_Shoot_static_0()

#define ui_update_Omni_Shoot_static() \
_ui_update_Omni_Shoot_static_0()

#define ui_remove_Omni_Shoot_static() \
_ui_remove_Omni_Shoot_static_0()
    


#ifdef __cplusplus
}
#endif

void ui_init();
void ui_updata();
void Supercapui_change(float votage);
void Shootui_change();
void Chassisui_change(float angle);
void Autoui_change();

#endif //UI_H
