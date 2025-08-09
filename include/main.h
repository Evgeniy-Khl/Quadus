#ifndef __MAIN_H
#define __MAIN_H

#define LED_DISPLAY

#include <Arduino.h>
#include <MyTelegramBot.h>  // Universal Telegram Bot Library written by Brian Lough: 
#include <WiFiManager.h>    //https://github.com/tzapu/WiFiManager
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
#include "telegram.h"
#include "programm.h"
#include "procedure.h"
#include "keypad.h"
#include "sensors.h"
#include "displLCD.h"
#include "setupLCD.h"

#define DEBUG

#ifdef DEBUG
  // Вариативные макросы, принимающие любое количество аргументов
  #define DEBUG_SPRINTF(...)  sprintf(__VA_ARGS__)
  #define MYDEBUG_PRINT(...)   Serial.print(__VA_ARGS__)
  #define MYDEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
  #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  // "Пустышки" остаются такими же
  #define DEBUG_SPRINTF(...)
  #define MYDEBUG_PRINT(...)
  #define MYDEBUG_PRINTLN(...)
#endif
// --- Конец блока макросов ---

#define LEDPIN 2
#define ONE_WIRE_BUS_PIN LEDPIN   // используется номер GPIO2
#define MAX_DEVICE 4              // ограничение количества датчиков
#define START_MARKER 0xDD	// Начало блока = 221

#define READDEFAULT 0
#define READEEPROM  1
#define SAVEEEPROM  2
#define READPROG    3
#define SAVEPROG    4
#define GET_VALUES	0x40    // 64
#define GET_EEPROM	0x41    // 65
#define SET_EEPROM	0x42    // 66
#define GET_PROG1	  0x50    // 80
#define GET_PROG2	  0x51    // 81
#define GET_PROG3	  0x52    // 82
#define GET_PROG4	  0x53    // 83

#define SET_PROG1	  0x30    // 48
#define SET_PROG2	  0x31    // 49
#define SET_PROG3	  0x32    // 50
#define SET_PROG4	  0x33    // 51

typedef struct {
  int16_t pvT;              // текущее значение
  int16_t pvErr;            // текущая ошибка
  int16_t previousValue;    // предыдущее значение
  uint8_t errDevice;        // нет ответа датчика
  uint8_t deviation;        // отклонение от заданного значения
  uint16_t duration;        // длительность зависания
} Ds;
extern Ds ds[];

// Для предотвращения выравнивания полей компилятором, что может нарушить карту памяти.
// В данном случае все поля одного типа, и проблема маловероятна, но это хорошая практика.
#pragma pack(push, 1)
struct Settings{
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

extern Settings settings;

struct TableForOneHour{
    uint8_t spT0on; 	  // 10-120 Уставка температуры T0 ON
    uint8_t spT0off; 	  // 10-120 Уставка температуры T0 OFF
    uint8_t spT1on; 	  // 10-100 Уставка температуры T1 или 0-100 Уставка относительной влажности ON
    uint8_t spT1off; 	  // 10-100 Уставка температуры T1 или 0-100 Уставка относительной влажности OFF
    uint8_t water0run;  // 0-120 мин. Длительность включ.состояниe полива № 1
    uint8_t water1run;  // 0-120 мин. Длительность включ.состояниe полива № 2
    uint8_t water2run;  // 0-120 мин. Длительность включ.состояниe полива № 3on
    uint8_t timerFlap;  // 0-60 Освещение маска 0x3F / 0-3 (0-0%; 1-30%; 2-60%; 3-100%) Заслонка текущее положение
};
#pragma pack(pop)
// 1-1час.;2-2час.;3-3час.;4-4час.;5-6час.;6-8час.;7-10час.;8-12час.;9-24час.;10-2сут.;11-3сут.;12-4сут.;13-5сут.;14-6сут.;15-7сут.;

//Programm 1: 0-3 -> p0;  4-7 -> p1;  8-11 -> p2;  12-15 -> p3;  16-19 -> p4;  20-23 -> p5;
//Programm 2: 0-3 -> p6;  4-7 -> p7;  8-11 -> p8;  12-15 -> p9;  16-19 -> p10; 20-23 -> p11;
//Programm 3: 0-3 -> p12; 4-7 -> p13; 8-11 -> p14; 12-15 -> p15; 16-19 -> p16; 20-23 -> p17;
union TableBuff {
    uint8_t buffer[8];
    struct TableForOneHour spHour;
};

extern TableBuff unTable;

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

extern union Byte portOut;
extern union Byte errorsFlag;
extern union Byte portFlag;

#define LIGHT		portOut.bitfield.a0  // Освещение
#define HEATER  portOut.bitfield.a1  // НАГРЕВАТЕЛЬ
#define HUMIDI	portOut.bitfield.a2  // УВЛАЖНИТЕЛЬ
#define RELAY1	portOut.bitfield.a3  // реле 1
#define RELAY2	portOut.bitfield.a4  // реле 2
#define RELAY3 	portOut.bitfield.a5  // реле 3

#define ERROR1    errorsFlag.bitfield.a0  // ОШИБКА ДАТЧИКА 0  --- потерян; 66,0-завис [E01]
#define ERROR2	  errorsFlag.bitfield.a1  // ОШИБКА ДАТЧИКА 1  --- потерян; 66,0-завис [E02]
#define ERROR4	  errorsFlag.bitfield.a2  // ОТКЛОНЕНИЕ КАНАЛ 0 [E04]
#define ERROR8	  errorsFlag.bitfield.a3  // ОТКЛОНЕНИЕ КАНАЛ 1 [E08]
#define ERROR10	  errorsFlag.bitfield.a4  // 
#define ERROR20	  errorsFlag.bitfield.a5  // 
#define OVERHEAT  errorsFlag.bitfield.a6  // 
#define FROZE	    errorsFlag.bitfield.a7  // завис датчик.

#define REACHED0    portFlag.bitfield.a0  // pvT[0]-ДОСТИГ spT[0]
#define REACHED1    portFlag.bitfield.a1  // pvT[1]-ДОСТИГ spT[1]
#define TURNSECOND  portFlag.bitfield.a2  // устанавливается в 1 если отсчет в секундах
#define RTCENABLE   portFlag.bitfield.a3  // разрешены часы реальеного времени
#define WIFIENABLE	portFlag.bitfield.a4  // разрешен WiFi
#define AM2301	    portFlag.bitfield.a5  // exist AM2301 flag
#define NEWSCREEN   portFlag.bitfield.a6  // новый экран
#define SAVING      portFlag.bitfield.a7  // проветривание

#define ON          1
#define PCF_ON      0
#define OFF         0
#define PCF_OFF     1

#ifdef LED_DISPLAY
  #define BEEP_PIN        0
  #define PWMOUT_PIN      15
  #define RESETDISPLAY    40  // 20/2=20 Sec.
  #define MINWAIT         100
  #define WAITCHECKKEYPAD 1000	// mSec. максимальная пауза перед реакцией на кнопку
  extern TM1638 module;
  // extern uint8_t data[8];
#else

#endif
//******************************************************** */
#define T0ON      22 	    // 10-120 Уставка температуры T0 ON
#define T0OFF     24 	    // 10-120 Уставка температуры T0 OFF
#define T1ON      18 	    // 10-100 Уставка температуры T1 или 0-100 Уставка относительной влажности ON
#define T1OFF     20 	    // 10-100 Уставка температуры T1 или 0-100 Уставка относительной влажности OFF
#define WT0ON     10      // 0-120 мин.Длительность включ.состояниe полива № 1
#define WT0OFF    1       // 0-15 пункт. Длительность отвключ.состояниe полива № 1
#define WT1ON     20      // 0-120 мин.Длительность включ.состояниe полива № 2
#define WT1OFF    7       // 0-15 пункт. Длительность отвключ.состояниe полива № 2
#define WT2ON     30      // 0-120 мин.Длительность включ.состояниe полива № 3
#define WT2OFF    10      // 0-15 пункт. Длительность отвключ.состояниe полива № 3
#define TIMERON   5       // 0-24 
#define TIMEROFF  22      // 0-24 
#define ALARM0    5 	    // 1-25 отклонение температуры T0
#define ALARM1    5 	    // 1-25 отклонение температуры T1

// 1-1час.;2-2час.;3-3час.;4-4час.;5-6час.;6-8час.;7-10час.;8-12час.;9-24час.;10-2сут.;11-3сут.;12-4сут.;13-5сут.;14-6сут.;15-7сут.;

extern const char* version;
extern char displStr[18];
extern char botToken[50];
extern char chatID [15];
extern WiFiClientSecure client;
extern MyTelegramBot bot;
extern bool shouldSaveConfig;
//-------------
extern uint8_t earlyMode, mode, tmrResetMode, quarter, errors, seconds;
extern int tmrTelegramOff;
extern long lastSendTime, allTime; 
extern Interval interval;
//-------------

extern RTC_DS3231 rtc;
// extern DateTime curT;
extern struct tm* timeinfo;
extern bool rtcTimeSet;

extern const char* ntpServer;
extern const char* tzInfo;
extern DHT dht;
extern DallasTemperature sensors;
enum SensorType {                       // Создаем перечисление (enum) для удобного хранения типа датчика
  UNKNOWN,
  SENSOR_DHT22,
  SENSOR_DS18B20
};
extern SensorType detectedSensor;
extern LiquidCrystal_I2C lcd;
extern int8_t dataOut[6];

extern bool newDispl;
extern long counterWait, counter10, counter1s;
extern
int8_t  displNum,           // вариант дисплея
        setupNum,           // пунк выбора установки
        pvT0,               // температура t1
        pvT1;               // температура t2

extern 
uint8_t numberOfDevices,    // число найденых датчиков
        resetDispl,         // время ожидания до возврата главного диплея
        halfSecond,         // счетчик полу-секунд
        pvFlap,             // текущее положение заслонки
        beepOn,             // время звучания бипера
        disableBeep,        // время запрета включения аварийной сигнализации
        keys,               // текущая кнопка
        keyCount,           // счетчик удержания кнопки
        lastKey,            // предыдущая кнопка
        countSeconds,       // счетчик секунд
        minutes,            // счетчик минут
        lastSyncDay,        // Переменная для хранения дня последней синхронизации
        sources;            // источники для реле 3 и реле 4
extern
int16_t pvTimeR1,           // текущее время реле 0
        pvTimeR2,           // текущее время реле 1
        pvTimeR3,           // текущее время реле 2
        editBuff0,          // временное хранилище редактируемой установки
        editBuff1;          // временное хранилище редактируемой установки
extern
uint16_t    pvRH,           // текущая относительная влажность
            pvTimer,        // текущее значение таймера
            waitCheckKeyPad;

extern const uint8_t tabRH[];
extern uint8_t dataLed[6];

byte writePCF8574(byte data);
byte readPCF8574();
void initWiFiManag(void);
void displSwitch();

//------------------------------------------------------------
extern const
uint8_t error_[8],          // ПОМИЛКА_
        connect[10],        // підключено
        config[12],         // Конфігурація
        no_[3],             // НЕ_ 
        saved[10],          // збережено!
        file_damaged[15],   // Файл пошкоджено
        wordSet[12],        // Встановлений
        timeout_[9],        // тайм-аут_
        invalid[12],        // неправильний
        manual_control[15], // Ручне керування
        set_permissions[16],// Встанов.дозволів
        restored[10],       // відновлена
        save_time[13],      // Зберегти час?
        time_saved[14],     // Час збережено.
        no_permissions[13], // немаe дозволу
        sensorsWord[7];     // Датчики

#endif /* __MAIN_H */
