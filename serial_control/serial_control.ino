#include <TM1637Display.h>

const int CLK_PIN = 18;    // TM1637 CLK
const int DIO_PIN = 19;    // TM1637 DIO
TM1637Display display(CLK_PIN, DIO_PIN);

bool running = false;
unsigned long lastTickMs = 0;
unsigned long seconds = 0;

void setup() {
  Serial.begin(115200);
  display.setBrightness(7);
  display.clear();
  showMMSS(0, 0, true);
  Serial.println("Type 1=start, 0=pause, R=reset");
}

void loop() {
  if (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '1') { running = true;  Serial.println("START"); }
    else if (c == '0') { running = false; Serial.println("PAUSE"); }
    else if (c == 'R' || c == 'r') {
      running = false; seconds = 0; showMMSS(0,0,true); Serial.println("RESET");
    }
  }

  unsigned long now = millis();
  if (running && now - lastTickMs >= 1000) {
    lastTickMs = now;
    seconds++;
    unsigned int mm = seconds / 60;
    unsigned int ss = seconds % 60;
    if (mm > 99) mm = 99;
    showMMSS(mm, ss, true);
  }
}

void showMMSS(unsigned int mm, unsigned int ss, bool colonOn) {
  uint16_t val = (mm/10)*1000 + (mm%10)*100 + (ss/10)*10 + (ss%10);
  uint8_t dots = colonOn ? 0b01000000 : 0;
  display.showNumberDecEx(val, dots, true);
}
