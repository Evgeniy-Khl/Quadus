#ifndef _SAVEDAILYDATA_H
#define _SAVEDAILYDATA_H

#include "main.h"

void saveDailyDataToFile(int day);
void clearEEPROM();
void checkAndManageSpace();
int findOldestDay();
void deleteFilesForDay(int day);

#endif /* _SAVEDAILYDATA_H */
