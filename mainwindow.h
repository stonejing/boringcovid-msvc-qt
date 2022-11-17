#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTextEdit>
#include <QPlainTextEdit>
#include <QSystemTrayIcon>

#include "json.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static QPlainTextEdit* Log;
    static QTextEdit* TeLog;

//    void setVisible(bool visible) override;

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;

    void PushButtonStartOnClick();

    void CreateActions();
    void CreateTrayIcon();

    using json = nlohmann::json;
    json json_config;

    int config_nums;

    void StartServer();
    void CancelConfigure();
    void ApplyConfigure();

    void WriteConfig();
    void ReadConfig();

    void AddConfig();
    void DeleteConfig();

    void ChangeLabelText();
    void AppendLog();

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QAction *restoreAction;
    QAction *quitAction;

};
#endif // MAINWINDOW_H
