#include "pins.h"

// Reads battery voltage (0-15)
float readBattery() {
  return ((float)analogRead(IGNI) / 1024.0) * 17.45; // 15.25 multiplier determined experimentally
}

int readHighBeam() {
  return digitalRead(HIBM);
}

int readLowBeam() {
  return digitalRead(LOBM);
}

int readReverseLight() {
  return digitalRead(REV);
}

int readRightIndicator() {
  return digitalRead(RIGHT);
}

int readLeftIndicator() {
  return digitalRead(LEFT);
}

// Read exponential fuel level
float readFuelLevel() {
  float fuelRaw = (float)analogRead(FUEL);

  if (fuelRaw < 25.0)
    return 0.0;
    
  float fuelProcessed = 1.55397 * pow(0.995521, fuelRaw);
  return min(fuelProcessed, 1.0);
}
