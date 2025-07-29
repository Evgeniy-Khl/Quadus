#include "keypad.h"

void checkkey(uint8_t key){
    switch (displNum){
    case 1:        
        resetDispl = RESETDISPLAY; // удерживаем режим установок 10 сек.
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
            case KEY_1:  break;
            case KEY_2:  break;
            case KEY_3:  break;
            case KEY_4:  break;
            case KEY_5:  break;
            case KEY_6:  break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }        
      break;
    case 3:
        switch (key){
            case KEY_1:  break;
            case KEY_2:  break;
            case KEY_3:  break;
            case KEY_4:  break;
            case KEY_5:  break;
            case KEY_6:  break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }          
      break;
    case 4:
        switch (key){
            case KEY_1:  break;
            case KEY_2:  break;
            case KEY_3:  break;
            case KEY_4:  break;
            case KEY_5:  break;
            case KEY_6:  break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }          
      break;
    case 5:
        switch (key){
            case KEY_1:  break;
            case KEY_2:  break;
            case KEY_3:  break;
            case KEY_4:  break;
            case KEY_5:  break;
            case KEY_6:  break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }          
      break;
    case 6:
        switch (key){
            case KEY_1:  break;
            case KEY_2:  break;
            case KEY_3:  break;
            case KEY_4:  break;
            case KEY_5:  break;
            case KEY_6:  break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    case 7:
        switch (key){
            case KEY_1:  break;
            case KEY_2:  break;
            case KEY_3:  break;
            case KEY_4:  break;
            case KEY_5:  break;
            case KEY_6:  break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    case 8:
        switch (key){
            case KEY_1:  break;
            case KEY_2:  break;
            case KEY_3:  break;
            case KEY_4:  break;
            case KEY_5:  break;
            case KEY_6:  break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    default:
        switch (key){
            case KEY_1:  break;
            case KEY_2:  break;
            case KEY_3:  break;
            case KEY_4:  break;
            case KEY_5:  break;
            case KEY_6:  break;
            case KEY_7: displIncr(); break;
            case KEY_8: displDecr(); break;
        }  
      break;
    }

}

void displIncr(){
    if(++displNum > 8) displNum = 0; 
    lcd.clear();
}

void displDecr(){
    if(--displNum < 0) displNum = 8; 
    lcd.clear();
}