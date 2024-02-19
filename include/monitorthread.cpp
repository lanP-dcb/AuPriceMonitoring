#include "monitorthread.h"

#include <QNetworkReply>

MonitorThread::MonitorThread()
{
    moveToThread(this);
    DEBUG(QSslSocket::sslLibraryBuildVersionString());
    DEBUG(QSslSocket::supportsSsl());
}

MonitorThread::~MonitorThread()
{
    if (nullptr != m_timer) {
        delete m_timer;
    }
}

void MonitorThread::run()
{
    m_timer = new QTimer(this);
    connect(m_timer,&QTimer::timeout, [&](){
        m_timer->stop();

        QEventLoop loop;
        QTimer http_timeout;
        connect(&http_timeout,&QTimer::timeout,&loop,&QEventLoop::quit);

        QNetworkAccessManager m_manager;
        connect(&m_manager,&QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

        QNetworkRequest m_request;
        m_request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json;charset=UTF-8");
        m_request.setUrl(QUrl("https://m.cmbchina.com/api/rate/gold"));

        http_timeout.start(10000);
        QNetworkReply *m_reply = m_manager.get(m_request);
        loop.exec();

        http_timeout.stop();
        if (!m_reply->isFinished()) {
            emit MonitorResultSignal(1,""); // 超时
        } else if (m_reply->error() != QNetworkReply::NoError) {
            emit MonitorResultSignal(2,m_reply->errorString().toUtf8());
        } else {
            emit MonitorResultSignal(0, m_reply->readAll());
        }
        m_timer->start(60000); // 1分钟刷新一次
    });
    m_timer->start(1000);
    exec();
}
