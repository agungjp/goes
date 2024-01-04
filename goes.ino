#include "iec104.h"

SoftwareSerial  SerialAT(10,8); // TX, RX
iec104 Srv(SerialAT);

// millis
unsigned long previousMillis=0;
unsigned long intervalMillis = 50000;
unsigned long previousMillis1=0;
unsigned long currentMillis = 0;
const unsigned long duration = 600000;

int oldReads;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  delay(1000);

  Serial.println("Gardu Online Surveillance - MAIN");

  wdt_disable();
  delay(3000);
  wdt_enable(WDTO_8S);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);

  pinMode(9,OUTPUT); // soft restart
  pinMode(2, INPUT_PULLUP); // di r/l
  digitalWrite(9,LOW);
  digitalWrite(2,LOW);

  delay(1000);
  wdt_reset();
  softRestartModem();
  wdt_reset();
  delay(7000);
  wdt_reset();

  SerialAT.begin(9600);
  while(!SerialAT);
  delay(1000);
  
  wdt_reset();
  setup_connection();
  wdt_reset();

  Serial.println("24 Arduino Ready Connect to SCADA");
}

void(* resetFunc) (void) = 0;

void loop() {

  wdt_reset();
  currentMillis = millis();

  int stat = digitalRead(2);
  int local = 0;
  
  if(stat==HIGH){
    local=1;
  }else{
    local=0;
  }


  Srv.Run();
  // Srv.setLocal(local);

  //  fungsi cek lru fail
  if((currentMillis-previousMillis1) >= (duration)){
    if(Srv.Reads > oldReads){
      Serial.println("GOES AMAN");
    }else{
      Serial.println("GOES RESTART");
      resetFunc();
    }
    oldReads = Srv.Reads;
    previousMillis1 = currentMillis; 
  }

}

void setup_connection(){
  delay(5000);
  wdt_reset();
  SerialAT.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  delay(5000);
  wdt_reset();
  SerialAT.println("AT+CGATT=1"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  wdt_reset();
  SerialAT.println("AT+CIPMODE=1"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  wdt_reset();
  SerialAT.println("AT+CSTT=\"m2mplnapd\""); //Once the handshake test is successful, it will back to OK
  updateSerial();
  wdt_reset();
  SerialAT.println("AT+CIICR"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  wdt_reset();
  SerialAT.println("AT+CIFSR"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  wdt_reset();
  SerialAT.println("AT+CIPSERVER=1,2404"); //Once the handshake test is successful, it will back to OK
  updateSerial();
}

void updateSerial(){
  delay(2000);
  while (SerialAT.available()) {
    Serial.write(SerialAT.read());//Forward what Serial received to Software Serial Port
  }
  while(Serial.available()) {
    SerialAT.write(Serial.read());//Forward what Software Serial received to Serial Port
  } 
}

void softRestartModem(){
  digitalWrite(9,HIGH);
  delay(500);
  digitalWrite(9,LOW);
  delay(500);
}
