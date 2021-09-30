#include "vehiclevalues.h"

VehicleValues::VehicleValues()
{
    reset();
}

void VehicleValues::reset() {
    rpm = 0.0f;
    mph = 0.0f;
    coolant = 170.0f;
    fuel = 0.0f;
    boost = 0.0f;
    voltage = 0.0f;
    odometer = 0.0f;
    tripOdometer = 0.0f;
    boostLaggingMax = 0.0f;
    reverse = false;
    oilPressure = 0.0f;
    leftBlinker = false;
    rightBlinker = false;
    lowBeam = false;
    highBeam = false;
    mil = false;
}

void VehicleValues::deserialize(const QJsonObject &json) {
    QMutexLocker locker(&mutex);

    if (json.contains("rpm") && json["rpm"].isDouble())
       rpm = json["rpm"].toDouble();

    if (json.contains("mph") && json["mph"].isDouble())
       mph = json["mph"].toDouble();

    if (json.contains("coolant") && json["coolant"].isDouble())
       coolant = json["coolant"].toDouble();

    if (json.contains("fuel") && json["fuel"].isDouble())
       fuel = json["fuel"].toDouble();

    if (json.contains("boost") && json["boost"].isDouble())
       boost = json["boost"].toDouble();

    if (json.contains("voltage") && json["voltage"].isDouble())
       voltage = json["voltage"].toDouble();

    if (json.contains("odometer") && json["odometer"].isDouble())
       odometer = json["odometer"].toDouble();

    if (json.contains("tripOdometer") && json["tripOdometer"].isDouble())
       tripOdometer = json["tripOdometer"].toDouble();

    if (json.contains("reverse") && json["reverse"].toBool())
       reverse = json["reverse"].toBool();

    if (json.contains("oilPressure") && json["oilPressure"].isDouble())
       oilPressure = json["oilPressure"].toDouble();

    if (json.contains("leftBlinker") && json["leftBlinker"].toBool())
       leftBlinker = json["leftBlinker"].toBool();

    if (json.contains("rightBlinker") && json["rightBlinker"].toBool())
       rightBlinker = json["rightBlinker"].toBool();

    if (json.contains("lowBeam") && json["lowBeam"].toBool())
       lowBeam = json["lowBeam"].toBool();

    if (json.contains("highBeam") && json["highBeam"].toBool())
       highBeam = json["highBeam"].toBool();

    if (json.contains("mil") && json["mil"].toBool())
       mil = json["mil"].toBool();
}
