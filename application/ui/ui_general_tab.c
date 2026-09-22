#include "ui_general_tab.h"


static void ui_general_container_init(lv_obj_t *parent)
{
    // 设置边框
    lv_obj_set_style_border_width(parent, 5, LV_PART_MAIN);
    lv_obj_set_style_border_color(parent, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
    // 设置圆角
    lv_obj_set_style_radius(parent, 10, LV_PART_MAIN);
    // 设置内边距
    lv_obj_set_style_pad_all(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(parent, 0, LV_PART_MAIN);
}


void ui_general_tab_init(lv_obj_t *parent,ui_t *ui)
{
    // 1. 设置网格布局
    lv_obj_set_layout(parent, LV_LAYOUT_GRID);

    // 2. 设置网格布局的行列数
    static int32_t column_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, column_dsc, row_dsc);

    // 对table格式设置
    // 1. 去除圆角边框
    lv_obj_set_style_radius(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(parent, 0, LV_PART_MAIN);
    // 2. 设置边距
    lv_obj_set_style_pad_all(parent, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_row(parent, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_column(parent, 10, LV_PART_MAIN);

    // 3.1 填充速度空间
    lv_obj_t *speed_obj = lv_obj_create(parent);
    // 放置到格子中
    lv_obj_set_grid_cell(speed_obj, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 0, 1);
    // 设置容器样式
    ui_general_container_init(speed_obj);

    // 3.2 填充速度值
    lv_obj_t *speed_label = lv_label_create(speed_obj);
    lv_label_bind_text(speed_label, &ui->data.speed, "%0.1f km/h");
    // 字体设置
    lv_obj_set_style_text_font(speed_label, &lv_font_montserrat_30, LV_PART_MAIN);
    // 对齐方式
    lv_obj_align(speed_label, LV_ALIGN_CENTER, 0, 0);

    // 4.1 填充距离空间
    lv_obj_t *distance_obj = lv_obj_create(parent);
    // 放置到格子中
    lv_obj_set_grid_cell(distance_obj, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    // 设置容器样式
    ui_general_container_init(distance_obj);
    // 4.2 填充距离值
    lv_obj_t *distance_label = lv_label_create(distance_obj);
    lv_label_bind_text(distance_label, &ui->data.distance, "%0.2f km");
    // 字体设置
    lv_obj_set_style_text_font(distance_label, &lv_font_montserrat_16, LV_PART_MAIN);
    // 对齐方式
    lv_obj_align(distance_label, LV_ALIGN_CENTER, 0, 0);

    // 5.1 填充坡度空间
    lv_obj_t *slope_obj = lv_obj_create(parent);
    // 放置到格子中
    lv_obj_set_grid_cell(slope_obj, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    // 设置容器样式
    ui_general_container_init(slope_obj);
    // 5.2 填充坡度值
    lv_obj_t *slope_label = lv_label_create(slope_obj);
    lv_label_bind_text(slope_label, &ui->data.slope, "%0.1f %%");
    // 字体设置
    lv_obj_set_style_text_font(slope_label, &lv_font_montserrat_16, LV_PART_MAIN);
    // 对齐方式
    lv_obj_align(slope_label, LV_ALIGN_CENTER, 0, 0);

    // 6.1 填充经纬度空间
    lv_obj_t *lat_lon_obj = lv_obj_create(parent);
    // 放置到格子中
    lv_obj_set_grid_cell(lat_lon_obj, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
    // 设置容器样式
    ui_general_container_init(lat_lon_obj);
    // 6.2 填充经纬度值
    // 6.2.1 维度值
    lv_obj_t *lat_label = lv_label_create(lat_lon_obj);
    lv_label_bind_text(lat_label, &ui->data.latitude, "%0.5f");
    // 字体设置
    lv_obj_set_style_text_font(lat_label, &lv_font_montserrat_16, LV_PART_MAIN);
    // 对齐方式
    lv_obj_align(lat_label, LV_ALIGN_CENTER, 0, -10);
    // 6.2.2 经度值
    lv_obj_t *lon_label = lv_label_create(lat_lon_obj);
    lv_label_bind_text(lon_label, &ui->data.longitude, "%0.5f");
    // 字体设置
    lv_obj_set_style_text_font(lon_label, &lv_font_montserrat_16, LV_PART_MAIN);
    // 对齐方式
    lv_obj_align(lon_label, LV_ALIGN_CENTER, 0, 10);

    // 7.1 填充时间空间
    lv_obj_t *time_obj = lv_obj_create(parent);
    // 放置到格子中
    lv_obj_set_grid_cell(time_obj, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
    // 设置容器样式
    ui_general_container_init(time_obj);
    // 7.2 填充时间值
    // 7.2.1 小时
    lv_obj_t *hour_label = lv_label_create(time_obj);
    lv_label_bind_text(hour_label, &ui->data.hour, "%02d");
    // 设置字体
    lv_obj_set_style_text_font(hour_label, &lv_font_montserrat_16, LV_PART_MAIN);
    // 设置对齐
    lv_obj_align(hour_label, LV_ALIGN_CENTER, -14, 0);
    // 7.2.2 分钟
    lv_obj_t *min_label = lv_label_create(time_obj);
    lv_label_bind_text(min_label, &ui->data.minute, ": %02d");
    // 设置字体
    lv_obj_set_style_text_font(min_label, &lv_font_montserrat_16, LV_PART_MAIN);
    // 设置对齐
    lv_obj_align(min_label, LV_ALIGN_CENTER, 10, 0);
}
