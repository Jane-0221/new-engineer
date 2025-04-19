#include "ui.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "Global_status.h"
#include "supercup.h"

void ui_init()
{
  ui_init_Omni_Shoot_Low();
  osDelay(20);
  ui_init_Omni_Chassis_high();
  osDelay(20);
  ui_remove_Omni_Shoot_static();
  osDelay(20);
}

void ui_updata()
{
  ui_update_Omni_Chassis_high();
  osDelay(5);
  ui_update_Omni_Shoot_Low();
  osDelay(5);
}

void Supercapui_change(float votage)
{
  float cnt = votage * votage;
  float percent = (cnt - 49.0f) / 480.0f;
  if (percent >= 1)
    percent = 1;
  if (percent <= 0)
    percent = 0;
  if (percent >= 0.6)
    ui_Omni_Chassis_high_Supercap_Line->color = 2; // 绿色
  if (percent <= 0.6)
    ui_Omni_Chassis_high_Supercap_Line->color = 3; // 橙色
  if (percent <= 0.3)
    ui_Omni_Chassis_high_Supercap_Line->color = 4; // 紫红色
  ui_Omni_Chassis_high_Supercap_Line->end_x = ui_Omni_Chassis_high_Supercap_Line->start_x + 980.0f * percent;

  if (Global.Cap.mode != FULL)
  {
    ui_Omni_Chassis_high_Supercap_rect->color = 6;
    ui_Omni_Chassis_high_Supercap_rect->width = 1;
  }
  else
  {
    ui_Omni_Chassis_high_Supercap_rect->color = 1;
    ui_Omni_Chassis_high_Supercap_rect->width = 8;
  }
}

void Shootui_change()
{
  // 摩擦轮
  if (Global.Shoot.shoot_status == OK)
  {
    ui_Omni_Shoot_Low_Shoot_Round->color = 2; // 开启绿色
    ui_Omni_Shoot_Low_Shootend_Round->color = 2;
  }
  else
  {
    ui_Omni_Shoot_Low_Shoot_Round->color = 4; // 关闭紫红色
    ui_Omni_Shoot_Low_Shootend_Round->color = 2;
  }
}

void Chassisui_change(float angle)
{
  float angle_start, angle_end;
  if (angle > 360)
    angle -= 360;
  angle_start = angle - 20.0f;
  angle_end = angle + 20.0f;
  if (angle_start <= 0.0f)
    angle_start += 360.0f;
  if (angle_end <= 0.0f)
    angle_end += 360.0f;
  if (angle_end >= 360.0f)
    angle_end -= 360.0f;
  if (angle_start >= 360.0f)
    angle_start -= 360.0f;
  ui_Omni_Chassis_high_Chassis_arc->start_angle = angle_start;
  ui_Omni_Chassis_high_Chassis_arc->end_angle = angle_end;
  if (Global.Chssis.mode == FLOW)
    ui_Omni_Chassis_high_Chassis_arc->color = 1; // 非小陀螺黄色
  else if (Global.Chssis.mode == SPIN_P)
    ui_Omni_Chassis_high_Chassis_arc->color = 2; // 小陀螺绿色
  else
    ui_Omni_Chassis_high_Chassis_arc->color = 1; // 默认黄色
}

void Autoui_change(void)
{
  static uint8_t auto_online;
  switch (Global.Auto.input.fire) // 自瞄圈
  {
  case 0:
    ui_Omni_Shoot_Low_Auto_rect->color = 4; // 紫红色
    break;
  case 1:
    ui_Omni_Shoot_Low_Auto_rect->color = 2; // 绿色
    break;
  default:
    ui_Omni_Shoot_Low_Auto_rect->color = 4; // 紫红色
    break;
  }
  if (Global.Auto.input.Auto_control_online == 1)
    auto_online = 10;
  if (Global.Auto.input.Auto_control_online == 0 && auto_online == 0)
  {
    ui_Omni_Shoot_Low_Auto_rect->color = 7; // 掉线黑色
  }
  auto_online--;
}
