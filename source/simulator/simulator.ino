#define FUEL       A10

void setup() {
   Serial.begin(115200);
}

void loop() {
  char output[25] = {0};
  
  int fuelRaw = analogRead(FUEL);
  float fuelProcessed = 1.55397 * pow(0.995521, fuelRaw);
  float fuelConstrained = min(fuelProcessed, 1.0);
  
  sprintf(output, "fuel:%f\n", fuelConstrained);
  Serial.print(output);

  delay(50);
}
