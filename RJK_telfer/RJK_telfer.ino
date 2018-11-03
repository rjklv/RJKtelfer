#define debug

#define whiteMax 255          //maximum intensity for ledPin4 (white channel)

#define blankTime 30 * 1000   //display blank time in milliseconds
#define buttonTime 500        //timount for scroll start in milliseconds
#define scrollTime 20         //scroll speed in milliseconds (some magic used)

#define debounceTime 30       //button bouncing timeout in milliseconds
#define dmxFailTime 500       //timeout for DXM failure detect in milliseconds

#ifndef debug
#include <DMXSerial.h>
#endif

#include <EEPROM.h>
#include "Button.h"
#include "SCT_Display.h"
#include "motorControl.h"


// system states
#define sysStateBoot        0
#define sysStateWork        1
#define sysStateCalibrate   2



#define enc1ClkPin 2
#define enc1DirPin 4

#define enc2ClkPin 3
#define enc2DirPin 5

unsigned long lastPacket = dmxFailTime;

volatile int8_t sysState = 0;
volatile int8_t prewSysState = 0;

uint8_t dig3=10,dig2=10,dig1=10,dig0=10;

// button input pins for production version
#define modePin1    A3
#define upPin2      A2
#define downPin3    A1
#define enterPin4   A0

Button modeBut = Button(modePin1, true, true, debounceTime);
Button upBut = Button(upPin2, true, true, debounceTime);
Button downBut = Button(downPin3, true, true, debounceTime);
Button enterBut = Button(enterPin4, true, true, debounceTime);

uint8_t rawButtons;

unsigned long scrollNext;
unsigned long blankNext;

uint16_t dmxaddr = 1;
uint16_t trueDmxAddr = 1;
uint8_t dmxMode = true;
uint8_t blankScreen=false;

uint8_t dmxOk;


volatile int enc1count;
volatile int oldenc1count;

volatile int enc2count;
volatile int oldenc2count;

volatile int calibration1;
volatile int calibration2;

volatile int position1;
volatile int position2;

volatile int oldPosition1;
volatile int oldPosition2;

volatile int dmxPosition1;
volatile int dmxPosition2;

volatile uint16_t dmxValue1;
volatile uint16_t dmxValue2;

void setup() {
  
  #ifdef debug
  Serial.begin( 115200 );
  Serial.setTimeout(10);
  #endif

  #ifndef debug
  DMXSerial.init(DMXReceiver);
  #endif
  
  setupDisplay();
  setupMotorIOpins();

  EEPROM.get(0, trueDmxAddr);
  trueDmxAddr = constrain(trueDmxAddr, 1, 512);

  EEPROM.get(2, dmxMode);

  EEPROM.get(3, calibration1);
  EEPROM.get(5, calibration2);

  EEPROM.get(7, position1);
  EEPROM.get(9, position2);

  //rawButtons = doDigit(dig3,dig2,dig1,dig0,dmxOk);
  rawButtons = doDigit(1,2,3,4,false);

  attachInterrupt( digitalPinToInterrupt( enc1ClkPin ), enc1irq, RISING);

  attachInterrupt( digitalPinToInterrupt( enc2ClkPin ), enc2irq, RISING);
  //telf2fwd();
  //while(true){
  //  delay(1000);
  //}
  blankNext = millis() + blankTime;
}

void numberToDigits(int indata){
    int tmp = indata - (indata % 100);
    dig2 = tmp / 100;
    tmp = indata - (indata % 10) - (dig2 * 100);
    dig1 = tmp / 10;
    dig0 = indata - (dig2 * 100) - (dig1 * 10);  
}

/*
void dmxValueToDisplay() {
  uint8_t a = highByte(dmxValue2);
  uint8_t b = a & 0xF;

  if ( b > 9 ) {
    dig2 = b + 1;
  } else {
    dig2 = b;
  }

  b = ( a & 0xF0) >> 4;
  if ( b > 9 ) {
    dig3 = b + 1;
  } else {
    dig3 = b;
  }

  a = lowByte(dmxValue2);
  b = a & 0xF;
  if ( b > 9 ) {
    dig0 = b + 1;
  } else {
    dig0 = b;
  }

  b = ( a & 0xF0) >> 4;
  if ( b > 9 ) {
    dig1 = b + 1;
  } else {
    dig1 = b;
  }


}
*/
void dmxToPosition() {
  #ifndef debug
      if (dmxMode){
        if (( trueDmxAddr ) <= 512) {
          dmxValue1 = DMXSerial.read( trueDmxAddr ) << 8;
        }
        if (( trueDmxAddr + 1 ) <= 512) {
          dmxValue1 = dmxValue1 + DMXSerial.read( trueDmxAddr + 1 );
        }
        if (( trueDmxAddr + 2 ) <= 512) {
          dmxValue2 = DMXSerial.read( trueDmxAddr + 2) << 8;
        }
        if (( trueDmxAddr + 3 ) <= 512) {
          dmxValue2 = dmxValue2 + DMXSerial.read( trueDmxAddr + 3 );
        }

      }
  #endif
}
enum _menuState{
  work=0,
    menuAddress=1,
      menuAddressSet=2,
    menuCal=3,
      menuCalEnd=4,
    menuExit=5,
  sleep=6,
};

uint8_t menuState=work;

void menuStateMachine() {
  switch (menuState){
    case sleep:
      dig0=10;
      dig1=10;
      dig2=10;
      dig3=10;
    case work:
      if (blankScreen==true){
        return;
      }
      if (modeBut.wasPressed()) {
        menuState=menuAddress;      
        return;
      }
      if (enterBut.wasPressed()) {
        menuState=work;
        return;
      }
      if (dmxMode){
        dig3=10;
        numberToDigits( trueDmxAddr );
        //numberToDigits( DMXSerial.read( trueDmxAddr ));
        //dmxValueToDisplay();
        
      } else {
        dig3='Q'-54;
        dig2='Q'-54;
        dig1='Q'-54;
        dig0='Q'-54;        
      }
      return;
//***************************************************************************            
    case menuAddress:
      if (modeBut.wasPressed()) {
        menuState=work;
        return;
      }
      if (enterBut.wasPressed()) {
        menuState=menuAddressSet;
        dmxaddr=trueDmxAddr;
        return;
      }
      if (upBut.wasPressed()) {
        menuState=menuExit;
        return;
      }
      if (downBut.wasPressed()) {
        menuState=menuCal;
        return;
      }

      dig3='A'-54;
      dig2='D'-54;
      dig1='D'-54;
      dig0='R'-54;
      return;
//***************************************************************************      
    case menuAddressSet:
      if (modeBut.wasPressed()) {
        menuState=work;
        return;
      }
      if (enterBut.wasPressed()) {
        menuState=menuAddress;
        trueDmxAddr = dmxaddr;
        EEPROM.put(0, trueDmxAddr);
        return;
      }                
      if (upBut.wasPressed()) {
        // dmx addr increment
        dmxaddr++;
        if (dmxaddr < 1) dmxaddr = 512;
        if (dmxaddr > 512) dmxaddr = 1;
        dig3='A'-54;
        numberToDigits(dmxaddr);
        return;
      }
      if (downBut.wasPressed()) {
        // dmx addr decrement
        dmxaddr--;
        if (dmxaddr < 1) dmxaddr = 512;
        if (dmxaddr > 512) dmxaddr = 1;
        dig3='A'-54;
        numberToDigits(dmxaddr);
        return;
      }
    if ( upBut.pressedFor(buttonTime) && (millis() >= scrollNext)) {
      dmxaddr++;
      scrollNext = millis() + scrollTime;
      if (dmxaddr < 1) dmxaddr = 512;
      if (dmxaddr > 512) dmxaddr = 1;
      dig3='A'-54;
      numberToDigits(dmxaddr);
    }

    if ( downBut.pressedFor(buttonTime) && (millis() >= scrollNext)) {
      dmxaddr--;
      scrollNext = millis() + scrollTime;
      if (dmxaddr < 1) dmxaddr = 512;
      if (dmxaddr > 512) dmxaddr = 1;
      dig3='A'-54;
      numberToDigits(dmxaddr);

    }

      dig3='A'-54;
      numberToDigits(dmxaddr);
      return;
//***************************************************************************
    case menuCal:
      if (modeBut.wasPressed()) {
        menuState=work;
        return;
      }
      if (enterBut.wasPressed()) {
        menuState=menuCalEnd;
        sysState = sysStateCalibrate;
        calibration1=0;
        calibration2=0;
        extButtonPSUon();
        prewSysState=sysState;        
        return;
      }
      if (upBut.wasPressed()) {
        menuState=menuAddress;
        return;
      }
      if (downBut.wasPressed()) {
        menuState=menuExit;
        return;
      }

      dig3='C'-54;
      dig2='A'-54;
      dig1='L'-54;
      dig0=10;      
      return;
//***************************************************************************
    case menuCalEnd:
      if (modeBut.wasPressed()) {
        menuState=work;
        return;
      }
      if (enterBut.wasPressed()) {
        sysState = sysStateWork;
        if ( prewSysState == sysStateCalibrate ){
          position1 = calibration1;
          position2 = calibration2;

          EEPROM.put(3, calibration1);
          EEPROM.put(5, calibration2);
          EEPROM.put(7, position1);
          EEPROM.put(9, position2);

          extButtonPSUoff();
          
          #ifdef debug
          dmxPosition1 = calibration1;
          dmxPosition2 = calibration2;
          #endif
          prewSysState=sysState;
        }
          menuState = menuCal; 
        return;
      }
      if (upBut.wasPressed()) {
        //menuState=menuAddress;
        return;
      }
      if (downBut.wasPressed()) {
        //menuState=menuExit;
        return;
      }

      dig3='C'-54;
      dig2=10;
      dig1='U'-54;
      dig0='P'-54;  
      return;
            
//***************************************************************************
    case menuExit:
      if (modeBut.wasPressed()) {
        menuState=work;
        return;
      }
      if (enterBut.wasPressed()) {
        menuState=work;
        return;
      }
      if (upBut.wasPressed()) {
        menuState=menuCal;
        return;
      }
      if (downBut.wasPressed()) {
        menuState=menuAddress;
        return;
      }

      dig3='E'-54;
      dig2='X'-54;
      dig1='I'-54;
      dig0='T'-54;      
      return;
    default:
      return;
  }  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //relaySequence();

  #ifndef debug
  lastPacket = DMXSerial.noDataSince();
  dmxOk = lastPacket < dmxFailTime ? true : false;
  #endif

  #ifdef debug
  lastPacket = 500;
  dmxOk=true;
  #endif
  
  rawButtons = doDigit(dig3,dig2,dig1,dig0,dmxOk);
  
  modeBut.read(rawButtons);
  upBut.read(rawButtons);
  downBut.read(rawButtons);
  enterBut.read(rawButtons);

  dmxToPosition();
  
  menuStateMachine();

  if ( ( menuState!=sleep ) && ( enterBut.isChanged() || downBut.isChanged() || upBut.isChanged() ) ) {
    //menuState=work;
    blankNext = millis() + blankTime;
    blankScreen=false;
  }
  
  if ( modeBut.isChanged()) {
    if ( menuState==sleep ) {
      menuState=work;
    }
    blankNext = millis() + blankTime;
    blankScreen=false;
  } else {    
    if (millis() >= blankNext) {
      //doBlankScreen
      menuState=sleep;
      blankScreen=true;
      dig0=10;
      dig1=10;
      dig2=10;
      dig3=10;
    }
  }

  #ifdef debug
  if ( Serial.available() ) {
    int a = Serial.parseInt();
    Serial.print( "Return: " );
    Serial.println( a );
    switch (a) {
      case (-1* sysStateWork):
        sysState = sysStateWork;
        if ( prewSysState == sysStateCalibrate ){
          position1 = calibration1;
          position2 = calibration2;
          prewSysState=sysState;
        }
        break;
/*
      case (-1* sysStateCalibrate):
        sysState = sysStateCalibrate;
        calibration1=0;
        calibration2=0;
        prewSysState=sysState;
        break;
*/
      case 0:
        break;
      default:
        dmxPosition1 = abs( a );
        dmxPosition2 = abs( a );
        break;
    }
    Serial.print( "Sys state: " );
    Serial.println( sysState );
  }
  #endif
/*
  if ( menuState == menuCalBegin ) {
        sysState = sysStateCalibrate;
        calibration1=0;
        calibration2=0;
        prewSysState=sysState;
  }
*/

  
  if ( oldenc1count != enc1count ) {

    #ifdef debug
    Serial.print( "State: " );
    Serial.print( sysState );
    //Serial.print( "\t\tEnc1:" );
    //Serial.print( enc1count );
    //Serial.print( "\t\tEnc2: " );
    //Serial.print( enc2count );
    Serial.print( "\t\tPos1: " );
    Serial.print( position1 );
    Serial.print( "\t\tPos2: " );
    Serial.print( position2 );
    Serial.print( "\t\tCal1: " );
    Serial.print( calibration1 );
    Serial.print( "\t\tCal2: " );
    Serial.print( calibration2 );
    Serial.print( "\t\tDMX1: " );
    Serial.print( dmxPosition1 );
    Serial.print( "\t\tDMX2: " );
    Serial.println( dmxPosition2 );
    #endif
    
    oldenc1count = enc1count;
  }

/*
  if ( oldenc2count != enc2count ) {
    Serial.print( enc1count );
    Serial.print("\t");
    Serial.println( enc2count );
    oldenc2count = enc2count;
  }
*/

  if ( sysState == sysStateWork ) {
    extButtonPSUoff();

    if ( oldPosition1 != position1 ) {
      EEPROM.put(7, position1);
    }

    if ( oldPosition2 != position2 ) {
      EEPROM.put(9, position2);
    }

    oldPosition1=position1;
    oldPosition2=position2;

    if ( position1 < 0 ) {
      telf1Stop();
      return;
    }
  
    if ( position1 > calibration1 ) {
      telf1Stop();
      return;
    }

    if ( position1 == dmxPosition1 ) {
      telf1Stop();
    }
    if ( position1 < dmxPosition1 ) {
      telf1fwd();
    }
  
    if (position1 > dmxPosition1 ) {
      telf1rev();
    }

    if ( position1 < 0 ) {
      telf1Stop();
      return;
    }
//***********************************************************************
    if ( position2 > calibration2 ) {
      telf2Stop();
      return;
    }

    if ( position2 == dmxPosition2 ) {
      telf2Stop();
    }
    if ( position2 < dmxPosition2 ) {
      telf2fwd();
    }
  
    if (position2 > dmxPosition2 ) {
      telf2rev();
    }
    if ( position2 < 0 ) {
      telf2Stop();
      return;
    }

    
  }
}

volatile uint8_t tmp1DirPin;

void enc1irq(){

  tmp1DirPin = digitalRead( enc1DirPin );
  
  if ( sysState == sysStateCalibrate ) {
    if ( tmp1DirPin ) {
      calibration1++;
    } else {
      calibration1--;
    }

  }
  if ( sysState == sysStateWork ) {
    if ( tmp1DirPin ) {
      position1++;
    } else {
      position1--;
    }

  }
  
  if ( tmp1DirPin ) {
    enc1count++;
  } else {
    enc1count--;
  }
}

volatile uint8_t tmp2DirPin;

void enc2irq(){

  tmp2DirPin = digitalRead( enc2DirPin );
  
  if ( sysState == sysStateCalibrate ) {
    if ( tmp2DirPin ) {
      calibration2++;
    } else {
      calibration2--;
    }

  }
  if ( sysState == sysStateWork ) {
    if ( tmp2DirPin ) {
      position2++;
    } else {
      position2--;
    }

  }

  if ( tmp2DirPin ) {
    enc2count++;
  } else {
    enc2count--;
  }
}


