// server.cpp
#include <main.h>
#include "server.h"

extern uint8_t  seconds, mode, quarter;
extern long lastSendTime;
extern int tableData[32][4], tmrTelegramOff;

void notFoundHandler() {
  server.send(404, "text/plain", "Not found");
}

String getFloat(float val, uint8_t brackets) {
  char buffer[8];
  if(brackets) snprintf(buffer, sizeof(buffer), "[%.1f]", val);
  else snprintf(buffer, sizeof(buffer), "%.1f", val);
  for (unsigned int i = 0; i < sizeof(buffer); i++) {
    if (buffer[i] == '.') {
      buffer[i] = ',';
      break;
    }
  }
  return String(buffer); // Возвращаем отформатированную строку
}

void respondsValues() {
    String string, jsonResponse;
    uint8_t num = settings.deviceNum & 0x0F;
    tmrTelegramOff = 300;
    JsonDocument data;
    data["model"] = "Квадус&nbsp;&nbsp;&nbsp;&nbsp;№:" + String(num);
    data["temperature0"] = getFloat((float)ds[0].pvT/10,0);
    data["temperature1"] = getFloat((float)ds[1].pvT/10,0);
    data["settemp0"] = getFloat(settings.spT0on,1);
    data["settemp1"] = getFloat(settings.spT1on,1);
    if(pvRH == 255) data["humidity"] = "***";
    else data["humidity"] = String(pvRH);
    if(AM2301) data["sethum"] = "[" + String(settings.spT1on) + "]";
    else  data["sethum"] = "[--]";
    
    // switch (settings.extendMode){
    //   case 1: string = "охолодження"; break;
    //   case 2: string = "осущення"; break;
    //   case 3: string = "охол. и осуш."; break;
    //   default: string = ""; break;
    //   }
    // data["ventmode"] = string;

    // switch (settings.extendMode){
    //   case 0: string = "сирена"; break;
    //   case 1: string = "відключення"; break;
    //   default: string = ""; break;
    // }
    // data["extmode"] = string;
    
    // switch (settings.mode){
    //   case 0: string = "немає"; break;
    //   case 1: string = "канал №1"; break;
    //   case 2: string = "канал №2"; break;
    //   case 3: string = "№1 и №2"; break;
    //   case 4: string = "імпульс"; break;
    //   default: string = ""; break;
    // }
    // data["relaymode"] = string;
    // data["checkDry"] = (settings.mode) ? "встановлене" : "немає";
    // data["rotation"] = String(pvTimer) + (TURNSECOND ? " сек." : " хвл.");

    // data["power"] = String(pctHeater) + "%";
    // data["flap"] = String(pvFlap) + "%";
    if((settings.program & 0xF) == 0) string = "немає";
    else string = "№" + String(settings.program & 0xF);
    data["program"] = string;
    data["currDay"] = "0 діб.";//String(upv.pv.currDay) + "діб.";
    data["led0"] = dataLed[0] ? "ON" : "OFF" ;  // НАГРЕВАТЕЛЬ
    data["led1"] = dataLed[1] ? "ON" : "OFF" ;  // УВЛАЖНИТЕЛЬ
    data["led2"] = dataLed[2] ? "ON" : "OFF" ;  // Поворот лотков
    data["led3"] = dataLed[3] ? "ON" : "OFF" ;  // Заслонка/вентилятор охлаждения
    data["led4"] = dataLed[4] ? "ON" : "OFF" ;  // Вспомогательный нагреватель
    data["led5"] = dataLed[5] ? "ON" : "OFF" ;  // Авария
    
    serializeJson(data, jsonResponse);
    // DEBUG_PRINTF("SERVER responds to the client with VALUES: %d,%ld\n",seconds,millis()-lastSendTime);
    // Serial.println("out=" + response);
    server.send(200, "application/json", jsonResponse);
    // DEBUG_PRINTF("END VALUES: %d,%ld\n",seconds,millis()-lastSendTime);
}

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
        doc["flap"] = settings.flap;
        doc["timerOn"] = settings.timerOn;
        doc["timerOff"] = settings.timerOff;
        doc["alarm0"] = settings.alarm0;
        doc["alarm1"] = settings.alarm1;
        doc["identif"] = settings.deviceNum & 0x0F;
        doc["status"] = 1;

        serializeJson(doc, jsonResponse); // Сериализуем JSON
        DEBUG_PRINTF("SERVER responds to the client with EEPROM: %d,%ld\n",seconds,millis()-lastSendTime);
        MYDEBUG_PRINTLN(jsonResponse);
        mode = SAVEEEPROM; interval = INTERVAL_1000;
        server.send(200, "application/json", jsonResponse); // Отправляем ответ
        // DEBUG_PRINTF("END EEPROM: %d,%ld\n",seconds,millis()-lastSendTime);
}

void acceptEeprom() {
  // Логирование всех параметров
  DEBUG_PRINTF("The SERVER has accepted settings.sp_structs[]: %d, %ld\n", seconds, millis() - lastSendTime);
  
  for (uint8_t i = 0; i < server.args(); i++) {
      String paramName = server.argName(i);
      String paramValue = server.arg(i);
      
      // Логирование параметров (раскомментируйте, если нужно)
      // DEBUG_PRINTF("Parameter: %s, Value: %s\n", paramName.c_str(), paramValue.c_str());
      
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
      else if (paramName == "alarm0") settings.alarm0 = paramValue.toInt();
      else if (paramName == "alarm1") settings.alarm1 = paramValue.toInt();
      else if (paramName == "identif") settings.deviceNum  = paramValue.toInt();
  }

  server.send(200); // Отправляем только статус 200

  saveSetPoint();
}

  void respondsProgram(){
    String jsonResponse;
    JsonDocument doc;
    mode = SAVEPROG; interval = INTERVAL_1000; quarter = SET_PROG4+1;
    uint8_t prg = settings.program;
    if(prg){
      for (int i = 1; i < 31; i++) {
          JsonArray row = doc.add<JsonArray>();
          // uint16_t memoryAddress = eepromMemoryAddressForDay(prg, i);
          // eepromRdBuff(memoryAddress, unTable.buffer, sizeof(unTable));
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
      DEBUG_PRINTF("SERVER responds to the client PROGRAM DATA #: %d,%ld\n",seconds,millis()-lastSendTime);
      MYDEBUG_PRINTLN("jsonResponse:"+jsonResponse);
      server.send(200, "application/json", jsonResponse);
    }
  }

  //https://arduinojson.org/v7/assistant/#/step1
  void programDeser(String input){
    // uint8_t prg = settings.program;
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
      settings.spT0on = data_i[0]; //
      settings.spT0off = data_i[0]; //
      settings.spT1on = data_i[1]; //
      settings.spT1off = data_i[1]; //
      settings.flap = data_i[3]; //
      settings.timerOn = data_i[3]; //
      settings.timerOff = data_i[3]; //
      settings.water0on = data_i[4]; //
      settings.water0off = data_i[4]; //
      settings.water1on = data_i[5]; //
      settings.water1off = data_i[5]; //
      settings.water2on = data_i[6]; //
      settings.water2off = data_i[6]; //
      
      // MYDEBUG_PRINT("spT0="); MYDEBUG_PRINT(settings.spT0); MYDEBUG_PRINT("; ");
      // MYDEBUG_PRINT("spT1="); MYDEBUG_PRINT(settings.spT1); MYDEBUG_PRINT("; ");
      // MYDEBUG_PRINT("spRH="); MYDEBUG_PRINT(settings.spRH); MYDEBUG_PRINT("; ");
      // MYDEBUG_PRINT("flap="); MYDEBUG_PRINT(settings.flap); MYDEBUG_PRINT("; ");
      // MYDEBUG_PRINT("timer0="); MYDEBUG_PRINT(settings.timer0); MYDEBUG_PRINT("; ");
      // MYDEBUG_PRINT("timer1="); MYDEBUG_PRINT(settings.timer1); MYDEBUG_PRINT("; ");
      // MYDEBUG_PRINT("aeration0="); MYDEBUG_PRINT(settings.aeration0); MYDEBUG_PRINT("; ");
      // MYDEBUG_PRINT("aeration1="); MYDEBUG_PRINT(settings.aeration1); MYDEBUG_PRINT("; ");
      // MYDEBUG_PRINTLN();
      // uint16_t memoryAddress = eepromMemoryAddressForDay(prg, i);
      // byte res = eepromWrBuff(memoryAddress, unTable.buffer, sizeof(unTable));

      MYDEBUG_PRINT("DAY:"); MYDEBUG_PRINT(i); 
      // MYDEBUG_PRINT("; ADD:"); MYDEBUG_PRINT(memoryAddress);
      // MYDEBUG_PRINT("; RES:"); MYDEBUG_PRINTLN(res);
    }
  }

  void acceptProgram() {
    String jsonData;

    // Проверка наличия параметра "data" в запросе
    if (server.hasArg("data")) {
        jsonData = server.arg("data");
        MYDEBUG_PRINTLN("jsonData: " + jsonData); // Логирование полученных данных
        
        // Отправляем статус 200
        server.send(200); 
        
        // Обработка полученных данных
        programDeser(jsonData);
        mode = SAVEPROG; interval = INTERVAL_1000;
        quarter = SET_PROG1;
        
        DEBUG_PRINTF("Accept Program: %d, %ld\n", seconds, millis() - lastSendTime);
    } else {
        // Отправка сообщения об ошибке, если параметр отсутствует
        server.send(400, "text/plain", "Ошибка: нет данных");
    }
  }