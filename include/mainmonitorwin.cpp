#include "mainmonitorwin.h"



MainMonitorWin::MainMonitorWin(QWidget *parent)
    : QWidget(parent)
{
    InitUi();
    // 鼠标是否穿透
    setAttribute(Qt::WA_TransparentForMouseEvents,
                 GetCfgValToBool(G_SYSTEM,"menu_set_mouse_penetration"));
    // 无边框
    setWindowFlags(Qt::FramelessWindowHint);
    // 窗口位置
    desktop = QApplication::desktop();
    QApplication::screens();
    if (GetCfgVal(G_SYSTEM,WIN_POS_X).isNull()) {
        SetCfgVal(G_SYSTEM,WIN_POS_X,desktop->width()-this->width());
        SetCfgVal(G_SYSTEM,WIN_POS_Y,desktop->height()-this->height());
    }
    move(GetCfgVal(G_SYSTEM,WIN_POS_X).toInt(), GetCfgVal(G_SYSTEM,WIN_POS_Y).toInt());

}

void MainMonitorWin::InitUi()
{
    QVBoxLayout *m_layout = new QVBoxLayout();
    QStringList obj_list;

    QHBoxLayout *line_1 = new QHBoxLayout;
    line_1->setSpacing(5);
    obj_list << "当前"   << "浮动" << "今高"   << "今低";
    foreach (const QString& str, obj_list) {
        QLabel *label = new QLabel();
        label->setText(str);
        label->setObjectName(str);
        label->setAlignment(Qt::AlignCenter);
        label->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
        line_1->addWidget(label,1);
    }


    QHBoxLayout *line_2 = new QHBoxLayout;
    line_2->setSpacing(5);
    cur = new QLabel();
    cur->setAlignment(Qt::AlignCenter);
    cur->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    upDown = new QLabel();
    upDown->setAlignment(Qt::AlignCenter);
    upDown->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    high = new QLabel();
    high->setAlignment(Qt::AlignCenter);
    high->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    low = new QLabel();
    low->setAlignment(Qt::AlignCenter);
    low->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    line_2->addWidget(cur,1);
    line_2->addWidget(upDown,1);
    line_2->addWidget(high,1);
    line_2->addWidget(low,1);


    errMsg = new QLabel();
    errMsg->setAlignment(Qt::AlignLeft);
    errMsg->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    time = new QLabel();
    time->setAlignment(Qt::AlignRight);
    time->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    QHBoxLayout *line_3 = new QHBoxLayout;
    line_3->setSpacing(5);
    line_3->addWidget(errMsg,1);
    line_3->addWidget(time,1);


    m_layout->addItem(line_1);
    m_layout->addItem(line_2);
    m_layout->addItem(line_3);

    m_layout->setSizeConstraint(QLayout::SetDefaultConstraint);
    this->setLayout(m_layout);
}

void MainMonitorWin::SetMenu(QMenu *menu)
{
    m_menu = menu;
}

void MainMonitorWin::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        if (pos_lock)
            return;
        mousePress = true;
        m_pos = e->pos();
    } else if (e->button() == Qt::RightButton) {
        m_menu->exec(e->globalPos());
    }
}

void MainMonitorWin::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePress) {
        move(e->pos()- m_pos + this->pos());
    }
}

void MainMonitorWin::mouseReleaseEvent(QMouseEvent *e)
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

void MainMonitorWin::SlotMainWindowOperation(QObject *obj, bool checked, int value)
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
//        QMessageBox::information(nullptr,tr("信息"),tr("该设置重启应用后生效"));
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

void MainMonitorWin::SlotSetWinOpacity(std::string key, int value)
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

void MainMonitorWin::MonitorResultSlot(int type, QByteArray byt)
{
    errMsg->setToolTip("");
    if (type != 0) {
        if (byt.isEmpty()) {
            errMsg->setText("请求超时");
        } else {
            errMsg->setText("请求错误");
            errMsg->setToolTip(byt);
        }
        return;
    }
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(byt,&error);
    if (error.error != QJsonParseError::NoError) {
        errMsg->setText("Json数据解析异常");
        errMsg->setToolTip(byt);
        return;
    }
    QJsonObject root = doc.object();
    if (root.value("returnCode").toString() != "SUC0000") {
        errMsg->setText("Json返回值报错");
        errMsg->setToolTip("Code:" + root.value("returnCode").toString() + "\nMsg:" + root.value("errorMsg").toString());
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
    time->setText(body.value("time").toString());
    UpdateGoldPrice();
}

void MainMonitorWin::SetShowGoldTypeSlot(QObject *sender, std::string key, int value)
{
    const QAction *action = dynamic_cast<QAction*>(sender);
    SetCfgVal(G_SYSTEM,GOLD_TYPE_NAME,action->text());
    SetCfgVal(G_SYSTEM,key,value - 25);
    UpdateGoldPrice();
}

void MainMonitorWin::UpdateGoldPrice()
{
    QString type = GetCfgVal(G_SYSTEM,GOLD_TYPE_NAME).toString();
    foreach (const QVariantMap& map, m_AuMapList) {
        if (map.value("variety").toString() != type)
            continue;
        cur->setText(map.value("curPrice").toString());
        upDown->setText(map.value("upDown").toString());
        high->setText(map.value("high").toString());
        low->setText(map.value("low").toString());
        break;
    }
}
