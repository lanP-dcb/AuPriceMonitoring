#ifndef MAINMONITORWIN_H
#define MAINMONITORWIN_H

//#if UNIX
//#include <libappindicator/app-indicator.h>
//#undef signals
//#include <gtk/gtk.h>
//#endif
#include "configmanagement.h"
#include <QGuiApplication>
#include <QScreen>
#include <QObject>
#include <QWidget>
#include <QMenu>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QEvent>
#include <QLayout>
#include <QLabel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

class MainMonitorWin : public QWidget
{
    Q_OBJECT
public:
    MainMonitorWin(QWidget *parent = nullptr);

    void InitUi();

    void SetMenu(QMenu* menu);

public slots:

    void SlotMainWindowOperation(QObject* obj,bool checked,int value);

    void SlotSetWinOpacity(std::string key,int value);

    void MonitorResultSlot(int type, QByteArray byt);

    void SetShowGoldTypeSlot(QObject* sender,std::string key,int value);

private slots:
    void mousePressEvent(QMouseEvent *e) override;

    void mouseMoveEvent(QMouseEvent *e) override;

    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QDesktopWidget* desktop;
    bool mousePress{false};             // 左键点击
    QPoint m_pos;                       // 窗口移动
    bool pos_lock;                      // 窗口位置锁定
    bool mouseTransparent;              // 鼠标穿透
    bool cross;                         // 屏幕越界
    QMenu *m_menu;                      // 菜单
    QList<QVariantMap> m_AuMapList;
    QLabel *cur,*upDown,*high,*low;
    QLabel *errMsg,*time;

    void UpdateGoldPrice();
};

#endif // MAINMONITORWIN_H
