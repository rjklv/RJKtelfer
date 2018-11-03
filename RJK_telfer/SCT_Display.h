#ifndef SCTdisplay
#define SCTdisplay

#include <Arduino.h>

//for production version
#define laPin 7
#define clkPin 9
#define sdiPin 8

//for 1st test version
//#define laPin A3
//#define clkPin A4
//#define sdiPin A5
 
#define seg0PinA 17
#define seg0PinB 16
#define seg0PinC 30
#define seg0PinD 28
#define seg0PinE 27
#define seg0PinF 18
#define seg0PinG 29
#define seg0PinDP 31

#define seg1PinA 20
#define seg1PinB 19
#define seg1PinC 25
#define seg1PinD 24
#define seg1PinE 23
#define seg1PinF 22
#define seg1PinG 21
#define seg1PinDP 26

#define seg2PinA 10
#define seg2PinB 9
#define seg2PinC 7
#define seg2PinD 5
#define seg2PinE 4
#define seg2PinF 11
#define seg2PinG 6
#define seg2PinDP 8

#define seg3PinA 13
#define seg3PinB 12
#define seg3PinC 2
#define seg3PinD 1
#define seg3PinE 0
#define seg3PinF 15
#define seg3PinG 14
#define seg3PinDP 3

#define segZero   0x7E
#define segOne    0x30
#define segTwo    0x6D
#define segThree  0x79
#define segFour   0x33
#define segFive   0x5B
#define segSix    0x5F
#define segSeven  0x70
#define segEight  0x7F
#define segNine   0x7B

#define segBlank  0x00
#define segLetA   0x77
#define segLetB   0x1F
#define segLetC   0x4E
#define segLetD   0x3D
#define segLetE   0x4F
#define segLetF   0x47

#define segLetG   0x5E
#define segLetH   0x17
#define segLetI   0x06
#define segLetJ   0x3C

#define segLetK   0x08

#define segLetL   0x0E
#define segLetM   0x54
#define segLetN   0x15
#define segLetO   0x1D
#define segLetP   0x67

#define segLetQ   0x08

#define segLetR   0x05
#define segLetS   0x5B
#define segLetT   0x0F
#define segLetU   0x3E

#define segLetV   0x08

#define segLetW   0x1C
#define segLetX   0x37
#define segLetY   0x3B

#define segLetZ   0x08

const uint8_t charGen[] = {segZero, segOne, segTwo, segThree, segFour, segFive, segSix, segSeven, segEight, segNine,
                          segBlank, segLetA, segLetB, segLetC, segLetD, segLetE, segLetF, segLetG, segLetH, segLetI,
                          segLetJ, segLetK, segLetL, segLetM, segLetN, segLetO, segLetP, segLetQ, segLetR, segLetS,
                          segLetT, segLetU, segLetV, segLetW, segLetX, segLetY, segLetZ};

const uint8_t segPins[4][8] = {
  {seg0PinG, seg0PinF, seg0PinE, seg0PinD, seg0PinC, seg0PinB, seg0PinA, seg0PinDP},
  {seg1PinG, seg1PinF, seg1PinE, seg1PinD, seg1PinC, seg1PinB, seg1PinA, seg1PinDP},
  {seg2PinG, seg2PinF, seg2PinE, seg2PinD, seg2PinC, seg2PinB, seg2PinA, seg2PinDP},
  {seg3PinG, seg3PinF, seg3PinE, seg3PinD, seg3PinC, seg3PinB, seg3PinA, seg3PinDP}

};

void setupDisplay(void);
void sendToSCT(void);
//uint8_t doDigit(int dmxaddr, uint8_t dp, uint8_t mode);
uint8_t doDigit(uint8_t dig3, uint8_t dig2, uint8_t dig1, uint8_t dig0, uint8_t dp);

#endif
