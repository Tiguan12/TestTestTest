#ifndef RECHENOPERATION_H
#define RECHENOPERATION_H

#include <QVector>


// Globale Variablen
extern double SOC;
extern double SOH;
extern double t_1;
extern double t_2;
extern double I_t1;
extern double I_t2;
extern double Q_N;
extern double Q_0;
extern double Q_t;
extern int reihe;
extern int spalte;
extern double** tabelle; // Verweise auf die Tabelle

// Funktionen deklarieren
void initialisiereTabelle(int reihe, int spalte);
double BerechnungSOC(double t_1, double t_2, double I_t1, double I_t2);
double BerechnungSOH();
double find_Q_min(double** tabelle, int reihe);
double BerechnungSOH(const QVector<QVector<double>> &tabelle, double Q_0, double Q_N);

#endif // RECHENOPERATION_H

