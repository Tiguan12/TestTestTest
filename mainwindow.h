#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QVector>

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
    //void on_progressBar_valueChanged(int value);

    void on_BatterieAnzeige_valueChanged(int value);
    void on_BatterieAnzeigeSOH_valueChanged(int value);
    void on_SohButton_clicked();  // ✔️ korrekt


    //void on_buttonNennSpg_3_clicked();

    void on_pushButtonQ_0_clicked();
    void logNachricht(const QString &nachricht);


    private:
    Ui::MainWindow *ui;
    QTimer* timer;
    QElapsedTimer zeit;

    double potential = 0.0;
    double current = 0.0;
    double nennKapazitaet = 2.0;

    QVector<QVector<double>> tabelle;  // Speicher für [Zeit, Spannung, Strom]
};

#endif // MAINWINDOW_H
