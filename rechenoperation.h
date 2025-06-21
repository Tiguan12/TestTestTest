#ifndef RECHENOPERATION_H
#define RECHENOPERATION_H

#include <QVector>
//#include "rechenoperation.h"  // z. B. Tabellen-Typ inkl. Qt-Klassen
#include <QMainWindow>

double BerechneSOC(double Q0, double Qt);
double BerechneSOH(double Q0, double Qmin, double QN);
double BerechneQmin2D(const QVector<QVector<double>>& tabelle, int spalte, int nZyklen);
//QVector<double> BerechneQmin2D(const QVector<QVector<double>>& tabelle, int spalte, int nZyklen);

//QVector<int> findeExtrema(const QVector<QVector<double>>& tabelle, bool sucheMax);
QVector<QVector<double>> findeExtrema1D(const QVector<QVector<double>>& tabelle, bool sucheMax);
QVector<int> findeExtremaTable(const QVector<QVector<double>>& tabelle, int spalte, bool sucheMax);

//double findeQzyklenwert(const Tabelle& tabelle, int nZyklen, bool sucheMax);
using Tabelle = QVector<QVector<double>>;
double BerechneQ0(const Tabelle& tabelle, int nZyklen);
double BerechneQN(const Tabelle& tabelle, int nZyklen);
//double BerechneQmin(const Tabelle& tabelle, int nZyklen);
double findeQzyklenwert(const QVector<double>& daten, int nZyklen, bool sucheMax);
void logNachricht(const QString& nachricht);



double BerechneQmin(const QVector<double>& spalteQt, int nZyklen);


#endif // RECHENOPERATION_H





















// #ifndef RECHENOPERATION_H
// #define RECHENOPERATION_H


// #include <QVector>

// #include <QMainWindow>

// // Globale Variablen (optional, besser in cpp belassen oder über Getter/Setter abstrahieren)
// extern double SOC;   // State of Charge
// extern double SOH;   // State of Health
// extern double Q_N;   // Nennkapazität
// extern double Q_0;   // Volle Ladung
// extern double Q_t;   // Momentane Ladung
// extern double Q_min; // Minimale Ladung


// // Funktionsprototypen
 //using Tabelle = QVector<QVector<double>>;

// double BerechnungSOC(double Q_0, double Q_t);
// double BerechnungSOH(double Q_0, double Q_min, double Q_N);
 double BerechneLadung(const Tabelle& tabelle);
// QVector<int> FindeMaxima(const QVector<double>& tabelle);
// QVector<int> FindeMinima(const QVector<double>& tabelle);
 QVector<double> geglaetteteWerte();
 QVector<double> GlaetteDaten(const QVector<double>& daten, int fensterGroesse);

// double BerechneQN(const Tabelle& tabelle, int nZyklen);
// double BerechneQmin(const Tabelle& tabelle, int nZyklen);
// double BerechneQ0(const Tabelle& tabelle, int nZyklen);


// #endif // RECHENOPERATION_H
