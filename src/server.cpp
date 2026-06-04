// server.cpp
#include <main.h>
#include "server.h"

extern uint8_t  seconds, mode, quarter;
extern long lastSendTime;
extern int tableData[32][4], tmrTelegramOff;

void notFoundHandler() {
  server.send(404, "text/plain", "Not found");
}

/**
 * @brief Respond with current system values in JSON format.
 */
void respondsValues() {
    char txt[20];
    String string, jsonResponse;
    uint8_t num = settings.deviceNum & 0x0F;
    tmrTelegramOff = 300;
    JsonDocument data;
    data["model"] = "Quadus&nbsp;&nbsp;&nbsp;&nbsp;№ "+String(num);
    data["temperature0"] = String(ds[0].pvT);
    data["settemp0"] = "["+String(settings.spT0on)+" - "+String(settings.spT0off)+"]";

    if(detectedSensor == SENSOR_DHT22){
        data["humidity"] = String(ds[1].pvT);
        data["sethum"] = "["+String(settings.spT1on)+" - "+String(settings.spT1off)+"]";
    }
    else {
        data["temperature1"] = String(ds[1].pvT);
        data["settemp1"] = "["+String(settings.spT1on)+" - "+String(settings.spT1off)+"]";
    }
    
    snprintf(txt, sizeof(txt),"%02u:%02u [%02u - %02u]",timeinfo->tm_hour,timeinfo->tm_min, settings.timerOn, settings.timerOff);
    if(LIGHT) data["light"] = "↓ " + String(txt);
    else data["light"] = "↑ " + String(txt);

    if(pvTimeR1 == -1){
        data["timer1"] = "T1 no permission";
    } else {
        if(RELAY1){    //-- OFF --
            uint8_t day = pvTimeR1 / 1440;
            uint8_t hour = (pvTimeR1 % 1440) / 60;
            uint8_t min = pvTimeR1 % 60;
            data["timer1"] = "↓ OFF "+String(day)+"d."+String(hour)+"h."+String(min)+"m.";
        } else {      //-- ON --
            data["timer1"] = "↑ ON "+String(pvTimeR1)+" min.";
        }
    } 
    if(pvTimeR2 == -1){
        data["timer2"] = "T2 no permission";
    } else {
        if(RELAY2){    //-- OFF --
            uint8_t day = pvTimeR2 / 1440;
            uint8_t hour = (pvTimeR2 % 1440) / 60;
            uint8_t min = pvTimeR2 % 60;
            data["timer2"] = "↓ OFF "+String(day)+"d."+String(hour)+"h."+String(min)+"m.";
        } else {      //-- ON --
            data["timer2"] = "↑ ON "+String(pvTimeR2)+" min.";
        }
    } 
    if(pvTimeR3 == -1){
        data["timer3"] = "T3 no permission";
    } else {
        if(RELAY3){    //-- OFF --
            uint8_t day = pvTimeR3 / 1440;
            uint8_t hour = (pvTimeR3 % 1440) / 60;
            uint8_t min = pvTimeR3 % 60;
            data["timer3"] = "↓ OFF "+String(day)+"d."+String(hour)+"h."+String(min)+"m.";
        } else {      //-- ON --
            data["timer3"] = "↑ ON "+String(pvTimeR3)+" min.";
        }
    } 
    data["error1"] = ERROR1;
    data["error2"] = ERROR2;
    data["error4"] = ERROR4;
    data["error8"] = ERROR8;
    data["flap"] = String(pvFlap) + "%";
    if((settings.program & 0xF) == 0) string = "none";
    else string = "#" + String(settings.program & 0xF);
    data["program"] = string;
    snprintf(txt,sizeof(txt),"%02d.%02d.%04d %02d:%02d:%02d",
                      timeinfo->tm_mday, timeinfo->tm_mon + 1,
                      timeinfo->tm_year + 1900, timeinfo->tm_hour,
                      timeinfo->tm_min, timeinfo->tm_sec);
    data["currDay"] = txt;
    data["led0"] = dataLed[0] ? "ON" : "OFF" ;  // Light
    data["led1"] = dataLed[1] ? "ON" : "OFF" ;  // HEATER
    data["led2"] = dataLed[2] ? "ON" : "OFF" ;  // HUMIDIFIER
    data["led3"] = dataLed[3] ? "ON" : "OFF" ;  // Timer 1
    data["led4"] = dataLed[4] ? "ON" : "OFF" ;  // Timer 2
    data["led5"] = dataLed[5] ? "ON" : "OFF" ;  // Timer 3
    data["led6"] = dataLed[6] ? "ON" : "OFF" ;  // Alarm
    
    serializeJson(data, jsonResponse);
    server.send(200, "application/json", jsonResponse);
}

/**
 * @brief Respond with EEPROM settings in JSON format.
 */
void respondsEeprom(){
    String jsonResponse;
    JsonDocument doc;
        doc["spT0on"] = settings.spT0on;
        doc["spT0off"] = settings.spT0off;
        doc["spT1on"] = settings.spT1on;
        doc["spT1off"] = settings.spT1off;
        doc["water0on"] = settings.water0on;
        doc["water0off"] = settings.water0off;
        doc["water1on"] = settings.water1on;
        doc["water1off"] = settings.water1off;
        doc["water2on"] = settings.water2on;
        doc["water2off"] = settings.water2off;
        doc["flpNow"] = settings.flap;
        doc["timerOn"] = settings.timerOn;
        doc["timerOff"] = settings.timerOff;
        doc["alarm0"] = settings.alarm0;
        doc["alarm1"] = settings.alarm1;
        doc["deviceNum"] = settings.deviceNum;
        doc["program"] = settings.program;
        doc["modeHeater"] = settings.modeHeater & 0x0F;
        doc["modeHumidi"] = settings.modeHumidi & 0x0F;
        doc["modeRelay1"] = settings.modeRelay1 & 0x0F;
        doc["modeRelay2"] = settings.modeRelay2 & 0x0F;
        doc["modeRelay3"] = settings.modeRelay3 & 0x0F;
        doc["status"] = 1;

        serializeJson(doc, jsonResponse);
        DEBUG_PRINTF("SERVER responds with EEPROM: %d,%ld\n", seconds, millis() - lastSendTime);
        MYDEBUG_PRINTLN(jsonResponse);
        mode = SAVEEEPROM; interval = INTERVAL_1000;
        server.send(200, "application/json", jsonResponse);
}

/**
 * @brief Accept and save settings received from client.
 */
void acceptEeprom() {
  DEBUG_PRINTF("SERVER accepted settings: %d, %ld\n", seconds, millis() - lastSendTime);
  
  for (uint8_t i = 0; i < server.args(); i++) {
      String paramName = server.argName(i);
      String paramValue = server.arg(i);
      
      if (paramName == "spT0on") settings.spT0on = paramValue.toInt();
      else if (paramName == "spT0off") settings.spT0off = paramValue.toInt();
      else if (paramName == "spT1on") settings.spT1on = paramValue.toInt();
      else if (paramName == "spT1off") settings.spT1off = paramValue.toInt();
      else if (paramName == "water0on") settings.water0on = paramValue.toInt();
      else if (paramName == "water0off") settings.water0off = paramValue.toInt();
      else if (paramName == "water1on") settings.water1on = paramValue.toInt();
      else if (paramName == "water1off") settings.water1off = paramValue.toInt();
      else if (paramName == "water2on") settings.water2on = paramValue.toInt();
      else if (paramName == "water2off") settings.water2off = paramValue.toInt();
      else if (paramName == "flpNow") settings.flap = paramValue.toInt();
      else if (paramName == "timerOn") settings.timerOn = paramValue.toInt();
      else if (paramName == "timerOff") settings.timerOff = paramValue.toInt();
      else if (paramName == "alarm0") settings.alarm0 = paramValue.toInt();
      else if (paramName == "alarm1") settings.alarm1 = paramValue.toInt();
      else if (paramName == "deviceNum") settings.deviceNum  = paramValue.toInt();
      else if (paramName == "program") settings.program  = paramValue.toInt();
      else if (paramName == "modeHeater") settings.modeHeater  = paramValue.toInt();
      else if (paramName == "modeHumidi") settings.modeHumidi  = paramValue.toInt();
      else if (paramName == "modeRelay1") settings.modeRelay1  = paramValue.toInt();
      else if (paramName == "modeRelay2") settings.modeRelay2  = paramValue.toInt();
      else if (paramName == "modeRelay3") settings.modeRelay3  = paramValue.toInt();
  }

  server.send(200);
  saveSetPoint();
}

/**
 * @brief Respond with program data in JSON format.
 */
void respondsProgram(){
    String jsonResponse;
    JsonDocument doc;
    mode = SAVEPROG; interval = INTERVAL_1000;
    uint8_t prg = settings.program;
    if(prg){
      for (int i = 1; i < 31; i++) {
          JsonArray row = doc.add<JsonArray>();
          row.add(settings.spT0on);
          row.add(settings.spT0off);
          row.add(settings.spT1on);
          row.add(settings.spT1off);
          row.add(settings.flap);
          row.add(settings.timerOn);
          row.add(settings.timerOff);
          row.add(settings.water0on);
          row.add(settings.water0off);
          row.add(settings.water1on);
          row.add(settings.water1off);
          row.add(settings.water2on);
          row.add(settings.water2off);
      }
      serializeJson(doc, jsonResponse);
      DEBUG_PRINTF("SERVER responds with PROGRAM DATA: %d,%ld\n", seconds, millis() - lastSendTime);
      server.send(200, "application/json", jsonResponse);
    }
}

/**
 * @brief Deserialize program data from string.
 */
void programDeser(String input){
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, input);

    if (error) {
      MYDEBUG_PRINT("deserializeJson() FAILED: ");
      MYDEBUG_PRINTLN(error.c_str());
      return;
    }

    JsonArray data = doc["data"];
    MYDEBUG_PRINTLN("programDeser()");

    for (int i = 1; i < 31; i++) {
      JsonArray data_i = data[i];
      settings.spT0on = data_i[0];
      settings.spT0off = data_i[0];
      settings.spT1on = data_i[1];
      settings.spT1off = data_i[1];
      settings.flap = data_i[3];
      settings.timerOn = data_i[3];
      settings.timerOff = data_i[3];
      settings.water0on = data_i[4];
      settings.water0off = data_i[4];
      settings.water1on = data_i[5];
      settings.water1off = data_i[5];
      settings.water2on = data_i[6];
      settings.water2off = data_i[6];
      
      MYDEBUG_PRINT("DAY:"); MYDEBUG_PRINT(i); 
    }
}

/**
 * @brief Accept program data from client.
 */
void acceptProgram() {
    String jsonData;

    if (server.hasArg("data")) {
        jsonData = server.arg("data");
        MYDEBUG_PRINTLN("jsonData received");
        
        server.send(200); 
        
        programDeser(jsonData);
        mode = SAVEPROG; interval = INTERVAL_1000;
        
        DEBUG_PRINTF("Accept Program: %d, %ld\n", seconds, millis() - lastSendTime);
    } else {
        server.send(400, "text/plain", "Error: no data");
    }
}
