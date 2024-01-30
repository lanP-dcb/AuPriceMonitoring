#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_monitor = new MonitorThread();
    connect(m_monitor, &MonitorThread::MonitorResultSignal,
            this, &MainWindow::MonitorResultSlot);
    m_monitor->start();
}

MainWindow::~MainWindow()
{
    m_monitor->quit();
    m_monitor->wait();
    delete ui;
}

void MainWindow::MonitorResultSlot(int type, QByteArray byt)
{
    if (type != 0) {
        if (byt.isEmpty()) {
            qDebug() << "请求超时:";
        } else {
            qDebug() << "请求错误,错误消息:" + byt;
        }
    }
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(byt,&error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Json数据解析异常，原文如下：\n" << byt;
        return;
    }
    QJsonObject root = doc.object();
    if (root.value("returnCode").toString() != "SUC0000") {
        qDebug() << "Json返回值报错:\n"
                 << "Code:" + root.value("returnCode").toString()
                 << "\nMsg:" + root.value("errorMsg").toString();
        return ;
    }
    QJsonObject body = root.value("body").toObject();
    QJsonArray data = body.value("data").toArray();
    m_AuMapList.clear();
    foreach (const QJsonValue &val, data) {
        QVariantMap map;
        QJsonObject obj = val.toObject();
        map.insert("variety",obj.value("variety").toString());      // 品种
        map.insert("curPrice",obj.value("curPrice").toString());    // 最新价格
        map.insert("upDown",obj.value("upDown").toString());        // 涨跌
        map.insert("open",obj.value("open").toString());            // 开盘价
        map.insert("preClose",obj.value("preClose").toString());    // 昨收盘价
        map.insert("high",obj.value("high").toString());            // 最高价
        map.insert("low",obj.value("low").toString());              // 最低价
        map.insert("avePrice",obj.value("avePrice").toString());    // 平均价
        map.insert("tradeCount",obj.value("tradeCount").toString());// 成交量
        map.insert("time",obj.value("time").toString());            // 时间
        map.insert("goldNo",obj.value("goldNo").toString());        // 代号
        m_AuMapList.append(map);
    }
    qDebug() << m_AuMapList;
}

