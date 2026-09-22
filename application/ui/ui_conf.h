#ifndef __UI_CONF_H
#define __UI_CONF_H

#include "lvgl.h"

typedef struct ui_data
{
    // 主题类型
    // 卫星数
    lv_subject_t gps_sat_num;
    // 运动状态
    lv_subject_t run_status;
    // 电量
    lv_subject_t battery_level;

    // general_tab数据
    // 速度
    lv_subject_t speed;
    // 距离
    lv_subject_t distance;
    // 坡度
    lv_subject_t slope;
    // 经纬度
    lv_subject_t latitude;
    lv_subject_t longitude;
    // 时间
    lv_subject_t hour;
    lv_subject_t minute;
} ui_data_t;

typedef struct map_args
{
    // 显示屏幕的大小
    int32_t screen_width;
    int32_t screen_height;
    // 缩放等级 => 需要和滑块绑定 需要是主题
    lv_subject_t zoom_level;
    uint8_t old_zoom;

    // 像素点坐标
    float pixel_x;
    float pixel_y;
    // 瓦片坐标
    int32_t tile_x;
    int32_t tile_y;

    // 中心点的参数
    lv_obj_t * center_point;
    float center_x;
    float center_y;
    int center_size;

}map_args_t;

typedef struct ui
{
    // 保存页面里面所有的数据内容
    ui_data_t data;

    // ui的主题 => 状态栏  1tableview
    lv_obj_t *tableview;
    lv_obj_t *status_bar;

    lv_obj_t *general_tab;
    lv_obj_t *gpx_tab;
    lv_obj_t *setting_tab;

    lv_obj_t *language_dropdown;


    // 保存地图相关计算参数
    lv_obj_t *map_container;
    map_args_t map_args;
} ui_t;

#endif
