#ifndef motorControl
#define motorControl

#include <Arduino.h>

#define telf1EnaPin 13
#define telf1DirPin 12

#define telf2EnaPin 10
#define telf2DirPin 11

#define telferPSUpin A4
#define extButtonPSUpin A5

void setupMotorIOpins(void);

void allStop(void);
//uint8_t doDigit(uint8_t dig3, uint8_t dig2, uint8_t dig1, uint8_t dig0, uint8_t dp);

void extButtonPSUon(void);
void extButtonPSUoff(void);

void telferPSUon(void);
void telferPSUoff(void);

void telf1Stop(void);
void telf2Stop(void);

void telf1fwd(void);
void telf1rev(void);
void telf2fwd(void);
void telf2rev(void);

void relaySequence(void);
#endif
