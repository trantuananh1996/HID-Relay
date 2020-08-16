#include <hidboot.h>
#include <usbhid.h>
#include <usbhub.h>
#include <hidcomposite.h>

// #define _DEBUG
#define _COMBO = 0

#ifdef _DEBUG
#define DEBUG_PRINT(str) Serial.print(str)
#define DEBUG_PRINTF(str, format) Serial.print(str, format)
#define DEBUG_PRINTLN(str) Serial.println(str)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTF(x, format)
#define DEBUG_PRINTLN(x)
#endif

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif
#include <avr/power.h>

class HIDRelay : public HIDComposite {
 public:
  HIDRelay(USB *p) : HIDComposite(p){};

 protected:
  virtual void ParseHIDData(USBHID *hid, uint8_t ep, bool is_rpt_id,
                            uint8_t len, uint8_t *buf);
  bool SelectInterface(uint8_t iface, uint8_t proto);
};

// Return true for the interface we want to hook into
bool HIDRelay::SelectInterface(uint8_t iface, uint8_t proto) {
  if (proto != 0) return true;

  return false;
}

void HIDRelay::ParseHIDData(USBHID *hid, uint8_t ep, bool is_rpt_id,
                            uint8_t len, uint8_t *buf) {
  //    DEBUG_PRINT("Relay ");
  //    DEBUG_PRINTLN(len);

  for (uint8_t i = 0; i < len; i++) {
    uint8_t current = (uint8_t)buf[i];
    Serial.write(current);
  }
  DEBUG_PRINT("\r\n");
};

class KbdRptParser : public HIDRelay {
 public:
  KbdRptParser(USB *p) : HIDRelay(p){};
  void ParseHIDData(USBHID *hid, uint8_t ep, bool is_rpt_id, uint8_t len,
                    uint8_t *buf);
};

void KbdRptParser::ParseHIDData(USBHID *hid, uint8_t ep, bool is_rpt_id,
                                uint8_t len, uint8_t *buf) {
  uint8_t bufFirst = buf[0];

  if (bufFirst == 3) {  // Multi media key detector
    uint8_t bufSecond = buf[1];
    if (bufSecond == 233) {  // Volume up 233

      Serial.write((uint8_t)0xFD);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x10);
      Serial.write((uint8_t)0x00);
      Serial.write((uint8_t)0xFD);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x00);
      Serial.write((uint8_t)0x00);


      
      return;
    } else if (bufSecond == 234) {  // Volume down 234

      Serial.write((uint8_t)0xFD);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x20);
      Serial.write((uint8_t)0x00);
      Serial.write((uint8_t)0xFD);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x00);
      Serial.write((uint8_t)0x00);
      return;
    } else if (bufSecond == 226) {  // Mute 226

      Serial.write((uint8_t)0xFD);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x40);
      Serial.write((uint8_t)0x00);
      Serial.write((uint8_t)0xFD);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x00);
      Serial.write((uint8_t)0x00);
      return;
    } else if (bufSecond == 181) {  // Next 181

      Serial.write((uint8_t)0xFD);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x00);
      Serial.write((uint8_t)0x01);
      Serial.write((uint8_t)0xFD);
      Serial.write((uint8_t)0x03); 
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x00);
      Serial.write((uint8_t)0x00);
      return;
    } else if (bufSecond == 182) {  // Prev 182

      Serial.write((uint8_t)0xFD);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x00);
      Serial.write((uint8_t)0x02);
      Serial.write((uint8_t)0xFD);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x00);
      Serial.write((uint8_t)0x00);
      return;
    } else if (bufSecond == 205) {  // Play/Pause 205

      Serial.write((uint8_t)0xFD);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x80);
      Serial.write((uint8_t)0x00);
      Serial.write((uint8_t)0xFD);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x03);
      Serial.write((uint8_t)0x00);
      Serial.write((uint8_t)0x00);

      return;
    }
  }

  //    DEBUG_PRINTLN("Kbd");

  Serial.write((uint8_t)0xFD);  // BT-HID: Start Byte
  // Serial.write((uint8_t)0x09); // BT-HID: Data Length
  Serial.write((uint8_t)len + 1);  // BT-HID: Data Length
  Serial.write((uint8_t)0x01);     // BT-HID: Descriptor (1=Keyboard, 2=Mouse,
                                   // 3=KybdMouse combo)
  // Keyboard Data consists of a single byte for modifiers (Ctrl, alt, etc)
  // Followed by 0x00
  // Followed by 6 key codes (any or all may be 0x00 for no key pressed)
  HIDRelay::ParseHIDData(hid, ep, is_rpt_id, len, buf);
};

#ifdef _COMBO
class MouseRptParser : public HIDRelay {
 public:
  MouseRptParser(USB *p) : HIDRelay(p){};
  void ParseHIDData(USBHID *hid, uint8_t ep, bool is_rpt_id, uint8_t len,
                    uint8_t *buf);
};

void MouseRptParser::ParseHIDData(USBHID *hid, uint8_t ep, bool is_rpt_id,
                                  uint8_t len, uint8_t *buf) {
  //    DEBUG_PRINTLN("Mouse");
  Serial.write((uint8_t)0xFD);  // BT-HID: Start Byte
  Serial.write((uint8_t)0x05);  // BT-HID: Data Length
  // Serial.write((uint8_t)len+1); // BT-HID: Data Length
  Serial.write((uint8_t)0x02);  // BT-HID: Descriptor (1=Keyboard, 2=Mouse,
                                // 3=KybdMouse combo)
  // Mouse Data consists of a single byte for pressed buttons
  // Followed by 0x00
  // Followed by 1 byte each for X then Y movement
  // Lastly 1 byte for wheel(optional?)
  HIDRelay::ParseHIDData(hid, ep, is_rpt_id, len, buf);
  if (len < 4) {
    //      DEBUG_PRINTLN("ZeroPad");
    for (uint8_t i = 1; len + i < 5; i++) Serial.write((uint8_t)0x00);
  }
};
#endif

USB Usb;
#ifdef _COMBO
// USBHub     Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD | USB_HID_PROTOCOL_MOUSE> HidComposite(&Usb);
HIDBoot<USB_HID_PROTOCOL_MOUSE> HidMouse(&Usb);
#endif
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);

uint32_t next_time;

KbdRptParser KbdPrs(&Usb);
#ifdef _COMBO
MouseRptParser MousePrs(&Usb);
#endif

void setup() {
  ADCSRA = 0;           // disable ADC by setting ADCSRA register to 0
  power_adc_disable();  // disable the clock to the ADC module
  Serial.begin(115200);

#if !defined(__MIPSEL__)
  while (!Serial)
    ;  // Wait for serial port to connect - used on Leonardo, Teensy and other
       // boards with built-in USB CDC serial connection
#endif

  DEBUG_PRINTLN("Start");

  if (Usb.Init() == -1) DEBUG_PRINTLN("OSC did not start.");

  delay(200);

  next_time = millis() + 5000;

  //#ifdef _COMBO
  //    HidComposite.SetReportParser(0, &KbdPrs);
  //  HidComposite.SetReportParser(1, &MousePrs);
  // HidMouse.SetReportParser(0, &MousePrs);
  //#endif
  // HidKeyboard.SetReportParser(0, &KbdPrs);
}

void loop() { 
  Usb.Task(); 
   /* reading the keyboard */
       if(Serial.available()) {
         uint8_t data= Serial.read();
         /* sending to the phone */
        uint8_t  rcode = KbdPrs.SndRpt(1, &data);
       }
}
