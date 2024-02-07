#include "aupricemonitor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AuPriceMonitor m_monitor;
    return a.exec();
}
