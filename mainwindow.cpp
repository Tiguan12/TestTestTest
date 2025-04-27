#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QTableWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);

  // Verbindung nur einmal herstellen
   connect(timer, SIGNAL(timeout()),this,SLOT(WerteAusgabe()));


// Überschrift der Spalten: Neue Spalte 1; Neue Spalte 2...Neue Spalte 10
//   QStringList spaltenNamen;
//   for (int i = 0; i < 10; ++i) {
//       spaltenNamen << QString("Neue Spalte %1").arg(i + 1);
//   }
//   ui->tableWidget->setColumnCount(10);
//   ui->tableWidget->setHorizontalHeaderLabels(spaltenNamen);


   // Richtige Überschriften
   QStringList spaltenNamen;
   spaltenNamen << "Spannung" << "Strom" << "Leistung" << "Frequenz" << "Temperatur"
                << "Zeit" << "XXX" << "YYY" << "ZZZ" << "ABC";  // 10 Spaltennamen

   ui->tableWidget->setColumnCount(spaltenNamen.size());
   ui->tableWidget->setHorizontalHeaderLabels(spaltenNamen);
}




MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::WerteAusgabe()
{
    qDebug()<< "Hello World";

    int rowCount = 0;


    ui->tableWidget->insertRow(rowCount);

    ui->tableWidget->setItem(rowCount, 0, new QTableWidgetItem(rowCount));
    ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(QString("Eintrag 1")));
    ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString("Eintrag 1")));
    ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString("Eintrag 1")));
    ui->tableWidget->setItem(rowCount, 4, new QTableWidgetItem(QString("Eintrag 1")));
    ui->tableWidget->setItem(rowCount, 5, new QTableWidgetItem(QString("Eintrag 1")));
    ui->tableWidget->setItem(rowCount, 6, new QTableWidgetItem(QString("Eintrag 1")));
    ui->tableWidget->setItem(rowCount, 7, new QTableWidgetItem(QString("Eintrag 1")));
    ui->tableWidget->setItem(rowCount, 8, new QTableWidgetItem(QString("Eintrag 1")));
    ui->tableWidget->setItem(rowCount, 9, new QTableWidgetItem(QString("Eintrag 1")));


    rowCount++;

}


void MainWindow::on_startButton_clicked()
{

    // Timer starten, aber nicht erneut verbinden
    if (!timer->isActive()) {
        timer->start(1000);
    }
}


void MainWindow::on_stopButton_clicked()
{
    timer->stop();
}


void MainWindow::on_delButton_clicked()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
  }

