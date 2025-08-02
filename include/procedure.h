#ifndef _PROCEDURE_H
#define _PROCEDURE_H
#include "main.h"

typedef struct {
    float Ki, iPart, Kp, pPart, output;
} PIDController;


extern PIDController pid[];
extern uint8_t seconds;

void beeperOn(uint8_t val);
uint16_t lampUpdate(uint16_t xpos, uint16_t ypos);
void rotate_trays(void);
bool check_freeze(uint8_t i);
uint8_t RelayPos(unsigned char cn, unsigned char hysteresis);
uint8_t RelayNeg(uint8_t cn, uint8_t on, uint8_t off);
uint8_t checkSetpoint(void);
uint8_t checkConfig(void);
void printSetPoint();
void saveSetPoint();
bool loadSetPoint();
void printAddress(DeviceAddress deviceAddress);
void printBinary(unsigned char byte);
uint8_t tableRH(int16_t maxT, int16_t minT);
void alarm(uint8_t cn);
void reset(void);
void initEnvironment(void);
void syncTime();

#endif /* _PROCEDURE_H */
