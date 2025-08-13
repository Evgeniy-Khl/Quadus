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

void sendStatus() {
    // 1. Выделяем ОДИН большой буфер на стеке с запасом для всего сообщения.
    char welcomeMsg[1024];
    // 2. Вспомогательный буфер для форматирования и копирования.
    char tempBuffer[128];
    // 3. ОДИН временный объект String для всех операций конвертации из Flash.
    String tempString;

    // Безопасно получаем текущее время в начале функции.
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        MYDEBUG_PRINTLN("sendStatus: Failed to obtain time");
        bot.sendMessage(chatID, "Не вдалося отримати точний час.", "");
        // return;
    }

    uint8_t num = settings.deviceNum & 0x0F;

    // Используем указатели для безопасного и эффективного заполнения буфера.
    char* p = welcomeMsg;
    const char* end = welcomeMsg + sizeof(welcomeMsg); // Граница буфера

    // --- Формируем заголовок ---
    tempString = WORD_TITLE; // Присваиваем значение
    strcpy(tempBuffer, tempString.c_str());
    p += snprintf(p, end - p, "%s%u\n\n", tempBuffer, num);

    // --- Датчики ---
    tempString = WORD_T1; // Переиспользуем тот же объект
    strcpy(tempBuffer, tempString.c_str());
    p += snprintf(p, end - p, "%s%d °C\n", tempBuffer, ds[0].pvT);

    if (detectedSensor == SENSOR_DHT22) {
        tempString = WORD_HUMIDITY;
        strcpy(tempBuffer, tempString.c_str());
        p += snprintf(p, end - p, "%s%d%%\n", tempBuffer, ds[1].pvT);
    } else {
        tempString = WORD_T2;
        strcpy(tempBuffer, tempString.c_str());
        p += snprintf(p, end - p, "%s%d °C\n", tempBuffer, ds[1].pvT);
    }

    // --- Освещение ---
    snprintf(tempBuffer, sizeof(tempBuffer), "%02u:%02u [%02u - %02u]", timeinfo.tm_hour, timeinfo.tm_min, settings.timerOn, settings.timerOff);
    tempString = WORD_LIGHT;
    p += snprintf(p, end - p, "%s%s %s\n", tempString.c_str(), (LIGHT ? "↓" : "↑"), tempBuffer);
    
    // --- Таймер 1 ---
    // p += snprintf(p, end - p, "\n");
    tempString = WORD_TIMER1;
    strcpy(tempBuffer, tempString.c_str());
    if (pvTimeR1 == -1) {
        p += snprintf(p, end - p, "%sнемає дозволу", tempBuffer);
    } else {
        if (RELAY1) { //-- OFF --
            uint8_t day = pvTimeR1 / 1440;
            uint8_t hour = (pvTimeR1 % 1440) / 60;
            uint8_t min = pvTimeR1 % 60;
            p += snprintf(p, end - p, "%s↓ вимкн. %uд.%uг.%uх.", tempBuffer, day, hour, min);
        } else { //-- ON --
            p += snprintf(p, end - p, "%s↑ увімкн. %d хвл.", tempBuffer, pvTimeR1);
        }
    }

    // --- Таймер 2 ---
    p += snprintf(p, end - p, "\n");
    tempString = WORD_TIMER2;
    strcpy(tempBuffer, tempString.c_str());
    if (pvTimeR2 == -1) {
        p += snprintf(p, end - p, "%sнемає дозволу", tempBuffer);
    } else {
        if (RELAY2) { //-- OFF --
            uint8_t day = pvTimeR2 / 1440;
            uint8_t hour = (pvTimeR2 % 1440) / 60;
            uint8_t min = pvTimeR2 % 60;
            p += snprintf(p, end - p, "%s↓ вимкн. %uд.%uг.%uх.", tempBuffer, day, hour, min);
        } else { //-- ON --
            p += snprintf(p, end - p, "%s↑ увімкн. %d хвл.", tempBuffer, pvTimeR2);
        }
    }
    
    // --- Таймер 3 ---
    p += snprintf(p, end - p, "\n");
    tempString = WORD_TIMER3;
    strcpy(tempBuffer, tempString.c_str());
    if (pvTimeR3 == -1) {
        p += snprintf(p, end - p, "%sнемає дозволу", tempBuffer);
    } else {
        if (RELAY3) { //-- OFF --
            uint8_t day = pvTimeR3 / 1440;
            uint8_t hour = (pvTimeR3 % 1440) / 60;
            uint8_t min = pvTimeR3 % 60;
            p += snprintf(p, end - p, "%s↓ вимкн. %uд.%uг.%uх.", tempBuffer, day, hour, min);
        } else { //-- ON --
            p += snprintf(p, end - p, "%s↑ увімкн. %d хвл.", tempBuffer, pvTimeR3);
        }
    }
    
    p += snprintf(p, end - p, "\n");

    // --- Заслонка и Программа ---
    tempString = WORD_DAMPER;
    p += snprintf(p, end - p, "%s%d%%\n", tempString.c_str(), pvFlap);
    
    tempString = WORD_PROGRAM;
    if ((settings.program & 0x0F) == 0) {
        p += snprintf(p, end - p, "%sнемає\n", tempString.c_str());
    } else {
        p += snprintf(p, end - p, "%s№%d\n", tempString.c_str(), (settings.program & 0x0F));
    }

    // --- Дата и Время ---
    tempString = WORD_DATE;
    snprintf(tempBuffer, sizeof(tempBuffer), "%02d.%02d.%04d %02d:%02d:%02d",
             timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900,
             timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    p += snprintf(p, end - p, "%s%s\n", tempString.c_str(), tempBuffer);

    // --- Ошибки ---
    if (errorsFlag.value) {
        if (ERROR1) {
            tempString = WORD_ERROR1;
            p += snprintf(p, end - p, "%s\n", tempString.c_str());
        }
        if (ERROR2) {
            tempString = WORD_ERROR2;
            p += snprintf(p, end - p, "%s\n", tempString.c_str());
        }
        if (ERROR4) {
            tempString = WORD_ERROR4;
            p += snprintf(p, end - p, "%s\n", tempString.c_str());
        }
        if (ERROR8) {
            tempString = WORD_ERROR8;
            p += snprintf(p, end - p, "%s\n", tempString.c_str());
        }
    }

    // --- Завершаем Markdown блок ---
    strncat(welcomeMsg, "```", end - p - 1);

    // 5. Отправляем готовое, безопасно сформированное сообщение.
    bot.sendMessage(chatID, welcomeMsg, "Markdown");
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