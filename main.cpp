#include "letsstock.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LetsStock w;
    w.show();
    return a.exec();
}
