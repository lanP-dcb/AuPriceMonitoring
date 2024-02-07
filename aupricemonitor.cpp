#include "aupricemonitor.h"

AuPriceMonitor::AuPriceMonitor()
{
    m_mainWin = new MainMonitorWin();
    m_trayMngmt = new TrayManagement();
    if (!m_trayMngmt->InitUi()) {
        DEBUG("初始化托盘程序失败");
        exit(-1);
    }
    m_mainWin->SetMenu(m_trayMngmt->GetMenu());
    InitConnect();
    m_trayMngmt->InitCfg();

    m_thread = new MonitorThread();
    connect(m_thread, &MonitorThread::MonitorResultSignal,
            m_mainWin,&MainMonitorWin::MonitorResultSlot);
    m_thread->start();
}

AuPriceMonitor::~AuPriceMonitor()
{
    if (nullptr != m_trayMngmt) {
        delete m_trayMngmt;
    }
    if (nullptr != m_mainWin) {
        delete m_mainWin;
    }
}

void AuPriceMonitor::InitConnect()
{
    connect(m_trayMngmt, &TrayManagement::SignalMainWindowOperation,
            m_mainWin, &MainMonitorWin::SlotMainWindowOperation);
    connect(m_trayMngmt, &TrayManagement::SignalSetWinOpacity,
            m_mainWin, &MainMonitorWin::SlotSetWinOpacity);
    connect(m_trayMngmt, &TrayManagement::SignalSetShowGoldType,
            m_mainWin, &MainMonitorWin::SetShowGoldTypeSlot);
}
