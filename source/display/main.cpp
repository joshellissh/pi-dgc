#include "window.h"
#include <QApplication>
#include <QSurfaceFormat>
#include "commsserver.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Turn on anti-aliasing
    QSurfaceFormat fmt;
    fmt.setSamples(4);
    QSurfaceFormat::setDefaultFormat(fmt);

    VehicleValues vehicle;

    // Create server
    CommsServer server(vehicle);
    server.startServer();

    // Create window
    Window window(vehicle);
    window.show();

    return app.exec();
}
