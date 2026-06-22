#pragma once

#include "quantum.h"
#include "os_detection.h"

enum my_keymap_layers
{
    LAYER_BASE = 0,
    LAYER_LOWER,
    LAYER_RAISE,
    LAYER_POINTER,
};

#define LOWER MO(LAYER_LOWER)
#define RAISE MO(LAYER_RAISE)
#define POINT MO(LAYER_POINTER)

#define _LO(x) LT(LAYER_LOWER, x)
#define _RA(x) LT(LAYER_RAISE, x)
#define _PT(x) LT(LAYER_POINTER, x)

#define _C(x) CTL_T(x)
#define _A(x) ALT_T(x)
#define _G(x) GUI_T(x)
#define _LS(x) LSFT_T(x)
#define _RS(x) RSFT_T(x)

#define PT_Z LT(LAYER_POINTER, KC_Z)
#define PT_SLSH LT(LAYER_POINTER, KC_SLSH)

#define LO_SPC _LO(KC_SPC)
#define LO_COMM _LO(KC_COMM)
#define LO_DOT _LO(KC_DOT)

#define RA_COMM _RA(KC_COMM)
#define RA_DOT _RA(KC_DOT)

#define TO_PT TO(LAYER_POINTER)
#define TO_BA TO(LAYER_BASE)

#define SH____1 LSFT(KC_1)
#define SH____2 LSFT(KC_2)
#define SH____3 LSFT(KC_3)
#define SH____4 LSFT(KC_4)
#define SH____5 LSFT(KC_5)
#define SH____6 LSFT(KC_6)
#define SH____7 LSFT(KC_7)
#define SH____8 LSFT(KC_8)
#define SH____9 LSFT(KC_9)
#define SH____0 LSFT(KC_0)

#define SH__GRV LSFT(KC_GRV)
#define SH_MINS LSFT(KC_MINS)
#define SH__EQL LSFT(KC_EQL)
#define SH_LBRC LSFT(KC_LBRC)
#define SH_RBRC LSFT(KC_RBRC)
#define SH_BSLS LSFT(KC_BSLS)
#define SH_QUOT LSFT(KC_QUOT)
#define SH_SCLN LSFT(KC_SCLN)
#define SH_COMM LSFT(KC_COMM)
#define SH_DOT LSFT(KC_DOT)
#define SH_SLSH LSFT(KC_SLSH)

// 커스텀 키코드 정의
enum my_keycodes
{
    OS_VDL = QK_KB_0, // virtual desktop left
    OS_VDR,           // virtual desktop right
    OS_OVV,           // OS overview
    OS_WDL,           // word left
    OS_WDR,           // word right
    OS_LANG,          // input language
    OS_SRCH,          // system search
    OS_PSCR,          // full screenshot
    OS_CPTR,          // area screenshot
    OS_PATH,          // copy selected file/folder path
    OS_EMOJ,          // emoji picker
    OS_DEL,           // delete selected item
    OS_REN,           // rename selected item
    OS_LOCK,          // lock screen
    OS_APP,           // app switcher
    OS_CMD,           // lctl on Windows, lgui on macOS
    OS_CTL,           // lgui on Windows, lctl on macOS

    GO_LEFT = OS_VDL,
    GO_RGHT = OS_VDR,
    GO_UP = OS_OVV,
    WO_LEFT = OS_WDL,
    WO_RGHT = OS_WDR,
    MC_LCMD = OS_CMD,
    MC_LCTL = OS_CTL,
    OS_CPATH = OS_PATH,
};

// 핸들러 진입점: 처리했다면 false 반환(상위 처리 중단), 미처리면 true 반환
bool process_my_custom_keycodes(uint16_t keycode, bool pressed, os_variant_t host);
