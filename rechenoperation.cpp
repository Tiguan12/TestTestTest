
#include "rechenoperation.h"
#include <QDebug>


// Globale Variablen
double SOC = 0.0; // State of Charge
double SOH = 0.0; // State of Health
double t_1 = 0.0; // Startzeit
double t_2 = 0.0; // Endzeit
double I_t1 = 0.0; // Strom zum Zeitpunkt t_1
double I_t2 = 0.0; // Strom zum Zeitpunkt t_2
double Q_N = 0.0; // Nennkapazität einer Zelle
double Q_0 = 0.0; // Ladung einer vollen Zelle
double Q_t = 0.0;
int reihe = 1, spalte = 3;



double BerechnungSOC(double t_1, double t_2, double I_t1, double I_t2) {
    //double Q_0 = t_1 * I_t1;
    double Q_0 = 500.0;  // Beispiel: Ladung einer vollen Zelle
    double Q_t = (t_2 * I_t2) - (t_1 * I_t1);
    SOC = ((Q_0 - Q_t) / Q_0) * 100;
    qDebug() << "SOC berechnet:" << SOC << "%";
    return SOC;
}


// rechenoperation.cpp
double BerechnungSOH(const QVector<QVector<double>> &tabelle, double Q_0, double Q_N) {
    if (tabelle.isEmpty()) return 0.0;

    double Q_min = tabelle[0][2];
    for (int i = 1; i < tabelle.size(); ++i) {
        if (tabelle[i][2] < Q_min) {
            Q_min = tabelle[i][2];
        }
    }

    double SOH = ((Q_0 - Q_min) / (Q_N - Q_min)) * 100.0;
    qDebug() << "SOH berechnet:" << SOH << "%";
    return SOH;
}

double find_Q_min(double** tabelle, int reihe) {
    double Q_min = tabelle[0][2]; // Setze als initial den ersten Wert der Tabelle
    for (int i = 1; i < reihe; i++) {
        if (tabelle[i][2] < Q_min) {
            Q_min = tabelle[i][2];
        }
    }
    return Q_min;
}
