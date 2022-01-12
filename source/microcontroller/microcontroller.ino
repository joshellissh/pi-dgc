#include <avr/io.h>
#include <avr/interrupt.h>
#include <Smoothed.h>
#include <BMP085.h>
#include <Wire.h>
#include "readers.h"
#include "pins.h"
#include "types.h"
#include "eepromHelper.h"
#include "serialReader.h"

//#define DEBUG     true

#define LO_FREQ   15000000  // 15000000
#define MD_FREQ   250000    // 250000
#define HI_FREQ   100000    // 100000
#define NO_PULSE  99999

elapsedMicros lowFrequency;
elapsedMicros mediumFrequency;
elapsedMicros highFrequency;

bool startedShutdown = false;

Smoothed<float> battery;
Smoothed<float> fuelLevel;

volatile unsigned long lastPulse = 0;
volatile unsigned long pulseSeparation = 0;
volatile unsigned int pulseCounter = 0;

BMP085 ptSensor;

void setup() {
  Serial.begin(115200);

  // Turn on LED as a power indicator
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // Set value to expiration so it will fire immediately on boot
  lowFrequency = LO_FREQ;

  battery.begin(SMOOTHED_AVERAGE, 10);
  fuelLevel.begin(SMOOTHED_AVERAGE, 10);

  pinMode(VSS, INPUT_PULLUP);
  attachInterrupt(VSS, vssInterrupt, RISING);

  pinMode(HIBM, INPUT);
  pinMode(LOBM, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(REV, INPUT);
  pinMode(MIL, INPUT);
  pinMode(GAUGE_LIGHTS, INPUT);

  ptSensor.init();
}

void vssInterrupt() {
  unsigned long currentPulse = micros();
  
  if (lastPulse > currentPulse) {
    lastPulse = currentPulse;
    pulseSeparation = NO_PULSE;
  } else {
    pulseSeparation = currentPulse - lastPulse;
    lastPulse = currentPulse;
  }

  pulseCounter++;
}

void loop() {  
  // Check for commands from Pi
  char *serialMessage = getSerialMessage();
  if (serialMessage != NULL) {
    // Pi requesting odometer values  
    if (strcmp(serialMessage, "so") == 0)
      sendOdometerValues();

    // Pi requesting pulses per mile
    else if (strcmp(serialMessage, "sppm") == 0)
      sendPPM();

    // Pi writing odometer values
    else if(strstr(serialMessage, "wo:") != NULL) {
      char *values = serialMessage + 3;

      // Write trip odometer
      char *token = strtok(values, ",");
      tripOdometer = atof(token);
      writeMileage(TRIP, tripOdometer);

      // Write mileage odometer
      token = strtok(NULL, ",");
      odometer = atof(token);
      writeMileage(REGULAR, odometer);

      char output[512] = {0};
      sprintf(
        output, 
        "log:SERIAL_LOG-Wrote mileage (trip %f, odo %f) to EEPROM.",
        tripOdometer,
        odometer
      );
      Serial.println(output);

      sendOdometerValues();
    }

     // Pi writing pulses per mile
    else if(strstr(serialMessage, "wppm:") != NULL) {
      char *value = serialMessage + 5;
      int ppm = atoi(value);
      writePPM(ppm);

      char output[512] = {0};
      sprintf(
        output, 
        "log:SERIAL_LOG-Wrote PPM (%d) to EEPROM.",
        ppm
      );
      Serial.println(output);

      sendPPM();
    }
  }

  // Poll analog pins
  readAnalog();

  // High frequency updates
  if (highFrequency >= HI_FREQ) {
    highFrequency -= HI_FREQ;

    cli();
    unsigned int numPulses = pulseCounter;
    unsigned int pulseSep = pulseSeparation;
    sei();
    pulseCounter = 0;
    pulseSeparation = NO_PULSE;

    // Send numPulses & pulseSep
    char output[25] = {0};
    sprintf(output, "pulses:%d,%d", numPulses, pulseSep);
#ifndef DEBUG
    Serial.println(output);
#endif
  }

  // Medium frequency updates
  if (mediumFrequency >= MD_FREQ) {
    mediumFrequency -= MD_FREQ;

    char output[512] = {0};
    sprintf(
      output, 
      "batt:%f\nfuel:%f\nhi:%d\nleft:%d\nlo:%d\nrev:%d\nright:%d\nmil:%d\nglite:%d",
      battery.get(),
      fuelLevel.get(),
      readHighBeam(),
      readLeftIndicator(),
      readLowBeam(),
      readReverse(),
      readRightIndicator(),
      readMIL(),
      readGaugeLights()
    );
#ifndef DEBUG
    Serial.println(output);
#endif
  }

  // Low frequency updates
  if (lowFrequency >= LO_FREQ) {
    lowFrequency -= LO_FREQ;

    float temperature = ptSensor.bmp085GetTemperature(ptSensor.bmp085ReadUT());

    char output[256] = {0};
    sprintf(
      output, 
      "temp:%f\n",
      (temperature * 1.8) + 32.0 // Convert to degF
    );
#ifndef DEBUG
    Serial.println(output);
#endif
  }
}

void readAnalog() {
  battery.add(readBattery());
  fuelLevel.add(readFuelLevel()); 
}
