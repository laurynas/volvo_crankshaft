//Lbus = LIN BUS from Car
//Vss = Ground
//Vbb = +12V

// MCP2004 LIN bus frame:
// ZERO_BYTE SYN_BYTE ID_BYTE DATA_BYTES.. CHECKSUM_BYTE

#define DEBUG true

#include "Keyboard.h"
#include "SoftwareSerial.h"

// https://github.com/zapta/linbus/tree/master/analyzer/arduino
#include "src/lin_frame.h"

#define CS_PIN 15
#define RX_LED 17
#define RTI_RX_PIN 16
#define RTI_TX_PIN 10

#define ON_CLICK_DURATION 100
#define OFF_CLICK_DURATION 3000 // how long to hold "back" button to turn off

#define CLICK_TIMEOUT 100

#define HEARTBEAT_TIMEOUT 2000
#define RTI_INTERVAL 100

#define SYN_FIELD 0x55
#define SWM_ID 0x20

#define STATE_OFF 0
#define STATE_ON 1

// Volvo V50 2007 SWM key codes
//
// BTN_NEXT       20 0 10 0 0 EF
// BTN_PREV       20 0 2 0 0 FD
// BTN_VOL_UP     20 0 0 1 0 FE
// BTN_VOL_DOWN   20 0 80 0 0 7F
// BTN_BACK       20 0 1 0 0 F7
// BTN_ENTER      20 0 8 0 0 FE
// BTN_UP         20 1 0 0 0 FE
// BTN_DOWN       20 2 0 0 0 FD
// BTN_LEFT       20 4 0 0 0 FB
// BTN_RIGHT      20 8 0 0 0 F7
// IGN_KEY_ON     50 E 0 F1

#define JOYSTICK_UP 0x1
#define JOYSTICK_DOWN 0x2
#define JOYSTICK_LEFT 0x4
#define JOYSTICK_RIGHT 0x8
#define BUTTON_BACK 0x1
#define BUTTON_ENTER 0x8
#define BUTTON_NEXT 0x10
#define BUTTON_PREV 0x2

#define CRANKSHAFT_ANDROID_CONNECTED '+'
#define CRANKSHAFT_ANDROID_DISCONNECTED '-'

short rtiStep;

SoftwareSerial rtiSerial(RTI_RX_PIN, RTI_TX_PIN);

LinFrame frame = LinFrame();

unsigned long currentMillis, lastHeartbeat, lastRtiWrite, buttonDownAt, lastButtonAt;
short state = STATE_OFF;
byte currentButton, currentJoystickButton;

void setup() {
  pinMode(RX_LED, OUTPUT);

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  Serial.begin(9600);
  Serial1.begin(9600);
  rtiSerial.begin(2400);

  Keyboard.begin();

  //turn_on();
}

void loop() {
  currentMillis = millis();

  if (Serial.available())
    read_crankshaft();

  if (Serial1.available())
    read_lin_bus();

  timeout_button();
  check_ignition_key();
  rti();
}

void read_crankshaft() {
  byte b = Serial.read();

  switch (b) {
    case CRANKSHAFT_ANDROID_CONNECTED:
      if (state == STATE_OFF)
        turn_on();

    case CRANKSHAFT_ANDROID_DISCONNECTED:
      if (state == STATE_ON)
        turn_off();
  }

  if (DEBUG) {
    Serial.write(b);
//    Keyboard.write(b);
  }
}

void read_lin_bus() {
  byte b = Serial1.read();
  int n = frame.num_bytes();

  if (b == SYN_FIELD && n > 2 && frame.get_byte(n - 1) == 0) {
    digitalWrite(RX_LED, LOW);

    frame.pop_byte();
    handle_swm_frame();
    frame.reset();

    digitalWrite(RX_LED, HIGH);
  } else if (n == LinFrame::kMaxBytes) {
    frame.reset();
  } else {
    frame.append_byte(b);
  }
}

void handle_swm_frame() {
  if (frame.get_byte(0) != SWM_ID)
    return;

  lastHeartbeat = currentMillis;

  // skip zero values 20 0 0 0 0 FF
  if (frame.get_byte(5) == 0xFF)
    return;

  if (!frame.isValid())
    return;

  //  dump_frame();
  handle_buttons();
  handle_joystick();
}

void handle_joystick() {
  if (state != STATE_ON)
    return;
  
  byte button = frame.get_byte(1);

  if (button != currentJoystickButton) {
    currentJoystickButton = button;
    click_joystick(button);
  }  
}

void click_joystick(byte button) {
  switch (button) {
    case JOYSTICK_UP:
      Keyboard.write(KEY_UP_ARROW);
      debug("UP");
      break;
    case JOYSTICK_DOWN:
      Keyboard.write(KEY_DOWN_ARROW);
      debug("DOWN");
      break;
    case JOYSTICK_LEFT:
      Keyboard.write('1');
      debug("LEFT");
      break;
    case JOYSTICK_RIGHT:
      Keyboard.write('2');
      debug("RIGHT");
      break;  
  }
}

void handle_buttons() {
  byte button = frame.get_byte(2);

  if (!button)
    return;
  
  if (button != currentButton) {
    release_button(currentButton, currentMillis - buttonDownAt);
    click_button(button);
    
    currentButton = button;
    buttonDownAt = currentMillis;
  }

  lastButtonAt = currentMillis;
}

void click_button(byte button) {
  if (state != STATE_ON)
    return;
  
  switch (button) {
    case BUTTON_ENTER:
      Keyboard.write(KEY_RETURN);
      debug("ENTER");
      break;
    case BUTTON_BACK:
      Keyboard.write(KEY_ESC);
      debug("ESC");
      break;
    case BUTTON_PREV:
      Keyboard.write('V');
      debug("PREV");
      break;
    case BUTTON_NEXT:
      Keyboard.write('N');
      debug("NEXT");
      break;
  }
}

void timeout_button() {
  if (!currentButton) 
    return;

  if (currentMillis - lastButtonAt > CLICK_TIMEOUT) 
    release_button(currentButton, currentMillis - buttonDownAt);
}

void release_button(byte button, unsigned long clickDuration) {
  switch (button) {
    case BUTTON_ENTER:
      if (state == STATE_OFF && clickDuration > ON_CLICK_DURATION)
        turn_on();
      break;

    case BUTTON_BACK:
      if (clickDuration > OFF_CLICK_DURATION)
        turn_off();
      break;
  }

  currentButton = 0;
}

void check_ignition_key() {
  if (lastHeartbeat && currentMillis - lastHeartbeat > HEARTBEAT_TIMEOUT) {
    debug("Ignition off");
    lastHeartbeat = 0;
    turn_off();
  }
}

void turn_on() {
  debug("Turn on");
  state = STATE_ON;
}

void turn_off() {
  debug("Turn off");
  state = STATE_OFF;
}

// send serial data to Volvo RTI screen mechanism
void rti() {
  if (currentMillis - lastRtiWrite < RTI_INTERVAL) return;
  
  switch (rtiStep) {
    case 0: // mode
      if (state == STATE_OFF) 
        rti_print(0x46);
      else
        rti_print(0x40);

      if (state == STATE_OFF)
        debug("RTI OFF");
      else
        debug("RTI ON");
              
      rtiStep++;
      break;

    case 1: // brightness
      rti_print(0x20);
      rtiStep++;
      break;

    case 2: // sync
      rti_print(0x83);
      rtiStep = 0;
      break;    
  }

  lastRtiWrite = currentMillis;
}

void rti_print(char byte) {
  rtiSerial.print(byte);
}

// -- debugging

void dump_frame() {
  for (int i = 0; i < frame.num_bytes(); i++) {
    Serial.print(frame.get_byte(i), HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void debug(char* message) {
  if (DEBUG)
    Serial.println(message);
}
