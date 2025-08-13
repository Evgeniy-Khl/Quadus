#include "main.h"
void initWiFiManag(void){
    // The extra parameters to be configured (can be either global or just in the setup)
    // After connecting, parameter.getValue() will get you the configured value
    // id/name placeholder/prompt default length
    WiFiManagerParameter custom_botToken("botToken", "BOT token", botToken, 50);
    WiFiManagerParameter custom_chatID("chatID", "Chat ID", chatID, 11);

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    //set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    //add all your parameters here
    wifiManager.addParameter(&custom_botToken);
    wifiManager.addParameter(&custom_chatID);

    //------------------ reset settings ------------------------
    if(settings.special & 0x08){
      settings.special &= 0xF7;
      saveSetPoint();
      wifiManager.resetSettings();
    } 
    //----------------------------------------------------------
    //set minimu quality of signal so it ignores AP's under that quality
    //defaults to 8%
    //wifiManager.setMinimumSignalQuality();
    //----------------------------------------------------------
    uint8_t tt =  (settings.special & 0x03) * 60;
    MYDEBUG_PRINT("Устанавливаем таймаут для портала конфигурации (сек.):");
    MYDEBUG_PRINTLN(tt);
    //---- Устанавливаем таймаут для портала конфигурации в секундах ----
    lcd.clear();
    lcd.setCursor(0,0);
    myPrint(wordSet,sizeof(wordSet));
    lcd.setCursor(0,1);
    myPrint(timeout_,sizeof(timeout_));
    lcd.print(tt);
    lcd.print(" cek.");
    wifiManager.setConfigPortalTimeout(tt);  
    //-------------------------------------------------------------------
    // Пытаемся подключиться
    if (!wifiManager.autoConnect("GravitonAP")) {
      MYDEBUG_PRINTLN("Не удалось подключиться (истек таймаут). Продолжаем работу в оффлайн-режиме.");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Wi-Fi");
      lcd.setCursor(0,1);
      myPrint(no_,sizeof(no_));
      myPrint(connect,sizeof(connect));
      // Ничего не делаем здесь, чтобы программа просто продолжила выполнение
    } else {
        //------- if you get here you have connected to the WiFi -----------
        MYDEBUG_PRINT("Wi-Fi успешно подключен! Local ip:");
        MYDEBUG_PRINTLN(WiFi.localIP());	// Print ESP32 Local IP Address
        WIFIENABLE = 1;
        // IPAddress myIP = WiFi.localIP();
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Wi-Fi Local ip:");
        lcd.setCursor(0,1);
        lcd.print(WiFi.localIP());
        #ifdef ESP8266
          X509List cert(TELEGRAM_CERTIFICATE_ROOT);
          configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
          client.setTrustAnchors(&cert);      // Add root certificate for api.telegram.org
        #endif
        // --- БЕЗОПАСНОЕ КОПИРОВАНИЕ ДАННЫХ ---
        // Копируем не более 49 символов, чтобы гарантировать место для нуль-терминатора
        strncpy(botToken, custom_botToken.getValue(), sizeof(botToken) - 1);
        botToken[sizeof(botToken) - 1] = '\0'; // Гарантируем, что строка всегда заканчивается нуль-терминатором

        strncpy(chatID, custom_chatID.getValue(), sizeof(chatID) - 1);
        chatID[sizeof(chatID) - 1] = '\0'; // Гарантируем, что строка всегда заканчивается нуль-терминатором
        
        // strcpy(botToken, "7793816236:AAEZdUe3lGD-1-fT3eiSbWgmE2Ur_9mRBno");
        // strcpy(chatID, "1699762091");
        // shouldSaveConfig = true;
        MYDEBUG_PRINTLN("----The values in the file are ----");
        MYDEBUG_PRINTLN("botToken:" + String(botToken));
        MYDEBUG_PRINTLN("chatID:" + String(chatID));
        MYDEBUG_PRINTLN();
        //-------------- Проверяем, что botToken не пустая -----------------
        if (strlen(botToken) > 0) {
            bot.updateToken(botToken);
            // if(botSetup()) Serial.println("The command list was updated successfully.");
            uint8_t num = settings.deviceNum & 0x0F;
            bot.sendMessage(chatID, WORD_QUADUS + String(num), "");//bot.sendMessage("chat_id":"1699762091","text":"```\n КВАДУС v.0.0");
            BOTENABLE = 1;
            MYDEBUG_PRINTLN("bot.updateToken!");
        }
        else {
            MYDEBUG_PRINTLN("botToken = 0");
            BOTENABLE = 0;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("botToken!");
            lcd.setCursor(0,1);
            myPrint(no_,sizeof(no_));
            myPrint(connect,sizeof(connect));
            delay(3000);
        }
        //--------------- save the custom parameters to FS -------------------------
        if(shouldSaveConfig) {
            MYDEBUG_PRINTLN("--------Saving config----------");
            JsonDocument json;
            json["botToken"] = botToken;
            json["chatID"] = chatID;
            File configFile = LittleFS.open("/config.json", "w");
            if (!configFile) {
                MYDEBUG_PRINTLN("Failed to open config file for writing");
            } else {
                if (serializeJson(json, configFile) == 0) {
                    MYDEBUG_PRINTLN("Failed to write to file");
                } else {
                    MYDEBUG_PRINTLN("------Config saved successfully-------");
                }
                #ifdef DEBUG
                  serializeJson(json, Serial);
                #endif
                configFile.close();
            }
        }
        //============================== END SAVE =====================================
        server.on("/", HTTP_GET, []() {
          mode = READDEFAULT; interval = INTERVAL_4000; tmrTelegramOff = 300;
          if (!LittleFS.exists("/index.html")) {
            MYDEBUG_PRINTLN("index.html not found");
          } else {
            File file = LittleFS.open("/index.html", "r");
            if (!file) {
                server.send(404, "text/plain", "I can't open the index.html");
                return;
            }
            server.streamFile(file, "text/html");
            file.close();
          }
        });
        server.on("/setup", HTTP_GET, []() {
          DEBUG_PRINTF("/setup ----- EEPROM size: %d;  time: %d,%ld\n", EEPROM_SIZE,seconds,millis()-lastSendTime);
          File file = LittleFS.open("/setup.html", "r");
          if (!file) {
              server.send(404, "text/plain", "File Not Found");
              return;
          }
          server.streamFile(file, "text/html");
          file.close();
        });
        server.on("/table", HTTP_GET, []() {
          DEBUG_PRINTF("/setup ----- EEPROM size: %d;  time: %d,%ld\n", EEPROM_SIZE,seconds,millis()-lastSendTime);
          File file = LittleFS.open("/table.html", "r");
          if (!file) {
              server.send(404, "text/plain", "File Not Found");
              return;
          }
          server.streamFile(file, "text/html");
          file.close();
        });
        server.on("/getvalues", HTTP_GET, respondsValues);      // the server responds the completed index.html to the client
        server.on("/geteeprom", HTTP_GET, respondsEeprom);      // the server responds the completed setup.html to the client
        server.on("/seteeprom", HTTP_POST, acceptEeprom);       // the server accepts the edited setup.html from the client
        server.on("/get_table", HTTP_POST, respondsProgram);    // the server responds the completed table.html to the client
        server.on("/save_table", HTTP_GET, acceptProgram);      // the server accepts the edited table.html from the client
        server.onNotFound(notFoundHandler);
        
        server.begin();   // Start server
        MYDEBUG_PRINTLN("HTTP server started");
        
        uint16_t begHeapSize = ESP.getFreeHeap();    // Проверка доступной памяти
        DEBUG_PRINTF("Free heap size: %d\n", begHeapSize);
    }
}