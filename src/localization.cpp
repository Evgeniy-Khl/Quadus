#include "main.h"

#if defined(LANG_RU)
const char* const strings[] = {
    "ТРЕВОГА: Ошибка датчика нагрева!",
    "ТРЕВОГА: Ошибка датчика влажности!",
    "Целевые значения T2/RH достигнуты.",
    "ТРЕВОГА: T2/RH вне диапазона!",
    "Целевое значение T1 достигнуто.",
    "ТРЕВОГА: Температура T1 вне диапазона!",
    "Запуск системы. Версия: ",
    "Датчики: DHT22 обнаружен",
    "Датчики: DS18B20 обнаружено",
    "Датчики: НЕ ОБНАРУЖЕНЫ",
    "Ежедневная синхронизация RTC успешна.",
    "ОШИБКА: Файловой системы",
    "ОШИБКА: Сбой сериализации JSON",
    "Конфигурация сохранена в ФС.",
    "Ручное управление активировано.",
    "Ручное: Свет ",
    "Ручное: Нагрев ",
    "Ручное: Увлажн. ",
    "Ручное: Реле1 ",
    "Ручное: Реле2 ",
    "Ручное: Реле3 ",
    "Автоматическое управление восстановлено.",
    "Лог очищен (лимит размера)."
};
#elif defined(LANG_UA)
const char* const strings[] = {
    "ТРИВОГА: Помилка датчика нагріву!",
    "ТРИВОГА: Помилка датчика вологості!",
    "Цільові значення T2/RH досягнуті.",
    "ТРИВОГА: T2/RH поза діапазоном!",
    "Цільове значення T1 досягнуто.",
    "ТРИВОГА: Температура T1 поза діапазоном!",
    "Запуск системи. Версія: ",
    "Датчики: DHT22 виявлено",
    "Датчики: DS18B20 виявлено",
    "Датчики: НЕ ВИЯВЛЕНО",
    "Щоденна синхронізація RTC успішна.",
    "ПОМИЛКА: Файлової системи",
    "ПОМИЛКА: Збій серіалізації JSON",
    "Конфігурація збережена у ФС.",
    "Ручне управління активовано.",
    "Ручне: Світло ",
    "Ручне: Нагрів ",
    "Ручне: Зволож. ",
    "Ручне: Реле1 ",
    "Ручне: Реле1 ",
    "Ручне: Реле1 ",
    "Автоматичне управління відновлено.",
    "Лог очищено (ліміт розміру)."
};
#else // Default to English (LANG_EN)
const char* const strings[] = {
    "ALARM: Heater sensor error!",
    "ALARM: Humidity sensor error!",
    "Climate T2/RH target reached.",
    "ALARM: T2/RH out of range!",
    "Climate T1 target reached.",
    "ALARM: T1 temperature out of range!",
    "System startup. Version: ",
    "Sensors: DHT22 detected",
    "Sensors: DS18B20 detected",
    "Sensors: NONE found",
    "Daily RTC sync successful.",
    "ERROR: File system",
    "ERROR: JSON serialization failed",
    "Configuration saved to FS.",
    "Manual override activated.",
    "Manual: Light ",
    "Manual: Heater ",
    "Manual: Humidi ",
    "Manual: Relay1 ",
    "Manual: Relay2 ",
    "Manual: Relay3 ",
    "Automatic control restored.",
    "Log rotated (size limit)."
};
#endif

const char* getMsg(LogMsgId id) {
    return strings[id];
}
