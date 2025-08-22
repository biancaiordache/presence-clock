#include <TM1637Display.h>

const int CLK_PIN = 18;   // TM1637 CLK
const int DIO_PIN = 19;   // TM1637 DIO

TM1637Display display(CLK_PIN, DIO_PIN);

unsigned long lastTickMs = 0;
unsigned long seconds = 0;

void setup() {
  display.setBrightness(7);
  display.clear();
  showHHMM(0, 0, true); // show 00:00
}

void loop() {
  unsigned long now = millis();
  if (now - lastTickMs >= 1000) {
    lastTickMs = now;
    seconds++;

    unsigned long mins = seconds / 60;
    unsigned int hh = mins / 60;
    unsigned int mm = mins % 60;
    if (hh > 99) hh = 99;

    showHHMM(hh, mm, true);
  }
}

void showHHMM(unsigned int hh, unsigned int mm, bool colonOn) {
  uint16_t val = (hh/10)*1000 + (hh%10)*100 + (mm/10)*10 + (mm%10);
  uint8_t dots = colonOn ? 0b01000000 : 0;
  display.showNumberDecEx(val, dots, true);
}
