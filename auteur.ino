#include <SPI.h>
#include "waveforms.h"
#define averager_size 20000

String inputstring = "";
boolean stringcomplete = false;
String args[10];
unsigned long starttime, elapsedtime;
int averager[averager_size]; // the averaging array
unsigned int averager_i; // keeps track of where we are in the averager array
unsigned int averager_counter; // counts how many times the averager has been added to
double averager_sum; // variable that will hold the sum of averaging array
int minADC = 688; // these are reasonable defaults, will be changed with calibration
int maxADC = 3424;
int rangeADC = (maxADC - minADC) / 2;
double minDACvolt = 0.5518798828;
double maxDACvolt = 2.7585937500;
boolean lockamp_debug = false;
boolean averager_debug = false;

void setup() {
  analogWriteResolution(12);
  analogReadResolution(12);
  pinMode(A0, INPUT);
  while (!SerialUSB);
  SerialUSB.begin(115200);
  SerialUSB.println("Available commands: lockamp(repeat,frequency[frequency, frequency]) analogread analogwrite(0-4095) calibrate status debug(lockamp_debug, averager_debug)");
  SerialUSB.print("auteur is listening: ");
  inputstring.reserve(200); // Shell accepts up to 200 characters, 10 arguments
}
void loop() {
  if (SerialUSB.available())serialEvent(); // to enable input via native USB port, not needed if just using Serial/Programming port
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
  SerialUSB.println(inputstring);
  String *inputstring_processed;
  inputstring_processed = splitcmd(inputstring, ' ');
  if (inputstring_processed[0] == "lockamp") {
    int param1, param2, param3, param4;
    param1 = string2int(inputstring_processed[1]);
    param2 = string2int(inputstring_processed[2]);
    param3 = string2int(inputstring_processed[3]);
    param4 = string2int(inputstring_processed[4]);
    if (param2) {
      SerialUSB.print(param2);SerialUSB.println("Hz");
      lockinamplifier(param1, param2);
    }
    else
    {
      SerialUSB.println("You didn't specify the frequency");
    }
    if (param3) {
      SerialUSB.print(param3);SerialUSB.println("Hz");
      lockinamplifier(param1, param3);
    }
    if (param4) {
      SerialUSB.print(param4);SerialUSB.println("Hz");
      lockinamplifier(param1, param4);
    }
  }
  if (inputstring_processed[0] == "debug") {
    int param1 = string2int(inputstring_processed[1]);
    int param2 = string2int(inputstring_processed[2]);
    if (param1)
      lockamp_debug = true;
    else
      lockamp_debug = false;
    if (param2)
      averager_debug = true;
    else
      averager_debug = false;
  }
  if (inputstring_processed[0] == "analogread") {
    int input = analogRead(ADC0);
    double translation = int2voltage(input, 0, 3.3);
    SerialUSB.print(input); SerialUSB.print(" "); SerialUSB.print(translation, 3); SerialUSB.println("V");
  }
  if (inputstring_processed[0] == "analogwrite") {
    int param1 = string2int(inputstring_processed[1]);
    analogWrite(DAC0, param1);
    analogWrite(DAC1, param1);
  }
  if (inputstring_processed[0] == "calibrate") {
    calibrate();
  }
  if (inputstring_processed[0] == "status") {
    SerialUSB.print("minADC: "); SerialUSB.print(minADC); SerialUSB.print(" maxADC: "); SerialUSB.println(maxADC);
    SerialUSB.print("minDACvolt: "); SerialUSB.print(minDACvolt, 10); SerialUSB.print(" maxDACvolt: "); SerialUSB.println(maxDACvolt, 10);
    SerialUSB.print("lockamp_debug: "); SerialUSB.print(lockamp_debug); SerialUSB.print(" averager_debug: "); SerialUSB.println(averager_debug);
    SerialUSB.print("averager_counter: "); SerialUSB.println(averager_counter);
    SerialUSB.print("averager (1st 100): "); printthisarray(100, averager);
  }
  if (inputstring_processed[0] == "help")
  {
    SerialUSB.println("Available commands: lockamp(repeat,frequency[frequency, frequency]) analogread analogwrite(0-4095) calibrate status debug(lockamp_debug, averager_debug)");
  }
  SerialUSB.print("auteur > ");
}

void serialEvent() {
  while (SerialUSB.available()) {
    // get the new byte:
    char inChar = (char)SerialUSB.read();
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
