#ifndef __MAIN_H
#define __MAIN_H

#define LED_DISPLAY

#include <Arduino.h>
#include <WiFiManager.h>    // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
#define FlashFS LittleFS
#include <SPI.h>
#include <Wire.h>     // Библиотека для I2C связи
#include <LiquidCrystal_I2C.h>
#include "TM1638.h"
#include <RTClib.h>   // Библиотека для работы с RTC DS3231
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include "AT24C32.h"
#include "server.h"
#include "programm.h"
#include "procedure.h"
#include "keypad.h"
#include "sensors.h"
#include "displLCD.h"
#include "setupLCD.h"
#include "LogicManager.h"

#define DEBUG

#ifdef DEBUG
  #define DEBUG_SPRINTF(...)  sprintf(__VA_ARGS__)
  #define MYDEBUG_PRINT(...)   Serial.print(__VA_ARGS__)
  #define MYDEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define DEBUG_SPRINTF(...)
  #define MYDEBUG_PRINT(...)
  #define MYDEBUG_PRINTLN(...)
  #define DEBUG_PRINTF(...)
#endif

#define LEDPIN            2
#define ONE_WIRE_BUS_PIN  LEDPIN
#define MAX_DEVICE        2
#define START_MARKER      0xDD

#define READDEFAULT 0
#define READEEPROM  1
#define SAVEEEPROM  2
#define READPROG    3
#define SAVEPROG    4

typedef struct {
  int16_t  pvT;              // текущее значение
  int16_t  pvErr;            // текущая ошибка
  float   previousValue;    // предыдущее значение
  uint16_t errDevice;        // нет ответа датчика
  uint8_t deviation;        // отклонение от заданного значения
  uint16_t froze;           // длительность зависания
} Ds;

enum SensorType {
  UNKNOWN,
  SENSOR_DHT22,
  SENSOR_DS18B20
};

struct Bitfield {
    unsigned a0: 1;
    unsigned a1: 1;
    unsigned a2: 1;
    unsigned a3: 1;
    unsigned a4: 1;
    unsigned a5: 1;
    unsigned a6: 1;
    unsigned a7: 1;
};
 
union Byte {
    unsigned char value;
    struct Bitfield bitfield;
};

/**
 * @brief Структура для хранения текущего состояния системы.
 */
struct SystemState {
    Ds ds[2] = {{15,0,0,0,0},{10,0,0,0,0}};
    union Byte portOut;
    union Byte errorsFlag;
    union Byte portFlag;
    uint8_t dataLed[7];
    int16_t pvTimeR1 = 0;
    int16_t pvTimeR2 = 0;
    int16_t pvTimeR3 = 0;
    uint8_t pvFlap = 0;
    uint8_t numberOfDevices = 0;
    SensorType detectedSensor = UNKNOWN;
};

extern SystemState state;

#pragma pack(push, 1)
struct Settings {
    uint8_t spT0on; 	  // 10-120 Уставка температуры T0 ON
    uint8_t spT0off; 	  // 10-120 Уставка температуры T0 OFF
    uint8_t spT1on; 	  // 10-100 Уставка температуры T1 или 0-100 Уставка относительной влажности ON
    uint8_t spT1off; 	  // 10-100 Уставка температуры T1 или 0-100 Уставка относительной влажности OFF
    uint8_t water0on;   // 0-120 мин. Длительность включ.состояниe полива № 1
    uint8_t water0off;  // 0-15 пункт. Длительность отключ.состояниe полива № 1
    uint8_t water1on;   // 0-120 мин. Длительность включ.состояниe полива № 2
    uint8_t water1off;  // 0-15 пункт. Длительность отключ.состояниe полива № 2
    uint8_t water2on;   // 0-120 мин. Длительность включ.состояниe полива № 3on
    uint8_t water2off;  // 0-15 пункт. Длительность отключ.состояниe полива № 3on
    uint8_t flap;       // 0-100 Заслонка текущее положение
    uint8_t timerOn;    // 0-24 Освещение ON
    uint8_t timerOff;   // 0-24 Освещение OFF
    uint8_t alarm0;     // 0-24 отклонение t0
    uint8_t alarm1;     // 0-24 отклонение t1
    uint8_t special;    // 0-3=>0x03-initWiFiManag(); 0x04-syncTime(); 0x08-wifiManager.resetSettings();
    uint8_t deviceNum;  // 0-120 номер прибора;
    uint8_t program;    // 0-4 номер программы;
    uint8_t modeLight;    // маска 0x0F - разрешения реле освещения;
    uint8_t modeHeater;   // маска 0x0F - разрешения реле температуры;
    uint8_t modeHumidi;   // маска 0x0F - разрешения реле влажности;
    uint8_t modeRelay1;   // маска 0x0F - разрешения реле 3; маска 0xF0 - источник реле 3
    uint8_t modeRelay2;   // маска 0x0F - разрешения реле 3; маска 0xF0 - источник реле 4
    uint8_t modeRelay3;   // маска 0x0F - разрешения реле 3; маска 0xF0 - источник реле 5
};
#pragma pack(pop)

extern Settings settings;

struct TableForOneHour {
    uint8_t spT0on;
    uint8_t spT0off;
    uint8_t spT1on;
    uint8_t spT1off;
    uint8_t water0run;
    uint8_t water1run;
    uint8_t water2run;
    uint8_t timerFlap;
};

union TableBuff {
    uint8_t buffer[8];
    struct TableForOneHour spHour;
};

extern TableBuff unTable;

#define LIGHT		state.portOut.bitfield.a0
#define HEATER  state.portOut.bitfield.a1
#define HUMIDI	state.portOut.bitfield.a2
#define RELAY1	state.portOut.bitfield.a3
#define RELAY2	state.portOut.bitfield.a4
#define RELAY3 	state.portOut.bitfield.a5

#define ds              state.ds
#define dataLed         state.dataLed
#define pvTimeR1        state.pvTimeR1
#define pvTimeR2        state.pvTimeR2
#define pvTimeR3        state.pvTimeR3
#define pvFlap          state.pvFlap
#define numberOfDevices state.numberOfDevices
#define detectedSensor  state.detectedSensor

#define ERROR1    state.errorsFlag.bitfield.a0
#define ERROR2	  state.errorsFlag.bitfield.a1
#define ERROR4	  state.errorsFlag.bitfield.a2
#define ERROR8	  state.errorsFlag.bitfield.a3
#define ERROR10	  state.errorsFlag.bitfield.a4
#define ERROR20	  state.errorsFlag.bitfield.a5
#define OVERHEAT  state.errorsFlag.bitfield.a6
#define FROZE	    state.errorsFlag.bitfield.a7

#define REACHED0    state.portFlag.bitfield.a0
#define REACHED1    state.portFlag.bitfield.a1
#define TURNSECOND  state.portFlag.bitfield.a2
#define RTCENABLE   state.portFlag.bitfield.a3
#define WIFIENABLE	state.portFlag.bitfield.a4
#define RESERV      state.portFlag.bitfield.a5
#define NEWSCREEN   state.portFlag.bitfield.a6
#define SAVING      state.portFlag.bitfield.a7

#define PCF_ON      0
#define PCF_OFF     1

#define BEEP_PIN        0
#define PWMOUT_PIN      15
#define RESETDISPLAY    40
#define MINWAIT         100
#define WAITCHECKKEYPAD 1000

extern TM1638 module;

#define T0ON      22
#define T0OFF     24
#define T1ON      18
#define T1OFF     20
#define WT0ON     10
#define WT0OFF    1
#define WT1ON     20
#define WT1OFF    7
#define WT2ON     30
#define WT2OFF    10
#define TIMERON   5
#define TIMEROFF  22
#define ALARM0    5
#define ALARM1    5

extern const char* version;
extern char displStr[18];
extern char botToken[50];
extern char chatID [15];
extern bool shouldSaveConfig;

extern uint8_t earlyMode, mode, tmrResetMode, quarter, errors, seconds;
extern int tmrTelegramOff;
extern long lastSendTime, allTime; 
typedef enum { INTERVAL_1000 } Interval; // Simplification if needed, check original
extern Interval interval;

extern RTC_DS3231 rtc;
extern struct tm* timeinfo;
extern bool rtcTimeSet;

extern const char* ntpServer;
extern const char* tzInfo;
extern DHT dht;
extern DallasTemperature sensors;
extern DeviceAddress sensorAddresses[MAX_DEVICE];

extern LiquidCrystal_I2C lcd;
extern int8_t dataOut[6];

extern bool newDispl;
extern long counterWait, counter10, counter1s;
extern int8_t displNum, setupNum;

extern uint8_t resetDispl, halfSecond, beepOn, keys, keyCount, lastKey, countSeconds, minutes, lastSyncDay, sources;
extern int16_t editBuff0, editBuff1;
extern uint16_t pvTimer, disableBeep, waitCheckKeyPad;

extern const uint8_t tabRH[];

byte writePCF8574(byte data);
byte readPCF8574();
void initWiFiManag(void);
void displSwitch();
void sensorCheck();
void sensorType();
void checkDs18b20();
void checkkey(uint8_t keys);
void setupSwitch();
void myPrint(const uint8_t* str, size_t size);
void testProgs();

extern const uint8_t error_[8], connect[10], config[12], no_[3], saved[10], file_damaged[15], wordSet[12], timeout_[9], manual_control[15], set_permissions[16], restored[10], save_time[13], time_saved[14], no_permissions[13], sensorsWord[7];

#endif /* __MAIN_H */
