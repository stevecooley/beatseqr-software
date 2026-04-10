// config_menu.ino
//
// Modal config menu entered by double-tapping the Enter button.
//
// Navigation:
//   D-pad up/down   — scroll through items
//   D-pad left      — exit from anywhere (also cancels confirmation)
//   D-pad right     — exit when cursor is on "Exit" item
//   Enter           — select item; on "Exit" exits; on destructive items shows
//                     confirmation on line 2; on Mode item toggles immediately
//
// Menu items (in order):
//   0  Exit
//   1  Clear pattern   (confirmation required)
//   2  Clear all pats  (confirmation required)
//   3  Reset sliders   (confirmation required)
//   4  Mode            (toggles Simple / Advanced immediately)

#define CONFIG_ITEM_EXIT          0
#define CONFIG_ITEM_SAVE          1
#define CONFIG_ITEM_CLEAR_PAT     2
#define CONFIG_ITEM_CLEAR_ALL     3
#define CONFIG_ITEM_RESET_SLIDERS 4
#define CONFIG_ITEM_MODE          5
#define CONFIG_MENU_ITEM_COUNT    6

// line1_label: 14 chars printed after "> " on line 1.
// For Mode the value is appended at draw time so it fits on one line.
static const char* config_labels[CONFIG_MENU_ITEM_COUNT] = {
  "Exit          ",   // 14 chars
  "Save          ",   // 14 chars
  "Clear pattern ",   // 14 chars
  "Clear all pats",   // 14 chars
  "Reset sliders ",   // 14 chars
  "Mode:         "    // value overwritten at draw time
};

// Build the 14-char label for a given item index.
// For MODE, replaces trailing spaces with the current value.
void print_config_label(uint8_t item) {
  if (item == CONFIG_ITEM_MODE) {
    // "Mode: Simple  " or "Mode: Advanced" — exactly 14 chars
    lcd.print(advanced_mode ? "Mode: Advanced" : "Mode: Simple  ");
  } else {
    lcd.print(config_labels[item]);
  }
}

// ---------------------------------------------------------------------------

void draw_config_menu() {
  // Line 1: "> {current item label}" — 16 chars total
  lcd.print("?x00?y0");
  lcd.print("> ");
  print_config_label(config_menu_item);

  // Line 2: confirmation prompt if pending, otherwise next item with no cursor
  lcd.print("?x00?y1");
  if (config_confirm_pending) {
    lcd.print("Entr=ok  Lft=no ");
  } else {
    uint8_t next = (config_menu_item + 1) % CONFIG_MENU_ITEM_COUNT;
    lcd.print("  ");
    print_config_label(next);
  }
}

void enter_config_menu() {
  config_menu_active = true;
  config_menu_item = 0;
  config_confirm_pending = false;
  draw_config_menu();
}

void exit_config_menu() {
  config_menu_active = false;
  config_confirm_pending = false;
  // Force a full LCD redraw back to the main display.
  update_line1 = true;
  update_line2 = true;
  lcdflag = 255;
  next_lcdflag = 255;
  cursor_flag = true;
}

void run_config_menu() {
  if (!config_menu_active) return;

  // D-pad left always exits (or cancels confirmation).
  if (dpad_left_flag) {
    dpad_left_flag = false;
    if (config_confirm_pending) {
      config_confirm_pending = false;
      draw_config_menu();
    } else {
      exit_config_menu();
    }
    return;
  }

  // While a confirmation is pending, only Enter (confirm) and Left (cancel,
  // handled above) are meaningful. Consume up/down/right so they don't leak.
  if (config_confirm_pending) {
    dpad_up_flag = false;
    dpad_down_flag = false;
    dpad_right_flag = false;
    if (enterbutton_flag) {
      enterbutton_flag = false;
      switch (config_menu_item) {
        case CONFIG_ITEM_CLEAR_PAT:
          clear_pattern_memory_for_voice(0);
          break;
        case CONFIG_ITEM_CLEAR_ALL:
          clear_pattern_memory();
          break;
        case CONFIG_ITEM_RESET_SLIDERS:
          resetSliders();
          break;
      }
      config_confirm_pending = false;
      // After a destructive action, return to menu (don't auto-exit)
      draw_config_menu();
    }
    return;
  }

  // Scroll up (wraps).
  if (dpad_up_flag) {
    dpad_up_flag = false;
    config_menu_item = (config_menu_item + CONFIG_MENU_ITEM_COUNT - 1) % CONFIG_MENU_ITEM_COUNT;
    draw_config_menu();
  }

  // Scroll down (wraps).
  if (dpad_down_flag) {
    dpad_down_flag = false;
    config_menu_item = (config_menu_item + 1) % CONFIG_MENU_ITEM_COUNT;
    draw_config_menu();
  }

  // D-pad right exits only when cursor is on Exit item.
  if (dpad_right_flag) {
    dpad_right_flag = false;
    if (config_menu_item == CONFIG_ITEM_EXIT) {
      exit_config_menu();
    }
    // Right is a no-op on all other items to avoid accidental actions.
  }

  // Enter selects the current item.
  if (enterbutton_flag) {
    enterbutton_flag = false;
    switch (config_menu_item) {
      case CONFIG_ITEM_EXIT:
        exit_config_menu();
        break;
      case CONFIG_ITEM_SAVE:
        if (playstatus) {
          // Flash a warning on line 2 — don't save while playing.
          lcd.print("?x00?y1");
          lcd.print("Stop first!     ");
        } else {
          save_to_eeprom();
          lcdflag = 202;
          next_lcdflag = 202;
          exit_config_menu();
        }
        break;
      case CONFIG_ITEM_CLEAR_PAT:
      case CONFIG_ITEM_CLEAR_ALL:
      case CONFIG_ITEM_RESET_SLIDERS:
        config_confirm_pending = true;
        draw_config_menu();
        break;
      case CONFIG_ITEM_MODE:
        advanced_mode = !advanced_mode;
        Serial.print("mode: ");
        Serial.println(advanced_mode ? "Advanced" : "Simple");
        draw_config_menu();
        break;
    }
  }
}
