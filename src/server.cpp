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
 * Optimized to avoid large String allocations.
 */
void respondsValues() {
    char txt[64];
    uint8_t num = settings.deviceNum & 0x0F;
    tmrTelegramOff = 300;
    JsonDocument data;
    
    data["model"] = "Quadus&nbsp;&nbsp;&nbsp;&nbsp;№ " + String(num);
    
    // Format temperature with 0.1 precision
    snprintf(txt, sizeof(txt), "%d.%d", ds[0].pvT / 10, abs(ds[0].pvT % 10));
    data["temperature0"] = txt;
    
    snprintf(txt, sizeof(txt), "[%d.%d - %d.%d]", 
             settings.spT0on / 10, abs(settings.spT0on % 10),
             settings.spT0off / 10, abs(settings.spT0off % 10));
    data["settemp0"] = txt;

    if(detectedSensor == SENSOR_DHT22){
        snprintf(txt, sizeof(txt), "%d.%d", ds[1].pvT / 10, abs(ds[1].pvT % 10));
        data["humidity"] = txt;
        snprintf(txt, sizeof(txt), "[%d.%d - %d.%d]", 
                 settings.spT1on / 10, abs(settings.spT1on % 10),
                 settings.spT1off / 10, abs(settings.spT1off % 10));
        data["sethum"] = txt;
    }
    else {
        snprintf(txt, sizeof(txt), "%d.%d", ds[1].pvT / 10, abs(ds[1].pvT % 10));
        data["temperature1"] = txt;
        snprintf(txt, sizeof(txt), "[%d.%d - %d.%d]", 
                 settings.spT1on / 10, abs(settings.spT1on % 10),
                 settings.spT1off / 10, abs(settings.spT1off % 10));
        data["settemp1"] = txt;
    }
    
    snprintf(txt, sizeof(txt), "%s %02u:%02u [%02u - %02u]", 
             LIGHT ? "↓" : "↑", 
             timeinfo->tm_hour, timeinfo->tm_min, 
             settings.timerOn, settings.timerOff);
    data["light"] = txt;

    auto formatTimer = [&](int16_t pvTime, bool relayState, const char* label) {
        if (pvTime == -1) return String(label) + " no permission";
        if (relayState) { // OFF phase
            uint8_t day = pvTime / 1440;
            uint8_t hour = (pvTime % 1440) / 60;
            uint8_t min = pvTime % 60;
            snprintf(txt, sizeof(txt), "↓ OFF %dd.%dh.%dm.", day, hour, min);
        } else { // ON phase
            snprintf(txt, sizeof(txt), "↑ ON %d min.", pvTime);
        }
        return String(txt);
    };

    data["timer1"] = formatTimer(pvTimeR1, RELAY1, "T1");
    data["timer2"] = formatTimer(pvTimeR2, RELAY2, "T2");
    data["timer3"] = formatTimer(pvTimeR3, RELAY3, "T3");
    
    data["error1"] = ERROR1;
    data["error2"] = ERROR2;
    data["error4"] = ERROR4;
    data["error8"] = ERROR8;
    data["flap"] = String(pvFlap) + "%";
    
    data["program"] = ((settings.program & 0xF) == 0) ? "none" : "#" + String(settings.program & 0xF);
    
    snprintf(txt, sizeof(txt), "%02d.%02d.%04d %02d:%02d:%02d",
             timeinfo->tm_mday, timeinfo->tm_mon + 1,
             timeinfo->tm_year + 1900, timeinfo->tm_hour,
             timeinfo->tm_min, timeinfo->tm_sec);
    data["currDay"] = txt;
    
    data["led0"] = dataLed[0] ? "ON" : "OFF";
    data["led1"] = dataLed[1] ? "ON" : "OFF";
    data["led2"] = dataLed[2] ? "ON" : "OFF";
    data["led3"] = dataLed[3] ? "ON" : "OFF";
    data["led4"] = dataLed[4] ? "ON" : "OFF";
    data["led5"] = dataLed[5] ? "ON" : "OFF";
    data["led6"] = dataLed[6] ? "ON" : "OFF";
    
    // Direct serialization to stream to avoid heap fragmentation
    WiFiClient client = server.client();
    server.setContentLength(measureJson(data));
    server.send(200, "application/json", "");
    serializeJson(data, client);
}

/**
 * @brief Respond with EEPROM settings in JSON format.
 */
void respondsEeprom() {
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
    doc["hyst0"] = settings.hysteresis0;
    doc["hyst1"] = settings.hysteresis1;
    doc["deviceNum"] = settings.deviceNum;
    doc["program"] = settings.program;
    doc["modeHeater"] = settings.modeHeater & 0x0F;
    doc["modeHumidi"] = settings.modeHumidi & 0x0F;
    doc["modeRelay1"] = settings.modeRelay1 & 0x0F;
    doc["modeRelay2"] = settings.modeRelay2 & 0x0F;
    doc["modeRelay3"] = settings.modeRelay3 & 0x0F;
    doc["status"] = 1;

    WiFiClient client = server.client();
    server.setContentLength(measureJson(doc));
    server.send(200, "application/json", "");
    serializeJson(doc, client);
    
    mode = SAVEEEPROM; 
    interval = INTERVAL_1000;
}

/**
 * @brief Accept and save settings received from client.
 */
void acceptEeprom() {
    for (uint8_t i = 0; i < server.args(); i++) {
        String paramName = server.argName(i);
        String paramValue = server.arg(i);
        float valF = paramValue.toFloat();
        int16_t valScaled = (int16_t)round(valF * 10.0);
        int valInt = paramValue.toInt();
        
        if (paramName == "spT0on") settings.spT0on = valScaled;
        else if (paramName == "spT0off") settings.spT0off = valScaled;
        else if (paramName == "spT1on") settings.spT1on = valScaled;
        else if (paramName == "spT1off") settings.spT1off = valScaled;
        else if (paramName == "water0on") settings.water0on = valInt;
        else if (paramName == "water0off") settings.water0off = valInt;
        else if (paramName == "water1on") settings.water1on = valInt;
        else if (paramName == "water1off") settings.water1off = valInt;
        else if (paramName == "water2on") settings.water2on = valInt;
        else if (paramName == "water2off") settings.water2off = valInt;
        else if (paramName == "flpNow") settings.flap = valInt;
        else if (paramName == "timerOn") settings.timerOn = valInt;
        else if (paramName == "timerOff") settings.timerOff = valInt;
        else if (paramName == "alarm0") settings.alarm0 = valScaled;
        else if (paramName == "alarm1") settings.alarm1 = valScaled;
        else if (paramName == "hyst0") settings.hysteresis0 = valScaled;
        else if (paramName == "hyst1") settings.hysteresis1 = valScaled;
        else if (paramName == "deviceNum") settings.deviceNum  = valInt;
        else if (paramName == "program") settings.program  = valInt;
        else if (paramName == "modeHeater") settings.modeHeater  = valInt;
        else if (paramName == "modeHumidi") settings.modeHumidi  = valInt;
        else if (paramName == "modeRelay1") settings.modeRelay1  = valInt;
        else if (paramName == "modeRelay2") settings.modeRelay2  = valInt;
        else if (paramName == "modeRelay3") settings.modeRelay3  = valInt;
    }

    server.send(200);
    saveSetPoint();
}

/**
 * @brief Respond with program data in JSON format.
 */
void respondsProgram() {
    mode = SAVEPROG; 
    interval = INTERVAL_1000;
    uint8_t prg = settings.program;
    
    if (prg) {
        JsonDocument doc;
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
        
        WiFiClient client = server.client();
        server.setContentLength(measureJson(doc));
        server.send(200, "application/json", "");
        serializeJson(doc, client);
    } else {
        server.send(404, "text/plain", "No program active");
    }
}

/**
 * @brief Deserialize program data from string.
 */
void programDeser(String input) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, input);

    if (error) {
        MYDEBUG_PRINT("JSON deserialization error: ");
        MYDEBUG_PRINTLN(error.c_str());
        return;
    }

    JsonArray data = doc["data"];
    for (size_t i = 0; i < data.size() && i < 30; i++) {
        JsonArray data_i = data[i];
        if (data_i.size() >= 7) {
            settings.spT0on = data_i[0];
            settings.spT0off = data_i[1];
            settings.spT1on = data_i[2];
            settings.spT1off = data_i[3];
            settings.flap = data_i[4];
            settings.timerOn = data_i[5];
            settings.timerOff = data_i[6];
        }
    }
}

/**
 * @brief Accept program data from client.
 */
void acceptProgram() {
    if (server.hasArg("data")) {
        programDeser(server.arg("data"));
        mode = SAVEPROG; 
        interval = INTERVAL_1000;
        server.send(200); 
        DEBUG_PRINTLN("Program accepted and processed");
    } else {
        server.send(400, "text/plain", "Error: no data");
    }
}
