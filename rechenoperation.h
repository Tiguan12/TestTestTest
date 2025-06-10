#ifndef RECHENOPERATION_H
#define RECHENOPERATION_H

#include <QVector>


// Globale Variablen
extern double SOC;
extern double SOH;
extern double Q_N;
extern double Q_0;
extern double Q_t;


// Funktionen deklarieren

double BerechnungSOC(double Q_0, double Q_t);
double BerechnungSOH(double Q_0, double Q_min, double Q_N);



#endif // RECHENOPERATION_H

