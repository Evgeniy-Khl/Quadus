#include "keypad.h"

void checkkey(uint8_t key){
  // uint8_t topUser=31, topOwner=15, botUser=16;
  // beeperOn(5);     // Включаем бипер
  if(numSetup){     //==== режим РЕДАКТИРОВАНИЯ УСТАВОК И ПАРАМЕТРОВ ======
    resetDispl = RESETDISPLAY; // удерживаем режим установок 10 сек.
    switch (key){
        case KEY_1: waitCheckKeyPad = WAITCHECKKEYPAD;
                    if (++numSetup > 15) numSetup=1;         // Меню пользователя
                    switch (numSetup){
                        case 1:  editBuff = settings.spT0on; break;          // У1 уставка канал 1
                        case 2:  editBuff = settings.spT0off; break;          // У1 уставка канал 1
                        case 3:  editBuff = settings.spT1on; break;
                        case 4:  editBuff = settings.spT1off; break;
                        case 5:  editBuff = settings.water0on; break;        // У3 время отключенного состояния
                        case 6:  editBuff = settings.water0off; break;        // У3 время отключенного состояния
                        case 7:  editBuff = settings.water1on; break;        // У4 время включенного состояния (если не 0 то это секунды)
                        case 8:  editBuff = settings.water1off; break;        // У4 время включенного состояния (если не 0 то это секунды)
                        case 9:  editBuff = settings.water2on; break;     // У5 ПАУЗА ПРОВЕТРИВАНИЯ (минут)
                        case 10: editBuff = settings.water2off; break;     // У5 ПАУЗА ПРОВЕТРИВАНИЯ (минут)
                        case 11: editBuff = settings.flap; break;     // У6 ДЛИТЕЛЬНОСТЬ ПРОВЕТРИВАНИЯ (секунд)
                        case 12: editBuff = settings.timerOn; break;       // У7 включение охлаждения по каналу 1
                        case 13: editBuff = settings.timerOff; break;      // У8 включение охлаждения по каналу 1
                        case 14: editBuff = settings.alarm0; break;       // У9 включение охлаждения по каналу 2
                        case 15: editBuff = settings.alarm1; break;     // У10 включение охлаждения по каналу 2
                        case 16: editBuff = settings.deviceNum; break;       // У11 тревога по каналу 1
                        case 17: editBuff = settings.program; break;       // У12 тревога по каналу 2
                      }; 
          break;
        case KEY_2: if(keyCount == 1) waitCheckKeyPad = WAITCHECKKEYPAD;
                    editBuff++; 
                    if(waitCheckKeyPad > MINWAIT) waitCheckKeyPad -= 100; 
                    switch (numSetup){
                      case 1:  if(editBuff > 999) editBuff = 999;  break;
                      case 2:  if(editBuff > 999) editBuff = 999;  break;
                      case 3:  if(editBuff > 999) editBuff = 999;  break;
                      case 4:  if(editBuff > 999) editBuff = 999;  break;
                      case 5:  if(editBuff > 999) editBuff = 999;  break;
                      case 6:  if(editBuff > 999) editBuff = 999;  break;
                      case 7:  if(editBuff > 200) editBuff = 200;  break;
                      case 8:  if(editBuff > 150) editBuff = 150;  break;
                      case 9:  if(editBuff > 200) editBuff = 200;  break;
                      case 10: if(editBuff > 150) editBuff = 150;  break;
                      case 11: if(editBuff > 400) editBuff = 400;  break;
                      case 12: if(editBuff > 400) editBuff = 400;  break;
                      case 13: if(editBuff > 400) editBuff = 400;  break;
                      case 14: if(editBuff > 400) editBuff = 400;  break;
                      case 15: if(editBuff >   4) editBuff =   4;  break;
                      case 16: if(editBuff > 100) editBuff = 100;  break;
                      case 17: if(editBuff >   1) editBuff =   1;  break;
                    }
          break;
        case KEY_3: waitCheckKeyPad = WAITCHECKKEYPAD;
                    ++numSetup;
                    if (numSetup > 31 || numSetup < 16) numSetup = 16;// Меню специалиста
                    // switch (numSetup){
                    //     case 16: editBuff = settings.state; break;          // П01 текущее положение заслонки
                    //     case 17: editBuff = settings.mode; break;           // П02 = 0 задержка регулировки по влажному
                    //     case 18: editBuff = settings.mode; break;           // П03 = MINRELAYMODE релейный режим работы
                    //     case 19: editBuff = settings.extendMode; break;     // П04 = 0 0-СИРЕНА; 1-АВАРИЙНОЕ ОТКЛЮЧЕНИЕ;
                    //     case 20: editBuff = settings.extendMode; break;     // П05 = 1 1-ОХЛАЖДЕНИЕ; 2-ОСУШЕНИЕ; 3-ОХЛАЖДЕНИЕ + ОСУШЕНИЕ
                    //     case 21: editBuff = settings.pulse / 10; break;     // П06 = 0.1 - 10,0 сек.
                    //     case 22: editBuff = settings.pulse; break;          // П07 = 3000-  15 сек.
                    //     case 23: editBuff = settings.flapLimit; break;      // П08 = 40 close
                    //     case 24: editBuff = settings.flapLimit; break;      // П09 = 85 open
                    //     case 25: editBuff = settings.Kp; break;             // П10 = 20
                    //     case 26: editBuff = settings.Ki; break;             // П11 = 500
                    //     case 27: editBuff = settings.Kp; break;             // П12 = 15
                    //     case 28: editBuff = settings.Ki; break;             // П13 = 900 
                    //     case 29: editBuff = settings.spRH; break;           // П14 подстройка датчика HIH-5030-01
                    //     case 30: editBuff = settings.special; break;        // П15 таймаут для портала конфигурации WiFi
                    //     case 31: editBuff = settings.special; break;        // П16 номер прибора
                    // };
          break;
        case KEY_4: if(keyCount == 1) waitCheckKeyPad = WAITCHECKKEYPAD;
                    editBuff--; 
                    if(waitCheckKeyPad > MINWAIT) waitCheckKeyPad -= 100; 
                    switch (numSetup){
                      case 1:   if(editBuff <  10) editBuff =  10;  break;
                      case 2:   if(editBuff <  10) editBuff =  10;  break;
                      case 3:   if(editBuff <   1) editBuff =   1;  break;
                      case 4:   if(editBuff <   0) editBuff =   0;  break;
                      case 5:   if(editBuff <   1) editBuff =   1;  break;
                      case 6:   if(editBuff <   0) editBuff =   0;  break;
                      case 7:   if(editBuff <   1) editBuff =   1;  break;
                      case 8:   if(editBuff <   0) editBuff =   0;  break;
                      case 9:   if(editBuff <   1) editBuff =   1;  break;
                      case 10:  if(editBuff <   0) editBuff =   0;  break;
                      case 11:  if(editBuff <   5) editBuff =   5;  break;
                      case 12:  if(editBuff <   5) editBuff =   5;  break;
                      case 13:  if(editBuff <   5) editBuff =   5;  break;
                      case 14:  if(editBuff <   5) editBuff =   5;  break;
                      case 15:  if(editBuff <   0) editBuff =   0;  break;
                      case 16:  if(editBuff <   0) editBuff =   0;  break;
                      case 17:  if(editBuff <   0) editBuff =   0;  break;
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
        case KEY_1: numSetup = 1; editBuff = settings.spT0on; resetDispl = RESETDISPLAY; break;
        case KEY_2: if(settings.water0on) {pvTimer=settings.water1on;} 
                    else {pvTimer=settings.water2on;} 
                    TURN = PCF_ON;
                    writePCF8574(portOut.value);
            break;
        case KEY_3: if(++displNum > 4) displNum = 0;
                    resetDispl = RESETDISPLAY;
            break;
        case KEY_4: pvTimer=settings.water2on; 
                    TURN = PCF_OFF;
                    writePCF8574(portOut.value);
            break;
        case KEY_5: break;
        case KEY_6: break;
        case KEY_7: break;
        
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
      case 1:  settings.spT0on = editBuff; break;     // У1 уставка канал 1 (0.1 - 99.9 °C)
      case 2:  settings.spT0off = editBuff; break;    // У1 уставка канал 1 (0.1 - 99.9 °C)
      case 3:  settings.spT1on = editBuff; break;     // У2 уставка канал 2 (0.1 - 99.9 °C)
      case 4:  settings.spT1off = editBuff; break;    // У2 уставка канал 2 (0.1 - 99.9 °C)
      case 5:  settings.water0on = editBuff; break;   // У3 время отключенного состояния (1 - 999 мин.)
      case 6:  settings.water0off = editBuff; break;  // У4 время включенного состояния (0 - 999 сек.)
      case 7:  settings.water1on = editBuff; break;   // У5 ПАУЗА ПРОВЕТРИВАНИЯ (1 - 999 мин.)
      case 8:  settings.water1off = editBuff; break;  // У5 ПАУЗА ПРОВЕТРИВАНИЯ (1 - 999 мин.)
      case 9:  settings.water2on = editBuff; break;   // У6 ДЛИТЕЛЬНОСТЬ ПРОВЕТРИВАНИЯ (0 - 999 сек.)
      case 10: settings.water2off = editBuff; break;  // У6 ДЛИТЕЛЬНОСТЬ ПРОВЕТРИВАНИЯ (0 - 999 сек.)
      case 11: settings.flap =  editBuff; break;      // У7 включение охлаждения по каналу 1 (0,1 - 20,0 °C)
      case 12: settings.timerOn = editBuff; break;    // У8 включение охлаждения по каналу 1 (0,0 - 15,0 °C)
      case 13: settings.timerOff = editBuff; break;   // У8 включение охлаждения по каналу 1 (0,0 - 15,0 °C)
      case 14: settings.alarm0 =  editBuff; break;    // У9 включение охлаждения по каналу 2 (0,1 - 20,0 °C/%)
      case 15: settings.alarm1 = editBuff; break;     // У10 включение охлаждения по каналу 2 (0,0 - 15,0 °C/%)
      case 16: settings.deviceNum = editBuff;  break; // У11 тревога по каналу 1 (0,5 - 40,0 °C)
      case 17: settings.program = editBuff;  break;   // У12 тревога по каналу 2 (0,5 - 40,0 °C/%)
  };
  // saveConfig();
  numSetup=0;
}
