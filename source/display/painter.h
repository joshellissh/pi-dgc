#ifndef PAINTER_H
#define PAINTER_H

#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QCoreApplication>
#include <QImage>
#include <QTime>
#include <QCamera>
#include <QCameraImageCapture>
#include "images.h"
#include "drawing.h"
#include "variables.h"
#include "vehiclevalues.h"
#include "indicators.h"

class Painter : public QObject
{
    Q_OBJECT

public:
    Painter(VehicleValues &vehicle);

public:
    void paint(QPainter *painter, QPaintEvent *event, int elapsed);
    void initLoop();
    void updateIndicators();

private:
    QBrush backgroundBrush;
    QBrush shiftLightBrush;

    ImageMap *images;
    QFont largeGauge;
    QFont smallGauge;
    QFont normalText;
    QFont largeText;
    QPen gaugeTextPen;
    QPen lightTextPen;

    QCamera *camera;
    QCameraImageCapture *imageCapture;
    QImage cameraImage;

    unsigned long long frameCounter;
    Indicators indicators;
    VehicleValues *vehicle;

private slots:
    void processCapturedImage(int requestId, const QImage &img);
};

#endif // PAINTER_H
