#ifndef AUPRICEMONITOR_H
#define AUPRICEMONITOR_H

#include "include/configmanagement.h"
#include "include/mainmonitorwin.h"
#include "include/traymanagement.h"
#include "include/monitorthread.h"
#include <QObject>

class AuPriceMonitor : public QObject
{
    Q_OBJECT
public:
    AuPriceMonitor();
    ~AuPriceMonitor();

    void InitConnect();

private:
    MainMonitorWin *m_mainWin{};        // 主窗口
    TrayManagement *m_trayMngmt{};      // 系统托盘窗口
    MonitorThread *m_thread;
};

#endif // AUPRICEMONITOR_H
