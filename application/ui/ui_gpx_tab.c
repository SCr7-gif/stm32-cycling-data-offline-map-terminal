#include "ui_gpx_tab.h"

uint8_t map_buff[128];

uint8_t code_scroll_flag = 0;
uint8_t code_slider_flag = 0;
static void ui_map_container_init(lv_obj_t *parent)
{
    lv_obj_set_style_border_width(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(parent, 0, LV_PART_MAIN);
}

static void ui_map_content_container_init(lv_obj_t *parent)
{
    lv_obj_set_style_border_width(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(parent, 0, LV_PART_MAIN);

    lv_obj_set_style_bg_color(parent, lv_palette_lighten(LV_PALETTE_GREY, 2), LV_PART_MAIN);
}

static void ui_map_center_point_move(ui_t *ui)
{
    // 第一个刷写屏幕不需要考虑中心点
    if (ui->map_args.center_x != 0 && ui->map_args.center_y != 0)
    {

        // 重新确定位置
        int32_t x = ui->map_args.center_x - ui->map_args.tile_x * 256 + 256;
        int32_t y = ui->map_args.center_y - ui->map_args.tile_y * 256 + 256;

        lv_obj_set_pos(ui->map_args.center_point, x - ui->map_args.center_size / 2, y - ui->map_args.center_size / 2);

        // 获取中心点容器
        lv_obj_t *center_container = lv_obj_get_parent(ui->map_args.center_point);
        // 移动到中心点容器的最前面 => 需要移动的对象和地图图片一个层级
        lv_obj_move_foreground(center_container);
    }
}
/**
 * @brief 核心方法 => 根据经纬度计算出像素点坐标
 *   使用像素点坐标铺图  并且移动到中心为止
 *
 * @param map_container
 * @param ui
 */
static void ui_map_flush_img(lv_obj_t *map_container, ui_t *ui)
{
    // 3. 像素点坐标计算出瓦片的XY编号
    int tile_x, tile_y;
    pixel_to_tile_float(ui->map_args.pixel_x, ui->map_args.pixel_y, &tile_x, &tile_y);

    // 添加判断 => 性能提升
    // 如果滚动的距离比较小 => 不需要重新刷写的
    if (ui->map_args.tile_x != tile_x || ui->map_args.tile_y != tile_y)
    {
        ui->map_args.tile_x = tile_x;
        ui->map_args.tile_y = tile_y;
        // 需要重新刷写
        // 4. 铺地图
        for (uint8_t i = 0; i < 9; i++)
        {
            // 创建地图
            lv_obj_t *map_img = lv_image_create(map_container);
            // 设置读取的图片
            // lv_image_set_src(map_img, "C:/14/13372/7134/tile.bmp");
            lv_memzero(map_buff, 128);
            sprintf(map_buff, "C:/map/%d/%d/%d/tile.bmp", lv_subject_get_int(&ui->map_args.zoom_level), ui->map_args.tile_x + (i % 3) - 1, ui->map_args.tile_y + (i / 3) - 1);
            // printf("%s\n", map_buff);
            lv_image_set_src(map_img, map_buff);
            // 放置到网格
            lv_obj_set_grid_cell(map_img, LV_GRID_ALIGN_STRETCH, i % 3, 1, LV_GRID_ALIGN_STRETCH, i / 3, 1);
        }

        ui_map_center_point_move(ui);

        code_scroll_flag = 1;
        // 5. 滚动到中间图片
        lv_obj_scroll_to_x(map_container, 256 + ((uint32_t)(ui->map_args.pixel_x) % 256) - ui->map_args.screen_width / 2, LV_ANIM_OFF);
        lv_obj_scroll_to_y(map_container, 256 + ((uint32_t)(ui->map_args.pixel_y) % 256) - ui->map_args.screen_height / 2, LV_ANIM_OFF);
        printf("scroll!!!!!!!!!!!!!!!!\n");
    }
}

static void ui_map_center_point_init(lv_obj_t *map_container, ui_t *ui)
{
    // map_container有网格布局  不能直接移动obj中心点
    lv_obj_t *track_container = lv_obj_create(map_container);
    ui_map_container_init(track_container);
    lv_obj_set_pos(track_container, 0, 0);
    lv_obj_set_size(track_container, TILE_SIZE * 3, TILE_SIZE * 3); // 和整个地图区域一样大
    // 去除背景色 => 设置背景为透明
    lv_obj_set_style_bg_opa(track_container, LV_OPA_TRANSP, 0);
    lv_obj_remove_flag(track_container, LV_OBJ_FLAG_SCROLLABLE);

    // 添加中心点
    lv_obj_t *center_point = lv_obj_create(track_container);
    ui->map_args.center_point = center_point;
    // 常量值
    ui->map_args.center_size = 16;
    lv_obj_set_size(center_point, ui->map_args.center_size, ui->map_args.center_size);
    lv_obj_set_style_bg_color(center_point, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
    // 不是使用像素点坐标计算位置  中心点坐标的位置
    ui->map_args.center_x = ui->map_args.pixel_x;
    ui->map_args.center_y = ui->map_args.pixel_y;

    lv_obj_set_pos(center_point, 256 + (uint32_t)(ui->map_args.center_x) % 256 - ui->map_args.center_size / 2, 256 + (uint32_t)(ui->map_args.center_y) % 256 - ui->map_args.center_size / 2);
}

void ui_map_scroll_cb(lv_event_t *e)
{
    if (code_scroll_flag == 1)
    {
        code_scroll_flag = 0;
        return;
    }

    lv_obj_t *map_container = lv_event_get_target(e);
    // 获取user_data
    ui_t *ui = lv_event_get_user_data(e);

    // 1. 判断滚动的方向和距离
    int old_x = 256 + ((uint32_t)(ui->map_args.pixel_x) % 256) - ui->map_args.screen_width / 2;
    int old_y = 256 + ((uint32_t)(ui->map_args.pixel_y) % 256) - ui->map_args.screen_height / 2;

    int new_x = lv_obj_get_scroll_x(map_container);
    int new_y = lv_obj_get_scroll_y(map_container);
    // printf("old_x: %d, old_y: %d, new_x: %d, new_y: %d\n", old_x, old_y, new_x, new_y);

    // 使用中心点的坐标 来判断移动
    ui->map_args.pixel_x += (new_x - old_x);
    ui->map_args.pixel_y += (new_y - old_y);

    // printf("pixel_x: %f, pixel_y: %f\n", ui->map_args.pixel_x, ui->map_args.pixel_y);

    // 1. 统一的问题解决 => 使用中心点像素XY坐标 => 计算需要铺的9张图
    // 2. 移动到中心点为止
    ui_map_flush_img(map_container, ui);
}

static void ui_map_init(lv_obj_t *map_obj, ui_t *ui)
{
    // 填充9张图片 => 需要单独的图层
    lv_obj_t *map_container = lv_obj_create(map_obj);

    // 保存到ui
    ui->map_container = map_container;

    lv_obj_set_size(map_container, lv_pct(100), lv_pct(100));
    ui_map_container_init(map_container);

    // 去除弹性滚动
    lv_obj_remove_flag(map_container, LV_OBJ_FLAG_SCROLL_ELASTIC);
    // 去除链式滚动
    lv_obj_remove_flag(map_container, LV_OBJ_FLAG_SCROLL_CHAIN);
    // 滚动条隐藏显示
    lv_obj_set_scrollbar_mode(map_container, LV_SCROLLBAR_MODE_OFF);
    // 关闭滚动动量
    lv_obj_remove_flag(map_container, LV_OBJ_FLAG_SCROLL_MOMENTUM);

    // 显示屏的大小 只影响看的效果和中心点  => 不影响铺9张地图
    // 获取显示屏幕的大小
    lv_obj_update_layout(map_container);
    ui->map_args.screen_width = lv_obj_get_width(map_container);
    ui->map_args.screen_height = lv_obj_get_height(map_container);
    // printf("width: %d, height: %d\n", width, height);

    // 1. 设置9张地图的网格
    lv_obj_set_layout(map_container, LV_LAYOUT_GRID);
    // 网格大小一定和图片大小对应
    static int32_t column_dsc[] = {TILE_SIZE, TILE_SIZE, TILE_SIZE, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {TILE_SIZE, TILE_SIZE, TILE_SIZE, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(map_container, column_dsc, row_dsc);

    // 初始化缩放等级
    lv_subject_init_int(&ui->map_args.zoom_level, 14);
    // 2. 核心参数计算 => 使用经纬度计算像素点坐标
    latlng_to_pixel_float(lv_subject_get_float(&ui->data.longitude), lv_subject_get_float(&ui->data.latitude), lv_subject_get_int(&ui->map_args.zoom_level), &ui->map_args.pixel_x, &ui->map_args.pixel_y);
    printf("pixel_x: %f, pixel_y: %f\n", ui->map_args.pixel_x, ui->map_args.pixel_y);

    // 先刷写地图  后添加回调
    ui_map_flush_img(map_container, ui);

    // 添加地图的回调函数
    lv_obj_add_event_cb(map_container, ui_map_scroll_cb, LV_EVENT_SCROLL_END, ui);

    // 中心点的初始化
    ui_map_center_point_init(map_container, ui);
}

void ui_map_slider_cb(lv_event_t *e)
{
    if (code_slider_flag == 1)
    {
        code_slider_flag = 0;
        return;
    }

    ui_t *ui = lv_event_get_user_data(e);

    // 重新计算像素点坐标
    pixel_zoom_convert_float(ui->map_args.pixel_x, ui->map_args.pixel_y, ui->map_args.old_zoom, lv_subject_get_int((&ui->map_args.zoom_level)), &ui->map_args.pixel_x, &ui->map_args.pixel_y);

    // 重新计算中心点坐标
    pixel_zoom_convert_float(ui->map_args.center_x, ui->map_args.center_y, ui->map_args.old_zoom, lv_subject_get_int((&ui->map_args.zoom_level)), &ui->map_args.center_x, &ui->map_args.center_y);

    ui->map_args.old_zoom = lv_subject_get_int((&ui->map_args.zoom_level));

    ui_map_flush_img(ui->map_container, ui);
}

void ui_map_center_btn_cb(lv_event_t *e)
{
    ui_t *ui = lv_event_get_user_data(e);

    // 最初的缩放等级固定是14
    code_slider_flag = 1;
    lv_subject_set_int(&ui->map_args.zoom_level, 14);
    ui->map_args.old_zoom = 14;

    // 重新使用GPS坐标计算铺图
    latlng_to_pixel_float(lv_subject_get_float(&ui->data.longitude), lv_subject_get_float(&ui->data.latitude), lv_subject_get_int(&ui->map_args.zoom_level), &ui->map_args.pixel_x, &ui->map_args.pixel_y);

    // 重新赋值中心点坐标
    ui->map_args.center_x = ui->map_args.pixel_x;
    ui->map_args.center_y = ui->map_args.pixel_y;

    // 铺图
    ui_map_flush_img(ui->map_container, ui);
}

static void ui_map_slider_button_init(lv_obj_t *map_obj, ui_t *ui)
{
    // 滑块
    // 绑定数值 缩放等级
    lv_obj_t *slider_obj = lv_slider_create(map_obj);
    lv_obj_set_size(slider_obj, 15, LV_PCT(70));
    lv_obj_align(slider_obj, LV_ALIGN_RIGHT_MID, -10, 0);

    // 初始化缩放等级
    ui->map_args.old_zoom = 14;
    lv_subject_init_int(&ui->map_args.zoom_level, ui->map_args.old_zoom);
    lv_slider_set_range(slider_obj, 3, 14);
    // 绑定到主题
    lv_slider_bind_value(slider_obj, &ui->map_args.zoom_level);

    // 添加滑块的回调函数
    lv_obj_add_event_cb(slider_obj, ui_map_slider_cb, LV_EVENT_VALUE_CHANGED, ui);

    // 按钮
    lv_obj_t *center_btn = lv_button_create(map_obj);
    lv_obj_set_size(center_btn, 20, 20);
    lv_obj_align(center_btn, LV_ALIGN_BOTTOM_LEFT, 10, -10);

    // 添加按钮的图标
    lv_obj_t *center_btn_label = lv_label_create(center_btn);
    lv_label_set_text(center_btn_label, LV_SYMBOL_TINT);
    lv_obj_align(center_btn_label, LV_ALIGN_CENTER, 0, 0);

    // 添加按钮的回调函数
    lv_obj_add_event_cb(center_btn, ui_map_center_btn_cb, LV_EVENT_CLICKED, ui);
}
void ui_gpx_tab_init(lv_obj_t *parent, ui_t *ui)
{
    // 设置容器
    // 去除边框圆角
    lv_obj_set_style_border_width(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(parent, 0, LV_PART_MAIN);

    // 设置网格布局
    lv_obj_set_layout(parent, LV_LAYOUT_GRID);
    // 设置网格布局的行列数
    static int32_t column_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_FR(4), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(parent, column_dsc, row_dsc);

    // 1. 创建地图区域
    lv_obj_t *map_obj = lv_obj_create(parent);
    // 放置到格子中
    lv_obj_set_grid_cell(map_obj, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 0, 1);
    ui_map_container_init(map_obj);

    // TEST  填充颜色
    // lv_obj_set_style_bg_color(map_obj, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN);
    ui_map_init(map_obj, ui);

    // 添加功能组件  滑块  按钮 => 在map_obj的图层中
    ui_map_slider_button_init(map_obj, ui);

    // 2. 创建速度区域
    lv_obj_t *speed_obj = lv_obj_create(parent);
    // 放置到格子中
    lv_obj_set_grid_cell(speed_obj, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    // 初始化容器样式
    ui_map_content_container_init(speed_obj);

    // 2.1 填充速度值
    lv_obj_t *speed_label = lv_label_create(speed_obj);
    lv_label_bind_text(speed_label, &ui->data.speed, "%0.1f km/h");

    // 修改字体大小
    lv_obj_set_style_text_font(speed_label, &lv_font_montserrat_20, LV_PART_MAIN);

    // 修改居中
    lv_obj_align(speed_label, LV_ALIGN_CENTER, 0, 0);

    // 3. 创建距离区域
    lv_obj_t *distance_obj = lv_obj_create(parent);
    // 放置到格子中
    lv_obj_set_grid_cell(distance_obj, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    // 初始化容器格式
    ui_map_content_container_init(distance_obj);

    // 3.1 填充距离值
    lv_obj_t *distance_label = lv_label_create(distance_obj);
    lv_label_bind_text(distance_label, &ui->data.distance, "%0.2f km");
    // 修改字体大小
    lv_obj_set_style_text_font(distance_label, &lv_font_montserrat_20, LV_PART_MAIN);
    // 修改居中
    lv_obj_align(distance_label, LV_ALIGN_CENTER, 0, 0);
}
