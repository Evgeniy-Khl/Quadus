#include "keypad.h"

void checkkey(uint8_t key){
  // beeperOn(5);     // Включаем бипер
  if(numSetup){     //==== режим РЕДАКТИРОВАНИЯ УСТАВОК И ПАРАМЕТРОВ ======
    resetDispl = RESETDISPLAY; // удерживаем режим установок 10 сек.
    switch (key){
        case KEY_1: waitCheckKeyPad = WAITCHECKKEYPAD;
                    if (++numSetup > 15) numSetup=1;         // Меню пользователя
                    switch (numSetup){
                        case 1:  editBuff = settings.spT0on; break;     // У1 Уставка температуры T0 ON
                        case 2:  editBuff = settings.spT0off; break;    // У2 Уставка температуры T0 OFF
                        case 3:  editBuff = settings.spT1on; break;     // У3 Уставка температуры T1 или 0-100 Уставка относительной влажности ON
                        case 4:  editBuff = settings.spT1off; break;    // У4 Уставка температуры T1 или 0-100 Уставка относительной влажности OFF
                        case 5:  editBuff = settings.water0on; break;   // У5 Длительность включ.состояниe полива № 1
                        case 6:  editBuff = settings.water0off; break;  // У6 Длительность отключ.состояниe полива № 1
                        case 7:  editBuff = settings.water1on; break;   // У7 Длительность включ.состояниe полива № 2
                        case 8:  editBuff = settings.water1off; break;  // У8 Длительность отключ.состояниe полива № 2
                        case 9:  editBuff = settings.water2on; break;   // У9 Длительность включ.состояниe полива № 3
                        case 10: editBuff = settings.water2off; break;  // У10 Длительность отключ.состояниe полива № 3
                        case 11: editBuff = settings.flap; break;       // У11 Заслонка текущее положение
                        case 12: editBuff = settings.timerOn; break;    // У12 Освещение ON
                        case 13: editBuff = settings.timerOff; break;   // У13 Освещение OFF
                        case 14: editBuff = settings.alarm0; break;     // У14 отклонение t0
                        case 15: editBuff = settings.alarm1; break;     // У15 отклонение t1
                        case 16: editBuff = settings.deviceNum; break;  // У16 тномер прибора
                        case 17: editBuff = settings.program; break;    // У17 номер программы
                      }; 
          break;
        case KEY_2: if(keyCount == 1) waitCheckKeyPad = WAITCHECKKEYPAD;
                    editBuff++; 
                    if(waitCheckKeyPad > MINWAIT) waitCheckKeyPad -= 100; 
                    switch (numSetup){
                      case 1:  if(editBuff > 120) editBuff = 120;  break;
                      case 2:  if(editBuff > 120) editBuff = 120;  break;
                      case 3:  if(editBuff > 100) editBuff = 100;  break;
                      case 4:  if(editBuff > 100) editBuff = 100;  break;
                      case 5:  if(editBuff > 120) editBuff = 120;  break;
                      case 6:  if(editBuff >  15) editBuff =  15;  break;
                      case 7:  if(editBuff > 120) editBuff = 120;  break;
                      case 8:  if(editBuff >  15) editBuff =  15;  break;
                      case 9:  if(editBuff > 120) editBuff = 120;  break;
                      case 10: if(editBuff >  15) editBuff =  15;  break;
                      case 11: if(editBuff > 100) editBuff = 100;  break;
                      case 12: if(editBuff >  24) editBuff =  24;  break;
                      case 13: if(editBuff >  24) editBuff =  24;  break;
                      case 14: if(editBuff >  24) editBuff =  24;  break;
                      case 15: if(editBuff >  24) editBuff =  24;  break;
                      case 16: if(editBuff > 120) editBuff = 120;  break;
                      case 17: if(editBuff >   4) editBuff =   4;  break;
                    }
          break;
        // case KEY_3: waitCheckKeyPad = WAITCHECKKEYPAD;
        //             ++numSetup;
        //             if (numSetup > 31 || numSetup < 16) numSetup = 16;// Меню специалиста
                    
        //   break;
        case KEY_4: if(keyCount == 1) waitCheckKeyPad = WAITCHECKKEYPAD;
                    editBuff--; 
                    if(waitCheckKeyPad > MINWAIT) waitCheckKeyPad -= 100; 
                    switch (numSetup){
                      case 1:   if(editBuff <  10) editBuff =  10;  break;
                      case 2:   if(editBuff <  10) editBuff =  10;  break;
                      case 3:   if(editBuff <  10) editBuff =  10;  break;
                      case 4:   if(editBuff <  10) editBuff =  10;  break;
                      default:  if(editBuff <   0) editBuff =   0;  break;
                    }
          break;
        case KEY_7_2_4: reset();
                      beeperOn(50);
                      lcd.clear();
                      lcd.setCursor(0,0);
                      lcd.print("SETTINGS");
                      lcd.setCursor(0,1);
                      lcd.print("RESET");
                      delay(3000);
          break;
        case KEY_7_4_6: settings.special |= 0x07; saveConfig(); // reset WiFi settings
                      beeperOn(50); 
                      lcd.clear();
                      lcd.setCursor(0,0);
                      lcd.print("RESET WiFi");
                      lcd.setCursor(0,1);
                      lcd.print("REBOOT...");
                      delay(3000);
                      ESP.restart();
          break;
        case KEY_7_4_6_8: LittleFS.format();  // форматирование
                      beeperOn(100);
                      lcd.clear();
                      lcd.setCursor(0,0);
                      lcd.print("FORMAT FS");
                      lcd.setCursor(0,1);
                      lcd.print("REBOOT...");
                      delay(3000);
                      ESP.restart();
          break;
        case KEY_8: saveset(); break;
        default:    waitCheckKeyPad = WAITCHECKKEYPAD;
      }; 
  } else {  //==================== ОСНОВНОЙ РЕЖИМ РАБОТЫ =================================
    waitCheckKeyPad = WAITCHECKKEYPAD;
    switch (key) {
        // case KEY_1: numSetup = 1; editBuff = settings.spT0on; resetDispl = RESETDISPLAY; break;
        case KEY_2: if(++displNum > 3) displNum = 0; break;
        // case KEY_3:    break;
        case KEY_4: if(--displNum < 0) displNum = 3; break;
        // case KEY_5: break;
        // case KEY_6: break;
        // case KEY_7: break;
        
        // #ifdef DEBUG
        //   case KEY_7_1: errorsFlag.value = 0; ERROR1 = 1; break;
        //   case KEY_7_2: errorsFlag.value = 0; ERROR2 = 1; break;
        //   case KEY_7_3: errorsFlag.value = 0; ERROR4 = 1; break;
        //   case KEY_7_4: errorsFlag.value = 0; ERROR8 = 1; break;
        //   case KEY_7_5: errorsFlag.value = 0; ERROR10 = 1; break;
        //   case KEY_7_6: errorsFlag.value = 0; FROZE = 1;  break;
        //   case KEY_7_8: errorsFlag.value = 0; OVERHEAT = 1; break;
        //   case KEY_8: errorsFlag.value = 0; break;
        // #else
        // case KEY_8: if(errors && disableBeep==0) disableBeep=RESETDISPLAY;
        // #endif
          //  case KEY_4_3_2: pwTriac1=maxRun; CN2 = CN2ON; break;
          //  case KEY_5_2:   pvVenting+=10; DoAeration=1; beepOn=150; waitCheckKeyPad = WAITCHECKKEYPAD; break;               // ПРОВЕТРИВАНИЕ начато 
          //  case KEY_5_4:   pvWait=aeration[0]; DoAeration=0; pvFlap=flpNow; break;                               // ПРОВЕТРИВАНИЕ закончено
          //  case KEY_8_6_5: date = start(); if (programm) prg_stepoint(date,1); break;                        // старт новой инкубации
          //  case KEY_7:     if(programm){setprgday=1; read_prg(setprgday, programm); waitset=15;} break;      // просмотр, редактирование пр-мы.
    };
  };
}

void saveset(void){
  switch (numSetup){ //---------------------- Меню пользователя ---------------------------------------
      case 1:  settings.spT0on = editBuff; break;     // У1 Уставка температуры T0 ON
      case 2:  settings.spT0off = editBuff; break;    // У2 Уставка температуры T0 OFF
      case 3:  settings.spT1on = editBuff; break;     // У3 Уставка температуры T1 или 0-100 Уставка относительной влажности ON
      case 4:  settings.spT1off = editBuff; break;    // У4 Уставка температуры T1 или 0-100 Уставка относительной влажности OFF
      case 5:  settings.water0on = editBuff; break;   // У5 Длительность включ.состояниe полива № 1
      case 6:  settings.water0off = editBuff; break;  // У6 Длительность отключ.состояниe полива № 1
      case 7:  settings.water1on = editBuff; break;   // У7 Длительность включ.состояниe полива № 2
      case 8:  settings.water1off = editBuff; break;  // У8 Длительность отключ.состояниe полива № 2
      case 9:  settings.water2on = editBuff; break;   // У9 Длительность включ.состояниe полива № 3
      case 10: settings.water2off = editBuff; break;  // У10 Длительность отключ.состояниe полива № 3
      case 11: settings.flap =  editBuff; break;      // У11 Заслонка текущее положение
      case 12: settings.timerOn = editBuff; break;    // У12 Освещение ON
      case 13: settings.timerOff = editBuff; break;   // У13 Освещение OFF
      case 14: settings.alarm0 =  editBuff; break;    // У14 отклонение t0
      case 15: settings.alarm1 = editBuff; break;     // У15 отклонение t1
      case 16: settings.deviceNum = editBuff;  break; // У16 тномер прибора
      case 17: settings.program = editBuff;  break;   // У17 номер программы
  };
  saveConfig();
  numSetup=0;
}
