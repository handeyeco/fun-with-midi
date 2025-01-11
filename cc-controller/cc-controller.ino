#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

byte MIDI_CH = 1;

// pot pins
byte RV1_PIN = A1;
byte RV2_PIN = A0;

// declare read state
int RV1_STATE = 0;
int RV2_STATE = 0;

int read_and_map(byte pin) {
  int read = analogRead(pin);
  return map(read, 0, 1023, 0, 127);
}

// code that runs at the beginning of the script
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // set pin modes
  pinMode(RV1_PIN, INPUT);
  pinMode(RV2_PIN, INPUT);

  // initialize read state
  RV1_STATE = read_and_map(RV1_PIN);
  RV2_STATE = read_and_map(RV2_PIN);
}

// code that runs on each update loop of the script
void loop() {
  // read pins
  int RV1_READ = read_and_map(RV1_PIN);
  int RV2_READ = read_and_map(RV2_PIN);

  // RV1 adjusts which CC channel we're changing
  if (RV1_READ != RV1_STATE) {
    RV1_STATE = RV1_READ;
  }
  // RV2 adjusts the CC value
  if (RV2_READ != RV2_STATE) {
    RV2_STATE = RV2_READ;
    // sendControlChange(CC, value, channel)
    MIDI.sendControlChange(RV1_STATE, RV2_STATE, MIDI_CH);
  }
}