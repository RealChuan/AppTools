#include <tcpwidget.h>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TcpWidget w;
    w.show();

    return a.exec();
}
