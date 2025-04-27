#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "IVIUM_remdriver64.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    public slots:
    void WerteAusgabe();

    private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();

    void on_delButton_clicked();

    private:
    Ui::MainWindow *ui;
    QTimer *timer;
};
#endif // MAINWINDOW_H
