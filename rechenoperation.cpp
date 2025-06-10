
#include "rechenoperation.h"
#include <QDebug>


// Globale Variablen
double SOC = 0.0; // State of Charge
double SOH = 0.0; // State of Health
double Q_N = 0.0; // Nennkapazität einer Zelle
double Q_0 = 0.0; // Ladung einer vollen Zelle
double Q_t = 0.0; // Ladung zum Zeitpunkt "t"
double Q_min = 0.0;// Ladung einer leeren Zelle




double BerechnungSOC(double Q_0, double Q_t) {

    SOC = (Q_0 - Q_t) / Q_0 * 100;
    qDebug() << "SOC berechnet:" << SOC << "%";
    return SOC;
}


// rechenoperation.cpp
double BerechnungSOH(double Q_0, double Q_min, double Q_N){


    double SOH = ((Q_0 - Q_min) / (Q_N - Q_min)) * 100.0;
    qDebug() << "SOH berechnet:" << SOH << "%";
    return SOH;
}

