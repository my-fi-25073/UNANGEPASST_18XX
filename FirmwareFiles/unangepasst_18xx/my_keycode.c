#include "my_keycode.h"

typedef bool (*key_handler_t)(bool pressed, os_variant_t host);

typedef struct {
    uint16_t keycode;
    key_handler_t handler;
} key_handler_entry_t;

static void tap_with_mods(uint16_t mod1, uint16_t mod2, uint16_t mod3, uint16_t key)
{
    if (mod1 != KC_NO) { register_code(mod1); }
    if (mod2 != KC_NO) { register_code(mod2); }
    if (mod3 != KC_NO) { register_code(mod3); }
    tap_code(key);
    if (mod3 != KC_NO) { unregister_code(mod3); }
    if (mod2 != KC_NO) { unregister_code(mod2); }
    if (mod1 != KC_NO) { unregister_code(mod1); }
}

static void press_word_move(bool pressed, os_variant_t host, bool move_left)
{
    const uint16_t mod = (host == OS_WINDOWS) ? KC_LCTL : KC_LALT;
    const uint16_t key = move_left ? KC_LEFT : KC_RIGHT;

    if (pressed)
    {
        register_code(mod);
        register_code(key);
    }
    else
    {
        unregister_code(key);
        unregister_code(mod);
    }
}

static bool send_os_vdl(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_with_mods(KC_LGUI, KC_LCTL, KC_NO, KC_LEFT); }
    else { tap_with_mods(KC_LCTL, KC_NO, KC_NO, KC_LEFT); }
    return false;
}

static bool send_os_vdr(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_with_mods(KC_LGUI, KC_LCTL, KC_NO, KC_RIGHT); }
    else { tap_with_mods(KC_LCTL, KC_NO, KC_NO, KC_RIGHT); }
    return false;
}

static bool send_os_ovv(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_with_mods(KC_LGUI, KC_NO, KC_NO, KC_TAB); }
    else { tap_with_mods(KC_LCTL, KC_NO, KC_NO, KC_UP); }
    return false;
}

static bool send_os_wdl(bool pressed, os_variant_t host)
{
    press_word_move(pressed, host, true);
    return false;
}

static bool send_os_wdr(bool pressed, os_variant_t host)
{
    press_word_move(pressed, host, false);
    return false;
}

static bool send_os_lang(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_code(KC_RALT); }
    else { tap_with_mods(KC_LCTL, KC_NO, KC_NO, KC_SPACE); }
    return false;
}

static bool send_os_srch(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_with_mods(KC_LGUI, KC_NO, KC_NO, KC_S); }
    else { tap_with_mods(KC_LGUI, KC_NO, KC_NO, KC_SPACE); }
    return false;
}

static bool send_os_pscr(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_code(KC_PSCR); }
    else { tap_with_mods(KC_LGUI, KC_LSFT, KC_NO, KC_3); }
    return false;
}

static bool send_os_cptr(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_with_mods(KC_LGUI, KC_LSFT, KC_NO, KC_S); }
    else { tap_with_mods(KC_LGUI, KC_LSFT, KC_NO, KC_4); }
    return false;
}

static bool send_os_path(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_with_mods(KC_LCTL, KC_LSFT, KC_NO, KC_C); }
    else { tap_with_mods(KC_LGUI, KC_LALT, KC_NO, KC_C); }
    return false;
}

static bool send_os_emoj(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_with_mods(KC_LGUI, KC_NO, KC_NO, KC_DOT); }
    else { tap_with_mods(KC_LCTL, KC_LGUI, KC_NO, KC_SPACE); }
    return false;
}

static bool send_os_del(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_code(KC_DEL); }
    else { tap_with_mods(KC_LGUI, KC_NO, KC_NO, KC_DEL); }
    return false;
}

static bool send_os_ren(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_code(KC_F2); }
    else { tap_code(KC_ENT); }
    return false;
}

static bool send_os_lock(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_with_mods(KC_LGUI, KC_NO, KC_NO, KC_L); }
    else { tap_with_mods(KC_LCTL, KC_LGUI, KC_NO, KC_Q); }
    return false;
}

static bool send_os_app(bool pressed, os_variant_t host)
{
    if (!pressed) return false;
    if (host == OS_WINDOWS) { tap_with_mods(KC_LALT, KC_NO, KC_NO, KC_TAB); }
    else { tap_with_mods(KC_LGUI, KC_NO, KC_NO, KC_TAB); }
    return false;
}

static bool send_os_cmd(bool pressed, os_variant_t host)
{
    const uint16_t mod = (host == OS_WINDOWS) ? KC_LCTL : KC_LGUI;
    if (pressed) { register_code(mod); } else { unregister_code(mod); }
    return false;
}

static bool send_os_ctl(bool pressed, os_variant_t host)
{
    const uint16_t mod = (host == OS_WINDOWS) ? KC_LGUI : KC_LCTL;
    if (pressed) { register_code(mod); } else { unregister_code(mod); }
    return false;
}

static const key_handler_entry_t s_key_handlers[] = {
    { OS_VDL, send_os_vdl },
    { OS_VDR, send_os_vdr },
    { OS_OVV, send_os_ovv },
    { OS_WDL, send_os_wdl },
    { OS_WDR, send_os_wdr },
    { OS_LANG, send_os_lang },
    { OS_SRCH, send_os_srch },
    { OS_PSCR, send_os_pscr },
    { OS_CPTR, send_os_cptr },
    { OS_PATH, send_os_path },
    { OS_EMOJ, send_os_emoj },
    { OS_DEL, send_os_del },
    { OS_REN, send_os_ren },
    { OS_LOCK, send_os_lock },
    { OS_APP, send_os_app },
    { OS_CMD, send_os_cmd },
    { OS_CTL, send_os_ctl },
};

static inline key_handler_t find_key_handler(uint16_t kc)
{
    for (size_t i = 0; i < sizeof(s_key_handlers) / sizeof(s_key_handlers[0]); i++)
    {
        if (s_key_handlers[i].keycode == kc) return s_key_handlers[i].handler;
    }
    return NULL;
}

bool process_my_custom_keycodes(uint16_t keycode, bool pressed, os_variant_t host)
{
    key_handler_t handler = find_key_handler(keycode);
    if (handler != NULL)
    {
        return handler(pressed, host);
    }
    return true;
}
