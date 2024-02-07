#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "configmanagement.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 鼠标是否穿透
    setAttribute(Qt::WA_TransparentForMouseEvents,
                 GetCfgValToBool(G_SYSTEM,"menu_set_mouse_penetration"));
    // 无边框
//    setWindowFlags(Qt::FramelessWindowHint);
    // 窗口位置
    desktop = QApplication::desktop();
    QApplication::screens();
    if (GetCfgVal(G_SYSTEM,WIN_POS_X).isNull()) {
        SetCfgVal(G_SYSTEM,WIN_POS_X,desktop->width()-this->width());
        SetCfgVal(G_SYSTEM,WIN_POS_Y,desktop->height()-this->height());
    }
    move(GetCfgVal(G_SYSTEM,WIN_POS_X).toInt(), GetCfgVal(G_SYSTEM,WIN_POS_Y).toInt());


    m_trayMngmt = new TrayManagement();
    if (!m_trayMngmt->InitUi()) {
        DEBUG("初始化托盘程序失败");
        exit(-1);
    }
    m_menu = m_trayMngmt->GetMenu();

    m_monitor = new MonitorThread();
    connect(m_monitor, &MonitorThread::MonitorResultSignal,
            this, &MainWindow::MonitorResultSlot);
//    m_monitor->start();
    ui->comboBox->setCurrentIndex(GetCfgVal(G_SYSTEM,"curIndex").toInt());
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(UpdateInfoSlot(int)));
}

MainWindow::~MainWindow()
{
    m_monitor->quit();
    m_monitor->wait();
    delete ui;
}

void MainWindow::SlotMainWindowOperation(QObject *obj, bool checked, int value)
{
    SetCfgVal(G_SYSTEM,obj->objectName().toUtf8().data(),checked);
    switch (value) {
    case 2: { // 置顶窗口
        checked ? setWindowFlag(Qt::ToolTip, true)
                : setWindowFlag(Qt::ToolTip, false);
        show();
    }break;
    case 3:{ // 鼠标穿透
        static bool first_init = true;
        if (first_init) {
            first_init = false;
            break;
        }
        QMessageBox::information(nullptr,tr("信息"),tr("该设置重启应用后生效"));
        // checked ? mouseTransparent = true : mouseTransparent = false;
    }break;
    case 4:{ // 锁定窗口位置
        checked ? pos_lock = true : pos_lock = false;
    }break;
    case 7:{ // 显示主窗口
        checked ? show() : hide();
    }break;
    case 14:{ // 超出屏幕边界
        checked ? cross = true : cross = false;
    }
    default:break;
    }
}

void MainWindow::SlotSetWinOpacity(std::string key, int value)
{
    switch (value) {
    case 8:setWindowOpacity(1.0);break;
    case 9:setWindowOpacity(0.8);break;
    case 10:setWindowOpacity(0.6);break;
    case 11:setWindowOpacity(0.4);break;
    default:break;
    }
    SetCfgVal(G_SYSTEM,key,value - 8);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        DEBUG("");
        if (pos_lock)
            return;
        mousePress = true;
        m_pos = e->pos();
    } else if (e->button() == Qt::RightButton) {
        m_menu->exec(e->globalPos());
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePress) {
        move(e->pos()- m_pos + this->pos());
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
    (void)e;
    if (mousePress) {
        mousePress = false;
        if (!cross) {
            static int l_x = QGuiApplication::primaryScreen()->availableGeometry().x();
            static int l_y = QGuiApplication::primaryScreen()->availableGeometry().y();
            static int h_x = QGuiApplication::primaryScreen()->availableGeometry().width() - width();
            static int h_y = QGuiApplication::primaryScreen()->availableGeometry().height() - height();
            if (pos().x() < l_x)
                move(l_x, pos().y());
            if (pos().x() > h_x)
                move(h_x, pos().y());
            if (pos().y() < l_y)
                move(pos().x(),l_y);
            if (pos().y() > h_y)
                move(pos().x(), h_y);
        }
        SetCfgVal(G_SYSTEM,WIN_POS_X,pos().x());
        SetCfgVal(G_SYSTEM,WIN_POS_Y,pos().y());
    }
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
        //        map.insert("time",obj.value("time").toString());            // 时间
        //        map.insert("goldNo",obj.value("goldNo").toString());        // 代号
        m_AuMapList.append(map);
    }
    ui->label_time->setText(body.value("time").toString());
    foreach (const QVariantMap& map, m_AuMapList) {
        if (map.value("variety").toString() != ui->comboBox->currentText())
            continue;
        ui->label_cur->setText(map.value("curPrice").toString());
        ui->label_change->setText(map.value("upDown").toString());
        ui->label_top->setText(map.value("open").toString());
        ui->label_low->setText(map.value("low").toString());
        break;
    }}

void MainWindow::UpdateInfoSlot(int index)
{
    SetCfgVal(G_SYSTEM,"curIndex",ui->comboBox->currentIndex());
    foreach (const QVariantMap& map, m_AuMapList) {
        if (map.value("variety").toString() != ui->comboBox->currentText())
            continue;
        ui->label_cur->setText(map.value("curPrice").toString());
        ui->label_change->setText(map.value("upDown").toString());
        ui->label_top->setText(map.value("open").toString());
        ui->label_low->setText(map.value("low").toString());
        break;
    }
}

