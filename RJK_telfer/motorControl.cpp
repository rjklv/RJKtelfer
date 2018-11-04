#include "motorControl.h"

void setupMotorIOpins(){
  pinMode( telf1EnaPin, OUTPUT );
  pinMode( telf1DirPin, OUTPUT );

  pinMode( telf2EnaPin, OUTPUT );
  pinMode( telf2DirPin, OUTPUT );

  pinMode( telferPSUpin, OUTPUT );

  pinMode( extButtonPSUpin, OUTPUT );

  telferPSUon();
  
  allStop();
}

void telferPSUon() {
  digitalWrite( telferPSUpin, HIGH );
}

void telferPSUoff() {
  digitalWrite( telferPSUpin, LOW );
}

void extButtonPSUon() {
  digitalWrite( extButtonPSUpin, LOW );
}

void extButtonPSUoff() {
  digitalWrite( extButtonPSUpin, HIGH );
}
void allStop(){
  //telferPSUoff();
  digitalWrite( telf1EnaPin, LOW );
  digitalWrite( telf2EnaPin, LOW );
}

void telf1Stop(){
  digitalWrite( telf1EnaPin, LOW );
}

void telf2Stop(){
  digitalWrite( telf2EnaPin, LOW );
}

void telf1fwd(){
  telferPSUon();
  digitalWrite( telf1DirPin, HIGH);
  digitalWrite( telf1EnaPin, HIGH);
}

void telf1rev(){
  telferPSUon();
  digitalWrite( telf1DirPin, LOW);
  digitalWrite( telf1EnaPin, HIGH);
}

void telf2fwd(){
  telferPSUon();
  digitalWrite( telf2DirPin, HIGH);
  digitalWrite( telf2EnaPin, HIGH);
}

void telf2rev(){
  telferPSUon();
  digitalWrite( telf2DirPin, LOW);
  digitalWrite( telf2EnaPin, HIGH);
}

void relaySequence(){
  allStop();
  delay( 1000 );
  telf1fwd();
  delay( 1000 );
  allStop();
  telf1rev();
  delay( 1000 );
  allStop();
  telf2fwd();
  delay( 1000 );
  allStop();
  telf2rev();
  delay( 1000 );
}


