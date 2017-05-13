//接收端程序

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "HID-Project.h"

struct MODIFIERKEYS {
  uint8_t bmLeftCtrl : 1;
  uint8_t bmLeftShift : 1;
  uint8_t bmLeftAlt : 1;
  uint8_t bmLeftGUI : 1;
  uint8_t bmRightCtrl : 1;
  uint8_t bmRightShift : 1;
  uint8_t bmRightAlt : 1;
  uint8_t bmRightGUI : 1;
};

int PAYLOAD = 32;
String reciveDataString() {//从芯片里接收收到的信息
  byte data[PAYLOAD];//缓存
  String tmp; //字符串缓存
  Mirf.getData(data);    //接收数据到data数组
  for (int i = 0; i < sizeof(data); ++i ) { //遍历data数组转换字符
    tmp += char(data[i]);
    if (char(data[i]) == '\0')break; //拼接字符串直到完成
  }
  return tmp;
}

MODIFIERKEYS preMod;

void setup()
{
  Serial.begin(115200);
  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();

  Mirf.setRADDR((byte *)"FGHIJ"); //设置自己的地址（接收端地址），使用5个字符
  Mirf.payload = PAYLOAD;
  Mirf.channel = 4;   //设置使用的信道
  Mirf.config();
  Serial.println("Listening...");  //开始监听接收到的数据

  Keyboard.begin();
}



void loop()
{
  String tmp;
  if (Mirf.dataReady()) { //当接收到程序，便从串口输出接收到的数据
    tmp = reciveDataString();
    Serial.print("Got data: ");
    Serial.println(tmp);

    String mod = tmp.substring(0, 3);
    String dataStr = tmp.substring(3);
    uint8_t data = dataStr.toInt();

    //MOD键
    if (mod == "MD:") {
      MODIFIERKEYS curMod;
      curMod.bmLeftCtrl = (data & 128) >> 7;
      curMod.bmLeftShift = (data & 64) >> 6;
      curMod.bmLeftAlt = (data & 32) >> 5;
      curMod.bmLeftGUI = (data & 16) >> 4;
      curMod.bmRightCtrl = (data & 8) >> 3;
      curMod.bmRightShift = (data & 4) >> 2;
      curMod.bmRightAlt = (data & 2) >> 1;
      curMod.bmRightGUI = data & 1;

      if (curMod.bmLeftCtrl != preMod.bmLeftCtrl) {
        if (curMod.bmLeftCtrl == 1) {
          Keyboard.press(KEY_LEFT_CTRL);
        } else {
          Keyboard.release(KEY_LEFT_CTRL);
        }
      }
      if (curMod.bmLeftShift != preMod.bmLeftShift) {
        if (curMod.bmLeftShift == 1) {
          Keyboard.press(KEY_LEFT_SHIFT);
        } else {
          Keyboard.release(KEY_LEFT_SHIFT);
        }
      }
      if (curMod.bmLeftAlt != preMod.bmLeftAlt) {
        if (curMod.bmLeftAlt == 1) {
          Keyboard.press(KEY_LEFT_ALT);
        } else {
          Keyboard.release(KEY_LEFT_ALT);
        }
      }
      if (curMod.bmLeftGUI != preMod.bmLeftGUI) {
        if (curMod.bmLeftGUI == 1) {
          Keyboard.press(KEY_LEFT_GUI);
        } else {
          Keyboard.release(KEY_LEFT_GUI);
        }
      }
      if (curMod.bmRightCtrl != preMod.bmRightCtrl) {
        if (curMod.bmRightCtrl == 1) {
          Keyboard.press(KEY_RIGHT_CTRL);
        } else {
          Keyboard.release(KEY_RIGHT_CTRL);
        }
      }
      if (curMod.bmRightShift != preMod.bmRightShift) {
        if (curMod.bmRightShift == 1) {
          Keyboard.press(KEY_RIGHT_SHIFT);
        } else {
          Keyboard.release(KEY_RIGHT_SHIFT);
        }
      }
      if (curMod.bmRightAlt != preMod.bmRightAlt) {
        if (curMod.bmRightAlt == 1) {
          Keyboard.press(KEY_RIGHT_ALT);
        } else {
          Keyboard.release(KEY_RIGHT_ALT);
        }
      }
      if (curMod.bmRightGUI != preMod.bmRightGUI) {
        if (curMod.bmRightGUI == 1) {
          Keyboard.press(KEY_RIGHT_GUI);
        } else {
          Keyboard.release(KEY_RIGHT_GUI);
        }
      }
      preMod = curMod;
    }


    //按键释放
    else if (mod == "UP:") {
      Serial.println("release: " + dataStr);
      Keyboard.release(KeyboardKeycode(data));
    }
    //按键
    else if (mod == "DN:") {
      Serial.println("press: " + dataStr);
      Keyboard.press(KeyboardKeycode(data));
    }
  }
}



