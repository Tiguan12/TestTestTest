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
    //Q_0 = ladungswerte[index];
    ui->AnzeigeNspg->setValue(nennKapazitaet);

//Defaultwert für AnzahlZyklus
    nZyklen = 1;  // Zyklus explizit auf 1 setzen
    ui->Anzeige_NZyklen->setValue(nZyklen);
    logNachricht("Formierungszyklus auf 1 gesetzt.");



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

    // Werte zwischenspeichern
    QVector<double> zeile;
    zeile << (elapsed / 1000.0) << potential << current << Q;
    tabelle.append(zeile);

    berechneLadung();  // Q berechnen
    berechneQmin();    // neuen Hilfsfunktion, siehe unten

    // Jetzt sind Q, Q_0, Q_min korrekt

    ui->tableWidget->insertRow(rowCount);
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->scrollToBottom();
    ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(QString::number(rowCount + 1)));
    ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(timeString));
    ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::number(dialValue + 1)));
    ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::number(current * 1000)));
    ui->tableWidget->setItem(rowCount, 4, new QTableWidgetItem(QString::number(potential)));
    ui->tableWidget->setItem(rowCount, 5, new QTableWidgetItem(QString::number(nennKapazitaet)));
    ui->tableWidget->setItem(rowCount, 6, new QTableWidgetItem(QString::number(Q_0)));  // jetzt korrekt
    ui->tableWidget->setItem(rowCount, 7, new QTableWidgetItem(QString::number(Q)));
    ui->tableWidget->setItem(rowCount, 8, new QTableWidgetItem(QString::number(Q_min)));
    ui->tableWidget->setItem(rowCount, 9, new QTableWidgetItem(QString::number(nZyklen)));




    // //Hier werden die ausgelesenen Werte in einer Tabelle gespeichert
    // QVector<double> zeile;
    // zeile << (elapsed / 1000.0) << potential << current << Q;
    // tabelle.append(zeile);


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
        timer->start(intervall);
        messungGestartet = true;  // <- hier setzen
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
    messungGestartet = false;
    logNachricht("Messung gestoppt.");

    //*************************************************************************************
    // Vektor für Ladungswerte (Q)
    QVector<double> ladungswerte;
    ladungswerte.reserve(tabelle.size());  // Reserviert Speicherplatz für die erwartete Anzahl an Elementen

    for (int i = 0; i < tabelle.size(); i++) {
        QVector<double> zeile = tabelle[i];
        ladungswerte.append(zeile[3]);  // Spalte 3 = Ladung
    }

    // Vektoren für Indizes der Maxima und Minima
    QVector<int> maxima;
    QVector<int> minima;

    nZyklen = 1;  // Default falls pushButtonNZyklen nie gedrückt wurde

    for (int i = 1; i < ladungswerte.size() - 1; ++i) {
        if (ladungswerte[i] > ladungswerte[i - 1] && ladungswerte[i] > ladungswerte[i + 1]) {
            maxima.append(i);
        } else if (ladungswerte[i] < ladungswerte[i - 1] && ladungswerte[i] < ladungswerte[i + 1]) {
            minima.append(i);
        }
    }

    // Q_min bestimmen – basierend auf nZyklen
    if (minima.size() >= nZyklen) {
        int index = minima[nZyklen - 1];  // n-tes Minimum (oder erstes, wenn nZyklen=1)
        Q_min = ladungswerte[index];
        qDebug() << "Q_min =" << Q_min << " bei Zeit =" << tabelle[index][0];
    } else if (!minima.isEmpty()) {
        Q_min = ladungswerte[minima.first()];
        qDebug() << "Nur ein Minimum gefunden. Q_min =" << Q_min;
    } else {
        Q_min = 0.0;
        qDebug() << "Kein Minimum gefunden. Q_min = 0.0";
    }


    //*************************************************************************************

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

        double SOC = BerechnungSOC(Q_0, Q);
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

        double SOH = BerechnungSOH(Q_0, Q_min, Q_N);

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

//Berechnung der Ladung "Integration des Stroms über der Zeit"
void MainWindow::berechneLadung()
{

    for (int i = 1; i < tabelle.size(); ++i) {
        double t1 = tabelle[i - 1][0];
        double t2 = tabelle[i][0];
        double I1 = tabelle[i - 1][2];
        double I2 = tabelle[i][2];
        double dt = t2 - t1;
        double I_mittel = (I1 + I2) / 2.0;

        Q += I_mittel * dt;
    }

    qDebug() << "Ladung Q =" << Q << " C";
    logNachricht("Gesamtladung Q = " + QString::number(Q) + " C");

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
    logNachricht("Formierungszyklus auf 1 gesetzt.");

    if (nZyklen < 1) {
        logNachricht("Zyklusanzahl muss mindestens 1 sein.");
        nZyklen = 1;

    // Sicherstellen, dass es genug Daten gibt
    if (tabelle.size() < nZyklen) {
        logNachricht("Nicht genügend Daten zur Bestimmung von Q_0 und Q_N.");
        return;
    }

    // Lade Ladungswerte (Q) aus Spalte 3
    QVector<double> ladungswerte;
    for (const auto& zeile : tabelle) {
        ladungswerte.append(zeile[3]);
    }

    // Maxima suchen
    QVector<int> maxima;
    for (int i = 1; i < ladungswerte.size() - 1; ++i) {
        if (ladungswerte[i] > ladungswerte[i - 1] && ladungswerte[i] > ladungswerte[i + 1]) {
            maxima.append(i);
        }
    }

    if (maxima.size() >= nZyklen) {
        int index = maxima[nZyklen - 1];  // 1. Maximum
        Q_0 = ladungswerte[index];
        Q_N = Q_0;  // gleichsetzen

        logNachricht("Q_0 und Q_N wurden gesetzt auf: " + QString::number(Q_0));
    } else {
        logNachricht("Nicht genügend Maxima für Zyklusauswertung.");
    }
    }}

void MainWindow::berechneQmin()
{
    QVector<double> ladungswerte;
    for (const auto& zeile : tabelle)
        ladungswerte.append(zeile[3]);

    QVector<int> minima;
    for (int i = 1; i < ladungswerte.size() - 1; ++i) {
        if (ladungswerte[i] < ladungswerte[i - 1] && ladungswerte[i] < ladungswerte[i + 1])
            minima.append(i);
    }

    if (minima.size() >= nZyklen) {
        int index = minima[nZyklen - 1];
        Q_min = ladungswerte[index];
    } else if (!minima.isEmpty()) {
        Q_min = ladungswerte[minima.first()];
    } else {
        Q_min = 0.0;
    }
}
