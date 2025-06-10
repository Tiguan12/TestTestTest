#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector>
#include <QTableWidgetItem>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    private slots:
    void WerteAusgabe();
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_delButton_clicked();
    void on_Starten_clicked();
    void on_Beenden_clicked();
    void on_buttonNennSpg_clicked();
    void on_SocButton_clicked();
    void berechneLadung();
    void berechneQmin();



    void on_BatterieAnzeige_valueChanged(int value);
    void on_BatterieAnzeigeSOH_valueChanged(int value);
    void on_SohButton_clicked();  // ✔️ korrekt



    void logNachricht(const QString &nachricht);
    void on_dial_valueChanged(int value);
    void on_checkBox_clicked();
    void on_dial_Zeitintervall_valueChanged(int value);
    void on_radioButton_Zeitformat_1_clicked();
    void on_radioButton_Zeitformat_2_clicked();
    void on_copyButton_clicked();    
    void on_pushButtonNZyklen_clicked();

private:
    Ui::MainWindow *ui;
    QTimer* timer;
    QElapsedTimer zeit;

    double potential = 0.0;
    double current = 0.0;
    double nennKapazitaet = 2.0;
    int sekunden;
    int rowCount;
    bool messungGestartet;
    int intervall = 1000;
    QTableWidgetItem* timeItem; // Zeitempfänger

    QVector<QVector<double>> tabelle;  // Speicher für [Zeit, Spannung, Strom]
    QString currentTimeFormat = "hh:mm:ss"; //
    double Q = 0.0;
    double Q_0 = 0.0;
    double Q_N = 0.0;
    double Q_min = 0.0;
    double nZyklen = 1.0;

};

#endif // MAINWINDOW_H
