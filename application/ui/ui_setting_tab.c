#include "ui_setting_tab.h"

// 表示下拉菜单中的选项
static char *language_list = "English\n中文";

static void ui_setting_container_init(lv_obj_t *parent)
{
    lv_obj_set_style_pad_all(parent, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_row(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_column(parent, 10, LV_PART_MAIN);
    lv_obj_set_style_border_width(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(parent, 0, LV_PART_MAIN);
}

static void ui_dark_mode(ui_t *ui)
{
    // 1. 状态栏
    lv_obj_set_style_bg_color(ui->status_bar, lv_palette_lighten(LV_PALETTE_GREY, 1), LV_PART_MAIN);
    // 2. tableview背景色
    lv_obj_set_style_bg_color(ui->tableview, lv_palette_lighten(LV_PALETTE_GREY, 2), LV_PART_MAIN);
    // 3. 修改容器的颜色
    // 3.1 获取general_tab的子元素个数
    uint32_t child_num = lv_obj_get_child_count(ui->general_tab);
    // 3.2 修改general_tab的子元素背景颜色
    for (uint8_t i = 0; i < child_num; i++)
    {
        lv_obj_t *child = lv_obj_get_child(ui->general_tab, i);
        lv_obj_set_style_bg_color(child, lv_palette_lighten(LV_PALETTE_GREY, 1), LV_PART_MAIN);
    }

    // 3.3 修改setting_tab的子元素背景颜色
    child_num = lv_obj_get_child_count(ui->setting_tab);
    for (uint8_t i = 0; i < child_num; i++)
    {
        lv_obj_t *child = lv_obj_get_child(ui->setting_tab, i);
        lv_obj_set_style_bg_color(child, lv_palette_lighten(LV_PALETTE_GREY, 1), LV_PART_MAIN);
    }
    // 4. 下拉菜单的背景颜色
    lv_obj_set_style_bg_color(ui->language_dropdown, lv_palette_lighten(LV_PALETTE_GREY, 1), LV_PART_MAIN);
}

static void ui_light_mode(ui_t *ui)
{
    // 1. 状态栏
    lv_obj_set_style_bg_color(ui->status_bar, lv_palette_lighten(LV_PALETTE_GREY, 2), LV_PART_MAIN);
    // 2. tableview背景色
    lv_obj_set_style_bg_color(ui->tableview, lv_color_white(), LV_PART_MAIN);
    // 3. 修改容器的颜色
    // 3.1 获取general_tab的子元素个数
    uint32_t child_num = lv_obj_get_child_count(ui->general_tab);
    // 3.2 修改general_tab的子元素背景颜色
    for (uint8_t i = 0; i < child_num; i++)
    {
        lv_obj_t *child = lv_obj_get_child(ui->general_tab, i);
        lv_obj_set_style_bg_color(child, lv_color_white(), LV_PART_MAIN);
    }

    // 3.3 修改setting_tab的子元素背景颜色
    child_num = lv_obj_get_child_count(ui->setting_tab);
    for (uint8_t i = 0; i < child_num; i++)
    {
        lv_obj_t *child = lv_obj_get_child(ui->setting_tab, i);
        lv_obj_set_style_bg_color(child, lv_color_white(), LV_PART_MAIN);
    }
    // 4. 下拉菜单的背景颜色
    lv_obj_set_style_bg_color(ui->language_dropdown, lv_color_white(), LV_PART_MAIN);
}

void dark_mode_switch_event_cb(lv_event_t *e)
{
    // 获取到开关对象
    lv_obj_t *switch_t = lv_event_get_target_obj(e);
    // 获取当前开关的状态
    bool is_checked = lv_obj_has_state(switch_t, LV_STATE_CHECKED);

    // 获取user_data
    ui_t *ui = lv_event_get_user_data(e);
    if (is_checked)
    {
        // 切换到深色模式
        printf("dark mode\n");
        ui_dark_mode(ui);
    }
    else
    {
        // 切换到浅色模式
        printf("light mode\n");
        ui_light_mode(ui);
    }
}

void language_dropdown_event_cb(lv_event_t *e)
{
    // 获取下拉列表
    lv_obj_t *dropdown = lv_event_get_target_obj(e);
    // 获取哦下拉列表的选项
    uint8_t selected_index = lv_dropdown_get_selected(dropdown);

    printf("selected_index: %d\n", selected_index);
    // 根据当前的选项设置对应的语言
    lv_translation_set_language(languages[selected_index]);
}

void ui_setting_tab_init(lv_obj_t *parent, ui_t *ui)
{
    // 1. 设置为弹性布局
    lv_obj_set_layout(parent, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_COLUMN);

    // 2. tab格式设置
    lv_obj_set_style_pad_all(parent, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_row(parent, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_column(parent, 10, LV_PART_MAIN);
    lv_obj_set_style_border_width(parent, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(parent, 0, LV_PART_MAIN);
    // 设置对齐方式
    lv_obj_set_flex_align(parent, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    // 3. 创建一个语言设置容器
    lv_obj_t *language_container = lv_obj_create(parent);
    // 初始化格式
    ui_setting_container_init(language_container);
    // 设置大小
    lv_obj_set_size(language_container, LV_PCT(100), LV_SIZE_CONTENT);

    // 3.1 填写标签名称
    lv_obj_t *language_label = lv_label_create(language_container);

    // lv_label_set_text(language_label, "Language");
    // lv_label_set_text(language_label, lv_tr(tags[3]));
    ui_translation_register_label(language_label, tags[3]);
    // 修改字体大小
    // lv_obj_set_style_text_font(language_label, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_obj_align(language_label, LV_ALIGN_LEFT_MID, 0, 0);

    // 3.2 创建下拉菜单
    lv_obj_t *language_dropdown = lv_dropdown_create(language_container);
    lv_dropdown_set_options(language_dropdown, language_list);
    // lv_dropdown_set_dir(language_dropdown, LV_DIR_BOTTOM);
    // lv_dropdown_set_symbol(language_dropdown, LV_SYMBOL_DOWN);
    // 修改下拉菜单的大小
    lv_obj_set_size(language_dropdown, LV_PCT(50), LV_SIZE_CONTENT);
    lv_obj_align(language_dropdown, LV_ALIGN_RIGHT_MID, 0, 0);

    // 补全到ui指针中
    ui->language_dropdown = language_dropdown;

    // 添加下拉菜单的回调函数
    lv_obj_add_event_cb(language_dropdown, language_dropdown_event_cb, LV_EVENT_VALUE_CHANGED, ui);

    // 4. 创建显示模式的容器
    lv_obj_t *dark_mode_container = lv_obj_create(parent);
    // 初始化格式
    ui_setting_container_init(dark_mode_container);
    lv_obj_set_size(dark_mode_container, LV_PCT(100), LV_SIZE_CONTENT);

    // 4.1 填写标签名称
    lv_obj_t *dark_mode_label = lv_label_create(dark_mode_container);
    // lv_label_set_text(dark_mode_label, "Dark Mode");
    // lv_label_set_text(dark_mode_label, lv_tr(tags[4]));
    ui_translation_register_label(dark_mode_label, tags[4]);

    lv_obj_align(dark_mode_label, LV_ALIGN_LEFT_MID, 0, 0);

    // 4.2 创建开关
    lv_obj_t *dark_mode_switch = lv_switch_create(dark_mode_container);
    lv_obj_align(dark_mode_switch, LV_ALIGN_RIGHT_MID, -10, 0);

    // 添加开关的回调函数处理
    lv_obj_add_event_cb(dark_mode_switch, dark_mode_switch_event_cb, LV_EVENT_VALUE_CHANGED, ui);
}
