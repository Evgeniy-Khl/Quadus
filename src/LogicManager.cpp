#include "LogicManager.h"
#include "main.h"
#include "procedure.h"

LogicManager logicManager;

void LogicManager::processLighting() {
    if (!RTCENABLE || state.isManualOverride) return;
    
    uint8_t currentHour = timeinfo->tm_hour;
    if (checkLightState(currentHour, settings.timerOn, settings.timerOff)) {
        LIGHT = PCF_ON;
    } else {
        LIGHT = PCF_OFF;
    }
}

void LogicManager::processIrrigation() {
    if (state.isManualOverride) return;
    relaySwitch(1);
    relaySwitch(2);
    relaySwitch(3);
}

void LogicManager::processClimate() {
    if (state.isManualOverride) return;
    // Heater processing
    if (ds[0].pvT > 1250) { // 125.0°C
        if (!ERROR1) sysLogger.log("ALARM: Heater sensor error!");
        ERROR1 = 1;
    } else {
        HEATER = checkDeviceState(HEATER, ds[0].pvT, settings.spT0on, settings.spT0off, settings.modeHeater);
    }

    // Humidifier processing
    if (ds[1].pvT > 1250) { // 125.0°C or 125.0% RH
        if (!ERROR2) sysLogger.log("ALARM: Humidity sensor error!");
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
        dataLed[i] = (~state.portOut.value) & numBit;
    }
    dataLed[6] = state.errorsFlag.value;
}

void LogicManager::relaySwitch(uint8_t cn) {
    bool stateBit = PCF_OFF, prnBit = false;
    int16_t val = 0, spOn = 0, spOff = 0, permit = 0;
    
    switch (cn) {
        case 1:
            stateBit = RELAY1;
            val = pvTimeR1;
            spOn = settings.water0on;
            spOff = settings.water0off;
            permit = settings.modeRelay1 & 3;
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

    if (permit) {
        if (LIGHT == PCF_OFF && permit == 2) permit = 0;
        else if (LIGHT == PCF_ON && permit == 1) permit = 0;
    }

    if (permit == 0) {
        spOff = transformTimeOff(spOff);
        if (--val <= 0) {
            prnBit = true;
            if (stateBit) { //-- OFF --
                val = spOn;
                stateBit = PCF_ON;
                MYDEBUG_PRINT("spOn="); MYDEBUG_PRINT(spOn);
                MYDEBUG_PRINT("; Relay:"); MYDEBUG_PRINT(cn); MYDEBUG_PRINTLN(" state = ON");
            } else { //-- ON --
                val = spOff;
                stateBit = PCF_OFF;
                MYDEBUG_PRINT("spOff="); MYDEBUG_PRINT(spOff);
                MYDEBUG_PRINT("; Relay:"); MYDEBUG_PRINT(cn); MYDEBUG_PRINTLN(" state = OFF");
            }
        }
        switch (cn) {
            case 1: RELAY1 = stateBit; pvTimeR1 = val; break;
            case 2: RELAY2 = stateBit; pvTimeR2 = val; break;
            case 3: RELAY3 = stateBit; pvTimeR3 = val; break;
        }
        #ifdef DEBUG
        if (prnBit) printBinary(state.portOut.value);
        #endif
    } else {
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
            if (!REACHED1 && reached) sysLogger.log("Climate T2/RH target reached.");
            REACHED1 = reached;
            if (!ERROR8 && beep) sysLogger.log("ALARM: T2/RH out of range!");
            ERROR8 = beep;
        } else {
            if (!REACHED0 && reached) sysLogger.log("Climate T1 target reached.");
            REACHED0 = reached;
            if (!ERROR4 && beep) sysLogger.log("ALARM: T1 temperature out of range!");
            ERROR4 = beep;
        }

        if (state.errorsFlag.value) {
            uint8_t duration = (state.errorsFlag.value == 0x03) ? 100 : 50;
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
