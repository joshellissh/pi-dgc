#include "painter.h"
#include "converters.h"
#include <QCameraInfo>
#include <QList>
#include <QActionGroup>

Painter::Painter(VehicleValues &vehicle)
{
    this->vehicle = &vehicle;

    frameCounter = 0;

    backgroundBrush = QBrush(QColor(0, 0, 0));
    shiftLightBrush = QBrush(QColor(255, 0, 0));

    largeGauge.setFamily("bladitwo4fitalic");
    largeGauge.setPointSize(30);
    smallGauge.setFamily("bladitwo4fitalic");
    smallGauge.setPointSize(15);
    normalText.setFamily("Arial");
    normalText.setPointSize(12);
    largeText.setFamily("Arial");
    largeText.setPointSize(20);
    gaugeTextPen = QPen(Qt::white);

    // Load all gauge cluster images
    images = loadImages(QString("%1/images/").arg(QCoreApplication::applicationDirPath()));

    // List cameras
    const QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    for (const QCameraInfo &cameraInfo : availableCameras) {
        qDebug() << "Camera:" << cameraInfo.description();
    }

    // Start camera
    camera = new QCamera(QCamera::FrontFace);
    imageCapture = new QCameraImageCapture(camera);
    connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &Painter::processCapturedImage);
    camera->start();
}

void Painter::paint(QPainter *painter, QPaintEvent *event, int elapsed)
{
    if (indicators.shiftLight)
        painter->fillRect(event->rect(), shiftLightBrush);
    else
        painter->fillRect(event->rect(), backgroundBrush);

    // Run initialization sequence on startup
    if (frameCounter < 80) initLoop();

    // Set indicators according to vehicle values
    updateIndicators();

    // Draw center lines
    drawCenteredAt(painter, images->at("center_lines.png"), HALF_SCREEN_WIDTH, HALF_SCREEN_HEIGHT);

    // Left gauge background
    drawCenteredAt(painter, images->at("left_gauge_bg.png"), 249.0, 240.0);

    // Left gauge boost black overlay
    painter->setBrush(backgroundBrush);
    drawTorus(painter, 249, 240, 78, 125, degToRad(boostToAngle(vehicle->getBoost()) + 91.5f), degToRad(303), 365);

    // Left gauge foreground
    drawCenteredAt(painter, images->at("left_gauge_fg.png"), 249.0, 240.0);

    // Lagging indicator for maximum boost
    if (indicators.boostLaggingMax && vehicle->getBoostLaggingMax() != 0) {
        painter->setOpacity(indicators.boostLaggingMaxOpacity);
        drawCenteredWithOffsetAndRotation(painter, images->at("lagging_max_boost.png"), 249.0, 240.0, -99.0, 0.0, boostToAngle(vehicle->getBoostLaggingMax()));
        painter->setOpacity(1);
    }

    // Boost highlight leader
    if (vehicle->getBoost() > 0)
        drawCenteredWithOffsetAndRotation(painter, images->at("blue_hilite_leading.png"), 249.0, 240.0, -99.0, 0.0, boostToAngle(vehicle->getBoost()));

    // Right gauge green fuel background
    if (vehicle->getFuel() > 0.25f)
        drawCenteredAt(painter, images->at("right_gauge_bg.png"), 1031.0, 240.0);

    // Right gauge red fuel background
    if (vehicle->getFuel() <= 0.25f)
        drawCenteredAt(painter, images->at("right_gauge_bg_2.png"), 1031.0, 240.0);

    // Fuel gauge leading (green)
    if (vehicle->getFuel() > 0.25f)
        drawCenteredWithOffsetAndRotation(painter, images->at("green_hilite_leading.png"), 1031.0, 240.0, -99.0, 0.0, fuelToAngle(vehicle->getFuel()));

    // Fuel gauge leading (red)
    if (vehicle->getFuel() <= 0.25f)
        drawCenteredWithOffsetAndRotation(painter, images->at("red_hilite_leading.png"), 1031.0, 240.0, -99.0, 0.0, fuelToAngle(vehicle->getFuel()));

    // Right gauge fuel black overlay
    drawTorus(painter, 1031, 240, 78, 125, degToRad(fuelToAngle(vehicle->getFuel()) + 91.5f), degToRad(303.0f), 365);

    // Right gauge foreground
    drawCenteredAt(painter, images->at("right_gauge_fg.png"), 1031.0, 240.0);

    // Left gauge needle
    drawCenteredWithOffsetAndRotation(painter, images->at("big_needle.png"), 249.0, 240.0, -163.0, 0.0, rpmToAngle(vehicle->getRpm()));

    // Right gauge needle
    drawCenteredWithOffsetAndRotation(painter, images->at("big_needle.png"), 1031.0, 240.0, -163.0, 0.0, mphToAngle(vehicle->getMph()));

    // Coolant gauge needle
    drawCenteredWithOffsetAndRotation(painter, images->at("little_needle.png"), 249.0, 240.0, -191.0, 0.0, temperatureToAngle(vehicle->getCoolant()));

    // Oil gauge needle
    drawCenteredWithOffsetAndRotation(painter, images->at("little_needle.png"), 1031.0, 240.0, -191.0, 0.0, pressureToAngle(vehicle->getOilPressure()));

    // Center logo
    if (!vehicle->getReverse()) {
        painter->setOpacity(0.8f);
        drawCenteredAt(painter, images->at("center_logo.png"), 640.0, 240.0);
        painter->setOpacity(1.0f);
    } else {
        imageCapture->capture();
    }

    // Left indicator
    if (indicators.left)
        drawCenteredAt(painter, images->at("left_indicator.png"), 490.0, 115.0);

    // Right indicator
    if (indicators.right)
        drawCenteredAt(painter, images->at("right_indicator.png"), 790.0, 115.0);

    // Low beam indicator
    if (indicators.lowBeam)
        drawCenteredAt(painter, images->at("low_beam.png"), 565.0, 115.0);

    // Hight beam indicator
    if (indicators.highBeam)
        drawCenteredAt(painter, images->at("high_beam.png"), 565.0, 115.0);

    // Check engine indicator
    if (indicators.mil)
        drawCenteredAt(painter, images->at("mil.png"), 640.0, 115.0);

    // Oil indicator
    if (indicators.oil)
        drawCenteredAt(painter, images->at("oil.png"), 715.0, 115.0);

    // Battery indicator
    if (indicators.battery)
        drawCenteredAt(painter, images->at("battery.png"), 640.0, 362.0);

    // Low fuel indicator
    if (indicators.fuel)
        drawCenteredAt(painter, images->at("fuel.png"), 744.0, 362.0);

    // Coolant temp indicator
    if (indicators.coolant)
        drawCenteredAt(painter, images->at("coolant.png"), 537.0, 362.0);

    // Reset trip icon
    painter->setOpacity(0.8);
    drawCenteredAt(painter, images->at("reset.png"), 410.0, 423.0);
    painter->setOpacity(1.0);

    painter->setPen(gaugeTextPen);
    painter->setFont(largeGauge);
    // RPM
    painter->drawText(247.0f - 60.0f, 235.0f - 20.0f, 120.0f, 40.0f, Qt::AlignCenter | Qt::AlignHCenter, QString("%1").arg(vehicle->getRpm()));
    // MPH
    painter->drawText(1029.0f - 60.0f, 235.0f - 20.0f, 120.0f, 40.0f, Qt::AlignCenter | Qt::AlignHCenter, QString("%1").arg((int)vehicle->getMph()));
    // Boost
    painter->setFont(smallGauge);
    painter->drawText(247.0f - 60.0f, 340.0f - 10.0f, 120.0f, 20.0f, Qt::AlignCenter | Qt::AlignHCenter, QString("%1 psi").arg((int)vehicle->getBoost()));
    // Battery voltage
    QString voltage;
    voltage.sprintf("%.1f", vehicle->getVoltage());
    painter->drawText(1031.0f - 20.0f, 388.5f - 10.0f, 40.0f, 20.0f, Qt::AlignCenter | Qt::AlignHCenter, voltage);

    painter->setFont(normalText);
    painter->setOpacity(0.8);
    // Trip odometer
    QString tripText;
    tripText.sprintf("Trip: %.1f", vehicle->getTripOdometer());
    painter->drawText(430.0f, 415.0f, 300.0f, 20.0f, Qt::AlignLeading, tripText);

    // Odometer
    QString odometerText;
    odometerText.sprintf("%.1f", vehicle->getOdometer());
    painter->drawText(575.0f, 415.0f, 300.0f, 20.0f, Qt::AlignTrailing, odometerText);

    // Clock
    painter->setFont(largeText);
    painter->drawText(565.0f, 45.0f, 150.0f, 30.0f, Qt::AlignCenter | Qt::AlignTop, QTime::currentTime().toString("h:mm a"));
    painter->setOpacity(1.0);

    // Frame counter
//    painter->setFont(normalText);
//    painter->drawText(0.0f, 0.0f, 150.0f, 20.0f, Qt::AlignLeading, QString("%1").arg(frameCounter));
    frameCounter++;
}

// Startup animation
void Painter::initLoop() {
    if (frameCounter < 40) {
        vehicle->setRpm(vehicle->getRpm() + 200);
        vehicle->setMph(vehicle->getMph() + 4);
        vehicle->setCoolant(vehicle->getCoolant() + 2.5);
        vehicle->setFuel(vehicle->getFuel() + 0.025);
        vehicle->setBoost(vehicle->getBoost() + 0.8);
        vehicle->setVoltage(vehicle->getVoltage() + 0.375);
        vehicle->setOilPressure(vehicle->getOilPressure() + 2.25);
    }
    if (frameCounter >= 40) {
        vehicle->setRpm(vehicle->getRpm() - 200);
        vehicle->setMph(vehicle->getMph() - 4);
        vehicle->setCoolant(vehicle->getCoolant() - 2.5);
        vehicle->setFuel(vehicle->getFuel() - 0.025);
        vehicle->setBoost(vehicle->getBoost() - 0.8);
        vehicle->setVoltage(vehicle->getVoltage() - 0.375);
        vehicle->setOilPressure(vehicle->getOilPressure() - 2.25);
    }

    if (frameCounter >= 79) {
        vehicle->reset();
        vehicle->initialized = true;
    }
}

void Painter::updateIndicators() {
    // Fuel indicator
    if (vehicle->getFuel() <= 0.25) {
        indicators.fuel = true;
    } else {
        indicators.fuel = false;
    }

    // Boost lagging max
    if (vehicle->getBoost() > vehicle->getBoostLaggingMax() && frameCounter >= 80) {
        vehicle->setBoostLaggingMax(vehicle->getBoost());
        indicators.boostLaggingMax = false;

        qDebug() << "High boost set to" << vehicle->getBoostLaggingMax();
    }

    if (vehicle->getBoost() < vehicle->getBoostLaggingMax()) {
        if (!indicators.boostLaggingMax) {
            indicators.boostLaggingMax = true;
            indicators.boostLaggingMaxOpacity = 1.0f;
        }

        indicators.boostLaggingMaxOpacity -= 0.01;

        if (indicators.boostLaggingMaxOpacity <= 0) {
            indicators.boostLaggingMaxOpacity = 0;
            vehicle->setBoostLaggingMax(0);
        }
    }

    // Left blinker
    if (vehicle->getLeftBlinker()) {
        if (frameCounter < 80) {
            indicators.left = true;
        } else {
            indicators.left = QTime::currentTime().msec() < 500;
        }
    } else {
        indicators.left = false;
    }

    // Right blinker
    if (vehicle->getRightBlinker()) {
        if (frameCounter < 80) {
            indicators.right = true;
        } else {
            indicators.right = QTime::currentTime().msec() < 500;
        }
    } else {
        indicators.right = false;
    }

    indicators.lowBeam = vehicle->getLowBeam();
    indicators.highBeam = (vehicle->getHighBeam() && vehicle->getLowBeam());
    indicators.mil = vehicle->getMil();
    indicators.oil = vehicle->getOilPressure() < 10.0f;
    indicators.battery = vehicle->getVoltage() < 12.0f || vehicle->getVoltage() > 15.0f;
    indicators.fuel = vehicle->getFuel() <= 0.25f;
    indicators.coolant = vehicle->getCoolant() > 257.0f;
    indicators.shiftLight = vehicle->getRpm() > 6500.0f;
}

void Painter::processCapturedImage(int requestId, const QImage& img)
{
    Q_UNUSED(requestId);
    cameraImage = img;
}
