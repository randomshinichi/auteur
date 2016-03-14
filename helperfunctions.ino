void lockinamplifier(int rate, int repeat) {
  zerothisarray(averager_size, averager);
  averager_counter = 0;
  averager_i = 0;
  averager_sum = 0;
  int delaymicros = (1000000 / (rate * wave_samples))-39; // compensate for execution time of output_readin() algo
  SerialUSB.print("delaymicros: ");  SerialUSB.println(delaymicros);
  if (repeat > 0) {
    while (repeat > 0) {
      starttime=micros();
      output_readin(delaymicros);
      elapsedtime=micros()-starttime;
      SerialUSB.print(elapsedtime);SerialUSB.println(" micros taken by output_readin");
      repeat--;
      if (results_display) {
        {
          double result = averager_sum / averager_size;
          SerialUSB.print("results: ");  //ensure you have >10000 samples in averager to make a meaningful reading
          SerialUSB.println(averager_sum / averager_size, 10);
          SerialUSB.print("ave_voltage: "); SerialUSB.println(sinescale2voltage(result), 10);
        }
      }
    }
  }
  else if (repeat == 0) {
    while (1) {
      output_readin(delaymicros);
      if (results_display) {
        {
          double result = averager_sum / averager_size;
          SerialUSB.print("results: ");  //ensure you have >10000 samples in averager to make a meaningful reading
          SerialUSB.println(averager_sum / averager_size, 10);
          SerialUSB.print("ave_voltage: "); SerialUSB.println(sinescale2voltage(result), 10);
        }
      }
    }
  }
  else
  {
    output_readin(delaymicros);
  }
}
void output_readin(int delaymicros) {
  for (int i = 0; i < wave_samples; ++i) {
    analogWrite(DAC1, scaledsine[i]);
    //delayMicroseconds(delaymicros);
    //delayMicroseconds itself takes about 2 micros, so when benchmarking output_readin, add 2micros to the result (37+2=39)
    int input = analogRead(ADC0); //this will be between 0-4096, corresponding to 0 and 3.3V
    double answer = int2sinescale(input, minADC, maxADC) * int2sinescale(scaledsine[i], 0, 4095);
    add2averager(answer);
    if (lockamp_debug) {
      double output_voltage = int2voltage(scaledsine[i], minDACvolt, maxDACvolt);
      double input_voltage = int2voltage(input, 0, 3.3);
      SerialUSB.print("i: "); SerialUSB.print(i); SerialUSB.print(" scaledsine[i]: "); SerialUSB.println(scaledsine[i]);
      SerialUSB.print("I put "); SerialUSB.print(output_voltage, 10); SerialUSB.print("V out, read back: "); SerialUSB.println(input_voltage, 10);
    }
  }
}
inline void add2averager(double Voutput) {
  static double oldvalue = 0;
  if (averager_debug) {
    SerialUSB.print("averager_i: "); SerialUSB.println(averager_i);
  }
  if (averager_i == averager_size) {
    averager_i = 0;
  }
  oldvalue = averager[averager_i];
  averager[averager_i] = Voutput;
  averager_sum = averager_sum - oldvalue;
  averager_sum += Voutput;
  averager_i++;
  averager_counter++;
}
double sinescale2voltage(double input) {
  return (input * 1.65) + 1.65;
}
double int2voltage(int input, double vlow, double vhigh) { // voltage = int2voltage(800,0.55,2.75)
  double granularity = (vhigh - vlow) / 4096;
  granularity = (granularity * input) + vlow;
  return granularity;
}
inline double int2sinescale(int input, int minimum, int maximum) {
  double sinescale;
  int range = (maximum - minimum) / 2;
  input = input - range - minimum;
  sinescale = double(input) / double(range);
  return sinescale;
}
void calibrate() {
  minADC = calibrate_helper(0);
  maxADC = calibrate_helper(4095);
  SerialUSB.print("minADC: "); SerialUSB.print(minADC); SerialUSB.print(" maxADC: "); SerialUSB.println(maxADC);
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
void zerothisarray(int size, double thearray[]) {
  for (int i = 0; i < size; i++) {
    thearray[i] = 0;
  }
}
void printthisarray(int size, double thearray[]) {
  for (int i = 0; i < size; i++) {
    SerialUSB.print(thearray[i], 5);
    SerialUSB.print(",");
  }
  SerialUSB.println("");
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
