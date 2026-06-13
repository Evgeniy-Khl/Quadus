#include "Logger.h"
#include "main.h"

Logger sysLogger;

void Logger::log(const String& message) {
    maintain();
    File f = LittleFS.open(LOG_FILE, "a");
    if (f) {
        String entry = getTimestamp() + " " + message + "\n";
        f.print(entry);
        f.close();
        MYDEBUG_PRINT("LOG: "); MYDEBUG_PRINTLN(entry);
    }
}

void Logger::clear() {
    LittleFS.remove(LOG_FILE);
    MYDEBUG_PRINTLN("Log cleared.");
}

String Logger::getLogs() {
    if (!LittleFS.exists(LOG_FILE)) return "No logs found.";
    File f = LittleFS.open(LOG_FILE, "r");
    if (!f) return "Error opening log file.";
    String content = f.readString();
    f.close();
    return content;
}

void Logger::maintain() {
    if (!LittleFS.exists(LOG_FILE)) return;
    File f = LittleFS.open(LOG_FILE, "r");
    if (f) {
        size_t size = f.size();
        f.close();
        if (size > MAX_LOG_SIZE) {
            // Very simple rotation: delete if too big
            // In a more complex version, we could keep the last 50% of the file
            clear();
            log(getMsg(MSG_LOG_ROTATED));
        }
    }
}

String Logger::getTimestamp() {
    if (timeinfo && timeinfo->tm_year >= 100) {
        char buf[25];
        snprintf(buf, sizeof(buf), "[%02d.%02d %02d:%02d:%02d]", 
                 timeinfo->tm_mday, timeinfo->tm_mon + 1,
                 timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        return String(buf);
    }
    return "[00.00 00:00:00]";
}
