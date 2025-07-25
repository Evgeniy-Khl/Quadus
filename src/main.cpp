#include "main.h"
#include "my_settings.h"
#include <Wire.h> 

byte writePCF8574(byte data);

TM1638 module(13, 14, 12);    // Создаем объект module для TM1638
  // void ledDisplKeypad(long now);
  void ledSet(void);

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x20, 16, 2);

void setup()
{
	// initialize the LCD
	lcd.begin();

	// Turn on the blacklight and print a message.
	lcd.backlight();
	lcd.print("ESP8266 & I2C");
  uint8_t temp = writePCF8574(0xFF);
  DEBUG_PRINT("temp="); DEBUG_PRINTLN(temp);
}

void loop(){
  char txt[16];
	long now = millis();
  //-------------------------------------------- 10 mSec. --------------------------------------
  if(now - counter10 > 10){
    counter10 = now;

    if(beepOn) beepOn--; else digitalWrite(BEEP_PIN, HIGH);   // Выключаем бипер

    keys = module.getButtons();
    if(keys == 0) {waitCheckKeyPad = MINWAIT; keyCount = 0;}  // если не удерживается ни одна кнопка то сброс времени ожидания.
  }
  //-------------------------------------------- КЛАВИАТУРА --------------------------------------
    if(now - counterWait > waitCheckKeyPad){
      counterWait = now;
      keys = module.getButtons();
      
      if(lastKey == keys && keys > 0){
        keyCount++;
        checkkey(keys);
        // if(numSetup == 0) ledDispl(displNum);
        // else display_setup();
      } 
      else if(keys == 0) {waitCheckKeyPad = MINWAIT; keyCount = 0;}
      else lastKey = keys;
    }

  //============================= НОВАЯ ПОЛ-СЕКУНДА =================================
  if(now - counter1s > 1000){
    counter1s = now; 
    if(++halfSecond > 119) halfSecond = 0;
    // uint8_t temp = writePCF8574(halfSecond & 1);
    // DEBUG_PRINT("temp="); DEBUG_PRINTLN(temp);
    lcd.setCursor(0,1);
    // lcd.print("seconds:"); lcd.print(halfSecond);
    sprintf(txt,"sec:%3u; k=%3u",halfSecond,keys);
    lcd.print(txt);

  }
}

// Функция для записи байта на PCF8574
byte writePCF8574(byte data) {
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(data);
  byte error = Wire.endTransmission();
  if (error == 0) {
    //DEBUG_PRINT("Data written: 0b");
    //printBinary(data);
    //DEBUG_PRINTLN();
  } else {
    DEBUG_PRINT("\nError writing to PCF8574. Error code: ");
    DEBUG_PRINTLN(error);
  }
  return error;
}