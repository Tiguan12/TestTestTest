
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


// Integration der Stromstärke über der Zeit (Trapezregel)
double BerechneLadung(const Tabelle& tabelle)
{
    double Q = 0.0;

    for (int i = 1; i < tabelle.size(); ++i) {
        double t1 = tabelle[i - 1][0];
        double t2 = tabelle[i][0];
        double I1 = tabelle[i - 1][2];
        double I2 = tabelle[i][2];
        double dt = t2 - t1;
        double I_mittel = (I1 + I2) / 2.0;
        Q += I_mittel * dt;
    }

    return Q;
}

// Minimum Q für nZyklen bestimmen
double BerechneQmin(const Tabelle& tabelle, int nZyklen)
{
    QVector<double> ladungswerte;
    for (const auto& zeile : tabelle)
        ladungswerte.append(zeile[3]);

    QVector<int> minima = FindeMinima(ladungswerte);

    if (minima.size() >= nZyklen) {
        return ladungswerte[minima[nZyklen - 1]];
    } else if (!minima.isEmpty()) {
        return ladungswerte[minima.first()];
    } else {
        return 0.0;
    }
}

// Maxima-Finder (z. B. für Q_0)
QVector<int> FindeMaxima(const QVector<double>& werte)
{
    QVector<int> maxima;
    for (int i = 1; i < werte.size() - 1; ++i) {
        if (werte[i] > werte[i - 1] && werte[i] > werte[i + 1]) {
            maxima.append(i);
        }
    }
    return maxima;
}

// Minima-Finder (z. B. für Q_min)
QVector<int> FindeMinima(const QVector<double>& werte)
{
    QVector<int> minima;
    for (int i = 1; i < werte.size() - 1; ++i) {
        if (werte[i] < werte[i - 1] && werte[i] < werte[i + 1]) {
            minima.append(i);
        }
    }
    return minima;
}

