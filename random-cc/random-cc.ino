#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

byte MIDI_CH = 1;

// button pins
byte S3_PIN = 4;
byte S4_PIN = 3;
byte S5_PIN = 2;

// pot pins
byte RV1_PIN = A1;
byte RV2_PIN = A0;

// LED pins
byte GRN_LED_PIN = 6;
byte RED_LED_PIN = 7;

// declare read state
bool S3_STATE = 1;
bool S4_STATE = 1;
bool S5_STATE = 1;
int RV1_STATE = 0;
int RV2_STATE = 0;

// 0 = offset (no LEDs)
// 1 = depth (green LED)
// 2 = CC (red LED)
byte RV1_edit_mode = 0;
int offset = 0;
int depth = 127;

// 74 is commonly the CC for filter cutoff
byte cc = 74;

int MIN_SPEED = 60;
int MAX_SPEED = 1000;
int speed = MAX_SPEED;

unsigned long last_cc_sent = 0;

int read_and_map(byte pin) {
  int read = analogRead(pin);
  return map(read, 0, 1023, 0, 127);
}

// code that runs at the beginning of the script
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // set pin modes
  pinMode(S3_PIN, INPUT_PULLUP);
  pinMode(S4_PIN, INPUT_PULLUP);
  pinMode(S5_PIN, INPUT_PULLUP);
  pinMode(RV1_PIN, INPUT);
  pinMode(RV2_PIN, INPUT);
  pinMode(GRN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  // initialize read state
  S3_STATE = digitalRead(S3_PIN);
  S4_STATE = digitalRead(S4_PIN);
  S5_STATE = digitalRead(S5_PIN);
  RV1_STATE = read_and_map(RV1_PIN);
  RV2_STATE = read_and_map(RV2_PIN);

  // turn off LEDs
  digitalWrite(GRN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, HIGH);
}

// code that runs on each update loop of the script
void loop() {
  // read pins
  bool S3_READ = digitalRead(S3_PIN);
  bool S4_READ = digitalRead(S4_PIN);
  bool S5_READ = digitalRead(S5_PIN);
  int RV1_READ = read_and_map(RV1_PIN);
  int RV2_READ = read_and_map(RV2_PIN);

  byte old_RV1_edit_mode = RV1_edit_mode;

  // update edit state if button change detected
  if (S3_READ != S3_STATE) {
    S3_STATE = S3_READ;
    // set RV1_edit_mode to offset
    if (S3_STATE == LOW) {
      RV1_edit_mode = 0;
    }
  }

  if (S4_READ != S4_STATE) {
    S4_STATE = S4_READ;
    // set RV1_edit_mode to depth
    if (S4_STATE == LOW) {
      RV1_edit_mode = 1;
    }
  }

  if (S5_READ != S5_STATE) {
    S5_STATE = S5_READ;
    // set RV1_edit_mode to offset
    if (S5_STATE == LOW) {
      RV1_edit_mode = 2;
    }
  }

  // watch for changes to edit mode and update LEDs
  if (old_RV1_edit_mode != RV1_edit_mode) {
      digitalWrite(GRN_LED_PIN, RV1_edit_mode != 1);
      digitalWrite(RED_LED_PIN, RV1_edit_mode != 2);
  }

  // if RV1 changes, look at the edit mode
  // to determine which setting should be updated
  if (RV1_READ != RV1_STATE) {
    RV1_STATE = RV1_READ;
    if (RV1_edit_mode == 0) {
      offset = RV1_STATE;
    } else if (RV1_edit_mode == 1) {
      depth = RV1_STATE;
    } else if (RV1_edit_mode == 2) {
      cc = RV1_STATE;
    }
  }

  // use RV2 to update the speed of the interval
  if (RV2_READ != RV2_STATE) {
    RV2_STATE = RV2_READ;
    // we map pot reads from 0-1023 to 0-127 for MIDI
    // but now we're mapping 0-127 to MIN/MAX_SPEED
    speed = map(RV2_STATE, 0, 127, MIN_SPEED, MAX_SPEED);
  }

  // 1. check the current time
  // 2. compare it to the last time we sent a message
  // 3. if we're past our interval, send a new message
  unsigned long now = millis();
  if (now - last_cc_sent > speed) {
    last_cc_sent = now;
    // max is offset + depth, as long as it's between 0 and 127
    int max_val = constrain(offset + depth, 0, 127);
    int rand_val = random(offset, max_val);
    // sendControlChange(CC, value, channel)
    MIDI.sendControlChange(cc, rand_val, MIDI_CH);
  }
}