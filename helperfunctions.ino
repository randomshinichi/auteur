void lockinamplifier(int rate, int repeat) {
  zerothisarray(averager_size, averager);
  averager_counter = 0;
  averager_i = 0;
  averager_sum = 0;
  int delaymicros = 1000000 / (rate * wave_samples);
  if (repeat > 0) {
    while (repeat > 0) {
      output_readin(delaymicros);
      repeat--;
      if (results_display) {
        {
          double result = averager_sum / averager_size;
          Serial.print("results: ");  //ensure you have >10000 samples in averager to make a meaningful reading
          Serial.println(averager_sum / averager_size, 10);
          Serial.print("ave_voltage: "); Serial.println(sinescale2voltage(result), 10);
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
          Serial.print("results: ");  //ensure you have >10000 samples in averager to make a meaningful reading
          Serial.println(averager_sum / averager_size, 10);
          Serial.print("ave_voltage: "); Serial.println(sinescale2voltage(result), 10);
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
    delayMicroseconds(delaymicros - 51); //executing the following lines will take ~51micros
    int input = analogRead(ADC0); //this will be between 0-4096, corresponding to 0 and 3.3V
    double input_voltage = int2voltage(input, 0, 3.3);
    double output_voltage = int2voltage(scaledsine[i], minDACvolt, maxDACvolt);
    output_voltage += minDACvolt; // int2voltage above this shifted the whole value down by 0.553, correcting this back up
    double answer = voltage2sinescale(input_voltage) * voltage2sinescale(output_voltage);
    add2averager(answer);

    if (lockamp_debug) {
      Serial.print("i: "); Serial.print(i); Serial.print(" scaledsine[i]: "); Serial.println(scaledsine[i]);
      Serial.print("I put "); Serial.print(output_voltage, 10);Serial.print("V out, read back: "); Serial.println(input_voltage, 10); 
    }
  }
}
void add2averager(double Voutput) {
  static double oldvalue = 0;
  if (averager_debug) {
    Serial.print("averager_i: "); Serial.println(averager_i);
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
double voltage2sinescale(double input) {
  double range = maxDACvolt - minDACvolt;
  input = input - (range / 2) - minDACvolt; // move 0.553-2.759V curve down to have its center on the x axis
  return 2 * input / range; // scale it to 1
}
double sinescale2voltage(double input) {
  input = input * 1.65;
  return input + 1.65;
}
double int2voltage(int input, double vlow, double vhigh) { // voltage = int2voltage(800,0.55,2.75)
  double granularity = (vhigh - vlow) / 4096;
  return granularity * input;
}
void calibrate() {
  minDACvolt = calibrate_helper(0);
  maxDACvolt = calibrate_helper(4095);
  Serial.print(minDACvolt, 10); Serial.print(" "); Serial.println(maxDACvolt, 10);
}
double calibrate_helper(int x) {
  analogWrite(DAC1, x);
  double calib_accum = 0;
  int i = 40;
  while (i > 0) {
    calib_accum += int2voltage(analogRead(ADC0), 0, 3.3);
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
    Serial.print(thearray[i], 5);
    Serial.print(",");
  }
  Serial.println("");
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
