#include "commsthread.h"
#include "vehiclevalues.h"
#include <QJsonDocument>

CommsThread::CommsThread(qintptr ID, VehicleValues &vehicle, QObject *parent) :
    QThread(parent)
{
    this->vehicle = &vehicle;
    this->socketDescriptor = ID;
}

void CommsThread::run() {
    qDebug() << "CommsThread started";

    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    qDebug() << socketDescriptor << " Client connected";

    // Make thread loops
    exec();
}

void CommsThread::readyRead()
{
    // Read packet
    QByteArray packet = socket->readLine();
    QJsonDocument loadDoc = QJsonDocument::fromJson(packet);

    if (vehicle->initialized)
        vehicle->deserialize(loadDoc.object());

    // Confirm packet receipt
    socket->write("OK\n");

    // Reset trip if necessary
    if (vehicle->resetTrip) {
        vehicle->resetTrip = false;
        socket->write("RESET_TRIP\n");
    }
}

void CommsThread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    socket->deleteLater();
    exit(0);
}
