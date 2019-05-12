#include <QApplication>
#include "Server.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Server server;
    server.show();
    return app.exec();
}
