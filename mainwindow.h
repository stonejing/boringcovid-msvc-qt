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

    using json = nlohmann::json;
    json json_config;

    int config_nums;

    void Start();
    void Stop();
    void Apply();

    void WriteConfig();
    void ReadConfig();

    void Add();
    void Delete();
    void Copy();
    void MoveUp();
    void MoveDown();

    void ChangeLabelText();

private:
    void CreateActions();
    void CreateTrayIcon();

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
	
	QAction *none;
	QAction *pac;
	QAction *global;
	
	QAction *autoStartAction;
	QAction *aboutAction;
	
	QAction *minimizeAction;
	QAction *restoreAction;
    QAction *quitAction;

};
#endif // MAINWINDOW_H
