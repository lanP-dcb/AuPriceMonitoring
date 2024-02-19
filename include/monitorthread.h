#ifndef MONITORTHREAD_H
#define MONITORTHREAD_H

#include <QThread>
#include <QObject>
#include <QTimer>
#include <QHttpPart>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include "configmanagement.h"

class MonitorThread : public QThread
{
    Q_OBJECT
public:
    MonitorThread();

    ~MonitorThread();

    void run() override;

Q_SIGNALS:
    void MonitorResultSignal(int, QByteArray);

private:
    QTimer *m_timer{};
    QByteArray byt;
};

#endif // MONITORTHREAD_H
