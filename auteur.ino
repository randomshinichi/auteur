#include <SPI.h>
#include "waveforms.h"
#define averager_size 10000

String inputstring = "";
boolean stringcomplete = false;
String args[10];
unsigned long starttime, elapsedtime;
double averager[averager_size]; // the averaging array
unsigned int averager_i; // keeps track of where we are in the averager array
unsigned int averager_counter; // counts how many times the averager has been added to
double averager_sum; // constantly updated sum of averaging array
double minDACvolt=0.5521215820;
double maxDACvolt=2.7575463867;
boolean lockamp_debug = false;
boolean averager_debug = false;
boolean results_display = false;

void setup() {
  analogWriteResolution(12);
  analogReadResolution(12);
  pinMode(A0, INPUT);
  Serial.begin(115200);
  Serial.println("Available commands: lockamp(frequency,repeat) analogread analogwrite(0-4095) calibrate status debug(lockamp_debug, averager_debug, results_display)");
  Serial.print("auteur is listening: ");
  inputstring.reserve(200); // Shell accepts up to 200 characters, 10 arguments
}
void loop() {
  // put your main code here, to run repeatedly:
  if (stringcomplete) {
    shell();
    // clear the inputstring, reset parameters
    inputstring = "";
    stringcomplete = false;
    for (int i = 0; i < 10; i++) {
      args[i] = "";
    }
  }
}
void shell() {
  Serial.println(inputstring);
  String *inputstring_processed;
  inputstring_processed = splitcmd(inputstring, ' ');
  if (inputstring_processed[0] == "lockamp") {
    int param1, param2;
    param1 = string2int(inputstring_processed[1]);
    param2 = string2int(inputstring_processed[2]);
    lockinamplifier(param1, param2);
    Serial.print("averager_counter (ignore result below if this is <10,000): ");Serial.println(averager_counter);
    double averager_result = 2*averager_sum/averager_size;
    Serial.print("averager_result: ");Serial.println(averager_result,10);
  }
  if (inputstring_processed[0] == "debug") {
    int param1 = string2int(inputstring_processed[1]);
    int param2 = string2int(inputstring_processed[2]);
    int param3 = string2int(inputstring_processed[3]);
    if(param1)
      lockamp_debug = true;
    else
      lockamp_debug = false;
    if(param2)
      averager_debug = true;
    else
      averager_debug = false;
    if(param3)
      results_display = true;
    else
      results_display = false;
  }
  if (inputstring_processed[0] == "analogread") {
    int input = analogRead(ADC0);
    double translation = int2voltage(input, 0, 3.3);
    Serial.print(input); Serial.print(" "); Serial.print(translation,3); Serial.println("V");
  }
  if (inputstring_processed[0] == "analogwrite") {
    int param1 = string2int(inputstring_processed[1]);
    analogWrite(DAC0,param1);
    analogWrite(DAC1,param1);
  }
  if (inputstring_processed[0] == "calibrate") {
    calibrate();
  }
  if (inputstring_processed[0] == "status") {
    Serial.print("minDACvolt: ");Serial.print(minDACvolt,10);Serial.print(" maxDACvolt: ");Serial.println(maxDACvolt,10);
    Serial.print("lockamp_debug: ");Serial.print(lockamp_debug);Serial.print(" averager_debug: ");Serial.print(averager_debug);Serial.print(" results_display: ");Serial.println(results_display);
    Serial.print("averager_counter (ignore result below if this is <10,000): ");Serial.println(averager_counter);
    double averager_result = 2*averager_sum/averager_size;
    Serial.print("averager_result: ");Serial.println(averager_result,10);
    Serial.print("averager (1st 100): ");printthisarray(100, averager);
  }
  Serial.print("auteur > ");
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringcomplete = true;
    }
    else {
      inputstring += inChar;
    }
  }
}
// helperfunctions.ino is automatically appended here by the compiler
