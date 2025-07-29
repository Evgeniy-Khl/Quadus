#include "keypad.h"

void checkkey(uint8_t key){
    resetDispl = RESETDISPLAY; // удерживаем режим установок 10 сек.
    switch (displNum){
    case 1:        
        portOut.value = 0xFF;
        switch (key){
            case KEY_1: TURN = 0; break;
            case KEY_2: HEATER = 0; break;
            case KEY_3: HUMIDI = 0; break;
            case KEY_4: RELAY1 = 0; break;
            case KEY_5: RELAY2 = 0; break;
            case KEY_6: RELAY3 = 0; break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }
        writePCF8574(portOut.value);
      break;
    case 2:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,120); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,  0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,120); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,  0); break;
            case KEY_5: editBuff0 = settings.spT0off; editBuff1 = settings.spT0on; break;
            case KEY_6:
                settings.spT0off = editBuff0;
                settings.spT0on  = editBuff1;
                saveConfig();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }        
      break;
    case 3:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,100); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,  0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1,100); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,  0); break;
            case KEY_5: editBuff0 = settings.spT1off; editBuff1 = settings.spT1on; break;
            case KEY_6:
                settings.spT1off = editBuff0;
                settings.spT1on  = editBuff1;
                saveConfig();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }          
      break;
    case 4:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,120); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,  0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1, 15); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,  0); break;
            case KEY_5: editBuff0 = settings.water0on; editBuff1 = settings.water0off; break;
            case KEY_6:
                settings.water0on  = editBuff0;
                settings.water0off = editBuff1;
                saveConfig();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }          
      break;
    case 5:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,120); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,  0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1, 15); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,  0); break;
            case KEY_5: editBuff0 = settings.water1on; editBuff1 = settings.water1off; break;
            case KEY_6:
                settings.water1on  = editBuff0;
                settings.water1off = editBuff1;
                saveConfig();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }          
      break;
    case 6:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,120); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,  0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1, 15); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,  0); break;
            case KEY_5: editBuff0 = settings.water2on; editBuff1 = settings.water2off; break;
            case KEY_6:
                settings.water2on  = editBuff0;
                settings.water2off = editBuff1;
                saveConfig();   // 5 сек. кнопка не доступна
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
                saveConfig();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    case 8:
        switch (key){
            case KEY_1: editBuff0 = incrVal(editBuff0,120); break;
            case KEY_2: editBuff0 = decrVal(editBuff0,  0); break;
            case KEY_3: editBuff1 = incrVal(editBuff1, 15); break;
            case KEY_4: editBuff1 = decrVal(editBuff1,  0); break;
            case KEY_5: editBuff0 = settings.alarm0; editBuff1 = settings.alarm1; break;
            case KEY_6:
                settings.alarm0 = editBuff0;
                settings.alarm1 = editBuff1;
                saveConfig();   // 5 сек. кнопка не доступна
              break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    default:
        NEWSCREEN = 1;
        switch (key){
            case KEY_1: displNum = 2; lcd.clear(); break;
            case KEY_2: displNum = 3; lcd.clear(); break;
            case KEY_3: displNum = 4; lcd.clear(); break;
            case KEY_4: displNum = 5; lcd.clear(); break;
            case KEY_5: displNum = 6; lcd.clear(); break;
            case KEY_6: displNum = 7; lcd.clear(); break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }
      break;
    }

}

void displIncr(){
    waitCheckKeyPad = WAITCHECKKEYPAD;
    NEWSCREEN = 1;
    if(++displNum > 8) displNum = 0; 
    lcd.clear();
}

void displDecr(){
    waitCheckKeyPad = WAITCHECKKEYPAD;
    NEWSCREEN = 1;
    if(--displNum < 0) displNum = 8; 
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