// VIA/QMK 제어용 설정 저장/로드/제공
#include "my_config.h"

#include <stdbool.h>
#include <string.h>

#include "eeprom.h"

#ifdef VIA_ENABLE
#    include "via.h"
#endif

#ifndef BIT
#    define BIT(n) (1u << (n))
#endif

my_config_t g_my_config;

static bool s_my_config_dirty = false;

static uint8_t clamp_idx(uint8_t idx) {
    return (idx < MY_CONFIG_LED_COUNT) ? idx : (MY_CONFIG_LED_COUNT - 1u);
}

static uint32_t read_my_config_from_storage(my_config_t *config) {
#if defined(VIA_ENABLE) && (VIA_EEPROM_CUSTOM_CONFIG_SIZE > 0)
    return via_read_custom_config(config, 0, sizeof(*config));
#elif defined(EECONFIG_KB_DATA_SIZE) && (EECONFIG_KB_DATA_SIZE > 0)
    return eeconfig_read_kb_datablock(config, 0, sizeof(*config));
#else
    (void)config;
    return 0;
#endif
}

static uint32_t write_my_config_to_storage(const my_config_t *config) {
#if defined(VIA_ENABLE) && (VIA_EEPROM_CUSTOM_CONFIG_SIZE > 0)
    return via_update_custom_config(config, 0, sizeof(*config));
#elif defined(EECONFIG_KB_DATA_SIZE) && (EECONFIG_KB_DATA_SIZE > 0)
    return eeconfig_update_kb_datablock(config, 0, sizeof(*config));
#else
    (void)config;
    return 0;
#endif
}

static bool my_config_is_valid(const my_config_t *config) {
    return config->version == MY_CONFIG_VERSION;
}

static void my_config_apply_defaults(my_config_t *config) {
    memset(config, 0, sizeof(*config));

    config->version = MY_CONFIG_VERSION;

    // keymap.c 기본 효과: LED0: Edge+Invert(12), LED1: Edge(4), LED2: Hold+Invert(9), LED3..6:none
    config->led_flags[0] = (uint8_t)(BIT(2) | BIT(3));
    config->led_flags[1] = (uint8_t)(BIT(2));
    config->led_flags[2] = (uint8_t)(BIT(0) | BIT(3));

    // 인디케이터 기본: LED0:none(effect only), LED1:caps, LED2:num, LED3..6:none
    config->indicator_source[0] = IND_NONE;
    config->indicator_source[1] = IND_CAPS;
    config->indicator_source[2] = IND_NUM;
}

static void my_config_mark_dirty_if_changed(const my_config_t *before) {
    if (memcmp(before, &g_my_config, sizeof(g_my_config)) != 0) {
        s_my_config_dirty = true;
    }
}

void my_config_save(void) {
    if (!s_my_config_dirty) {
        return;
    }

    write_my_config_to_storage(&g_my_config);
    s_my_config_dirty = false;
}

void eeconfig_init_kb(void) {
    my_config_apply_defaults(&g_my_config);
    write_my_config_to_storage(&g_my_config);
    s_my_config_dirty = false;
    eeconfig_init_user();
}

void matrix_init_kb(void) {
    read_my_config_from_storage(&g_my_config);
    if (!my_config_is_valid(&g_my_config)) {
        my_config_apply_defaults(&g_my_config);
        write_my_config_to_storage(&g_my_config);
    }
    s_my_config_dirty = false;
    matrix_init_user();
}

uint8_t my_config_get_led_flags(uint8_t idx) {
    return g_my_config.led_flags[clamp_idx(idx)];
}

void my_config_set_led_flags(uint8_t idx, uint8_t flags) {
    my_config_t before              = g_my_config;
    g_my_config.led_flags[clamp_idx(idx)] = flags;
    my_config_mark_dirty_if_changed(&before);
}

uint8_t my_config_get_indicator(uint8_t idx) {
    return g_my_config.indicator_source[clamp_idx(idx)];
}

void my_config_set_indicator(uint8_t idx, uint8_t indicator) {
    my_config_t before                     = g_my_config;
    if (indicator > MY_CONFIG_INDICATOR_SOURCE_MAX) {
        indicator = IND_NONE;
    }
    g_my_config.indicator_source[clamp_idx(idx)] = indicator;
    my_config_mark_dirty_if_changed(&before);
}

#ifdef VIA_ENABLE
static void custom_config_get_value(uint8_t *data) {
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    if (*value_id >= id_custom_led_flags_0 && *value_id <= id_custom_led_flags_6) {
        value_data[0] = my_config_get_led_flags((uint8_t)(*value_id - id_custom_led_flags_0));
    } else if (*value_id >= id_custom_indicator_0 && *value_id <= id_custom_indicator_6) {
        value_data[0] = my_config_get_indicator((uint8_t)(*value_id - id_custom_indicator_0));
    }
}

static void custom_config_set_value(uint8_t *data) {
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    if (*value_id >= id_custom_led_flags_0 && *value_id <= id_custom_led_flags_6) {
        my_config_set_led_flags((uint8_t)(*value_id - id_custom_led_flags_0), value_data[0]);
    } else if (*value_id >= id_custom_indicator_0 && *value_id <= id_custom_indicator_6) {
        my_config_set_indicator((uint8_t)(*value_id - id_custom_indicator_0), value_data[0]);
    }
}

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    if (*channel_id != id_custom_channel) {
        *command_id = id_unhandled;
        return;
    }

    switch (*command_id) {
        case id_custom_set_value:
            custom_config_set_value(value_id_and_data);
            break;
        case id_custom_get_value:
            custom_config_get_value(value_id_and_data);
            break;
        case id_custom_save:
            my_config_save();
            break;
        default:
            *command_id = id_unhandled;
            break;
    }

    (void)length;
}
#endif
