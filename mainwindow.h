#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QVariantMap>
#include <QMenu>
#include <QAction>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QEvent>
#include <QScreen>
#include <QMessageBox>
#include <QDesktopWidget>
#include "monitorthread.h"
#include "traymanagement.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private Q_SLOTS:
    void SlotMainWindowOperation(QObject* obj,bool checked,int value);

    void SlotSetWinOpacity(std::string key,int value);

    void mousePressEvent(QMouseEvent *e) override;

    void mouseMoveEvent(QMouseEvent *e) override;

    void mouseReleaseEvent(QMouseEvent *e) override;

    void MonitorResultSlot(int type, QByteArray byt);

    void UpdateInfoSlot(int index);

private:
    Ui::MainWindow *ui;
    MonitorThread *m_monitor;
    QList<QVariantMap>  m_AuMapList;
    TrayManagement *m_trayMngmt;
    QDesktopWidget *desktop;
    bool mousePress{false};             // 左键点击
    QPoint m_pos;                       // 窗口移动
    bool pos_lock;                      // 窗口位置锁定
    bool mouseTransparent;              // 鼠标穿透
    bool cross;                         // 屏幕越界
    QMenu *m_menu;                      // 菜单
};
#endif // MAINWINDOW_H
