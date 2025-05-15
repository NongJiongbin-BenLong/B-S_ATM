#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);                                  // 加载UI界面
    ui->stackedWidget->setCurrentWidget(ui->page_wait); // 设置初始显示的页面为"page_wait"

    // 设置服务器IP和端口。活动事件初始化
    // 127.0.0.1为本地回环地址，具体时需要正确ip地址
    ip = "127.0.0.1";
    S_port = "2525";
    port = S_port.toUShort();
    ActEven = "";

    // tcp创建
    tcp = new QTcpSocket(this);

    connect(tcp, &QTcpSocket::readyRead, this, [=] { // 当有数据可读时触发
        // 读取数据并打印
        qDebug() << "Received string:";
        QString rs = QString::fromUtf8(tcp->readAll());
        qDebug() << rs;
        qDebug() << "\n";
        // 根据分隔符分割字符串为多个部分并清理无用符号
        QStringList parts = rs.split(QRegularExpression("[ :\n!！\r]"));
        parts.removeAll(""); // 移除所有空字符串
        if (ActEven == "Sign:sign" && parts.length() == 2)
        { // sign界面的注册事件响应
            response_on_pushButton_sign_s_clicked(parts);
        }
        if (ActEven == "Sign:hello")
        { // sign界面的用户查询事件响应
            response_on_lineEdit_userid_s_editingFinished(parts);
        }
        if (ActEven == "Login:hello")
        { // login界面的用户查询事件响应
            response_on_pushButton_next_l_clicked(parts);
        }
        if (ActEven == "Login:pasword" && parts.length() == 2)
        { // login界面的密码验证事件响应
            response_on_pushButton_login_p_clicked(parts);
        }
        if (ActEven == "Balance" && parts.length() == 2)
        { // 余额查询事件响应
            response_on_pushButton_balance_f_clicked(parts);
        }
        if (ActEven == "Withdrawal" && parts.length() == 2)
        { // 取款事件响应
            response_on_pushButton_submit_wd_clicked(parts);
        }
        if (ActEven == "Deposit" && parts.length() == 2)
        { // 存款事件响应
            response_on_pushButton_submit_d_clicked(parts);
        }
    });
    connect(tcp, &QTcpSocket::disconnected, this, [=]() { // 当断开连接时触发
        tcp->close();
        m_status->setPixmap(QPixmap(":/image/failed.jpg").scaled(20, 20));
        m_status->setProperty("status", "disconnect");
    });

    connect(tcp, &QTcpSocket::connected, this, [=] { // 当连接成功时触发
        m_status->setPixmap(QPixmap(":/image/success.jpg").scaled(20, 20));
        m_status->setProperty("status", "connect");

        if (ActEven == "Login:connect")
        { // login界面的连接请求响应
            on_pushButton_clear_l_clicked();
            ui->stackedWidget->setCurrentWidget(ui->page_login);
        }
        else if (ActEven == "Sign:connect")
        { // sign界面的连接请求响应
            on_pushButton_clear_s_clicked();
            ui->stackedWidget->setCurrentWidget(ui->page_sign);
        }
    });

    // 设置按钮初始化
    m_set = new QPushButton(this);
    m_set->setIconSize(QSize(20, 20));
    m_set->setIcon(QIcon(":/image/set.png"));
    ui->statusbar->addWidget(m_set);
    connect(m_set, SIGNAL(clicked()), this, SLOT(on_m_set_clicked())); // 设置按钮的点击事件

    // 状态栏初始化
    m_status = new QLabel(this);
    m_status->setPixmap(QPixmap(":/image/failed.jpg").scaled(20, 20));
    ui->statusbar->addWidget(new QLabel("链接状态: "));
    m_status->setProperty("status", "disconnect");
    ui->statusbar->addWidget(m_status);

    // sign界面的控件限定和初始化
    ui->lineEdit_pwd1_s->setValidator(new QRegExpValidator(QRegExp("\\d{6}"))); // 密码输入框限定为6位数字
    ui->lineEdit_pwd2_s->setValidator(new QRegExpValidator(QRegExp("\\d{6}"))); // 确认密码输入框限定为6位数字
    ui->label_tip_s->setStyleSheet("color:red;");                               // 设置提示标签为红色
    ui->label_userid_s->setProperty("color", "black");                          // 设置用户ID标签初始为黑色

    // login界面的控件初始化
    ui->label_tip_l->setStyleSheet("color:red;"); // 设置提示标签为红色

    // password界面的控件限定和初始化
    ui->lineEdit_pwd_p->setValidator(new QRegExpValidator(QRegExp("\\d{6}"))); // 密码输入框限定为6位数字
    ui->label_tip_p->setStyleSheet("color:red;");                              // 设置提示标签为红色

    // withdrawal界面的控件限定和初始化
    ui->lineEdit_wd->setValidator(new QRegExpValidator(QRegExp("\\d{4}"))); // 密码输入框限定为4位数字
    ui->label_tip_wd->setStyleSheet("color:red;");                          // 设置提示标签为红色

    // deposit界面的控件限定和初始化
    ui->lineEdit_d->setValidator(new QRegExpValidator(QRegExp("\\d{5}"))); // 密码输入框限定为5位数字
    ui->label_tip_d->setStyleSheet("color:red;");                          // 设置提示标签为红色
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::TCPDisconnect()
{ // 断开TCP连接
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "询问", "您是否确定要退出ATM？",
                                  QMessageBox::Yes | QMessageBox::No); // 显示询问对话框
    if (reply == QMessageBox::No)
    {
        return;
    } // 如果用户选择取消，不执行断开操作
    else
    { // 如果用户选择确定，执行断开操作
        QString msg = "BYE";
        tcp->write(msg.toUtf8()); // 发送断开连接的BYE消息
        tcp->close();             // 关闭TCP连接
        // 清空所有输入框和标签的内容，并将页面切换回等待页面
        on_pushButton_clear_l_clicked();
        on_pushButton_clear_s_clicked();
        on_pushButton_clear_p_clicked();
        on_pushButton_clear_wd_clicked();
        on_pushButton_clear_d_clicked();
        ui->stackedWidget->setCurrentWidget(ui->page_wait);
    }
}
void MainWindow::on_m_set_clicked()
{ // 设置按钮的点击事件
    bool ok;
    QString text = QInputDialog::getText(this, tr("地址"), tr("请输入服务器所在ip地址："), QLineEdit::Normal, ip, &ok); // 弹出输入对话框，获取用户输入的服务器IP地址
    QRegExp ipRegex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");          // 正则表达式匹配IPv4地址格式
    if (ok)                                                                                                             // 如果用户点击了确定按钮
    {
        if (!text.isEmpty() && ipRegex.exactMatch(text))
        { // 如果输入的IP地址不为空且符合格式
            ip = text;
        }
        else
        { // 如果输入的IP地址为空或不符合格式
            QMessageBox::warning(this, "Warning!", "输入不符合IPv4有效地址格式,服务器ip地址修改失败！");
            return;
        }
    }
    else
    { // 如果用户点击了取消按钮
        return;
    }
}
// page_wait，主界面
void MainWindow::on_pushButton_login_clicked()
{ // 登录按钮的点击事件
    ActEven = "Login:connect";
    tcp->connectToHost(QHostAddress(ip), port); // 连接到服务器
    QTimer::singleShot(3000, this, [=]() {      // 3秒后检查连接状态
        if (tcp->state() != QAbstractSocket::ConnectedState)
        { // 如果连接状态不是已连接
            tcp->abort();
            QMessageBox::critical(this, "连接超时", "服务器无响应"); // 弹出连接超时对话框
        }
    });
}

void MainWindow::on_pushButton_sign_clicked()
{ // 注册按钮的点击事件
    ActEven = "Sign:connect";
    tcp->connectToHost(QHostAddress(ip), port); // 连接到服务器
    QTimer::singleShot(3000, this, [=]() {      // 3秒后检查连接状态
        if (tcp->state() != QAbstractSocket::ConnectedState)
        { // 如果连接状态不是已连接
            tcp->abort();
            QMessageBox::critical(this, "连接超时", "服务器无响应"); // 弹出连接超时对话框
        }
    });
}
// page_sign，注册界面
void MainWindow::on_pushButton_exit_s_clicked()
{ // 退出按钮的点击事件
    TCPDisconnect();
}

void MainWindow::on_pushButton_clear_s_clicked()
{ // 清空按钮的点击事件
    // 清空所有输入框和标签的内容
    ui->lineEdit_userid_s->clear();
    ui->lineEdit_pwd1_s->clear();
    ui->lineEdit_pwd2_s->clear();
    ui->label_tip_s->clear();
    ui->label_userid_s->setStyleSheet("color:black;");
}

void MainWindow::on_pushButton_sign_s_clicked()
{ // 注册按钮的点击事件
    QString userid = ui->lineEdit_userid_s->text();
    QString pwd1 = ui->lineEdit_pwd1_s->text();
    QString pwd2 = ui->lineEdit_pwd2_s->text();
    // 如果用户ID标签的颜色不是绿色（已经过用户名查询），不执行注册操作
    if (ui->label_userid_s->property("color") != "green")
    {
        qDebug() << "Not Green";
        return;
    }
    else if (pwd1.isEmpty())
    {
        ui->label_tip_s->setText("请输入密码！");
        return;
    }
    else if (pwd2.isEmpty())
    {
        ui->label_tip_s->setText("请再次输入密码！");
        return;
    }
    else if (pwd1 != pwd2)
    { // 如果两次输入的密码不匹配
        ui->label_tip_s->setText("两次密码不匹配！");
        return;
    }
    // 发送注册请求消息
    ActEven = "Sign:sign";
    QString msg = "SIGN " + userid + " " + pwd1 + "\n";
    tcp->write(msg.toUtf8());
}
void MainWindow::response_on_pushButton_sign_s_clicked(QStringList parts)
{ // 处理注册请求响应消息
    if (parts[0] == "525" && parts[1] == "OK")
    { // 如果注册成功
        QMessageBox::information(this, "Info", "注册成功！");
        TCPDisconnect();
        ;
    }
    else
    { // 如果注册失败
        QMessageBox::information(this, "Info", "注册失败！请重新尝试！");
        return;
    }
}
void MainWindow::on_lineEdit_userid_s_editingFinished()
{ // 当用户ID输入框编辑完成时触发用户名查询事件
    QString userid = ui->lineEdit_userid_s->text();
    if (userid.isEmpty())
    {
        ui->label_tip_s->setText("卡号不能为空！");
        return;
    } // 如果用户ID为空，显示提示信息
    else
    { // 发送用户名查询请求消息
        ActEven = "Sign:hello";
        QString msg = "HELO " + userid + "\n";
        tcp->write(msg.toUtf8());
    }
}
void MainWindow::response_on_lineEdit_userid_s_editingFinished(QStringList parts)
{ // 处理sign界面的用户名查询请求响应消息
    if (parts.length() == 3 && parts[0] == "500" && parts[1] == "AUTH" && parts[2] == "REQUIRE")
    {
        // 如果用户名查询成功即用户名已存在，不能注册
        ui->label_tip_s->setText("该卡号已注册！请登录或重新输入!");
        ui->label_userid_s->setStyleSheet("color:red;");
        ui->label_userid_s->setProperty("color", "red");
        return;
        ;
    }
    else
    { // 如果用户名查询失败即用户名不存在，可以注册
        ui->label_tip_s->setText("");
        ui->label_userid_s->setStyleSheet("color:green;");
        ui->label_userid_s->setProperty("color", "green");
    }
}

// page_login，登录界面
void MainWindow::on_pushButton_exit_l_clicked()
{ // 退出按钮的点击事件
    TCPDisconnect();
}

void MainWindow::on_pushButton_clear_l_clicked()
{ // 清空按钮的点击事件
    ui->lineEdit_userid_l->clear();
    ui->label_tip_l->clear();
}

void MainWindow::on_pushButton_next_l_clicked()
{ // 下一步（密码验证）按钮的点击事件
    QString userid = ui->lineEdit_userid_l->text();
    if (userid.isEmpty())
    {
        ui->label_tip_l->setText("卡号不能为空！");
        return;
    }
    else
    {
        // 发送用户名查询请求消息
        ActEven = "Login:hello";
        QString msg = "HELO " + userid + "\n";
        tcp->write(msg.toUtf8());
    }
}
void MainWindow::response_on_pushButton_next_l_clicked(QStringList parts)
{
    // 处理login界面的用户名查询请求响应消息
    // 用户名查询成功即用户名存在，进入密码验证页面
    if (parts.length() == 3 && parts[0] == "500" && parts[1] == "AUTH" && parts[2] == "REQUIRE")
    {
        ui->label_tip_l->setText("");
        ui->label_tip_p->setText("");
        ui->stackedWidget->setCurrentWidget(ui->page_password);
        ;
    }
    else
    { // 用户名查询失败，显示提示信息
        ui->label_tip_l->setText("查询不到该卡号！请重新输入！");
        return;
    }
}
// page_password，密码验证界面
void MainWindow::on_pushButton_exit_p_clicked()
{ // 退出按钮的点击事件
    TCPDisconnect();
}

void MainWindow::on_pushButton_clear_p_clicked()
{ // 清空按钮的点击事件
    ui->lineEdit_pwd_p->clear();
    ui->label_tip_p->clear();
}

void MainWindow::on_pushButton_login_p_clicked()
{ // 登录按钮的点击事件
    QString pwd = ui->lineEdit_pwd_p->text();
    // 如果密码输入框为空，显示提示信息
    if (pwd.isEmpty())
    {
        ui->label_tip_p->setText("请输入密码！");
        return;
    }
    else
    {
        // 发送密码验证请求消息
        ActEven = "Login:pasword";
        QString msg = "PASS " + pwd+"\n";
        tcp->write(msg.toUtf8());
    }
}
void MainWindow::response_on_pushButton_login_p_clicked(QStringList parts)
{
    // 处理login界面的密码验证请求响应消息
    if (parts[0] == "525" && parts[1] == "OK")
    {
        // 密码验证成功，进入功能页面
        ui->label_tip_p->setText("");
        skipToFunction();
    }
    else
    { // 密码验证失败，显示提示信息
        ui->label_tip_p->setText("密码输入错误！请重新尝试！");
        return;
    }
}
// page_function，功能选择界面
void MainWindow::skipToFunction()
{
    // 跳转至功能选择界面，显示欢迎信息并启用余额查询按钮
    ui->textBrowser_f->setText("亲爱的用户，欢迎使用ATM！");
    ui->pushButton_balance_f->setDisabled(false);
    ui->pushButton_balance_f->setVisible(true);
    ui->stackedWidget->setCurrentWidget(ui->page_function);
}
void MainWindow::on_pushButton_deposit_f_clicked()
{ // 存款按钮的点击事件
    on_pushButton_clear_d_clicked();
    ui->stackedWidget->setCurrentWidget(ui->page_deposit);
}
void MainWindow::on_pushButton_withdrawal_f_clicked()
{ // 取款按钮的点击事件
    on_pushButton_clear_wd_clicked();
    ui->stackedWidget->setCurrentWidget(ui->page_withdrawal);
}
void MainWindow::on_pushButton_balance_f_clicked()
{ // 余额查询按钮的点击事件
    // 发送余额查询请求消息
    ActEven = "Balance";
    QString msg = "BALA\n";
    tcp->write(msg.toUtf8());
}
void MainWindow::response_on_pushButton_balance_f_clicked(QStringList parts)
{
    // 处理余额查询请求响应消息
    if (parts[0] == "AMNT")
    { // 如果查询成功，显示余额信息，并禁用隐藏余额查询按钮
        ui->textBrowser_f->setText("您的余额是： " + parts[1] + " ￥。");
        ui->pushButton_balance_f->setDisabled(true);
        ui->pushButton_balance_f->setVisible(false);
    }
    else
    { // 如果查询失败，显示提示信息
        ui->textBrowser_f->setText("余额获取失败！请重新尝试！");
        return;
    }
}

void MainWindow::on_pushButton_exit_f_clicked()
{ // 退出按钮的点击事件
    TCPDisconnect();
}
// page_withdrawal，取款界面
// 100-5000元取款额的选择按钮点击事件
void MainWindow::on_pushButton_100_wd_clicked()
{
    ui->lineEdit_wd->setText("100");
}

void MainWindow::on_pushButton_200_wd_clicked()
{
    ui->lineEdit_wd->setText("200");
}

void MainWindow::on_pushButton_500_wd_clicked()
{
    ui->lineEdit_wd->setText("500");
}

void MainWindow::on_pushButton_800_wd_clicked()
{
    ui->lineEdit_wd->setText("800");
}

void MainWindow::on_pushButton_1000_wd_clicked()
{
    ui->lineEdit_wd->setText("1000");
}

void MainWindow::on_pushButton_2000_wd_clicked()
{
    ui->lineEdit_wd->setText("2000");
}

void MainWindow::on_pushButton_3000_wd_clicked()
{
    ui->lineEdit_wd->setText("3000");
}

void MainWindow::on_pushButton_5000_wd_clicked()
{
    ui->lineEdit_wd->setText("5000");
}

void MainWindow::on_pushButton_exit_wd_clicked()
{ // 退出按钮的点击事件
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "询问", "是否要放弃取款？",
                                  QMessageBox::Yes | QMessageBox::No); // 显示询问对话框
    if (reply == QMessageBox::No)
    {
        return;
    } // 如果用户选择取消，不执行退出操作
    else
    {
        skipToFunction();
    } // 如果用户选择确定，跳转到功能选择页面
}

void MainWindow::on_pushButton_clear_wd_clicked()
{ // 清空按钮的点击事件
    ui->lineEdit_wd->clear();
    ui->label_tip_wd->clear();
}

void MainWindow::on_pushButton_submit_wd_clicked()
{ // 提交按钮的点击事件
    QString amnt = ui->lineEdit_wd->text();
    if (amnt.right(2) == "00" && (amnt.length() == 3 || (amnt.length() == 4 && amnt.left(1) <= "5")))
    {
        // 如果输入的金额符合要求（整百且小于等于5000），发送取款请求消息
        ActEven = "Withdrawal";
        QString msg = "WDRA " + amnt+"\n";
        tcp->write(msg.toUtf8());
    }
    else
    { // 如果输入的金额不符合要求，显示提示信息
        ui->label_tip_wd->setText("输入金额必须为整百数！并且一次取款金额不能超过5000￥！");
        return;
    }
}
void MainWindow::response_on_pushButton_submit_wd_clicked(QStringList parts)
{
    // 处理取款请求响应消息
    if (parts[0] == "525" && parts[1] == "OK")
    {
        // 如果取款成功，显示提示信息并跳转到功能选择页面
        QMessageBox::information(this, "Info", "取款成功！");
        skipToFunction();
    }
    else
    { // 如果取款失败，显示提示信息
        QMessageBox::information(this, "Info", "余额不足！");
        return;
    }
}
// page_deposit，存款界面
// 100-10000元存款额的选择按钮点击事件
void MainWindow::on_pushButton_100_d_clicked()
{
    ui->lineEdit_d->setText("100");
}

void MainWindow::on_pushButton_200_d_clicked()
{
    ui->lineEdit_d->setText("200");
}

void MainWindow::on_pushButton_500_d_clicked()
{
    ui->lineEdit_d->setText("500");
}

void MainWindow::on_pushButton_1000_d_clicked()
{
    ui->lineEdit_d->setText("1000");
}

void MainWindow::on_pushButton_2000_d_clicked()
{
    ui->lineEdit_d->setText("2000");
}

void MainWindow::on_pushButton_5000_d_clicked()
{
    ui->lineEdit_d->setText("5000");
}
void MainWindow::on_pushButton_8000_d_clicked()
{
    ui->lineEdit_d->setText("8000");
}
void MainWindow::on_pushButton_10000_d_clicked()
{
    ui->lineEdit_d->setText("10000");
}
void MainWindow::on_pushButton_exit_d_clicked()
{ // 退出按钮的点击事件
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "询问", "是否要放弃存款？",
                                  QMessageBox::Yes | QMessageBox::No); // 显示询问对话框
    if (reply == QMessageBox::No)
    {
        return;
    } // 如果用户选择取消，不执行退出操作
    else
    {
        skipToFunction();
    } // 如果用户选择确定，跳转到功能选择页面
}

void MainWindow::on_pushButton_clear_d_clicked()
{ // 清空按钮的点击事件
    ui->lineEdit_d->clear();
    ui->label_tip_d->clear();
}

void MainWindow::on_pushButton_submit_d_clicked()
{ // 提交按钮的点击事件
    QString amnt = ui->lineEdit_d->text();
    if (amnt.right(2) == "00" && (amnt.length() <= 4 || amnt == "10000"))
    {
        // 如果输入的金额符合要求（整百且小于10000），发送存款请求消息
        ActEven = "Deposit";
        QString msg = "DEPO " + amnt + "\n";
        tcp->write(msg.toUtf8());
    }
    else
    { // 如果输入的金额不符合要求，显示提示信息
        ui->label_tip_d->setText("输入金额必须为整百数！并且一次存款金额不能超过10000￥！");
        return;
    }
}
void MainWindow::response_on_pushButton_submit_d_clicked(QStringList parts)
{
    // 处理存款请求响应消息
    if (parts[0] == "525" && parts[1] == "OK")
    { // 如果存款成功，显示提示信息并跳转到功能选择页面
        QMessageBox::information(this, "Info", "存款成功！");
        skipToFunction();
    }
    else
    { // 如果存款失败，显示提示信息
        QMessageBox::information(this, "Info", "存款失败！请重新尝试！");
        return;
    }
}
