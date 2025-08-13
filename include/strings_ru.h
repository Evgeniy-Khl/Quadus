// strings_ru.h
#ifndef STRINGS_RU_H
#define STRINGS_RU_H

#include <Arduino.h>

#define MAIN_MENU           F("[ \
    {\"command\":\"help\",  \"description\":\"Получите помощь в использовании бота\"}, \
    {\"command\":\"start\", \"description\":\"Сообщение отправляется, когда вы открываете чат с ботом\"}, \
    {\"command\":\"status\",\"description\":\"Текущее состояние устройства.\"} \
    ]")
#define WORD_QUADUS         F("КВАДУС № ")
#define WORD_TITLE          F("```\n КВАДУС № ")
#define WORD_T1             F("- температура 1: ")
#define WORD_T2             F("- температура 2: ")
#define WORD_HUMIDITY       F("- влажность: ")
#define WORD_LIGHT          F("- освещение: ")
#define WORD_TIMER1         F("- таймер 1: ")
#define WORD_TIMER2         F("- таймер 2: ")
#define WORD_TIMER3         F("- таймер 3: ")
#define WORD_DAMPER         F("- заслонка: ")
#define WORD_PROGRAM        F("- программа: ")
#define WORD_DATE           F("- дата: ")
#define WORD_ERROR1         F("- ошибка: датчик 1")
#define WORD_ERROR2         F("- ошибка: датчик 2")
#define WORD_ERROR4         F("- отклонение датчик 1")
#define WORD_ERROR8         F("- отклонение датчик 2")
#define TXT_START           F("/start")
#define TXT_OPTIONS         F("/options")
#define TXT_STATUS          F("/status")

#define STR_CONNECTING      F("Подключение к серверу")
#define STR_DATA_RETRIEVED  F("Данные успешно получены")
#define STR_ERROR           F("Произошла ошибка")
#define SENSOR_ERROR_1      F("SENSOR ERROR #1\n")
#define SENSOR_ERROR_2      F("SENSOR ERROR #2\n")
#define SENSOR_ERROR_4      F("TEMPERATURE DEVIATION #1\n")
#define SENSOR_ERROR_8      F("TEMPERATURE DEVIATION #2\n")
#define NEW_STR             F("\n")

#endif // STRINGS_RU_H