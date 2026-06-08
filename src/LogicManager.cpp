#include "LogicManager.h"
#include "main.h"
#include "procedure.h"

LogicManager logicManager;

void LogicManager::processLighting() {
    if (!RTCENABLE || isManualOverride) return;
    
    uint8_t currentHour = timeinfo->tm_hour;
    if (checkLightState(currentHour, settings.timerOn, settings.timerOff)) {
        LIGHT = PCF_ON;
    } else {
        LIGHT = PCF_OFF;
    }
}

void LogicManager::processIrrigation() {
    if (isManualOverride) return;
    relaySwitch(1);
    relaySwitch(2);
    relaySwitch(3);
}

void LogicManager::processClimate() {
    if (isManualOverride) return;
    // Heater processing
    if (ds[0].pvT > 1250) { // 125.0°C
        if (!ERROR1) sysLogger.log(getMsg(MSG_HEATER_ERR));
        ERROR1 = 1;
    } else {
        HEATER = checkDeviceState(HEATER, ds[0].pvT, settings.spT0on, settings.spT0off, settings.modeHeater);
    }

    // Humidifier processing
    if (ds[1].pvT > 1250) { // 125.0°C or 125.0% RH
        if (!ERROR2) sysLogger.log(getMsg(MSG_HUMIDITY_ERR));
        ERROR2 = 1;
    } else {
        HUMIDI = checkDeviceState(HUMIDI, ds[1].pvT, settings.spT1on, settings.spT1off, settings.modeHumidi);
    }
}

void LogicManager::processAlarms() {
    processAlarm(0);
    processAlarm(1);
}

void LogicManager::updateStatusLeds() {
    for (uint8_t i = 0; i < 6; i++) {
        uint8_t numBit = 1 << i;
        dataLed[i] = (~portOut.value) & numBit;
    }
    dataLed[6] = errorsFlag.value;
}

void LogicManager::relaySwitch(uint8_t cn) {
    // stateBit stores current relay state (active low for PCF8574: 0 = ON, 1 = OFF)
    bool stateBit = PCF_OFF, prnBit = false;
    // val: remaining time, spOn: ON duration, spOff: OFF interval, permit: operation mode
    int16_t val = 0, spOn = 0, spOff = 0, permit = 0;
    
    // Step 1: Initialize local variables based on the requested channel
    switch (cn) {
        case 1:
            stateBit = RELAY1;
            val = pvTimeR1;
            spOn = settings.water0on;
            spOff = settings.water0off;
            permit = settings.modeRelay1 & 3; // Mask to get the operating mode bits
            break;
        case 2:
            stateBit = RELAY2;
            val = pvTimeR2;
            spOn = settings.water1on;
            spOff = settings.water1off;
            permit = settings.modeRelay2 & 3;
            break;
        case 3:
            stateBit = RELAY3;
            val = pvTimeR3;
            spOn = settings.water2on;
            spOff = settings.water2off;
            permit = settings.modeRelay3 & 3;
            break;
    }

    // Step 2: Check conditional permissions based on light status
    // Operating mode bits: 0 = Always, 1 = Only when LIGHT is OFF, 2 = Only when LIGHT is ON
    if (permit) {
        if (LIGHT == PCF_OFF && permit == 2) permit = 0;
        else if (LIGHT == PCF_ON && permit == 1) permit = 0;
    }

    // Step 3: Logic execution
    if (permit == 0) { // If operation is permitted for the current lighting state
        // Convert the OFF interval index from settings to actual minutes/hours/days
        spOff = transformTimeOff(spOff);
        
        // Decrement the timer. If it reaches zero, toggle the relay state
        if (--val <= 0) {
            prnBit = true;
            if (stateBit == PCF_OFF) { // Current state is OFF -> Switch to ON phase
                val = spOn;            // Set timer to "ON duration" from settings
                stateBit = PCF_ON;     // Physical state becomes ON (0)
                MYDEBUG_PRINT("spOn="); MYDEBUG_PRINT(spOn);
                MYDEBUG_PRINT("; Relay:"); MYDEBUG_PRINT(cn); MYDEBUG_PRINTLN(" state = ON");
            } else { // Current state is ON -> Switch to OFF phase
                val = spOff;           // Set timer to calculated "OFF interval"
                stateBit = PCF_OFF;    // Physical state becomes OFF (1)
                MYDEBUG_PRINT("spOff="); MYDEBUG_PRINT(spOff);
                MYDEBUG_PRINT("; Relay:"); MYDEBUG_PRINT(cn); MYDEBUG_PRINTLN(" state = OFF");
            }
        }
        
        // Step 4: Save updated state and timer back to global system variables
        switch (cn) {
            case 1: RELAY1 = stateBit; pvTimeR1 = val; break;
            case 2: RELAY2 = stateBit; pvTimeR2 = val; break;
            case 3: RELAY3 = stateBit; pvTimeR3 = val; break;
        }
        #ifdef DEBUG
        if (prnBit) printBinary(portOut.value);
        #endif
    } else { 
        // If operation is NOT permitted by lighting mode, force the relay OFF and reset timer
        switch (cn) {
            case 1: RELAY1 = PCF_OFF; pvTimeR1 = -1; break;
            case 2: RELAY2 = PCF_OFF; pvTimeR2 = -1; break;
            case 3: RELAY3 = PCF_OFF; pvTimeR3 = -1; break;
        }
    }
}

bool LogicManager::checkDeviceState(bool previousState, int16_t currentTemp, int16_t onTemp, int16_t offTemp, uint8_t permit) {
    if (permit) {
        if (LIGHT == PCF_OFF && permit == 2) permit = 0;
        else if (LIGHT == PCF_ON && permit == 1) permit = 0;
    }

    if (permit == 0) {
        if (onTemp == offTemp) return PCF_OFF;
        
        // Determine which hysteresis to use (T0 or T1)
        int16_t hyst = (&onTemp == &settings.spT0on || &onTemp == &settings.spT0off) ? settings.hysteresis0 : settings.hysteresis1;

        if (onTemp < offTemp) { // Heating mode
            // Turn ON if temperature drops to or below onTemp
            if (currentTemp <= onTemp) return PCF_ON;
            // Turn OFF if temperature reaches offTemp - hyst
            if (currentTemp >= (offTemp - hyst)) return PCF_OFF;
        } else { // Cooling mode
            // Turn ON if temperature reaches or exceeds onTemp
            if (currentTemp >= onTemp) return PCF_ON;
            // Turn OFF if temperature drops to offTemp + hyst
            if (currentTemp <= (offTemp + hyst)) return PCF_OFF;
        }
        return previousState;
    } else {
        return PCF_OFF;
    }
}

bool LogicManager::checkLightState(uint8_t currentHour, uint8_t onHour, uint8_t offHour) {
    if (onHour == offHour) return false;
    if (onHour < offHour) return (currentHour >= onHour && currentHour < offHour);
    else return (currentHour >= onHour || currentHour < offHour);
}

void LogicManager::processAlarm(uint8_t cn) {
    int16_t val, maxVal, minVal, alarmVal, permit;
    bool reached, beep = false;
    val = ds[cn].pvT;
    
    if (cn) {
        maxVal = max(settings.spT1on, settings.spT1off);
        minVal = min(settings.spT1on, settings.spT1off);
        alarmVal = settings.alarm1;
        permit = settings.modeHumidi;
        reached = REACHED1;
    } else {
        maxVal = max(settings.spT0on, settings.spT0off);
        minVal = min(settings.spT0on, settings.spT0off);
        alarmVal = settings.alarm0;
        permit = settings.modeHeater;
        reached = REACHED0;
    }

    if (permit) {
        if (LIGHT == PCF_OFF && permit == 2) permit = 0;
        else if (LIGHT == PCF_ON && permit == 1) permit = 0;
    }

    if (permit == 0) {
        if (reached) {
            if (val <= (minVal - alarmVal) || val >= (maxVal + alarmVal)) beep = true;
        } else if (val >= minVal && val <= maxVal) {
            reached = true;
        }

        if (cn) {
            if (!REACHED1 && reached) sysLogger.log(getMsg(MSG_CLIMATE_T2_REACHED));
            REACHED1 = reached;
            if (!ERROR8 && beep) sysLogger.log(getMsg(MSG_ALARM_T2_RANGE));
            ERROR8 = beep;
        } else {
            if (!REACHED0 && reached) sysLogger.log(getMsg(MSG_CLIMATE_T1_REACHED));
            REACHED0 = reached;
            if (!ERROR4 && beep) sysLogger.log(getMsg(MSG_ALARM_T1_RANGE));
            ERROR4 = beep;
        }

        if (errorsFlag.value) {
            uint8_t duration = (errorsFlag.value == 0x03) ? 100 : 50;
            if (disableBeep == 0) beeperOn(duration);
            else disableBeep--;
        } else {
            disableBeep = 0;
        }
    }
}

uint16_t LogicManager::transformTimeOff(uint8_t point) {
    uint16_t val = point;
    switch (point) {
        case 5: val = 6; break;
        case 6: val = 8; break;
        case 7: val = 10; break;
        case 8: val = 12; break;
        case 9: val = 24; break;
        case 10: val = 2; break;
        case 11: val = 3; break;
        case 12: val = 4; break;
        case 13: val = 5; break;
        case 14: val = 6; break;
        case 15: val = 7; break;
    }
    if (point < 10) val *= 60;
    else val *= (60 * 24);
    return val;
}
