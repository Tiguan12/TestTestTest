#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QElapsedTimer>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "Ivium_remdriver64.h"
#include "rechenoperation.h"
#include <QDebug>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Automatisch generierte Zeilennummern ausblenden
    ui->tableWidget->verticalHeader()->setVisible(false);

    this->showMaximized();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(WerteAusgabe()));

    int result = IV_open();  // Verbindung öffnen

    if (result == 0) {
        IV_getpotential(&potential);
        IV_getcurrent(&current);

        logNachricht("Der Spannungswert ist: " + QString::number(potential));
        logNachricht("Der Stromwert ist: " + QString::number(current));
    }

    nennKapazitaet = 2.0;
    ui->AnzeigeNspg->setValue(nennKapazitaet);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::WerteAusgabe()
{
    QElapsedTimer timerTest;
    timerTest.start();

    rowCount = ui->tableWidget->rowCount();
    int dialValue = ui->dial->value();

    int elapsed = zeit.elapsed();
    sekunden = elapsed / 1000;


    // Umwandlung der verstrichenen Zeit in QTime
    QTime time(0, 0);
    time = time.addSecs(sekunden);

    timeItem = new QTableWidgetItem();

    //*****************************************************************************************************

    QString timeString;

    IV_getpotential(&potential);
    IV_getcurrent(&current);


    double kapazitaet = current * sekunden;

    if (ui->radioButton_Zeitformat_1->isChecked()) {
        timeString = QString::number(sekunden);
    }
    else {
        timeString = time.toString("hh:mm:ss:zzz");
    }



    ui->tableWidget->insertRow(rowCount);
    ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(rowCount + 1)));
    ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(timeString));
    ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::number(dialValue)));
    ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(potential)));
    ui->tableWidget->setItem(rowCount, 4, new QTableWidgetItem(QString::number(current)));
    ui->tableWidget->setItem(rowCount, 5, new QTableWidgetItem(QString::number(nennKapazitaet)));
    ui->tableWidget->setItem(rowCount, 6, new QTableWidgetItem(QString::number(Q_0)));

    QVector<double> zeile;
    zeile << (elapsed / 1000.0) << potential << current << kapazitaet;
    tabelle.append(zeile);

    logNachricht("Wert gespeichert: " + QString::number(zeile[0]) + " s, "
                 + QString::number(zeile[1]) + " V, "
                 + QString::number(zeile[2]) + " A, "
                 + QString::number(zeile[3]) + " As");

    logNachricht("WerteAusgabe dauerte: " + QString::number(timerTest.elapsed()) + " ms");
}

void MainWindow::on_startButton_clicked()
{
    if (!timer->isActive()) {
        zeit.start();
        timer->start(1000);
    } else {
        logNachricht("Timer läuft bereits.");
    }

    // Öffnen der Verbindung zur IVIUM-Gerät
    if (IV_open() == 0) {  // 0 bedeutet Erfolg, je nach Rückgabewert von IV_open
        qDebug() << "IVIUM-Gerät erfolgreich geöffnet.";

        // Wert für die Spannung (Potential) definieren
        double potential = 0.0;

        // Abrufen des Potentials vom IVIUM-Gerät
        IV_getpotential(&potential);  // Übergebe die Adresse des Variablen 'potential'

        // Ausgabe des Potentials
        qDebug() << "Aktuelles Potential:" << potential;

        // Gerät schließen
        IV_close();
    } else {
        qDebug() << "Fehler beim Öffnen des IVIUM-Geräts.";
    }





}


void MainWindow::on_stopButton_clicked()
{
    timer->stop();
    logNachricht("Messung gestoppt.");
}

void MainWindow::on_delButton_clicked()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    tabelle.clear();
    logNachricht("Tabelle und Daten gelöscht.");
}

void MainWindow::on_Starten_clicked()
{
    QProcess::startDetached("C:/IviumStat/IviumSoft.exe");
    logNachricht("IviumSoft.exe gestartet.");
}

void MainWindow::on_Beenden_clicked()
{
    QProcess::execute("taskkill", QStringList() << "/f" << "/im" << "IviumSoft.exe");
    logNachricht("IviumSoft.exe beendet.");
}

void MainWindow::on_buttonNennSpg_clicked()
{
    nennKapazitaet = ui->doubleSpinBoxNennSpg->value();
    ui->AnzeigeNspg->setValue(nennKapazitaet);
    logNachricht("Nennkapazität gesetzt auf: " + QString::number(nennKapazitaet));
}

void MainWindow::on_SocButton_clicked()
{
    if (tabelle.size() >= 2) {
        double t_1 = tabelle[0][0];
        double I_t1 = tabelle[0][2];
        double t_2 = tabelle.last()[0];
        double I_t2 = tabelle.last()[2];

        double SOC = BerechnungSOC(t_1, t_2, I_t1, I_t2);

        logNachricht("SOC: " + QString::number(SOC) + " %");

        ui->BatterieAnzeige->setValue(static_cast<int>(SOC));
        ui->ProzentAnzeigeSOC->setValue(static_cast<int>(SOC));
    } else {
        logNachricht("SOC konnte nicht berechnet werden – nicht genügend Daten.");
    }
}

void MainWindow::on_SohButton_clicked()
{
    if (tabelle.size() >= 2) {
        double Q_0 = tabelle[0][3];
        on_pushButtonQ_0_clicked(); // Q_0 aus UI holen
        Q_0 = ui->doubleSpinBox_Q_0->value();

        double Q_N = nennKapazitaet;
        double SOH = BerechnungSOH(tabelle, Q_0, Q_N);

        logNachricht("SOH: " + QString::number(SOH) + " %");

        ui->BatterieAnzeigeSOH->setValue(static_cast<int>(SOH));
        ui->ProzentAnzeigeSOH->setValue(static_cast<int>(SOH));
    } else {
        logNachricht("SOH konnte nicht berechnet werden – nicht genügend Daten.");
    }
}

void MainWindow::on_BatterieAnzeige_valueChanged(int value)
{
    QString style;
    if (value <= 20) {
        style = "QProgressBar::chunk { background-color: red; }";
    } else if (value <= 50) {
        style = "QProgressBar::chunk { background-color: yellow; }";
    } else {
        style = "QProgressBar::chunk { background-color: green; }";
    }
    ui->BatterieAnzeige->setStyleSheet(style);
}

void MainWindow::on_BatterieAnzeigeSOH_valueChanged(int value)
{
    QString style;
    if (value <= 70) {
        style = "QProgressBar::chunk { background-color: red; }";
    } else if (value <= 80) {
        style = "QProgressBar::chunk { background-color: yellow; }";
    } else {
        style = "QProgressBar::chunk { background-color: blue; }";
    }
    ui->BatterieAnzeigeSOH->setStyleSheet(style);
}

void MainWindow::on_pushButtonQ_0_clicked()
{
    Q_0 = ui->doubleSpinBox_Q_0->value();
    ui->Anzeige_Q_0->setValue(Q_0);
    logNachricht("Q_0 gesetzt auf: " + QString::number(Q_0));
}

void MainWindow::logNachricht(const QString &nachricht)
{
    ui->textAusgabe->append(nachricht);

    QFile file("log.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
            << " - " << nachricht << "\n";
        file.close();
    }
}

void MainWindow::on_dial_valueChanged(int value)
{
    // Mapping des Dial-Werts (0-20) auf einen Kanal (1-20)
    long channel = value + 1;
    IV_SelectChannel(&channel);
}

void MainWindow::on_checkBox_clicked()
{
    if (ui->checkBox->isChecked()) {
        // Wenn die Checkbox aktiviert ist, den Dial aktivieren
        ui->dial->setEnabled(true);

        // Optional: Setze den Dial auf den aktuellen Wert, falls gewünscht
        int currentDialValue = ui->dial->value();
        long channel = static_cast<long>(currentDialValue);  // Konvertiere int zu long
        IV_SelectChannel(&channel);  // Setze den Kanal auf den aktuellen Dial-Wert
    } else {
        // Wenn die Checkbox deaktiviert ist, den Dial deaktivieren
        ui->dial->setEnabled(false);

        // Optional: Setze den Dial auf einen Standardwert (z.B. 0)
        ui->dial->setValue(0);

        // Optional: Setze den Kanal auf einen inaktiven Wert, z.B. 0 oder -1
        long disabledChannel = 0;  // 0 als Platzhalter für "kein Kanal ausgewählt"
        IV_SelectChannel(&disabledChannel);  // Deaktiviere den Kanal
    }
}

void MainWindow::on_dial_Zeitintervall_valueChanged(int value)
{
    int intervall = value * 1000;
    timer->stop();
    timer->setInterval(intervall);
    timer->start();
    logNachricht("Abtastintervall auf " + QString::number(intervall / 1000) + " Sekunden gesetzt.");
}



void MainWindow::on_radioButton_Zeitformat_1_clicked()
{

}

void MainWindow::on_radioButton_Zeitformat_2_clicked()
{

}
