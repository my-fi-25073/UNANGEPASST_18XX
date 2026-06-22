// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "my_config.h"
#include "my_effect.h"
#include "my_keycode.h"
#include "os_detection.h"

typedef bool (*key_handler_t)(bool pressed, os_variant_t host);

// 효과 비트 매크로는 my_effect.h에서 제공

// LED pins follow KiCad nets LED1..LED7.
#define LED_PIN_0 B1
#define LED_PIN_1 B2
#define LED_PIN_2 B3
#define LED_PIN_3 B4
#define LED_PIN_4 B5
#define LED_PIN_5 B6
#define LED_PIN_6 B7

#define PIN_COUNT MY_CONFIG_LED_COUNT
static const pin_t kPins[PIN_COUNT] = {LED_PIN_0, LED_PIN_1, LED_PIN_2, LED_PIN_3, LED_PIN_4, LED_PIN_5, LED_PIN_6};

static inline uint8_t get_pin_mode(uint8_t idx)
{
    return my_config_get_led_flags(idx);
}
static inline my_indicator_source_t get_indicator_src(uint8_t idx)
{
    return (my_indicator_source_t)my_config_get_indicator(idx);
}

static inline bool require_typing_state_update(void)
{
    for (uint8_t i = 0; i < PIN_COUNT; i++)
    {
        if ((get_pin_mode(i) & EFFECT_NEEDS_STATE) != 0)
            return true;
    }
    return false;
}

static bool is_indicator_active(my_indicator_source_t ind, led_t leds)
{
    if (ind == IND_SCROLL)
        return leds.scroll_lock;
    if (ind == IND_CAPS)
        return leds.caps_lock;
    if (ind == IND_NUM)
        return leds.num_lock;
    if (ind == IND_KANA)
        return leds.kana;
    if (ind >= IND_LAYER0 && ind <= IND_LAYER3)
    {
        uint8_t target = (uint8_t)(ind - IND_LAYER0);
        return (get_highest_layer(layer_state) == target);
    }
    return false;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // clang-format off
    [0] = LAYOUT(
        // Row0
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        // Row1
        KC_NO, KC_NO, KC_NO, KC_NO,
        // Row2
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        // Row3
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        // Row4
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        // Row5
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,
        // Row6
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO
    )
    // clang-format on
};

void keyboard_post_init_user(void)
{
    for (uint8_t i = 0; i < PIN_COUNT; i++)
    {
        setPinOutput(kPins[i]);
    }
    my_effect_init();
}

void matrix_scan_user(void)
{
    led_t leds = host_keyboard_led_state();
    for (uint8_t i = 0; i < PIN_COUNT; i++)
    {
        const pin_t pin = kPins[i];
        const my_indicator_source_t ind = get_indicator_src(i);
        const uint8_t mode = get_pin_mode(i);

        bool ind_on = is_indicator_active(ind, leds);

        if (ind_on)
        {
            writePinHigh(pin);
        }
        else
        {
            my_effect_apply_pin_effect(pin, mode);
        }
    }
}

void housekeeping_task_user(void)
{
    led_t leds = host_keyboard_led_state();
    for (uint8_t i = 0; i < PIN_COUNT; i++)
    {
        const pin_t pin = kPins[i];
        const my_indicator_source_t ind = get_indicator_src(i);
        const uint8_t mode = get_pin_mode(i);

        // 인디케이터 ON이면 이펙트 무시
        bool ind_on = is_indicator_active(ind, leds);
        my_effect_update_effects_for_pin(pin, mode, ind_on);
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t* record)
{
    if (require_typing_state_update())
    {
        my_effect_update_typing_state_from_key_event(record->event.pressed);
    }

    os_variant_t host = detected_host_os();
    if (!process_my_custom_keycodes(keycode, record->event.pressed, host))
        return false;

    return true;
}
