#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include <hidboot.h>
#include <usbhub.h>

int PAYLOAD = 32;
void sendDataString(String str) {//发送数据，直接接受字符串形式参数
  Serial.println("Sending: " + str);
  byte data[PAYLOAD]; //缓存
  str.getBytes(data, PAYLOAD); //把字符串转换为byte数组，最大长度不超过32
  Mirf.send(data);//调用类方法来发送数据
  while (Mirf.isSending()) {}
}

class KbdRptParser : public KeyboardReportParser
{
  protected:
    void OnControlKeysChanged(uint8_t before, uint8_t after);
    void OnKeyDown  (uint8_t mod, uint8_t key);
    void OnKeyUp  (uint8_t mod, uint8_t key);
};

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {
  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;

  int modData = 0;
  modData += afterMod.bmLeftCtrl << 7;
  modData += afterMod.bmLeftShift << 6;
  modData += afterMod.bmLeftAlt << 5;
  modData += afterMod.bmLeftGUI << 4;
  modData += afterMod.bmRightCtrl << 3;
  modData += afterMod.bmRightShift << 2;
  modData += afterMod.bmRightAlt << 1;
  modData += afterMod.bmRightGUI;

  String modStr = "MD:";
  modStr.concat(modData);
  sendDataString(modStr);
}
void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  String typeStr = "DN:";
  typeStr.concat(key);
  sendDataString(typeStr);
}
void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
  String typeStr = "UP:";
  typeStr.concat(key);
  sendDataString(typeStr);
}

USB     Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);
KbdRptParser Prs;

void setup()
{
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  Mirf.setTADDR((byte *)"FGHIJ"); //设置接收端地址
  Mirf.setRADDR((byte *)"ABCDE"); //设置自己的地址（发送端地址），使用5个字符
  Mirf.payload = PAYLOAD;
  Mirf.channel = 4;              //设置所用信道
  Mirf.config();

  Serial.begin( 115200 );
  Serial.println("Start");
  sendDataString("Started!");

  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");

  delay( 200 );
  HidKeyboard.SetReportParser(0, &Prs);
}

void loop()
{
  Usb.Task();
}

