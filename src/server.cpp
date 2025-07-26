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
    tmrTelegramOff = 300;
    JsonDocument data;
    data["model"] = "Клімат-5.25&nbsp;&nbsp;&nbsp;&nbsp;№:" + String(unTable.spHour.special);
    data["temperature0"] = getFloat((float)ds[0].pvT/10,0);
    data["temperature1"] = getFloat((float)ds[1].pvT/10,0);
    data["settemp0"] = getFloat(unTable.spHour.spT0on,1);
    data["settemp1"] = getFloat(unTable.spHour.spT1on,1);
    if(pvRH == 255) data["humidity"] = "***";
    else data["humidity"] = String(pvRH);
    if(AM2301) data["sethum"] = "[" + String(unTable.spHour.spT1on) + "]";
    else  data["sethum"] = "[--]";
    
    // switch (unTable.spHour.extendMode){
    //   case 1: string = "охолодження"; break;
    //   case 2: string = "осущення"; break;
    //   case 3: string = "охол. и осуш."; break;
    //   default: string = ""; break;
    //   }
    // data["ventmode"] = string;

    // switch (unTable.spHour.extendMode){
    //   case 0: string = "сирена"; break;
    //   case 1: string = "відключення"; break;
    //   default: string = ""; break;
    // }
    // data["extmode"] = string;
    
    // switch (unTable.spHour.mode){
    //   case 0: string = "немає"; break;
    //   case 1: string = "канал №1"; break;
    //   case 2: string = "канал №2"; break;
    //   case 3: string = "№1 и №2"; break;
    //   case 4: string = "імпульс"; break;
    //   default: string = ""; break;
    // }
    // data["relaymode"] = string;
    // data["checkDry"] = (unTable.spHour.mode) ? "встановлене" : "немає";
    // data["rotation"] = String(pvTimer) + (TURNSECOND ? " сек." : " хвл.");

    // data["power"] = String(pctHeater) + "%";
    // data["flap"] = String(pvFlap) + "%";
    if((unTable.spHour.special & 0xF) == 0) string = "немає";
    else string = "№" + String(unTable.spHour.special & 0xF);
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
        doc["spT0on"] = unTable.spHour.spT0on;
        doc["spT0off"] = unTable.spHour.spT0off;
        doc["spT1on"] = unTable.spHour.spT1on;
        doc["spT1off"] = unTable.spHour.spT1off;
        doc["watering0"] = unTable.spHour.watering0;
        doc["watering1"] = unTable.spHour.watering1;
        doc["watering2"] = unTable.spHour.watering2;
        doc["timerFlap"] = unTable.spHour.timerFlap;
        doc["alarm0"] = unTable.spHour.alarm0;
        doc["alarm1"] = unTable.spHour.alarm1;
        doc["identif"] = unTable.spHour.special;
        doc["status"] = 1;

        serializeJson(doc, jsonResponse); // Сериализуем JSON
        DEBUG_PRINTF("SERVER responds to the client with EEPROM: %d,%ld\n",seconds,millis()-lastSendTime);
        DEBUG_PRINTLN(jsonResponse);
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
      
      if (paramName == "spT0on") unTable.spHour.spT0on = paramValue.toInt();
      else if (paramName == "spT0off") unTable.spHour.spT0off = paramValue.toInt();
      else if (paramName == "spT1on") unTable.spHour.spT1on = paramValue.toInt();
      else if (paramName == "spT1off") unTable.spHour.spT1off = paramValue.toInt();
      else if (paramName == "watering0") unTable.spHour.watering0 = paramValue.toInt();
      else if (paramName == "watering1") unTable.spHour.watering1 = paramValue.toInt();
      else if (paramName == "watering2") unTable.spHour.watering2 = paramValue.toInt();
      else if (paramName == "alarm0") unTable.spHour.alarm0 = paramValue.toInt();
      else if (paramName == "alarm1") unTable.spHour.alarm1 = paramValue.toInt();
      else if (paramName == "identif") unTable.spHour.special = paramValue.toInt();
  }

  server.send(200); // Отправляем только статус 200

  saveConfig();
}

  void respondsProgram(){
    String jsonResponse;
    JsonDocument doc;
    mode = SAVEPROG; interval = INTERVAL_1000; quarter = SET_PROG4+1;
    uint8_t prg = unTable.spHour.special & 0xF;
    if(prg){
      for (int i = 1; i < 31; i++) {
          JsonArray row = doc.add<JsonArray>();
          uint16_t memoryAddress = eepromMemoryAddressForDay(prg, i);
          eepromRdBuff(memoryAddress, unTable.buffer, sizeof(unTable));
          row.add(unTable.spHour.spT0on);
          row.add(unTable.spHour.spT0off);
          row.add(unTable.spHour.spT1on);
          row.add(unTable.spHour.spT1off);
          row.add(unTable.spHour.timerFlap);
          row.add(unTable.spHour.watering0);
          row.add(unTable.spHour.watering1);
          row.add(unTable.spHour.watering2);
      }
      serializeJson(doc, jsonResponse);
      DEBUG_PRINTF("SERVER responds to the client PROGRAM DATA #: %d,%ld\n",seconds,millis()-lastSendTime);
      DEBUG_PRINTLN("jsonResponse:"+jsonResponse);
      server.send(200, "application/json", jsonResponse);
    }
  }

  //https://arduinojson.org/v7/assistant/#/step1
  void programDeser(String input){
    uint8_t prg = unTable.spHour.special & 0xF;
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, input);

    if (error) {
      DEBUG_PRINT("deserializeJson() FAILED: ");
      DEBUG_PRINTLN(error.c_str());
      return;
    }

    JsonArray data = doc["data"];
    DEBUG_PRINTLN("programDeser()");

    for (int i = 1; i < 31; i++) {
      JsonArray data_i = data[i];
      unTable.spHour.spT0on = data_i[0]; //
      unTable.spHour.spT0off = data_i[0]; //
      unTable.spHour.spT1on = data_i[1]; //
      unTable.spHour.spT1off = data_i[1]; //
      unTable.spHour.timerFlap = data_i[3]; //
      unTable.spHour.watering0 = data_i[4]; //
      unTable.spHour.watering1 = data_i[5]; //
      unTable.spHour.watering2 = data_i[6]; //
      
      // DEBUG_PRINT("spT0="); DEBUG_PRINT(unTable.spHour.spT0); DEBUG_PRINT("; ");
      // DEBUG_PRINT("spT1="); DEBUG_PRINT(unTable.spHour.spT1); DEBUG_PRINT("; ");
      // DEBUG_PRINT("spRH="); DEBUG_PRINT(unTable.spHour.spRH); DEBUG_PRINT("; ");
      // DEBUG_PRINT("flap="); DEBUG_PRINT(unTable.spHour.flap); DEBUG_PRINT("; ");
      // DEBUG_PRINT("timer0="); DEBUG_PRINT(unTable.spHour.timer0); DEBUG_PRINT("; ");
      // DEBUG_PRINT("timer1="); DEBUG_PRINT(unTable.spHour.timer1); DEBUG_PRINT("; ");
      // DEBUG_PRINT("aeration0="); DEBUG_PRINT(unTable.spHour.aeration0); DEBUG_PRINT("; ");
      // DEBUG_PRINT("aeration1="); DEBUG_PRINT(unTable.spHour.aeration1); DEBUG_PRINT("; ");
      // DEBUG_PRINTLN();
      uint16_t memoryAddress = eepromMemoryAddressForDay(prg, i);
      byte res = eepromWrBuff(memoryAddress, unTable.buffer, sizeof(unTable));

      DEBUG_PRINT("DAY:"); DEBUG_PRINT(i); 
      DEBUG_PRINT("; ADD:"); DEBUG_PRINT(memoryAddress);
      DEBUG_PRINT("; RES:"); DEBUG_PRINTLN(res);
    }
  }

  void acceptProgram() {
    String jsonData;

    // Проверка наличия параметра "data" в запросе
    if (server.hasArg("data")) {
        jsonData = server.arg("data");
        DEBUG_PRINTLN("jsonData: " + jsonData); // Логирование полученных данных
        
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