
#define m1ena 13
#define m1dir 12

#define m2ena 10
#define m2dir 11

#define enc1clk 2
#define enc1dir 4

#define enc2clk 3
#define enc2dir 5

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(m1ena,OUTPUT);
  pinMode(m1dir,OUTPUT);

  pinMode(m2ena,OUTPUT);
  pinMode(m2dir,OUTPUT);
  allOff();

  attachInterrupt(digitalPinToInterrupt(enc1clk),enc1irq, RISING);

  attachInterrupt(digitalPinToInterrupt(enc2clk),enc2irq, RISING);
  //telf2fwd();
  //while(true){
  //  delay(1000);
  //}
}

volatile int enc1count;
volatile int oldenc1count;

volatile int enc2count;
volatile int oldenc2count;

void loop() {
  // put your main code here, to run repeatedly:
  //relaySequence();
  if (oldenc1count != enc1count) {
    Serial.print(enc1count);
    Serial.print("\t");
    Serial.println(enc2count);
    oldenc1count = enc1count;
  }

  if (oldenc2count != enc2count) {
    Serial.print(enc1count);
    Serial.print("\t");
    Serial.println(enc2count);
    oldenc2count = enc2count;
  }


}


void enc1irq(){
  if (digitalRead(enc1dir)) {
    enc1count++;
  } else {
    enc1count--;
  }
}


void enc2irq(){
  if (digitalRead(enc2dir)) {
    enc2count++;
  } else {
    enc2count--;
  }
}
void relaySequence(){
  allOff();
  delay(1000);
  telf1fwd();
  delay(1000);
  allOff();
  telf1rev();
  delay(1000);
  allOff();
  telf2fwd();
  delay(1000);
  allOff();
  telf2rev();
  delay(1000);
}
void allOff(){
  digitalWrite(m1ena,LOW);
  digitalWrite(m2ena,LOW);
}

void telf1fwd(){
  digitalWrite(m1dir,HIGH);
  digitalWrite(m1ena,HIGH);
}

void telf1rev(){
  digitalWrite(m1dir,LOW);
  digitalWrite(m1ena,HIGH);
}

void telf2fwd(){
  digitalWrite(m2dir,HIGH);
  digitalWrite(m2ena,HIGH);
}

void telf2rev(){
  digitalWrite(m2dir,LOW);
  digitalWrite(m2ena,HIGH);
}


