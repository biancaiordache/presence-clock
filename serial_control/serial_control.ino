#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <TM1637Display.h>

// --- Display pins ---
const int CLK_PIN = 18;    // TM1637 CLK -> GPIO18
const int DIO_PIN = 19;    // TM1637 DIO -> GPIO19
TM1637Display display(CLK_PIN, DIO_PIN);

// --- Timer state ---
bool running = false;
unsigned long lastTickMs = 0;
unsigned long seconds = 0;
char lastCmd = 0;
unsigned long lastCmdTime = 0;   // cooldown against floods

// --- BLE UUIDs (UART-like) ---
#define SERVICE_UUID        "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e"  // write / write_no_resp

// Draw MM:SS (colonOn true/false)
static inline void showMMSS(unsigned int mm, unsigned int ss, bool colonOn) {
  uint16_t val = (mm/10)*1000 + (mm%10)*100 + (ss/10)*10 + (ss%10);
  display.showNumberDecEx(val, colonOn ? 0b01000000 : 0, true);
}

class CommandCallback: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();      // Arduino String
    if (value.length() == 0) return;

    char cmd = value.charAt(0);
    if (cmd == '\n' || cmd == '\r') return;

    // Cooldown + duplicate guard to stop app flood loops
    unsigned long now = millis();
    if (cmd == lastCmd && (now - lastCmdTime) < 300) return;
    lastCmd = cmd;
    lastCmdTime = now;

    if (cmd == '1') {
      running = true;
      lastTickMs = millis();                          // ✅ start fresh, no backlog
      showMMSS(seconds/60, seconds%60, true);         // show current time, colon ON
    } else if (cmd == '0') {
      running = false;
      showMMSS(seconds/60, seconds%60, false);        // colon OFF when paused
    } else if (cmd == 'R' || cmd == 'r') {
      running = false;
      seconds = 0;
      lastTickMs = millis();                          // ✅ next start waits a full second
      showMMSS(0, 0, false);                          // reset display, colon OFF
    }
  }
};

void setup() {
  display.setBrightness(7);
  display.clear();
  showMMSS(0, 0, false);                 // start at 00:00, colon off

  BLEDevice::init("PresenceClock");
  BLEServer* server = BLEDevice::createServer();
  BLEService* svc = server->createService(SERVICE_UUID);

  BLECharacteristic* ch = svc->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  ch->setCallbacks(new CommandCallback());
  svc->start();

  BLEAdvertising* adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->start();
}

void loop() {
  unsigned long now = millis();
  if (running && now - lastTickMs >= 1000) {
    lastTickMs = now;
    seconds++;

    unsigned int mm = seconds / 60;
    unsigned int ss = seconds % 60;
    if (mm > 99) mm = 99;
    showMMSS(mm, ss, true);              // colon ON while running
  }
  // tiny yield to BLE stack
  delay(1);
}
