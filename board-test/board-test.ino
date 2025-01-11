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

// pot change threshold,
// for what we consider a change
int RV_THRESH = 3;

// code that runs at the beginning of the script
void setup() {
  // setup for serial printing
  Serial.begin(9600);

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
  RV1_STATE = analogRead(RV1_PIN);
  RV2_STATE = analogRead(RV2_PIN);
}

// code that runs on each update loop of the script
void loop() {
  // read pins
  bool S3_READ = digitalRead(S3_PIN);
  bool S4_READ = digitalRead(S4_PIN);
  bool S5_READ = digitalRead(S5_PIN);
  int RV1_READ = analogRead(RV1_PIN);
  int RV2_READ = analogRead(RV2_PIN);

  // if a button is pressed, light LED
  bool pressed = S3_READ && S4_READ && S5_READ;
  digitalWrite(GRN_LED_PIN, pressed);
  digitalWrite(RED_LED_PIN, pressed);

  // print and update state if change detected
  if (S3_READ != S3_STATE) {
    Serial.println("S3");
    S3_STATE = S3_READ;
  }
  if (S4_READ != S4_STATE) {
    Serial.println("S4");
    S4_STATE = S4_READ;
  }
  if (S5_READ != S5_STATE) {
    Serial.println("S5");
    S5_STATE = S5_READ;
  }
  if (abs(RV1_READ - RV1_STATE) > RV_THRESH) {
    Serial.println("RV1");
    RV1_STATE = RV1_READ;
  }
  if (abs(RV2_READ - RV2_STATE) > RV_THRESH) {
    Serial.println("RV2");
    RV2_STATE = RV2_READ;
  }
}