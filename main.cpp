#include <QApplication>
#include "mainwindow.h"
#include "avideowidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("Agora");
    app.setOrganizationDomain("agora.io");

    qmlRegisterType<AVideoWidget>("AVideoWidget", 1, 0, "AVideoWidget");

    MainWindow w;
    w.show();
    return app.exec();
}

