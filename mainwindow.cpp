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
#include <QVector>
#include <QClipboard>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , messungGestartet(false)  // <- hier hinzugefügt
{
    ui->setupUi(this);
//***********************************************

    initializeUI();

//**********************************************************

    initializeTimer();

//*********************************************************

    initializeHardware();

//*******************************************************************

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::WerteAusgabe()
{
    QElapsedTimer timerTest;
    timerTest.start();

    // Hole aktuelle Werte
    IV_getpotential(&potential);
    IV_getcurrent(&current);

    rowCount = ui->tableWidget->rowCount();
    int dialValue = ui->dial->value();
    int elapsed = zeit.elapsed();
    sekunden = elapsed / 1000;
    QTime time(0, 0);
    time = time.addSecs(sekunden);

    QString timeString = ui->radioButton_Zeitformat_1->isChecked() ?
                             QString::number(sekunden) :
                             time.toString("hh:mm:ss:zzz");



    // //Defaultwert für AnzahlZyklusif (nZyklen <= 0) {
    // if (nZyklen <= 0) {
    //     nZyklen = 0;
    //     ui->Anzeige_NZyklen->setValue(nZyklen);
    //     logNachricht("Formierungszyklus automatisch auf 1 gesetzt.");
    // }





    // // Werte zwischenspeichern
    // QVector<double> zeile;
    // zeile << (elapsed / 1000.0) << potential << current << Q_t;
    // tabelle.append(zeile);

    // Q_t= BerechneLadung(tabelle);  // Q berechnen
    // Q_min = BerechneQmin(tabelle, nZyklen);   // Erste Minimum nach der Formierung
    // Q_N = BerechneQN(tabelle, nZyklen);     // Erste Maximum nach der Formierung
    // Q_0 = BerechneQ0(tabelle, nZyklen);      // Jedes Maximum nach der Formierung


    QVector<double> zeile;
    zeile << (elapsed / 1000.0) << potential << current << 0.0;  // Q_t vorerst 0
    tabelle.append(zeile);

    // static bool ersterDurchgang = true;  // statisch, damit der Wert zwischen den Aufrufen erhalten bleibt
    // static double erstesQmin = 0.0;
    // static double erstesQN = 0.0;

    Q_t = BerechneLadung(tabelle);
    tabelle.last()[3] = Q_t;


//***********************************************************************************************
//Info:

    // Dann ruf sie so auf:
    // Maxima: findeExtrema(daten, true)
    // Minima: findeExtrema(daten, false)

    // double BerechneQ0(const Tabelle& t, int z) { return findeQzyklenwert(t, z, true); }
    // double BerechneQN(const Tabelle& t, int z) { return findeQzyklenwert(t, z, true); }
    // double BerechneQmin(const Tabelle& t, int z) { return findeQzyklenwert(t, z, false); }

//*************************************************************************************************
    Q_N = BerechneQN(tabelle, nZyklen);
    Q_0 = BerechneQ0(tabelle, nZyklen);
    //Q_min = BerechneQmin(tabelle, nZyklen);

    // Schritt 1: Extrahiere Spalte 3 (Q_t Werte) aus der Tabelle
    QVector<double> ladungswerte;
    for (const auto& zeile : tabelle) {
        ladungswerte.append(zeile[3]);
    }

    // Schritt 2: Rufe findeExtrema mit dem 1D-Vektor auf
    bool sucheMax = true; // oder false, je nachdem ob Maxima oder Minima gesucht sind
    QVector<int> extremaIndices = findeExtrema(ladungswerte, sucheMax);

    // Optional: Ausgabe der gefundenen Extrempunkte
    for (int idx : extremaIndices) {
        qDebug() << "Extremum bei Index" << idx << "mit Wert" << ladungswerte[idx];
    }






    if (!nennSpgManuellGesetzt) {
        // Wenn Button nicht geklickt wurde, Q_N aus Berechnung holen
        Q_N = BerechneQN(tabelle, nZyklen);
        ui->AnzeigeNspg->setValue(Q_N);
        logNachricht("Nennkapazität berechnet: " + QString::number(Q_N));
    } else {
        // Q_N wurde manuell gesetzt, also verwende den manuellen Wert
        logNachricht("Nennkapazität aus manuellem Wert: " + QString::number(Q_N));
    }




    // Jetzt sind Q, Q_0, Q_min korrekt

    ui->tableWidget->insertRow(rowCount);
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->scrollToBottom();
    ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(rowCount + 1)));
    ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(timeString));
    ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::number(dialValue + 1)));
    ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(current * 1000)));
    ui->tableWidget->setItem(rowCount, 4, new QTableWidgetItem(QString::number(potential)));

    //ui->tableWidget->setItem(rowCount, 5, new QTableWidgetItem(QString::number(Q_N)));
    QTableWidgetItem* itemQN = new QTableWidgetItem(QString::number(Q_N));
    itemQN->setBackground(Qt::yellow);
    itemQN->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(rowCount, 5, itemQN);

    ui->tableWidget->setItem(rowCount, 6, new QTableWidgetItem(QString::number(Q_0)));  // jetzt korrekt
    ui->tableWidget->setItem(rowCount, 7, new QTableWidgetItem(QString::number(Q_t)));
    ui->tableWidget->setItem(rowCount, 8, new QTableWidgetItem(QString::number(Q_min)));
    ui->tableWidget->setItem(rowCount, 9, new QTableWidgetItem(QString::number(nZyklen)));


    logNachricht("Wert gespeichert: " + QString::number(zeile[0]) + " s, "
                 + QString::number(zeile[1]) + " V, "
                 + QString::number(zeile[2]) + " A, "
                 + QString::number(zeile[3]) + " As");


    logNachricht("WerteAusgabe dauerte: " + QString::number(timerTest.elapsed()) + " ms");
    Q_min = BerechneQmin(tabelle, nZyklen);
    qDebug() << "📊 WerteAusgabe – Q_min =" << Q_min
             << "| Q_0 =" << Q_0
             << "| Q_N =" << Q_N
             << "| Tabelle.size =" << tabelle.size()
             << "| letzte Q_t (tabelle.last()[3]) =" << tabelle.last()[3];


}

void MainWindow::on_startButton_clicked()
{
   if (!timer->isActive()) {
            tabelle.clear(); // Testweise hier hinzufügen
            Q_N = Q_0 = Q_min = Q_t = 0;

            zeit.start();
            timer->start(intervall);
            messungGestartet = true;
        } else {
            logNachricht("Timer läuft bereits.");
        }

ersterDurchgang = true;

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
    messungGestartet = false;
    logNachricht("Messung gestoppt.");

    // //*************************************************************************************
    // //*************************************************************************************



    QVector<QVector<double>> matrix = {
        //{0.58234, -0.12345, 1.23456, 0.98765, 2.34567, -2.45678, 0.11111, 1.67890, -0.98765, 0.43210, 0.25468, 1.89123, 0.67895, -1.12345, 0.34567, 2.99876, 0.45678, 1.23456, -1.87654, 0.56789, 1.09876, 0.21098, 1.54321, -0.65432, 0.78901, 0.34567, 0.12345, 1.67890, -0.23456, 0.98765, 0.43210, 0.67834, 0.54321, 1.32109, -0.98765, 0.45678, 2.45678, 0.87654, -1.23456, 0.23456, 1.23457, 0.34568, 0.56790, -0.34567, 0.78902, 0.67895, 1.89012, 0.12346, 1.34567, 0.23457, 0.45679, 0.98766, -2.11223, 0.32109, 0.43211, 1.56791, 0.65432, -0.76543, 0.54322, 1.21098, 0.67836, 1.23458, 0.87655, 0.43212, 1.34568, -1.54321, 0.65433, 2.12345, 0.32110, 0.98767, 0.23458, 1.67891, 0.54323, -0.87654, 0.78903, 0.45680, 1.23459, 0.67837, 0.32111, 1.98765, 0.54324, 0.23459, 1.21099, 0.87656, -0.12346, 0.65434, 0.78904, 1.43210, 0.32112, 0.54325, 0.98768, 0.67838, 0.23460, -1.98765, 0.45681, 1.21000, 0.87657, 0.54326, 0.32113, 0.67839, 0.98769, 0.76543, 0.12347, 0.65435, 0.78905, 0.43213, 1.21001, 0.65436, 0.87658, 0.54327, 0.32114, 0.98770, 0.67840, 0.23461, 1.43211, 0.45682, 0.78906, -0.32112, 0.98771, 0.54328, 1.21002, 0.87659, 0.65437, 0.54329, 0.67841, 1.11111, 0.23462, 0.98772, 0.32115, 0.45683, 0.78907, 0.65438, 0.87660, 1.54322, 0.54330, 0.98773, -0.45682, 0.67842, 0.23463, 1.21003, 0.32116, 0.65439, 0.98774, 0.54331, 0.87661, 0.67843, -2.98765, 0.12389, 1.45678, 0.98765, 2.23456, 0.34567, -0.54321, 1.09876, 0.23456, -1.34567, 0.98765, 1.23456, 0.45678, 0.78901, 1.56789, -0.23456, 0.34567, 1.67890, 0.45678, 0.12345, 0.99999, -2.22222, 0.11112, 0.33333, 2.11111, 0.22223, 1.11112, -1.11111, 0.44444, 0.55555, 0.66666, -0.99999, 0.10101, 0.20202, 1.30303, 0.40404, 0.50505, -1.20202, 0.60606, 0.70707, 1.80808, 0.90909, -0.90909, 0.11223, 0.22334, 1.33445, 0.556677, 0.66778, 0.77889, 1.88990, 0.99001, 0.11113, 0.22224, 0.33335, 0.44446, 0.55556, 0.66667, 0.77778, 0.88889, -1.99999, 0.99998, 0.88888, 0.77777, 0.66665, 0.55554, 0.44443, 0.33332, 1.22222, 0.99997, 0.88887, 0.77776, 0.66664, 0.55553, -0.33331, 0.44442, 1.11110, 0.88886, 0.77775, 0.66663, 0.55552, 0.44441, 0.33330},

        {0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 0.9, 0.8, 0.7},
        {0.3, 0.2, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8},
        {0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.2, 0.3, 0.4},
        {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 0.8},
        {0.5, 0.8, 0.7, 0.8, 0.9, 1.0, 0.9, 0.8, 0.7, 0.6},
        {0.4, 0.3, 0.2, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7},
        {0.2, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9},
        {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0},
        {0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 0.9},
        {0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 0.9, 0.8},
        {0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 0.9, 0.8, 0.7},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }
    };

    QVector<int> Quimbi= findeExtrema(matrix[4], false);

    qDebug() <<"Das Maximum ist:" << Quimbi;
}


// void MainWindow::on_delButton_clicked()
// {
//     ui->tableWidget->clearContents();
//     ui->tableWidget->setRowCount(0);
//     tabelle.clear();
//     logNachricht("Tabelle und Daten gelöscht.");
//     Q_N = BerechneQN(tabelle, nZyklen);  // Q_N zurücksetzen

// }

void MainWindow::on_delButton_clicked()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    tabelle.clear();
    Q_N = 0;
    Q_0 = 0;
    Q_min = 0;
    Q_t = 0;
    nennSpgManuellGesetzt = false;
    logNachricht("Tabelle und Daten gelöscht. Q_N zurückgesetzt.");
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

// void MainWindow::on_buttonNennSpg_clicked()
// {
//     Q_N = ui->doubleSpinBoxNennSpg->value();
//     ui->AnzeigeNspg->setValue(Q_N);
//     logNachricht("Nennkapazität gesetzt auf: " + QString::number(Q_N));
// }


// Slot bei Buttonklick
void MainWindow::on_buttonNennSpg_clicked()
{
    Q_N = ui->doubleSpinBoxNennSpg->value();
    ui->AnzeigeNspg->setValue(Q_N);
    logNachricht("Nennkapazität manuell gesetzt auf: " + QString::number(Q_N));
    nennSpgManuellGesetzt = true;
}


void MainWindow::on_SocButton_clicked()
{
    if (tabelle.size() >= 2) {

        double SOC = BerechneSOC(Q_0, Q_t);
        ;

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

        double SOH = BerechneSOH(Q_0, Q_min, Q_N);

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
    ui->lcdNumber->display(value+1);
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
    intervall = value * 1000;

    if (messungGestartet) {
        timer->stop();
        timer->setInterval(intervall);
        timer->start();
        logNachricht("Abtastintervall auf " + QString::number(intervall / 1000) + " Sekunden gesetzt.");
    } else {
        logNachricht("Abtastintervall geändert, aber Messung ist noch nicht gestartet.");
    }
}



void MainWindow::on_copyButton_clicked()
{
    QString copiedText;

    QTableWidget* table = ui->tableWidget;

    int rows = table->rowCount();
    int cols = table->columnCount();

    for (int row = 0; row < rows; ++row) {
        QStringList rowItems;
        for (int col = 0; col < cols; ++col) {
            QTableWidgetItem* item = table->item(row, col);
            if (item) {
                rowItems << item->text();
            } else {
                rowItems << "";
            }
        }
        copiedText += rowItems.join('\t');
        copiedText += '\n';
    }

    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(copiedText);

    logNachricht("Tabelleninhalt wurde in die Zwischenablage kopiert.");
}

void MainWindow::on_radioButton_Zeitformat_1_clicked()
{

}

void MainWindow::on_radioButton_Zeitformat_2_clicked()
{

}



void MainWindow::on_pushButtonNZyklen_clicked()
{
    nZyklen = ui->doubleSpinBox_NZyklen->value();
    ui->Anzeige_NZyklen->setValue(nZyklen);
    logNachricht("Formierungszyklus auf " + QString::number(nZyklen) + " gesetzt.");

    // // Funktion zur Bestimmung von Q_0 und Q_N aufrufen
    // bestimmeQ_0_Q_N(tabelle, nZyklen, Q_0, Q_N, Q_N);
}

void MainWindow::initializeUI()
{

    // Manuelle Verbindung von Signal und Slot
    connect(ui->pushButtonNZyklen, &QPushButton::clicked, this, &MainWindow::on_pushButtonNZyklen_clicked);
    connect(ui->buttonNennSpg, &QPushButton::clicked, this, &MainWindow::on_buttonNennSpg_clicked);
    ui->tableWidget->verticalHeader()->setVisible(false);
    this->showMaximized();
}

void MainWindow::initializeTimer()
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::WerteAusgabe);
}

void MainWindow::initializeHardware()
{
    int result = IV_open();
    if (result == 0) {
        IV_getpotential(&potential);
        IV_getcurrent(&current);
        logNachricht("Der Spannungswert ist: " + QString::number(potential));
        logNachricht("Der Stromwert ist: " + QString::number(current));
    }
}







void MainWindow::on_QNManuellButton_clicked()
{
    if (ui->buttonNennSpg->isEnabled()) {
        // Button ist momentan aktiviert – also deaktivieren
        ui->buttonNennSpg->setEnabled(false);
    } else {
        // Button ist momentan deaktiviert – also aktivieren
        ui->buttonNennSpg->setEnabled(true);
    }

}






