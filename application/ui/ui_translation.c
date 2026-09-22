#include "ui_translation.h"

void on_language_change(lv_event_t *e)
{
    lv_obj_t *label = lv_event_get_target_obj(e);
    const char *tag = (const char *)lv_event_get_user_data(e);
    lv_label_set_text(label, lv_tr(tag));
}

void ui_translation_register_label(lv_obj_t *label, const char *tag)
{
    // 添加绑定
    lv_label_set_text(label, lv_tr(tag));
    // 添加回调
    lv_obj_add_event_cb(label, on_language_change, LV_EVENT_TRANSLATION_LANGUAGE_CHANGED, (void *)tag);
}
