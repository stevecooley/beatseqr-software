// TRS MIDI output on pin 16 (PC22 = SERCOM1 PAD[0]) at 31250 baud.
// TX-only — pin 17 (PC23, D-pad left) is never touched.
//
// SERCOM1_GCLK_ID_CORE = 8  (SAMD51 instance header, confirmed)
// MCLK_APBAMASK_SERCOM1    (APBA domain, confirmed)
//
// Baud calculation (16x arithmetic oversampling, GCLK0 = 120 MHz):
//   BAUD = 65536 * (1 - 16 * 31250 / 120000000) = 65263
//   Actual = 31244 Hz, error < 0.02% (MIDI spec allows ±1%)
//
// Call init_trs_midi() from setup() after Serial1.begin().
// Call trs_write(b) to send one byte; it blocks only until DRE is set.

void init_trs_midi() {
    // 1. Enable MCLK for SERCOM1 (APBA domain)
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_SERCOM1;

    // 2. Route GCLK0 (120 MHz) to SERCOM1 core clock
    GCLK->PCHCTRL[SERCOM1_GCLK_ID_CORE].reg =
        GCLK_PCHCTRL_GEN_GCLK0_Val | GCLK_PCHCTRL_CHEN;
    while (!(GCLK->PCHCTRL[SERCOM1_GCLK_ID_CORE].reg & GCLK_PCHCTRL_CHEN));

    // 3. Mux pin 16 (PC22) to SERCOM1 PAD[0], peripheral function C (value 2).
    //    Only this pin is touched — pin 17 (PC23, D-pad left) stays as GPIO.
    PORT->Group[2].PINCFG[22].bit.PMUXEN = 1;  // Port C = group 2, pin 22
    PORT->Group[2].PMUX[11].bit.PMUXE    = 2;  // PMUX[22/2=11], even pin → PMUXE, MUX_C

    // 4. Software reset SERCOM1
    SERCOM1->USART.CTRLA.bit.SWRST = 1;
    while (SERCOM1->USART.SYNCBUSY.bit.SWRST);

    // 5. UART: internal clock, 16x arithmetic oversampling, TX on PAD[0], LSB first
    SERCOM1->USART.CTRLA.reg =
        SERCOM_USART_CTRLA_MODE(1)  |   // async UART, internal clock
        SERCOM_USART_CTRLA_SAMPR(0) |   // 16x oversampling, arithmetic
        SERCOM_USART_CTRLA_TXPO(0)  |   // TX → PAD[0] (pin 16)
        SERCOM_USART_CTRLA_RXPO(1)  |   // RX → PAD[1] (not pin-muxed, ignored)
        SERCOM_USART_CTRLA_DORD;        // LSB first (standard UART / MIDI)

    // 6. 8-bit characters, 1 stop bit, TX enabled only
    SERCOM1->USART.CTRLB.reg =
        SERCOM_USART_CTRLB_CHSIZE(0) |  // 8-bit
        SERCOM_USART_CTRLB_TXEN;        // TX only — RXEN not set
    while (SERCOM1->USART.SYNCBUSY.bit.CTRLB);

    // 7. Baud rate register
    SERCOM1->USART.BAUD.reg = 65263;

    // 8. Enable
    SERCOM1->USART.CTRLA.bit.ENABLE = 1;
    while (SERCOM1->USART.SYNCBUSY.bit.ENABLE);
}

// Send one byte to TRS MIDI out.
// Blocks only until the Data Register Empty flag is set (one byte deep at most).
void trs_write(uint8_t b) {
    while (!SERCOM1->USART.INTFLAG.bit.DRE);
    SERCOM1->USART.DATA.reg = b;
}
