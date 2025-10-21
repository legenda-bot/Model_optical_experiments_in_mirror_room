#include <QApplication>
#include <QSurfaceFormat>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("Mirror Room Optical Experiments");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("OpticalLab");

    // Set OpenGL format if needed for better rendering
    QSurfaceFormat format;
    format.setSamples(4); // Multisampling for smoother lines
    QSurfaceFormat::setDefaultFormat(format);

    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
