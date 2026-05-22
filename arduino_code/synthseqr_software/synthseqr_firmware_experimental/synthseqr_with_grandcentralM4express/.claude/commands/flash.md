Flash the synthseqr firmware to the connected Adafruit Grand Central M4 Express using arduino-cli.

Steps:
1. Regenerate the firmware version header (writes commit-count + dirty-tree suffix into version.h, which the sketch consumes via FIRMWARE_VERSION):
   /Users/stevecooley/Library/CloudStorage/Dropbox/arduino/beatseqr-software/arduino_code/synthseqr_software/synthseqr_firmware_experimental/synthseqr_with_grandcentralM4express/generate_version.sh
2. Run `arduino-cli board list` to find connected boards. Look for the Grand Central M4 (FQBN: adafruit:samd:adafruit_grandcentral_m4) and note its port. On macOS it will be something like /dev/cu.usbmodem* or /dev/tty.usbmodem*. If no board is detected, tell the user to check the USB connection and try again.
3. Compile the sketch:
   arduino-cli compile --fqbn adafruit:samd:adafruit_grandcentral_m4 /Users/stevecooley/Library/CloudStorage/Dropbox/arduino/beatseqr-software/arduino_code/synthseqr_software/synthseqr_firmware_experimental/synthseqr_with_grandcentralM4express
4. If compilation succeeds, upload to the detected port:
   arduino-cli upload -p <port> --fqbn adafruit:samd:adafruit_grandcentral_m4 /Users/stevecooley/Library/CloudStorage/Dropbox/arduino/beatseqr-software/arduino_code/synthseqr_software/synthseqr_firmware_experimental/synthseqr_with_grandcentralM4express
5. Report success or show any errors from compilation or upload. Include the firmware version line printed by generate_version.sh in your success report.

If the board list shows multiple devices and you cannot confidently identify the Grand Central M4, list them and ask the user which port to use.
