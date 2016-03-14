void lockinamplifier(int repeat, int rate) {
  zerothisarray(averager_size, averager);
  averager_counter = 0;
  averager_i = 0;
  averager_sum = 0;
    
  int delaymicros = (1000000 / (rate * wave_samples)) - 8; // compensate for execution time of output_readin() algo
  rangeADC = (maxADC - minADC) / 2; // calculates rangeADC again just in case you didn't run calibrate() before
  if (repeat > 0) {
    while (repeat > 0) {
      output_readin(delaymicros);
      repeat--;
    }
  }
  else if (repeat == 0) {
    while (1) {
      output_readin(delaymicros);
    }
  }
  else
  {
    SerialUSB.println("Did you just tell me to repeat the wavelength a negative amount of times?");
  }
  int scaledown = 2048 * rangeADC; // this number will normalize the huge numbers in the sum
  sum_normthisarray(averager_size, averager, scaledown);
  if (averager_debug) {
    SerialUSB.print("scaledown: "); SerialUSB.println(scaledown);
    SerialUSB.print("averager_sum: "); SerialUSB.println(averager_sum);
  }
  double averager_result = 2 * averager_sum / averager_size;
  SerialUSB.print("averager_result: "); SerialUSB.print(averager_result, 10);
  if (averager_counter >= averager_size)
  {
    SerialUSB.println(" OK");
  }
  else
  {
    SerialUSB.print(" INVALID: need "); SerialUSB.print(averager_size - averager_counter); SerialUSB.println(" more samples");
  }
}
void output_readin(int delaymicros) {
  for (int i = 0; i < wave_samples; ++i) {
    analogWrite(DAC1, scaledsine[i]);
    delayMicroseconds(delaymicros);
    //delayMicroseconds itself adds about 2 micros, so when benchmarking output_readin, add 2micros to the result (37+2=39)
    // e.g. now output_readin() takes 8 micros. make lockinamplifier() compensate for 10
    int input = analogRead(ADC0); //this will be between 0-4095, corresponding to 0 and 3.3V
    int answer = (input - minADC - rangeADC) * (scaledsine[i] - 2047);
    add2averager(answer);
    if (lockamp_debug) {
      double output_voltage = int2voltage(scaledsine[i], minDACvolt, maxDACvolt);
      double input_voltage = int2voltage(input, 0, 3.3);
      SerialUSB.print("i: "); SerialUSB.print(i); SerialUSB.print(" scaledsine[i]: "); SerialUSB.println(scaledsine[i]);
      SerialUSB.print("I put "); SerialUSB.print(output_voltage, 10); SerialUSB.print("V out, read back: "); SerialUSB.println(input_voltage, 10);
    }
  }
}
inline void add2averager(int answer) {
  if (averager_i == averager_size) {
    averager_i = 0;
  }
  averager[averager_i] = answer;
  averager_i++;
  averager_counter++;
}
double int2voltage(int input, double vlow, double vhigh) { // voltage = int2voltage(800,0.55,2.75)
  double granularity = (vhigh - vlow) / 4096;
  double voltage = (granularity * input) + vlow;
  return voltage;
}
void calibrate() {
  minADC = calibrate_helper(0);
  maxADC = calibrate_helper(4095);
  rangeADC = (maxADC - minADC) / 2;
  SerialUSB.print("minADC: "); SerialUSB.print(minADC); SerialUSB.print(" maxADC: "); SerialUSB.print(maxADC);  SerialUSB.print(" rangeADC: "); SerialUSB.println(rangeADC);
  minDACvolt = int2voltage(minADC, 0, 3.3);
  maxDACvolt = int2voltage(maxADC, 0, 3.3);
  SerialUSB.print("minDACvolt: "); SerialUSB.print(minDACvolt, 10); SerialUSB.print(" maxDACvolt: "); SerialUSB.println(maxDACvolt, 10);
}
int calibrate_helper(int x) {
  analogWrite(DAC1, x);
  int calib_accum = 0;
  int i = 40;
  while (i > 0) {
    calib_accum += analogRead(ADC0);
    delay(200);
    i--;
  }
  calib_accum = calib_accum / 40;
  return calib_accum;
}
void zerothisarray(int size, int thearray[]) {
  for (int i = 0; i < size; i++) {
    thearray[i] = 0;
  }
}
void printthisarray(int size, int thearray[]) {
  for (int i = 0; i < size; i++) {
    SerialUSB.print(thearray[i], 5);
    SerialUSB.print(",");
  }
  SerialUSB.println("");
}
void sum_normthisarray(int size, int thearray[], int scale) { // usually takes 15,000 microsecs
  for (int i = 0; i < size; i++) {
    averager_sum += double(thearray[i]) / double(scale);
  }
}
String* splitcmd(String command, char delimiter) {
  int endposition = -1;
  int startposition = 0;
  int index = 0;

  do {
    endposition = command.indexOf(delimiter, endposition + 1);
    args[index] = command.substring(startposition, endposition);
    index++;
    startposition = endposition;
  }
  while (endposition != -1);
  return args;
}
int string2int(String input) {
  char cstring[100];
  input.toCharArray(cstring, 100);
  return atoi(cstring);
}
