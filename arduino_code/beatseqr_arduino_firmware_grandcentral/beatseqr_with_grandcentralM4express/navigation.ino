// navigation.ino
//
// Beatseqr has no D-pad, so this file is much leaner than Synthseqr's.
// Tempo and swing are hardware knobs (see knob_routine.ino).
// Config menu navigation uses knob jog-wheel + param_rec (see config_menu.ino).
//
// This file provides:
//   listen_for_navigation_events() — adv copy cancel; called every loop
//   setExternalClockMode(bool)     — used by config menu

// listen_for_navigation_events
//
// In Synthseqr this handled D-pad tempo/pattern events. In Beatseqr the only
// out-of-menu navigation event is cancelling an in-progress advanced pattern
// copy. The cancel is triggered by pressing param_rec (Enter) while a copy
// phase is active — same natural "abort" gesture as pressing Esc/Back.
void listen_for_navigation_events() {
  if ((adv_copy_waiting_source || adv_copy_armed) && param_rec_flag) {
    param_rec_flag = false;
    adv_copy_waiting_source = false;
    adv_copy_armed = false;
    update_line1 = true;
    update_line2 = true;
    next_lcdflag = 255;
    Serial.println("copy cancelled");
    return;
  }
}

// setExternalClockMode — switch between internal TC4 and external USB-MIDI clock.
// Called from the config menu Clock item.
void setExternalClockMode(bool enable) {
  if (enable == external_clock_mode) return;
  external_clock_mode = enable;
  if (external_clock_mode) {
    stopSequencerTimer();
  } else {
    setSequencerTimerPeriod(60000000UL / (unsigned long)TEMPO / 24UL);
    startSequencerTimer();
  }
  Serial.print("clock mode: ");
  Serial.println(external_clock_mode ? "EXT" : "INT");
}
