// strings_en.h
#ifndef STRINGS_EN_H
#define STRINGS_EN_H

#include <Arduino.h>

#define MAIN_MENU           F("[ \
    {\"command\":\"help\",  \"description\":\"Get help using the bot\"}, \
    {\"command\":\"start\", \"description\":\"A message is sent when you open a chat with a bot\"}, \
    {\"command\":\"status\",\"description\":\"Current device status.\"} \
    ]")
#define WORD_TITLE          F("```\n QUADUS")
#define ID_TITLE            F("  ID:")
#define WORD_T1             F("- t1: ")
#define WORD_T2             F("- t2: ")
#define WORD_HUMIDITY       F("- humidity: ")
#define WORD_LIGHT          F("- lighting: ")
#define WORD_TIMER1         F("- timer 1: ")
#define WORD_TIMER2         F("- timer 2: ")
#define WORD_TIMER3         F("- timer 3: ")
#define WORD_DAMPER         F("- damper: ")
#define WORD_PROGRAM        F("- program: ")
#define WORD_DATE           F("- date: ")
#define WORD_ERROR1         F("- error: sensor 1")
#define WORD_ERROR2         F("- error: sensor 2")
#define WORD_ERROR4         F("- deviation sensor 1")
#define WORD_ERROR8         F("- deviation sensor 2")
#define WORD_DAYS           F("days")
#define TXT_START           F("/start")
#define TXT_OPTIONS         F("/options")
#define TXT_STATUS          F("/status")

#define STR_CONNECTING      F("Connecting to server")
#define STR_DATA_RETRIEVED  F("Data retrieved successfully")
#define STR_ERROR           F("An error occurred")
#define SENSOR_ERROR_1      F("SENSOR ERROR #1\n")
#define SENSOR_ERROR_2      F("SENSOR ERROR #2\n")
#define SENSOR_ERROR_4      F("TEMPERATURE DEVIATION #1\n")
#define SENSOR_ERROR_8      F("TEMPERATURE DEVIATION #2\n")
#define NEW_STR             F("\n")

#endif // STRINGS_EN_H