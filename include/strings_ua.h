// strings_ua.h
#ifndef STRINGS_UA_H
#define STRINGS_UA_H

#include <Arduino.h>

#define MAIN_MENU           F("[ \
    {\"command\":\"help\",  \"description\":\"Отримайте допомогу з використанням бота\"}, \
    {\"command\":\"start\", \"description\":\"Повідомлення яке надсилається, коли ви відкриваєте чат із ботом\"}, \
    {\"command\":\"status\",\"description\":\"Поточний стан пристрою.\"} \
    ]")
#define WORD_QUADUS         F("КВАДУС № ")
#define WORD_TITLE          F("```\n КВАДУС № ")
#define WORD_T1             F("- температура 1: ")
#define WORD_T2             F("- температура 2: ")
#define WORD_HUMIDITY       F("- вологість: ")
#define WORD_LIGHT          F("- освітлення: ")
#define WORD_TIMER1         F("- таймер 1: ")
#define WORD_TIMER2         F("- таймер 2: ")
#define WORD_TIMER3         F("- таймер 3: ")
#define WORD_DAMPER         F("- заслінка: ")
#define WORD_PROGRAM        F("- програма: ")
#define WORD_DATE           F("- дата: ")
#define WORD_ERROR1         F("- помилка: датчик 1")
#define WORD_ERROR2         F("- помилка: датчик 2")
#define WORD_ERROR4         F("- відхилення датчик 1")
#define WORD_ERROR8         F("- відхилення датчик 2")
#define WORD_DAYS           F("діб")
#define TXT_START           F("/start")
#define TXT_OPTIONS         F("/options")
#define TXT_STATUS          F("/status")

#define STR_CONNECTING      F("Підключення до сервера")
#define STR_DATA_RETRIEVED  F("Дані успішно отримані")
#define STR_ERROR           F("Сталася помилка")
#define SENSOR_ERROR_1      F("ПОМИЛКА ДАТЧИКА №1\n")
#define SENSOR_ERROR_2      F("ПОМИЛКА ДАТЧИКА №2\n")
#define SENSOR_ERROR_4      F("ВІДХИЛЕННЯ ТЕМПЕРАТУРИ №1\n")
#define SENSOR_ERROR_8      F("ВІДХИЛЕННЯ ТЕМПЕРАТУРИ №2\n")
#define NEW_STR             F("\n")

#endif // STRINGS_UA_H