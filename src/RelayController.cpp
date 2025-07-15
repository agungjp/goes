#include "RelayController.h"

RelayController::RelayController() {
  // Init default, diisi di begin()
}

void RelayController::begin() {
  // CB1
  relays[0].pinOpen = 6;   // CB1 open
  relays[0].pinClose = 7;  // CB1 close
  relays[0].state = 0;
  relays[0].startTime = 0;

  // CB2
  relays[1].pinOpen = 17;  // CB2 open (A3)
  relays[1].pinClose = 11; // CB2 close
  relays[1].state = 0;
  relays[1].startTime = 0;

  pinMode(relays[0].pinOpen, OUTPUT);
  pinMode(relays[0].pinClose, OUTPUT);
  pinMode(relays[1].pinOpen, OUTPUT);
  pinMode(relays[1].pinClose, OUTPUT);

  digitalWrite(relays[0].pinOpen, LOW);
  digitalWrite(relays[0].pinClose, LOW);
  digitalWrite(relays[1].pinOpen, LOW);
  digitalWrite(relays[1].pinClose, LOW);
}

void RelayController::trigger(uint8_t relayId, uint8_t command) {
  if (relayId < 1 || relayId > 2) return;
  Relay &r = relays[relayId-1];
  if (r.state != 0) return; // Sudah aktif, tunggu selesai
  if (command == 1) { // Open
    digitalWrite(r.pinOpen, HIGH);
    r.startTime = millis();
    r.state = 1;
  } else if (command == 2) { // Close
    digitalWrite(r.pinClose, HIGH);
    r.startTime = millis();
    r.state = 2;
  }
}

void RelayController::update() {
  for (uint8_t i = 0; i < 2; i++) {
    Relay &r = relays[i];
    if (r.state != 0 && (millis() - r.startTime >= r.duration)) {
      digitalWrite(r.pinOpen, LOW);
      digitalWrite(r.pinClose, LOW);
      r.state = 0;
    }
  }
}

void RelayController::resetAll() {
  for (uint8_t i = 0; i < 2; i++) {
    digitalWrite(relays[i].pinOpen, LOW);
    digitalWrite(relays[i].pinClose, LOW);
    relays[i].state = 0;
  }
}
