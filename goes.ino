// Program Arduino UNO untuk membaca digital input dan mengontrol digital output

// Pin definisi
const int pinLocalRemote = 2; // 1: Local, 0: Remote
const int pinGFD = 3;         // 1: GFD ON, 0: GFD OFF
const int pinCB_1 = 4;        // Status CB pin 1
const int pinCB_2 = 5;        // Status CB pin 2
const int pinCBOpen = 6;      // PD6 (pin 12) - CB OPEN
const int pinCBClose = 7;     // PD7 (pin 13) - CB CLOSE

// Status yang akan disimpan
bool statusLocalRemote;
bool statusGFD;
String statusCB;

void setup() {
  Serial.begin(9600);

  // Set pin-pin sebagai input dengan resistor internal
  pinMode(pinLocalRemote, INPUT_PULLUP);
  pinMode(pinGFD, INPUT_PULLUP);
  pinMode(pinCB_1, INPUT_PULLUP);
  pinMode(pinCB_2, INPUT_PULLUP);

  // Set pin output untuk kontrol CB
  pinMode(pinCBOpen, OUTPUT);
  pinMode(pinCBClose, OUTPUT);
  digitalWrite(pinCBOpen, LOW);
  digitalWrite(pinCBClose, LOW);
}

void loop() {
  // Baca pin Local/Remote
  statusLocalRemote = digitalRead(pinLocalRemote) == LOW; // LOW = Remote, HIGH = Local

  // Baca pin GFD
  statusGFD = digitalRead(pinGFD);

  // Cetak hasil pembacaan ke Serial Monitor
  bacaStatusCB();
  Serial.print("Status: ");
  Serial.print(statusLocalRemote ? "Remote" : "Local");
  Serial.print(" | GFD: ");
  Serial.print(statusGFD ? "ON" : "OFF");
  Serial.print(" | CB: ");
  Serial.println(statusCB);

  // Cek input dari user di Serial
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // hilangkan spasi atau newline

    if (!statusLocalRemote) {
      Serial.println("MODE LOCAL - Perintah CB tidak bisa dieksekusi.");
    } else {
      if (command.equalsIgnoreCase("CB OPEN")) {
        if (statusCB == "CB OPEN") {
          Serial.println("CB sudah dalam keadaan OPEN - perintah dibatalkan.");
        } else {
          digitalWrite(pinCBOpen, HIGH);
          delay(800);
          digitalWrite(pinCBOpen, LOW);
          delay(1000); // beri waktu perubahan relay terbaca
          bacaStatusCB();
          Serial.println("Perintah CB OPEN dieksekusi.");
        }
      } else if (command.equalsIgnoreCase("CB CLOSE")) {
        if (statusCB == "CB CLOSE") {
          Serial.println("CB sudah dalam keadaan CLOSE - perintah dibatalkan.");
        } else {
          digitalWrite(pinCBClose, HIGH);
          delay(800);
          digitalWrite(pinCBClose, LOW);
          delay(1000); // beri waktu perubahan relay terbaca
          bacaStatusCB();
          Serial.println("Perintah CB CLOSE dieksekusi.");
        }
      } else {
        Serial.println("Perintah tidak dikenal.");
      }
    }
  }

  delay(1000); // baca setiap 1 detik
}

void bacaStatusCB() {
  // Baca status CB
  bool CB_pin6 = digitalRead(pinCB_1);
  bool CB_pin11 = digitalRead(pinCB_2);

  if (CB_pin6 == LOW && CB_pin11 == LOW) {
    statusCB = "UNKNOWN";
  }
  else if (CB_pin6 == HIGH && CB_pin11 == LOW) {
    statusCB = "CB OPEN";
  }
  else if (CB_pin6 == LOW && CB_pin11 == HIGH) {
    statusCB = "CB CLOSE";
  }
  else {
    statusCB = "UNKNOWN";
  }
}