#include "window.h"
#include <math.h>

Window::Window(VehicleValues &vehicle)
{
    this->vehicle = &vehicle;
    this->painter = new Painter(vehicle);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setGeometry(0, 0, 1280, 480);

    GLWidget *openGL = new GLWidget(painter, this);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, openGL, &GLWidget::animate);
    timer->start(17); // ~60 FPS
}

void Window::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        this->close();
    }
}

void Window::mousePressEvent(QMouseEvent *event) {

    // See if reset button was pressed
    if (sqrt(pow(415.0 - event->x(), 2) + pow(420.0 - event->y(), 2) * 1.0) < 30.0f) {
        qDebug() << "Trip reset pressed.";
        vehicle->resetTrip = true;
    }
}
