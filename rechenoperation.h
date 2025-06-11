#ifndef RECHENOPERATION_H
#define RECHENOPERATION_H

#include <QVector>

// Globale Variablen (optional, besser in cpp belassen oder über Getter/Setter abstrahieren)
extern double SOC;   // State of Charge
extern double SOH;   // State of Health
extern double Q_N;   // Nennkapazität
extern double Q_0;   // Volle Ladung
extern double Q_t;   // Momentane Ladung
extern double Q_min; // Minimale Ladung

// Funktionsprototypen


using Tabelle = QVector<QVector<double>>;
double BerechnungSOC(double Q_0, double Q_t);
double BerechnungSOH(double Q_0, double Q_min, double Q_N);
double BerechneLadung(const Tabelle& tabelle);
double BerechneQmin(const Tabelle& tabelle, int nZyklen);
QVector<int> FindeMaxima(const QVector<double>& werte);
QVector<int> FindeMinima(const QVector<double>& werte);



#endif // RECHENOPERATION_H

