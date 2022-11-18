#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <thread>
#include "boringcovid.h"
#include <fstream>
#include <QDebug>
#include <QMessageBox>
#include <QActionGroup>

QPlainTextEdit* MainWindow::Log = nullptr;
QTextEdit* MainWindow::TeLog = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , config_nums(0)
{
    ui->setupUi(this);

    connect(ui->pushButton_start, &QPushButton::clicked, this, &MainWindow::Start);
    connect(ui->pushButton_stop, &QPushButton::clicked, this, &MainWindow::Stop);
    connect(ui->pushButton_apply, &QPushButton::clicked, this, &MainWindow::Apply);
    connect(ui->pushButton_add, &QPushButton::clicked, this, &MainWindow::Add);
    connect(ui->pushButton_delete, &QPushButton::clicked, this, &MainWindow::Delete);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::ChangeLabelText);

    CreateActions();
    CreateTrayIcon();
    ReadConfig();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ChangeLabelText()
{
    if(config_nums == 0) return;
    int index = ui->listWidget->currentRow();
    std::string server_address = json_config["configs"][index]["server_address"];
    std::string password = json_config["configs"][index]["password"];
    int server_port = json_config["configs"][index]["server_port"];
    std::string method = json_config["configs"][index]["method"];
    ui->lineEdit_remote_address->setText(QString::fromStdString(server_address));
    ui->lineEdit_remote_port->setText(QString::number(server_port));
    ui->lineEdit_password->setText(QString::fromStdString(password));
    ui->comboBox_method->setCurrentText(QString::fromStdString(method));
}

void MainWindow::WriteConfig()
{
    std::fstream fs;
    fs.open("config.json", std::fstream::in | std::fstream::out | std::fstream::trunc);
    fs << json_config << "\n";
    fs.close();
}

void MainWindow::ReadConfig()
{
    std::fstream fs;
    fs.open("config.json", std::fstream::in);
    if(!fs.good()) return;
    json_config = json::parse(fs);
    config_nums = (int)json_config["configs"].size();
    QStringList list_names;
    for(int i = 0; i < config_nums; i++)
    {
        std::string server_address = json_config["configs"][i]["server_address"];
        int server_port = json_config["configs"][i]["server_port"];
        std::string list_name = server_address + ":" + std::to_string(server_port);
        list_names.push_back(QString::fromStdString(list_name));
    }
    ui->listWidget->addItems(list_names);
    fs.close();
}

void MainWindow::Add()
{
    QString server_address = ui->lineEdit_remote_address->text();
    int server_port = ui->lineEdit_remote_port->text().toInt();
    QString password = ui->lineEdit_password->text();
    QString method = ui->comboBox_method->currentText();

    if(!server_address.isEmpty() && (server_port != 0) && !password.isEmpty())
    {
        QListWidgetItem *newItem = new QListWidgetItem;
        QString itemText = server_address + ":" + QString::number(server_port);
        newItem->setText(itemText);
        ui->listWidget->addItem(newItem);
        json_config["configs"][config_nums]["server_address"] = server_address.toStdString();
        json_config["configs"][config_nums]["server_port"] = server_port;
        json_config["configs"][config_nums]["password"] = password.toStdString();
        json_config["configs"][config_nums]["method"] = method.toStdString();
        config_nums++;
        WriteConfig();
    }
    else
    {
        qDebug() << "server_address or server_port or password is null" ;
    }
}

void MainWindow::Delete()
{
    int index = ui->listWidget->currentRow();

    if(index >= 0 && index < ui->listWidget->count())
    {
        json_config["configs"].erase(index);
        ui->listWidget->takeItem(index);
        config_nums--;
        ChangeLabelText();
        WriteConfig();
    }
}

void MainWindow::Copy()
{

}

void MainWindow::MoveUp()
{

}

void MainWindow::MoveDown()
{

}

// start local proxy server
void MainWindow::Start()
{
//    BoringCovid *boring = new BoringCovid();
//    std::string address = ui->lineEdit_remote_address->text().toStdString();
//    std::string password = ui->lineEdit_password->text().toStdString();
//    std::thread my_thread(&BoringCovid::StartServer, boring);
//    my_thread.detach();
    qDebug() << "Server Started!!!";

    ui->pushButton_start->setEnabled(false);
    ui->pushButton_start->setText("running");
    ui->pushButton_start->setStyleSheet("QPushButton {background-color: #A3C1DA; color: red;}");

    hide();
}

// no need to do it
void MainWindow::Stop()
{
    ui->pushButton_start->setEnabled(true);
    ui->pushButton_start->setText("start");
    ui->pushButton_start->setStyleSheet(styleSheet());
    return;
}

// change remote address configure and restart local proxy server
void MainWindow::Apply()
{
    qDebug() << "Apply Button Clicked.";
    return;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void MainWindow::CreateActions()
{
    none = new QAction(tr("&none"), this);
    // none system proxy
    pac = new QAction(tr("&pac"), this);
    // set system proxy to pac
    global = new QAction(tr("&global"), this);
    // set global proxy mode
    // simple way to change registery

    autoStartAction = new QAction(tr("&auto start"), this);
    // change registery to set auto start setting
    aboutAction = new QAction(tr("&about"), this);
    // show about information

    minimizeAction = new QAction(tr("&minimize"), this);
    connect(minimizeAction, &QAction::triggered, this, &MainWindow::hide);

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &MainWindow::show);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    autoStartAction->setCheckable(true);
}

void MainWindow::CreateTrayIcon()
{
    trayIconMenu = new QMenu(this);
    QMenu *systemProxyMenu = new QMenu("system proxy");
    QMenu *ServerListsMenu = new QMenu("server list");
	
    trayIconMenu->addMenu(systemProxyMenu);
    trayIconMenu->addMenu(ServerListsMenu);

    QActionGroup *alignmentGroup = new QActionGroup(this);

    alignmentGroup->addAction(none);
    alignmentGroup->addAction(pac);
    alignmentGroup->addAction(global);

    none->setCheckable(true);
    pac->setCheckable(true);
    global->setCheckable(true);

    systemProxyMenu->addActions(alignmentGroup->actions());
	
    trayIconMenu->setStyleSheet("color: rgb(255, 0, 0)");

	trayIconMenu->addAction(autoStartAction);
	trayIconMenu->addAction(aboutAction);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    QIcon icon = QIcon(":/assets/icon32.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    trayIcon->setVisible(true);
    trayIcon->show();
}

