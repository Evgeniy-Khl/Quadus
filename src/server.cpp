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
    char txt[64];
    uint8_t num = settings.deviceNum & 0x0F;
    tmrTelegramOff = 300;
    JsonDocument data;
    
    data["model"] = "Quadus&nbsp;&nbsp;&nbsp;&nbsp;№ " + String(num);
    
    snprintf(txt, sizeof(txt), "%d.%d", ds[0].pvT / 10, abs(ds[0].pvT % 10));
    data["temperature0"] = txt;
    
    snprintf(txt, sizeof(txt), "[%d.%d - %d.%d]", 
             settings.spT0on / 10, abs(settings.spT0on % 10),
             settings.spT0off / 10, abs(settings.spT0off % 10));
    data["settemp0"] = txt;

    if(hasDHT22){
        snprintf(txt, sizeof(txt), "%d.%d", pvRH / 10, abs(pvRH % 10));
        data["humidity"] = txt;
        snprintf(txt, sizeof(txt), "[%d.%d - %d.%d]", 
                 settings.spT1on / 10, abs(settings.spT1on % 10),
                 settings.spT1off / 10, abs(settings.spT1off % 10));
        data["sethum"] = txt;
        data["isTableRH"] = false;
    }
    else {
        if (numberOfDS18 >= 2) {
            snprintf(txt, sizeof(txt), "%d.%d", ds[1].pvT / 10, abs(ds[1].pvT % 10));
            data["temperature1"] = txt;
        }
        
        snprintf(txt, sizeof(txt), "[%d.%d - %d.%d]", 
                 settings.spT1on / 10, abs(settings.spT1on % 10),
                 settings.spT1off / 10, abs(settings.spT1off % 10));
        data["settemp1"] = txt;
        
        if (numberOfDS18 >= 2) {
            snprintf(txt, sizeof(txt), "%d.%d", pvRH / 10, abs(pvRH % 10));
            data["humidity"] = txt;
            data["sethum"] = data["settemp1"];
            data["isTableRH"] = true;
        } else {
            data["isTableRH"] = false;
        }
    }
    
    if (timeinfo) {
        snprintf(txt, sizeof(txt), "%s %02u:%02u [%02u - %02u]", 
                 LIGHT == PCF_OFF ? "↓" : "↑", 
                 timeinfo->tm_hour, timeinfo->tm_min, 
                 settings.timerOn, settings.timerOff);
    } else {
        snprintf(txt, sizeof(txt), "%s 00:00 [%02u - %02u]", 
                 LIGHT == PCF_OFF ? "↓" : "↑", 
                 settings.timerOn, settings.timerOff);
    }
    data["light"] = txt;

    auto formatTimer = [&](int16_t pvTime, bool relayState, int8_t manualMode, const char* label) {
        if (manualMode != -1) return String(label) + (relayState == PCF_OFF ? " Ручне OFF" : " Ручне ON");
        if (pvTime == -1) return String(label) + " немає дозволу";
        if (relayState == PCF_OFF) { // OFF phase
            uint8_t day = pvTime / 1440;
            uint8_t hour = (pvTime % 1440) / 60;
            uint8_t min = pvTime % 60;
            snprintf(txt, sizeof(txt), "↓ OFF %d діб %d год. %d хвл.", day, hour, min);
        } else { // ON phase
            snprintf(txt, sizeof(txt), "↑ ON %d хвл.", pvTime);
        }
        return String(txt);
    };

    data["timer1"] = formatTimer(pvTimeR1, RELAY1, dataOut[3], "T1");
    data["timer2"] = formatTimer(pvTimeR2, RELAY2, dataOut[4], "T2");
    data["timer3"] = formatTimer(pvTimeR3, RELAY3, dataOut[5], "T3");

    data["mR1"] = settings.modeRelay1 & 0x0F;
    data["mR2"] = settings.modeRelay2 & 0x0F;
    data["mR3"] = settings.modeRelay3 & 0x0F;
    
    data["errorsFlag"] = errorsFlag.value;
    data["error1"] = ERROR1;
    data["error2"] = ERROR2;
    data["error4"] = ERROR4;
    data["error8"] = ERROR8;
    
    // Raw values for alarm calculations on client side
    data["pvT0"] = ds[0].pvT;
    data["spT0min"] = min(settings.spT0on, settings.spT0off);
    data["spT0max"] = max(settings.spT0on, settings.spT0off);
    data["alarm0"] = settings.alarm0;
    
    data["pvT1"] = ds[1].pvT;
    data["spT1min"] = min(settings.spT1on, settings.spT1off);
    data["spT1max"] = max(settings.spT1on, settings.spT1off);
    data["alarm1"] = settings.alarm1;

    data["flap"] = String(pvFlap) + "%";
    
    data["program"] = ((settings.program & 0xF) == 0) ? "немає" : "#" + String(settings.program & 0xF);
    
    if (timeinfo) {
        snprintf(txt, sizeof(txt), "%02d.%02d.%04d %02d:%02d:%02d",
                 timeinfo->tm_mday, timeinfo->tm_mon + 1,
                 timeinfo->tm_year + 1900, timeinfo->tm_hour,
                 timeinfo->tm_min, timeinfo->tm_sec);
    } else {
        snprintf(txt, sizeof(txt), "00.00.0000 00:00:00");
    }
    data["currDay"] = txt;
    
    data["led0"] = dataLed[0] ? "ON" : "OFF";
    data["led1"] = dataLed[1] ? "ON" : "OFF";
    data["led2"] = dataLed[2] ? "ON" : "OFF";
    data["led3"] = dataLed[3] ? "ON" : "OFF";
    data["led4"] = dataLed[4] ? "ON" : "OFF";
    data["led5"] = dataLed[5] ? "ON" : "OFF";
    data["led6"] = dataLed[6] ? "ON" : "OFF";
    
    WiFiClient client = server.client();
    server.setContentLength(measureJson(data));
    server.send(200, "application/json", "");
    serializeJson(data, client);
}

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

void acceptEeprom() {
    if (server.hasArg("plain")) {
        bool success = false;
        
        // Scope to free memory immediately after parsing
        {
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, server.arg("plain"));
            
            if (!error) {
                JsonObject obj = doc.as<JsonObject>();
                
                auto getScaled = [&](const char* key, int16_t current) -> int16_t {
                    if (obj[key].is<float>() || obj[key].is<int>()) return (int16_t)round(obj[key].as<float>() * 10.0);
                    return current;
                };
                auto getUint8 = [&](const char* key, uint8_t current) -> uint8_t {
                    if (obj[key].is<int>()) return obj[key].as<uint8_t>();
                    return current;
                };

                settings.spT0on = getScaled("spT0on", settings.spT0on);
                settings.spT0off = getScaled("spT0off", settings.spT0off);
                settings.spT1on = getScaled("spT1on", settings.spT1on);
                settings.spT1off = getScaled("spT1off", settings.spT1off);
                settings.water0on = getUint8("water0on", settings.water0on);
                settings.water0off = getUint8("water0off", settings.water0off);
                settings.water1on = getUint8("water1on", settings.water1on);
                settings.water1off = getUint8("water1off", settings.water1off);
                settings.water2on = getUint8("water2on", settings.water2on);
                settings.water2off = getUint8("water2off", settings.water2off);
                settings.flap = getUint8("flpNow", settings.flap);
                settings.timerOn = getUint8("timerOn", settings.timerOn);
                settings.timerOff = getUint8("timerOff", settings.timerOff);
                settings.alarm0 = getScaled("alarm0", settings.alarm0);
                settings.alarm1 = getScaled("alarm1", settings.alarm1);
                settings.hysteresis0 = getScaled("hyst0", settings.hysteresis0);
                settings.hysteresis1 = getScaled("hyst1", settings.hysteresis1);
                settings.deviceNum = getUint8("deviceNum", settings.deviceNum);
                settings.program = getUint8("program", settings.program);
                settings.modeHeater = getUint8("modeHeater", settings.modeHeater);
                settings.modeHumidi = getUint8("modeHumidi", settings.modeHumidi);
                settings.modeRelay1 = getUint8("modeRelay1", settings.modeRelay1);
                settings.modeRelay2 = getUint8("modeRelay2", settings.modeRelay2);
                settings.modeRelay3 = getUint8("modeRelay3", settings.modeRelay3);

                success = true;
            }
        }
        
        if (success) {
            server.send(200, "application/json", "{\"status\":\"ok\"}");
            // Call saveSetPoint AFTER sending the response
            saveSetPoint();
            return;
        }
    }
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
}

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

void acceptProgram() {
    if (server.hasArg("plain")) {
        programDeser(server.arg("plain"));
        mode = SAVEPROG; 
        interval = INTERVAL_1000;
        server.send(200, "application/json", "{\"status\":\"ok\"}");
        DEBUG_PRINTLN("Program accepted and processed");
    } else {
        server.send(400, "application/json", "{\"error\":\"no data\"}");
    }
}

void handleManualControl() {
    if (server.hasArg("plain")) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, server.arg("plain"));
        if (!error) {
            if (doc["rel1"].is<int8_t>()) { dataOut[0] = doc["rel1"].as<int8_t>(); if (dataOut[0] != -1) LIGHT = (dataOut[0] == 1) ? PCF_ON : PCF_OFF; sysLogger.log(String(getMsg(MSG_MANUAL_LIGHT)) + dataOut[0]); }
            if (doc["rel2"].is<int8_t>()) { dataOut[1] = doc["rel2"].as<int8_t>(); if (dataOut[1] != -1) HEATER = (dataOut[1] == 1) ? PCF_ON : PCF_OFF; sysLogger.log(String(getMsg(MSG_MANUAL_HEATER)) + dataOut[1]); }
            if (doc["rel3"].is<int8_t>()) { dataOut[2] = doc["rel3"].as<int8_t>(); if (dataOut[2] != -1) HUMIDI = (dataOut[2] == 1) ? PCF_ON : PCF_OFF; sysLogger.log(String(getMsg(MSG_MANUAL_HUMIDI)) + dataOut[2]); }
            if (doc["rel4"].is<int8_t>()) { dataOut[3] = doc["rel4"].as<int8_t>(); if (dataOut[3] != -1) { RELAY1 = (dataOut[3] == 1) ? PCF_ON : PCF_OFF; pvTimeR1 = -1; } sysLogger.log(String(getMsg(MSG_MANUAL_RELAY1)) + dataOut[3]); }
            if (doc["rel5"].is<int8_t>()) { dataOut[4] = doc["rel5"].as<int8_t>(); if (dataOut[4] != -1) { RELAY2 = (dataOut[4] == 1) ? PCF_ON : PCF_OFF; pvTimeR2 = -1; } sysLogger.log(String(getMsg(MSG_MANUAL_RELAY2)) + dataOut[4]); }
            if (doc["rel6"].is<int8_t>()) { dataOut[5] = doc["rel6"].as<int8_t>(); if (dataOut[5] != -1) { RELAY3 = (dataOut[5] == 1) ? PCF_ON : PCF_OFF; pvTimeR3 = -1; } sysLogger.log(String(getMsg(MSG_MANUAL_RELAY3)) + dataOut[5]); }
            
            server.send(200, "application/json", "{\"status\":\"ok\"}");
            return;
        }
    }
    server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
}

void resetAutoControl() {
    for (int i = 0; i < 6; i++) dataOut[i] = -1;
    sysLogger.log(getMsg(MSG_AUTO_RESTORED));
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void handleGetRelayStates() {
    JsonDocument doc;
    doc["rel1_m"] = dataOut[0];
    doc["rel2_m"] = dataOut[1];
    doc["rel3_m"] = dataOut[2];
    doc["rel4_m"] = dataOut[3];
    doc["rel5_m"] = dataOut[4];
    doc["rel6_m"] = dataOut[5];
    
    doc["rel1"] = (LIGHT == PCF_ON);
    doc["rel2"] = (HEATER == PCF_ON);
    doc["rel3"] = (HUMIDI == PCF_ON);
    doc["rel4"] = (RELAY1 == PCF_ON);
    doc["rel5"] = (RELAY2 == PCF_ON);
    doc["rel6"] = (RELAY3 == PCF_ON);

    WiFiClient client = server.client();
    server.setContentLength(measureJson(doc));
    server.send(200, "application/json", "");
    serializeJson(doc, client);
}

void handleGetLogs() {
    if (LittleFS.exists(LOG_FILE)) {
        File f = LittleFS.open(LOG_FILE, "r");
        server.streamFile(f, "text/plain");
        f.close();
    } else {
        server.send(200, "text/plain", "No logs found.");
    }
}

void handleClearLogs() {
    sysLogger.clear();
    server.send(200, "application/json", "{\"status\":\"ok\"}");
}
