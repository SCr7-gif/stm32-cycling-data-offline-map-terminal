#ifndef __UI_TRANSLATION_H
#define __UI_TRANSLATION_H

#include "lvgl.h"
static const char *languages[] = {"en", "zh", NULL};
static const char *tags[] = {"GPX", "General", "Setting", "Language", "Dark Mode", NULL};
static const char *translations[] = {
    "RoadMap", "路书",
    "General", "通用",
    "Setting", "设置",
    "language", "语言",
    "Dark Mode", "深色模式"};

void ui_translation_register_label(lv_obj_t *label, const char *tag);

#endif
