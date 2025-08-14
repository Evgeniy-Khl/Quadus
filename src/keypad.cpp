#include "keypad.h"

void checkkey(uint8_t key){
    resetDispl = RESETDISPLAY; // удерживаем режим установок 10 сек.
    switch (setupNum){
    case 1:        
        waitCheckKeyPad = WAITCHECKKEYPAD * 5;  // 5 сек. кнопка не доступна
        switch (key){
            case KEY_1: if(++dataOut[0] > 1) dataOut[0] = -1; break;
            case KEY_2: if(++dataOut[1] > 1) dataOut[1] = -1; break;
            case KEY_3: if(++dataOut[2] > 1) dataOut[2] = -1; break;
            case KEY_4: if(++dataOut[3] > 1) dataOut[3] = -1; break;
            case KEY_5: if(++dataOut[4] > 1) dataOut[4] = -1; break;
            case KEY_6: if(++dataOut[5] > 1) dataOut[5] = -1; break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }
        // преобразуем номер кнопки во включение одного реле
        for (uint8_t i = 0; i < 6; i++){
          uint8_t val = (1 << i);
          switch (dataOut[i]){
          case 0: portOut.value &= ~val; break;
          case 1: portOut.value |= val; break;
          default:  break;
          } 
        }
        DEBUG_PRINT("portOut.value="); DEBUG_PRINTLN(portOut.value);
        // writePCF8574(portOut.value);
      break;
    case 2:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,99); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,  0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,99); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,  0); break;
            case KEY_5: editBuff0 = settings.spT0on; editBuff1 = settings.spT0off; break;
            case KEY_6:
                settings.spT0on = editBuff0;
                settings.spT0off  = editBuff1;
                doSave();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }        
      break;
    case 3:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,99); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,  0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,99); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,  0); break;
            case KEY_5: editBuff0 = settings.spT1on; editBuff1 = settings.spT1off; break;
            case KEY_6:
                settings.spT1on = editBuff0;
                settings.spT1off  = editBuff1;
                doSave();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }          
      break;
    case 4:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,99); break;
            case KEY_2: editBuff0 = decrVal(editBuff0, 0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,15); break;
            case KEY_4: editBuff1 = decrVal(editBuff1, 0); break;
            case KEY_5: editBuff0 = settings.water0on; editBuff1 = settings.water0off; break;
            case KEY_6:
                settings.water0on  = editBuff0;
                settings.water0off = editBuff1;
                doSave();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }          
      break;
    case 5:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,99); break;
            case KEY_2: editBuff0 = decrVal(editBuff0, 0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,15); break;
            case KEY_4: editBuff1 = decrVal(editBuff1, 0); break;
            case KEY_5: editBuff0 = settings.water1on; editBuff1 = settings.water1off; break;
            case KEY_6:
                settings.water1on  = editBuff0;
                settings.water1off = editBuff1;
                doSave();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }          
      break;
    case 6:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,99); break;
            case KEY_2: editBuff0 = decrVal(editBuff0, 0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,15); break;
            case KEY_4: editBuff1 = decrVal(editBuff1, 0); break;
            case KEY_5: editBuff0 = settings.water2on; editBuff1 = settings.water2off; break;
            case KEY_6:
                settings.water2on  = editBuff0;
                settings.water2off = editBuff1;
                doSave();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    case 7:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,23); break;
            case KEY_2: editBuff0 = decrVal(editBuff0, 0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,23); break;
            case KEY_4: editBuff1 = decrVal(editBuff1, 0); break;
            case KEY_5: editBuff0 = settings.timerOn; editBuff1 = settings.timerOff; break;
            case KEY_6:
                settings.timerOn  = editBuff0;
                settings.timerOff = editBuff1;
                doSave();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    case 8:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,99); break;
            case KEY_2: editBuff0 = decrVal(editBuff0, 0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,99); break;
            case KEY_4: editBuff1 = decrVal(editBuff1, 0); break;
            case KEY_5: editBuff0 = settings.alarm0; editBuff1 = settings.alarm1; break;
            case KEY_6:
                settings.alarm0 = editBuff0;
                settings.alarm1 = editBuff1;
                doSave();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    case 9://---------- установка разрешений для реле ----------------
        waitCheckKeyPad = WAITCHECKKEYPAD * 5;  // 5 сек. кнопка не доступна
        switch (key){
            case KEY_1: if(++settings.modeRelay1 > 2) settings.modeRelay1 = 0; break;
            case KEY_2: if(++settings.modeRelay2 > 2) settings.modeRelay2 = 0; break;
            case KEY_3: if(++settings.modeRelay3 > 2) settings.modeRelay3 = 0; break;
            case KEY_6: doSave(); break;   // 5 сек. кнопка не доступна
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }
      break;
    case 10://---------- установка разрешений для температуры ----------------
        waitCheckKeyPad = WAITCHECKKEYPAD * 5;  // 5 сек. кнопка не доступна
        switch (key){
            case KEY_1: if(++settings.modeHeater > 2) settings.modeHeater = 0; break;
            case KEY_2: if(++settings.modeHumidi > 2) settings.modeHumidi = 0; break;
            case KEY_6: doSave(); break;   // 5 сек. кнопка не доступна
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }
      break;
    case 11:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,100); break;
            case KEY_2: editBuff0 = decrVal(editBuff0, 0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1, 4); break;
            case KEY_4: editBuff1 = decrVal(editBuff1, 0); break;
            case KEY_5: editBuff0 = settings.flap; editBuff1 = settings.program; break;
            case KEY_6:
                settings.flap = editBuff0;
                settings.program = editBuff1;
                doSave();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    case 12:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,99); break;
            case KEY_2: editBuff0 = decrVal(editBuff0, 0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1, 3); break;
            case KEY_4: editBuff1 = decrVal(editBuff1, 0); break;
            case KEY_5: editBuff0 = settings.deviceNum; editBuff1 = settings.special; break;
            case KEY_6:
                settings.deviceNum = editBuff0;
                settings.special = editBuff1;
                doSave();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    default:
        NEWSCREEN = 1;
        waitCheckKeyPad = WAITCHECKKEYPAD;
        switch (key){
            case KEY_1: displNum = 1; lcd.clear(); disableBeep = 600; break;
            case KEY_2: displNum = 2; lcd.clear(); disableBeep = 600; break;
            case KEY_3: displNum = 3; lcd.clear(); disableBeep = 600; break;
            case KEY_4: displNum = 4; lcd.clear(); disableBeep = 600; break;
            case KEY_5: displNum = 5; lcd.clear(); disableBeep = 600; break;
            case KEY_6: displNum = 0; lcd.clear(); disableBeep = 600; break;
            case KEY_6_1: displNum = 0; lcd.clear(); break;
            case KEY_6_2: displNum = 0; lcd.clear(); break;
            case KEY_6_3: displNum = 0; lcd.clear(); break;
            case KEY_6_4: displNum = 0; lcd.clear(); break;
            case KEY_6_5: displNum = 0; lcd.clear(); break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
            case KEY_6_5_1: reset(); break;//ESP.restart();
            case KEY_6_5_2: settings.special |= 0x04; doSave(); ESP.restart(); break;// syncTime();
            case KEY_6_5_3: settings.special |= 0x08; doSave(); ESP.restart(); break;// wifiManager.resetSettings()
            case KEY_6_5_4: LittleFS.format(); ESP.restart(); break;
        }
      break;
    }
}

void displIncr(){
    waitCheckKeyPad = WAITCHECKKEYPAD;
    NEWSCREEN = 1;
    if(++setupNum > 12) setupNum = 0; 
    lcd.clear();
}

void displDecr(){
    waitCheckKeyPad = WAITCHECKKEYPAD;
    NEWSCREEN = 1;
    if(--setupNum < 0) setupNum = 12; 
    lcd.clear();
}

int16_t incrVal(int16_t val, uint8_t max){
    if(keyCount == 1) waitCheckKeyPad = WAITCHECKKEYPAD;
    else if(waitCheckKeyPad > MINWAIT) waitCheckKeyPad -= 100;
    if(++val > max) val = max; 
    return val;
}

int16_t decrVal(int16_t val, uint8_t min){
    if(keyCount == 1) waitCheckKeyPad = WAITCHECKKEYPAD;
    else if(waitCheckKeyPad > MINWAIT) waitCheckKeyPad -= 100;
    if(--val < min) val = min; 
    return val;
}

void doSave(){
  saveSetPoint();   // 5 сек. кнопка не доступна
  setupNum = 0;
  resetDispl = 0;
}