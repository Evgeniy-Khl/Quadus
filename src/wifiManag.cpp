#include "main.h"

void saveConfigCallback();

void initWiFiManag(void){
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    //set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);

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
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Wi-Fi Local ip:");
        lcd.setCursor(0,1);
        lcd.print(WiFi.localIP());
        
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
          DEBUG_PRINTF("/table ----- EEPROM size: %d;  time: %d,%ld\n", EEPROM_SIZE,seconds,millis()-lastSendTime);
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
        // server.on("/get_table", HTTP_POST, respondsProgram);    // the server responds the completed table.html to the client
        // server.on("/save_table", HTTP_GET, acceptProgram);      // the server accepts the edited table.html from the client
        server.onNotFound(notFoundHandler);
        
        server.begin();   // Start server
        MYDEBUG_PRINTLN("HTTP server started");
        
        uint16_t heapSize = ESP.getFreeHeap();    // Проверка доступной памяти
        DEBUG_PRINTF("Free heap size: %d\n", heapSize);
    }
}

//callback notifying us of the need to save config
void saveConfigCallback() {
    DEBUG_PRINTLN("Should save config");
    shouldSaveConfig = true;
}
