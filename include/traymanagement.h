#ifndef TRAYMANAGEMENT_H
#define TRAYMANAGEMENT_H

#include "configmanagement.h"
#include <QObject>
#include <QFile>
#include <QMenu>
#include <QAction>
#include <QMap>
#include <QSystemTrayIcon>
#include <QDomDocument>
#include <QActionGroup>

class TrayManagement : public QObject
{
    Q_OBJECT

    QSystemTrayIcon *m_trayIcon{};
    QMenu *m_trayMenu{};
    QString obj_name;
    QMap<QString, int> m_obj_map;
    QList<QAction*> m_init_action;
public:
    TrayManagement();
    ~TrayManagement();

    bool InitUi();
    bool InitCfg();
    QMenu* GetMenu();

private:
    void InitMenu(const QDomNodeList& nodelist, QMenu* menu);
    void InitGroup(const QDomNodeList& nodelist, QActionGroup* group);

private slots:
    void QActionEmitForwardSlots(bool checked);

signals:
    void SignalMainWindowOperation(QObject*,bool,int);
    void SignalSetWinOpacity(std::string,int);
    void SignalSetShowGoldType(QObject*,std::string,int);
};

#endif // TRAYMANAGEMENT_H
