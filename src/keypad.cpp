#include "keypad.h"

void checkkey(uint8_t key){
    resetDispl = RESETDISPLAY; // hold setup mode for 20-40 sec.
    switch (setupNum){
    case 1:        
        waitCheckKeyPad = WAITCHECKKEYPAD * 5;  // 5 sec lockout
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
        // convert key number to relay switching
        for (uint8_t i = 0; i < 6; i++){
          uint8_t val = (1 << i);
          switch (dataOut[i]){
          case 0: portOut.value |= val; break;  // 0 = OFF (set bit to 1)
          case 1: portOut.value &= ~val; break; // 1 = ON (clear bit to 0)
          default:  break;
          } 
        }
        DEBUG_PRINT("portOut.value="); DEBUG_PRINTLN(portOut.value);
      break;
    case 2: // t1 увiмкнути / вимкнути
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0, 999); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,   0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1, 999); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,   0); break;
            case KEY_5: editBuff0 = settings.spT0on; editBuff1 = settings.spT0off; break;
            case KEY_6:
                settings.spT0on = editBuff0;
                settings.spT0off  = editBuff1;
                doSave();
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }        
      break;
    case 3: // t2 увiмкнути / вимкнути
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0, 999); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,   0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1, 999); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,   0); break;
            case KEY_5: editBuff0 = settings.spT1on; editBuff1 = settings.spT1off; break;
            case KEY_6:
                settings.spT1on = editBuff0;
                settings.spT1off  = editBuff1;
                doSave();
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }          
      break;
    case 4: // setRelay(1)
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0, 120); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,   0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,  15); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,   0); break;
            case KEY_5: editBuff0 = settings.water0on; editBuff1 = settings.water0off; break;
            case KEY_6:
                settings.water0on  = (uint8_t)editBuff0;
                settings.water0off = (uint8_t)editBuff1;
                doSave();
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }          
      break;
    case 5: // setRelay(2)
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0, 120); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,   0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,  15); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,   0); break;
            case KEY_5: editBuff0 = settings.water1on; editBuff1 = settings.water1off; break;
            case KEY_6:
                settings.water1on  = (uint8_t)editBuff0;
                settings.water1off = (uint8_t)editBuff1;
                doSave();
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }          
      break;
    case 6: // setRelay(3)
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0, 120); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,   0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,  15); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,   0); break;
            case KEY_5: editBuff0 = settings.water2on; editBuff1 = settings.water2off; break;
            case KEY_6:
                settings.water2on  = (uint8_t)editBuff0;
                settings.water2off = (uint8_t)editBuff1;
                doSave();
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    case 7: //CB увімкнути / вимкнути
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0, 23); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,  0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1, 23); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,  0); break;
            case KEY_5: editBuff0 = settings.timerOn; editBuff1 = settings.timerOff; break;
            case KEY_6:
                settings.timerOn  = (uint8_t)editBuff0;
                settings.timerOff = (uint8_t)editBuff1;
                doSave();
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    case 8: //t1 тривога / t2 тривога
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0, 99); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,  0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1, 99); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,  0); break;
            case KEY_5: editBuff0 = settings.alarm0; editBuff1 = settings.alarm1; break;
            case KEY_6:
                settings.alarm0 = editBuff0;
                settings.alarm1 = editBuff1;
                doSave();
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    case 9: // setRelayMode()
        waitCheckKeyPad = WAITCHECKKEYPAD * 5;
        switch (key){
            case KEY_1: if(++settings.modeRelay1 > 2) settings.modeRelay1 = 0; break;
            case KEY_2: if(++settings.modeRelay2 > 2) settings.modeRelay2 = 0; break;
            case KEY_3: if(++settings.modeRelay3 > 2) settings.modeRelay3 = 0; break;
            case KEY_6: doSave(); break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }
      break;
    case 10:  // setClimatMode()
        waitCheckKeyPad = WAITCHECKKEYPAD * 5;
        switch (key){
            case KEY_1: if(++settings.modeHeater > 2) settings.modeHeater = 0; break;
            case KEY_2: if(++settings.modeHumidi > 2) settings.modeHumidi = 0; break;
            case KEY_6: doSave(); break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }
      break;
    case 11:  //Заслінка / Програма
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0, 100); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,   0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,   4); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,   0); break;
            case KEY_5: editBuff0 = settings.flap; editBuff1 = settings.program; break;
            case KEY_6:
                settings.flap = (uint8_t)editBuff0;
                settings.program = (uint8_t)editBuff1;
                doSave();
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    case 12:  //Номер / Спец
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0, 99); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,  0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,  3); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,  0); break;
            case KEY_5: editBuff0 = settings.deviceNum; editBuff1 = settings.special; break;
            case KEY_6:
                settings.deviceNum = (uint8_t)editBuff0;
                settings.special = (uint8_t)editBuff1;
                doSave();
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
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
            case KEY_6_5_1: reset(); break;
            case KEY_6_5_2: settings.special |= 0x04; doSave(); ESP.restart(); break;
            case KEY_6_5_3: settings.special |= 0x08; doSave(); ESP.restart(); break;
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

int16_t incrVal(int16_t val, int16_t max){
    if(keyCount == 1) waitCheckKeyPad = WAITCHECKKEYPAD;
    else if(waitCheckKeyPad > MINWAIT) waitCheckKeyPad -= 100;
    if(++val > max) val = max; 
    return val;
}

int16_t decrVal(int16_t val, int16_t min){
    if(keyCount == 1) waitCheckKeyPad = WAITCHECKKEYPAD;
    else if(waitCheckKeyPad > MINWAIT) waitCheckKeyPad -= 100;
    if(--val < min) val = min; 
    return val;
}

void doSave(){
  saveSetPoint();
  setupNum = 0;
  resetDispl = 0;
}
