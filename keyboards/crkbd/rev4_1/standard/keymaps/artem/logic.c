#include "definitions.h"
// #include "send_string_keycodes.h"
//
// --- Global Variable Definitions ---
// The variables are actually created and assigned their initial values here.
//
bool is_mac = false;
bool is_russian = false;
enum LAYERS active_default_layer = U_BASE;


//
// --- F-Key Mapping for Layer State ---
//

// This array maps a layer index to its standard (English) F-key.
// We use PROGMEM to store this constant data in flash memory instead of RAM.
// const int layer_fkey_map[] = {
//     [U_BASE]   = X_F13,
//     [U_EXTRA]  = X_F16,
//     [U_TAP]    = X_F17,
//     [U_BUTTON] = X_F18,
//     [U_NAV]    = X_F19,
//     [U_MOUSE]  = X_F20,
//     [U_MEDIA]  = X_F21,
//     [U_NUM]    = X_F22,
//     [U_SYM]    = X_F23,
//     [U_FUN]    = X_F24
// };

// We also need to know the size of our map for safety.
// #define LAYER_FKEY_MAP_SIZE (sizeof(layer_fkey_map) / sizeof(int))



//
// --- Tap Dance Logic ---
// //
tap_dance_action_t tap_dance_actions[TAP_DANCE_ACTIONS_SIZE] = {
    // The bootloader/reset function is unique, so we keep it as is.
    [U_TD_BOOT]     = ACTION_TAP_DANCE_FN(u_td_fn_boot),
    // All layer setters now use our generic function and helper macro.
    [U_TD_U_BASE]   = TD_SET_LAYER(U_BASE),
    [U_TD_U_EXTRA]  = TD_SET_LAYER(U_EXTRA),
    [U_TD_U_TAP]    = TD_SET_LAYER(U_TAP),
    [U_TD_U_BUTTON] = TD_SET_LAYER(U_BUTTON),
    [U_TD_U_NAV]    = TD_SET_LAYER(U_NAV),
    [U_TD_U_MOUSE]  = TD_SET_LAYER(U_MOUSE),
    [U_TD_U_MEDIA]  = TD_SET_LAYER(U_MEDIA),
    [U_TD_U_NUM]    = TD_SET_LAYER(U_NUM),
    [U_TD_U_SYM]    = TD_SET_LAYER(U_SYM),
    [U_TD_U_FUN]    = TD_SET_LAYER(U_FUN)
};


void set_default_layer_td(tap_dance_state_t *state, void *user_data) {
  // We only act on a double tap, just like before.
  if (state->count == 2) {
    // The 'user_data' pointer is repurposed to carry our layer number.
    // We safely cast it back to an integer.
    uint8_t layer = (uint8_t)(uintptr_t)user_data;

    // Now we use the 'layer' variable to set the default layer.
    default_layer_set((layer_state_t)1 << layer);
    active_default_layer = layer;
  }
}

// double tap to activate 
void u_td_fn_boot(tap_dance_state_t *state, void *user_data) {
  if (state->count == 2) {
    reset_keyboard();
  }
}

//
// --- Custom Functions & QMK Callbacks ---
// All your feature logic now lives here.
//
void os_swap_animation(bool mac) {
#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
    // This logic is now centralized here.
    if (is_mac) {
        // White is saturation 0, max value
        rgb_matrix_sethsv_noeeprom(HSV_CYAN);
    } else {
        // Blue is a hue of ~170, full saturation, max value
        rgb_matrix_sethsv_noeeprom(HSV_MAGENTA);
    }
    uprintf("os swap\n");
#endif
}

// shift functions

const key_override_t capsword_key_override = ko_make_basic(MOD_MASK_SHIFT, CW_TOGG, KC_CAPS);

#define KEY_OVERRIDE_COUNT 1
const key_override_t *key_overrides[] = {
    &capsword_key_override,
    NULL
};



// On Init
void keyboard_post_init_user(void) {
    is_mac = true;

    os_swap_animation(is_mac);
}


// Hanlding copy-paste and other OS/language-specific lkey_to_sendlkey_to_sendcommands
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) { // Only run this logic when the key is pressed down
        switch (keycode) {
            case U_CPY: // Our custom copy key
                if (is_mac) {
                    register_code16(G(KC_C)); // Send Left GUI (Command) + C
                } else {
                   register_code16(LCTL(S(KC_C))); // Send Left Control + C
                }
                return false; // We've handled this key, don't process it further

            case U_PST: // Our custom paste key
                if (is_mac) {
                    register_code16(G(KC_V)); // Send Left GUI (Command) + V
                } else {
                    register_code16(C(KC_V)); // Send Left Control + V
                }
                return false;

            case U_CUT: // Our custom cut key
                if (is_mac) {
                    register_code16(G(KC_X)); // Send Left GUI (Command) + X
                } else {
                    register_code16(C(KC_X)); // Send Left Control + X
                }
                return false;
            
            case U_UND: // Our custom undo key
                if (is_mac) {
                    register_code16(G(KC_Z)); // Send Left GUI (Command) + Z
                } else {
                    register_code16(C(KC_Z)); // Send Left Control + Z
                }
                return false;
            
            case U_RDO: // Our custom redo key
                if (is_mac) {
                    register_code16(G(KC_Z));// Send Left GUI (Command) + Z
                } else {
                    register_code16(C(KC_Z)); // Send Left Control + Z
                }

            case OS_SWAP: // Our OS toggle key
                is_mac = !is_mac; // Flip the boolean value
                // eeconfig_update_user_with_value(0, is_mac); // Save the new value to EEPROM
                // Optional: add feedback like a sound or LED change
                os_swap_animation(is_mac);
                return false;

            case U_DOT:
                if (is_mac) {
                    register_code16(S(KC_7)); // <-- REPLACE with your Mac Russian Dot keycode
                } else {
                    register_code16(KC_SLSH); // <-- REPLACE with your Windows Russian Dot keycode
                }
                
                return false;

            case U_COMM:
                if (is_russian) {
                    // Russian Comma Logic
                    if (is_mac) {
                        register_code16(S(KC_6)); // <-- REPLACE with your Mac Russian Comma keycode
                    } else {
                        register_code16(S(KC_SLSH)); // <-- REPLACE with your Windows Russian Comma keycode
                    }
                } else {
                    // English Comma (standard)
                    register_code16(KC_COMM);
                }
                return false;

            case LANG_SW:
                // 1. Toggle the language state
                is_russian = !is_russian;
                // eeconfig_update_user_with_value(1, is_russian); // Save to address 1

                // 2. Change the keyboard layer
                if (is_russian) {
                    default_layer_set((layer_state_t)1 << U_EXTRA); 
                    active_default_layer = U_EXTRA;
                } else {
                    default_layer_set((layer_state_t)1 << U_BASE); 
                    active_default_layer = U_BASE;
                }

                // 3. Send the OS command to switch language
                if (is_mac) {
                    register_code16(G(KC_SPC)); // Command + Space for macOS
                } else {
                    register_code16(A(KC_LSFT)); // Alt + Shift for Windows (check your OS settings)
                }
                return false; // We will unregister this complex key on release
        }
    } else { // On key release
         switch (keycode) {
            case U_CPY:
                if (is_mac) { unregister_code16(G(KC_C)); } else { unregister_code16(LCTL(S(KC_C)));}
                break;
            case U_PST:
                if (is_mac) { unregister_code16(G(KC_V)); } else { unregister_code16(C(KC_V)); }
                break;
            case U_CUT:
                if (is_mac) { unregister_code16(G(KC_X)); } else { unregister_code16(C(KC_X)); }
                break;
            case U_UND:
                if (is_mac) { unregister_code16(G(KC_Z)); } else { unregister_code16(C(KC_Z)); }
                break;
            case U_RDO:
                if (is_mac) { unregister_code16(G(KC_Z)); } else { unregister_code16(C(KC_Z)); }
                break;
            case U_DOT:
                // Unregister all possibilities to be safe
                if (is_mac) {
                    unregister_code16(S(KC_7));
                } else {
                    unregister_code16(KC_SLSH);
                }
                break;
            case U_COMM:
                unregister_code16(S(KC_6));
                unregister_code16(S(KC_SLSH));
                unregister_code16(KC_COMM);
                break;
            case LANG_SW:
                // Unregister the OS switch command
                unregister_code16(G(KC_SPC));
                unregister_code16(A(KC_LSFT));
                break;
         }
    }
    return true; // Process all other keys normally
};


layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);

    // This is the only line that needs to be changed.
    uint8_t data_to_send[32] = {0}; // Use uint8_t instead of char

    if (layer == U_BASE) {
        data_to_send[0] = active_default_layer;
    } else {
        data_to_send[0] = layer;
    }
    
    host_raw_hid_send(data_to_send, sizeof(data_to_send));

    uprintf("Sent Raw HID data: Layer=%d\n", data_to_send[0]);

    return state;
}

