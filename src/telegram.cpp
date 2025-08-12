/*
Complete project details at https://RandomNerdTutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/
https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
*/
#include <main.h>
#include <MyTelegramBot.h>
#include "server.h"

extern char botToken[], chatID [];
extern MyTelegramBot bot;

bool botSetup(){
  return bot.setMyCommands(MAIN_MENU);
}

/******************************************************************
* errors = 0x01   // ОШИБКА ДАТЧИКА 0  199-потерян; 66,0-завис [E01]
* errors = 0x02   // ОШИБКА ДАТЧИКА 1  199-потерян; 66,0-завис [E02]
* errors = 0x04   // ОТКЛОНЕНИЕ КАНАЛ 0 [E04]
* errors = 0x08   // ОТКЛОНЕНИЕ КАНАЛ 1 [E08]
*******************************************************************/
// void sendErrMessages(int err){
//   String errMess = WORD_TITLE + String("Клімат-5.25") + ID_TITLE + String(1) + NEW_STR + NEW_STR;
//   if(errorsFlag.value & 1) errMess += SENSOR_ERROR_1;
//   if(errorsFlag.value & 2) errMess += SENSOR_ERROR_2;
//   if(errorsFlag.value & 4){
//     errMess += SENSOR_ERROR_4;
//     errMess += WORD_T1 + getFloat((float)ds[0].pvT/10,0) + NEW_STR;
//   }
//   if(errorsFlag.value & 8){
//     errMess += SENSOR_ERROR_8;
//     errMess += WORD_T2 + getFloat((float)ds[1].pvT/10,0) + NEW_STR;
//   }
//   errMess += "```";
//   bot.sendMessage(chatID, errMess, "Markdown");
//   // String keyboardJson = "[[{ \"text\" : \"Get a report\",  \"callback_data\" : \"/status\" }],[{ \"text\" : \"Help\", \"callback_data\" : \"/start\" }]]";
//   // bot.sendMessageWithInlineKeyboard(chatID, errMess, "Markdown", keyboardJson);
// }

void sendStatus(){
  char txt[20];
  String string;
  uint8_t num = settings.deviceNum & 0x0F;
  String welcome = WORD_TITLE + String(version) + ID_TITLE + String(num) + NEW_STR + NEW_STR;
  welcome += WORD_T1 + String(ds[0].pvT) + NEW_STR;
  // if(detectedSensor == SENSOR_DHT22) welcome += WORD_HUMIDITY + String(ds[1].pvT) + "%" + NEW_STR;
  // else welcome += WORD_T2 + String(ds[1].pvT) + NEW_STR;
  // snprintf(txt, sizeof(txt),"%02u:%02u [%02u - %02u]",timeinfo->tm_hour,timeinfo->tm_min, settings.timerOn, settings.timerOff);
  // if(LIGHT) welcome += WORD_LIGHT + String("↓ ") + String(txt) + NEW_STR;
  // else welcome += WORD_LIGHT + String("↑ ") + String(txt) + NEW_STR;
  // if(pvTimeR1 == -1){
  //     welcome += WORD_TIMER1 + String("немає дозволу");
  // } else {
  //     if(RELAY1){    //-- OFF --
  //         uint8_t day = pvTimeR1 / 1440;
  //         uint8_t hour = (pvTimeR1 % 1440) / 60;
  //         uint8_t min = pvTimeR1 % 60;
  //         welcome += WORD_TIMER1 + String("↓ вимкн. ")+String(day)+String("д.")+String(hour)+String("г.")+String(min)+String("х."); // ↓вимкн.0 дiб. 00:00
  //     } else {      //-- ON --
  //         welcome += WORD_TIMER1+String("↑ увімкн. ")+String(pvTimeR1)+String(" хвл."); // ↑увімкн.19 хвл.
  //     }
  // } 
  // welcome += NEW_STR;
  // if(pvTimeR2 == -1){
  //     welcome += WORD_TIMER2 + String("немає дозволу");
  // } else {
  //     if(RELAY2){    //-- OFF --
  //         uint8_t day = pvTimeR2 / 1440;
  //         uint8_t hour = (pvTimeR2 % 1440) / 60;
  //         uint8_t min = pvTimeR2 % 60;
  //         welcome += WORD_TIMER2 + String("↓ вимкн. ")+String(day)+String("д.")+String(hour)+String("г.")+String(min)+String("х."); // ↓вимкн.0 дiб. 00:00
  //     } else {      //-- ON --
  //         welcome += WORD_TIMER2+String("↑ увімкн. ")+String(pvTimeR1)+String(" хвл."); // ↑увімкн.19 хвл.
  //     }
  // } 
  // welcome += NEW_STR;
  // if(pvTimeR3 == -1){
  //     welcome += WORD_TIMER3 + String("немає дозволу");
  // } else {
  //     if(RELAY3){    //-- OFF --
  //         uint8_t day = pvTimeR3 / 1440;
  //         uint8_t hour = (pvTimeR3 % 1440) / 60;
  //         uint8_t min = pvTimeR3 % 60;
  //         welcome += WORD_TIMER3 + String("↓ вимкн. ")+String(day)+String("д.")+String(hour)+String("г.")+String(min)+String("х."); // ↓вимкн.0 дiб. 00:00
  //     } else {      //-- ON --
  //         welcome += WORD_TIMER3+String("↑ увімкн. ")+String(pvTimeR1)+String(" хвл."); // ↑увімкн.19 хвл.
  //     }
  // } 
  // welcome += NEW_STR;
  // welcome += WORD_DAMPER + String(pvFlap) + "%" + NEW_STR;
  // if((settings.program & 0xF) == 0) string = "немає";
  //   else string = "№" + String(settings.program & 0xF);
  // welcome += WORD_PROGRAM + string + NEW_STR;
  // snprintf(txt,sizeof(txt),"%02d.%02d.%04d %02d:%02d:%02d",
  //                     timeinfo->tm_mday, timeinfo->tm_mon + 1,
  //                     timeinfo->tm_year + 1900, timeinfo->tm_hour,
  //                     timeinfo->tm_min, timeinfo->tm_sec);
  // welcome += WORD_DATE + String(txt) + NEW_STR;
  // if(errorsFlag.value){
  //     if(ERROR1) welcome += WORD_ERROR1 + String(errorsFlag.value) + NEW_STR;
  //     if(ERROR2) welcome += WORD_ERROR2 + String(errorsFlag.value) + NEW_STR;
  //     if(ERROR4) welcome += WORD_ERROR4 + String(errorsFlag.value) + NEW_STR;
  //     if(ERROR8) welcome += WORD_ERROR8 + String(errorsFlag.value) + NEW_STR;
  // }
  
  welcome += "```";
  bot.sendMessage(chatID, welcome, "Markdown");
}

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
    DEBUG_PRINTLN("handleNewMessages: "+String(numNewMessages));
    for (int i=0; i<numNewMessages; i++) {
        // Chat id of the requester
        String chat_id = String(bot.messages[i].chat_id);
        if (chat_id != chatID){
          bot.sendMessage(chat_id, "Unauthorized user", "");
          continue;
        }
        
        // Print the received message
        String text = bot.messages[i].text;
        DEBUG_PRINTLN("received message: " + text);
    
        String from_name = bot.messages[i].from_name;
    
        if (text == TXT_START) {
          String welcome = "Welcome, " + from_name + ".\n";
          welcome += "Use the following commands to control your outputs.\n\n";
          welcome += "/led_on to turn GPIO ON \n";
          welcome += "/led_off to turn GPIO OFF \n";
          welcome += "/state to request current GPIO state \n";
          bot.sendMessage(chat_id, welcome, "");
        }
        if (text == TXT_OPTIONS){
          String keyboardJson = "[[{ \"text\" : \"Go to Graviton\", \"url\" : \"https://graviton.com.ua/ua/\" }],[{ \"text\" : \"Send\", \"callback_data\" : \"/start\" }]]";
          bot.sendMessageWithInlineKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson);
        }
        if (text == TXT_STATUS) sendStatus();
    }
}
  
//callback notifying us of the need to save config
void saveConfigCallback() {
    DEBUG_PRINTLN("Should save config");
    shouldSaveConfig = true;
}