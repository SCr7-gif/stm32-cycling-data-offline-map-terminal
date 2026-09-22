#ifndef __UI__
#define __UI__

#include "lvgl.h"
#include "stdio.h"
#include "ui_conf.h"
#include "ui_general_tab.h"
#include "ui_setting_tab.h"
#include "ui_gpx_tab.h"
#include "ui_translation.h"

// 表示状态栏中的运行状态内容
static const char *run_status_text[] = {
    "Stopped",
    "Running",
};


/**
 * @brief 创建ui
 *
 */
void ui_create(void);


/**
 * @brief 更新电量
 * 
 * @param ui 
 * @param isCharging  是否在充电
 * @param percentage  电量百分比
 */
void ui_update_battery(ui_t *ui, bool isCharging, uint8_t percentage);

#endif // __UI__
