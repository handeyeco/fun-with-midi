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

// filter cutoff on NTS-1
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

  // print and update state if change detected
  if (S3_READ != S3_STATE) {
    S3_STATE = S3_READ;
    // set RV1_edit_mode to offset
    if (!S3_STATE) {
      RV1_edit_mode = 0;
      digitalWrite(GRN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, HIGH);
    }
  }
  if (S4_READ != S4_STATE) {
    S4_STATE = S4_READ;
    // set RV1_edit_mode to depth
    if (!S4_STATE) {
      RV1_edit_mode = 1;
      digitalWrite(GRN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, HIGH);
    }
  }
  if (S5_READ != S5_STATE) {
    S5_STATE = S5_READ;
    // set RV1_edit_mode to offset
    if (!S5_STATE) {
      RV1_edit_mode = 2;
      digitalWrite(GRN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
    }
  }
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
  if (RV2_READ != RV2_STATE) {
    RV2_STATE = RV2_READ;
    speed = map(RV2_STATE, 0, 127, MIN_SPEED, MAX_SPEED);
  }

  unsigned long now = millis();
  if (now - last_cc_sent > speed) {
    last_cc_sent = now;
    int max_val = constrain(offset + depth, 0, 127);
    int rand_val = random(offset, max_val);
    MIDI.sendControlChange(cc, rand_val, MIDI_CH);
  }
}