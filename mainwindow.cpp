#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <thread>
#include "boringcovid.h"
#include <fstream>
#include <QDebug>
#include <QMessageBox>

QPlainTextEdit* MainWindow::Log = nullptr;
QTextEdit* MainWindow::TeLog = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , config_nums(0)
{
    ui->setupUi(this);

    connect(ui->pushButton_start, &QPushButton::clicked, this, &MainWindow::StartServer);
    connect(ui->pushButton_stop, &QPushButton::clicked, this, &MainWindow::CancelConfigure);
    connect(ui->pushButton_add, &QPushButton::clicked, this, &MainWindow::AddConfig);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::ChangeLabelText);
    connect(ui->pushButton_delete, &QPushButton::clicked, this, &MainWindow::DeleteConfig);
    connect(ui->pushButton_apply, &QPushButton::clicked, this, &MainWindow::ApplyConfigure);
    ReadConfig();

    CreateActions();
    CreateTrayIcon();

    trayIcon->show();
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

void MainWindow::AddConfig()
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

void MainWindow::DeleteConfig()
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

void MainWindow::closeEvent(QCloseEvent *event)
{
//    if(trayIcon->isVisible())
//    {
////        QMessageBox::information(this, tr("Systray"),
////                                 tr("DDDDD"));
//        hide();
//        event->ignore();
//    }
    hide();
    event->ignore();
}

void MainWindow::CreateActions()
{
    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &MainWindow::show);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MainWindow::CreateTrayIcon()
{
    trayIconMenu = new QMenu(this);

    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    QIcon icon = QIcon(":/assets/icon32.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);
    trayIcon->setVisible(true);
}


void MainWindow::StartServer()
{
    BoringCovid *boring = new BoringCovid();
    std::string address = ui->lineEdit_remote_address->text().toStdString();
    std::string password = ui->lineEdit_password->text().toStdString();
    std::thread my_thread(&BoringCovid::StartServer, boring);
    my_thread.detach();

    ui->pushButton_start->setEnabled(false);
    ui->pushButton_start->setText("running");
    ui->pushButton_start->setStyleSheet("QPushButton {background-color: #A3C1DA; color: red;}");
}

void MainWindow::CancelConfigure()
{
    ui->pushButton_start->setEnabled(true);
    ui->pushButton_start->setText("start");
    ui->pushButton_start->setStyleSheet(styleSheet());
    return;
}

void MainWindow::ApplyConfigure()
{
    qDebug() << "this is a test";
    return;
}

