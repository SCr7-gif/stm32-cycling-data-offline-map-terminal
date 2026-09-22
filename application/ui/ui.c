#include "ui.h"

ui_t *ui;


static void ui_container_style_init(lv_obj_t *obj)
{
    // 删除内边距
    lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(obj, 0, LV_PART_MAIN);
    // 删除边框和圆角
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
}

static lv_obj_t *init_screen(void)
{
    // 设置屏幕
    lv_obj_t *screen = lv_screen_active();
    // 弹性布局
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    // 删除内边距
    lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_row(screen, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(screen, 0, LV_PART_MAIN);
    return screen;
}

static void init_data(ui_t *ui)
{
    // 卫星数
    lv_subject_init_int(&ui->data.gps_sat_num, 0);
    // 运动状态
    lv_subject_init_pointer(&ui->data.run_status, (void *)run_status_text[0]);
    // 电量
    lv_subject_init_pointer(&ui->data.battery_level,(void *) LV_SYMBOL_CHARGE);

    // general_tab 数据
    lv_subject_init_float(&ui->data.speed, 0.0f);
    lv_subject_init_float(&ui->data.distance, 0.0f);
    lv_subject_init_float(&ui->data.slope, 0.0f);
    lv_subject_init_float(&ui->data.latitude, 22.628743f);
    lv_subject_init_float(&ui->data.longitude, 113.835402f);

    lv_subject_init_int(&ui->data.hour, 9);
    lv_subject_init_int(&ui->data.minute, 11);
}

static void init_status_bar(ui_t *ui, lv_obj_t *screen)
{
    //  1. 创建状态栏
    ui->status_bar = lv_obj_create(screen);
    lv_obj_set_width(ui->status_bar, LV_PCT(100));
    // 状态栏高度是1
    lv_obj_set_flex_grow(ui->status_bar, 1);
    ui_container_style_init(ui->status_bar);
    lv_obj_set_style_bg_color(ui->status_bar, lv_palette_lighten(LV_PALETTE_GREY, 2), LV_PART_MAIN);

    // 添加状态栏内容  gps卫星数   运动状态   电量
    // 1. gps卫星数
    lv_obj_t *gps_label = lv_label_create(ui->status_bar);
    lv_obj_set_style_text_font(gps_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_bind_text(gps_label, &ui->data.gps_sat_num, LV_SYMBOL_GPS " %d");
    // 左对齐
    lv_obj_align(gps_label, LV_ALIGN_TOP_LEFT, 10, 10);

    // 2. 运行状态
    lv_obj_t *run_label = lv_label_create(ui->status_bar);
    // 修改字体大小
    lv_obj_set_style_text_font(run_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_bind_text(run_label, &ui->data.run_status, "%s");
    // 中间对齐
    lv_obj_align(run_label, LV_ALIGN_TOP_MID, 0, 10);

    // 3. 电量
    lv_obj_t *battery_label = lv_label_create(ui->status_bar);
    lv_label_bind_text(battery_label, &ui->data.battery_level, "%s");
    lv_obj_set_style_text_font(battery_label, &lv_font_montserrat_14, LV_PART_MAIN);
    // 右对齐
    lv_obj_align(battery_label, LV_ALIGN_TOP_RIGHT, -10, 10);
}

static void init_table_view(ui_t *ui, lv_obj_t *screen)
{

    // 2. 创建tableview
    ui->tableview = lv_tabview_create(screen);

    // tableview 的宽度是屏幕的100%  高度是9:1
    lv_obj_set_width(ui->tableview, LV_PCT(100));
    lv_obj_set_flex_grow(ui->tableview, 9);
    // 设置tabview的tabbar大小为10%
    lv_tabview_set_tab_bar_size(ui->tableview, LV_PCT(10));
    lv_tabview_set_tab_bar_position(ui->tableview, LV_DIR_BOTTOM);
    ui_container_style_init(ui->tableview);

    lv_obj_t *gpx_table = lv_tabview_add_tab(ui->tableview, "GPX");
    lv_obj_t *general_table = lv_tabview_add_tab(ui->tableview, "General");
    lv_obj_t *setting_table = lv_tabview_add_tab(ui->tableview, "Setting");

    // 添加到ui中
    ui->gpx_tab = gpx_table;
    ui->general_tab = general_table;
    ui->setting_tab = setting_table;

    // 将table_bar中的文本添加到翻译中  => 添加文本标签  添加回调函数
    // 1. 获取table_bar
    lv_obj_t *table_bar = lv_tabview_get_tab_bar(ui->tableview);
    // 获取gpx_label
    lv_obj_t *gpx_label = lv_obj_get_child(lv_obj_get_child(table_bar, 0), 0);
    // lv_label_set_text(gpx_label, lv_tr(tags[0]));
    ui_translation_register_label(gpx_label, tags[0]);

    // 获取general_label
    lv_obj_t *general_label = lv_obj_get_child(lv_obj_get_child(table_bar, 1), 0);
    // lv_label_set_text(general_label, lv_tr(tags[1]));
    ui_translation_register_label(general_label, tags[1]);

    // 获取setting_label
    lv_obj_t *setting_label = lv_obj_get_child(lv_obj_get_child(table_bar, 2), 0);
    // lv_label_set_text(setting_label, lv_tr(tags[2]));
    ui_translation_register_label(setting_label, tags[2]);

    // 1. 初始化页面general_tab
    ui_general_tab_init(general_table, ui);

    // 2. 初始化设置页面setting_tab
    ui_setting_tab_init(setting_table, ui);

    // 3. 初始化gpx页面gpx_tab
    ui_gpx_tab_init(gpx_table, ui);
}

/**
 * @brief 创建ui
 *
 */
void ui_create(void)
{
    // 初始化UI
    ui = lv_malloc_zeroed(sizeof(ui_t));
    lv_obj_t *screen = init_screen();

    // 添加翻译的静态文本
    lv_translation_add_static(languages, tags, translations);
    lv_translation_set_language(languages[0]);

    // 初始化数据
    init_data(ui);

    init_status_bar(ui, screen);

    init_table_view(ui, screen);
}

/**
 * @brief 更新电量
 *
 * @param ui
 * @param isCharging  是否在充电
 * @param percentage  电量百分比
 */
void ui_update_battery(ui_t *ui, bool isCharging, uint8_t percentage)
{

    static const char *battery_symbols[] = {
        LV_SYMBOL_BATTERY_EMPTY,
        LV_SYMBOL_BATTERY_1,
        LV_SYMBOL_BATTERY_2,
        LV_SYMBOL_BATTERY_3,
        LV_SYMBOL_BATTERY_FULL,
        LV_SYMBOL_CHARGE};

    if (isCharging)
    {
        lv_subject_set_pointer(&ui->data.battery_level,(void *) battery_symbols[5]);
    }
    else
    {
        lv_subject_set_pointer(&ui->data.battery_level, (void *)battery_symbols[percentage / 20]);
    }
}
