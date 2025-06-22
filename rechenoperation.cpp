#include "rechenoperation.h"
#include <QDebug>
#include <limits>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <algorithm>

// ------------- 1D-Extrema-Suche ----------------
QVector<int> findeExtrema1D(const QVector<double>& daten, bool sucheMax) {
    QVector<int> indices;
    int n = daten.size();
    if (n < 3) return indices;
    for (int i = 1; i + 1 < n; ++i) {
        double prev = daten[i - 1];
        double curr = daten[i];
        double next = daten[i + 1];
        if (sucheMax ? (curr > prev && curr > next)
                     : (curr < prev && curr < next)) {
            indices.append(i);
        }
    }
    return indices;
}

// ------------- 2D-Extrema-Suche ----------------
QVector<int> findeExtremaTable(const QVector<QVector<double>>& tabelle, int spalte, bool sucheMax) {
    QVector<int> indices;
    int n = tabelle.size();
    if (n < 3) return indices;
    for (int i = 1; i + 1 < n; ++i) {
        double prev = tabelle[i - 1][spalte];
        double curr = tabelle[i][spalte];
        double next = tabelle[i + 1][spalte];
        if (sucheMax ? (curr > prev && curr > next)
                     : (curr < prev && curr < next)) {
            indices.append(i);
        }
    }
    return indices;
}

// ---------- Integration der Stromstärke ----------
double BerechneLadung(const Tabelle& tabelle) {
    double Q = 0.0;
    for (int i = 1; i < tabelle.size(); ++i) {
        double t1 = tabelle[i - 1][0];
        double t2 = tabelle[i][0];
        double I1 = tabelle[i - 1][2];
        double I2 = tabelle[i][2];
        double dt = t2 - t1;
        Q += (I1 + I2) / 2.0 * dt;
    }
    qDebug() << "Ladung Q =" << Q << "C";
    return Q;
}

// ---------- BerechneQmin nutzt eine 1D-Extrema-Suche ----------
// ------------------------------------------------
// Wrapper: Bekommt Spalte aus Tabelle, bestimmt erstes Minimum
// ------------------------------------------------
// ------------------------------------------------
// 2D-Wrapper: Bestimmt das N-te Minimum in einer Spalte
// ------------------------------------------------
double BerechneQmin2D(const QVector<QVector<double>>& tabelle, int spalte, int nZyklen)
{
    // 1) Spalte extrahieren
    QVector<double> spalteDaten;
    spalteDaten.reserve(tabelle.size());
    for (const auto& zeile : tabelle)
        spalteDaten.append(zeile[spalte]);

    // 2) Indizes der Minima finden (sucheMax=false)
    QVector<int> minIndices = findeExtremaTable(tabelle, spalte, false);

    // 3) Überprüfen, ob nZyklen im gültigen Bereich liegt
    if (nZyklen < 0 || nZyklen >= minIndices.size())
        return std::numeric_limits<double>::quiet_NaN();

    // 4) Alle Minima ab dem gewünschten Zyklus extrahieren
    QVector<double> result;
    for (int i = nZyklen; i < minIndices.size(); ++i)
    {
        int row = minIndices[i];
        result.append(spalteDaten[row]);
    }

    // Rückgabe des letzten Minimums oder NaN, wenn keine gefunden wurden
    return result.isEmpty() ? std::numeric_limits<double>::quiet_NaN() : result.last();
}


double BerechneQ_0(const QVector<QVector<double>>& tabelle, int spalte, int nZyklen)
{
    // 1) Spalte extrahieren
    QVector<double> spalteDaten;
    spalteDaten.reserve(tabelle.size());
    for (const auto& zeile : tabelle)
        spalteDaten.append(zeile[spalte]);

    // 2) Indizes der Maxima finden (sucheMax=true)
    QVector<int> maxIndices = findeExtremaTable(tabelle, spalte, true);

    // 3) Überprüfen, ob nZyklen im gültigen Bereich liegt
    if (nZyklen < 0 || nZyklen >= maxIndices.size())
        return std::numeric_limits<double>::quiet_NaN(); // NaN zurückgeben, wenn ungültig

    // 4) Alle Maxima ab dem gewünschten Zyklus extrahieren
    QVector<double> result;
    for (int i = nZyklen; i < maxIndices.size(); ++i)
    {
        int row = maxIndices[i];
        result.append(spalteDaten[row]);
    }

    // 5) Das letzte Maximum zurückgeben
    return result.isEmpty() ? std::numeric_limits<double>::quiet_NaN() : result.last();
}

double BerechneQ_N(const QVector<QVector<double>>& tabelle, int spalte, int nZyklen)
{
    // 1) Spalte extrahieren
    QVector<double> spalteDaten;
    spalteDaten.reserve(tabelle.size());
    for (const auto& zeile : tabelle)
        spalteDaten.append(zeile[spalte]);

    // 2) Indizes der Maxima finden (sucheMax = true)
    QVector<int> maxIndices = findeExtremaTable(tabelle, spalte, true);

    // 3) Gültigkeit prüfen
    if (nZyklen < 0 || nZyklen >= maxIndices.size())
        return std::numeric_limits<double>::quiet_NaN();

    // 4) Genau **das erste** Maximum ab nZyklen zurückgeben
    int row = maxIndices[nZyklen];
    return spalteDaten[row];
}


// ---------- SOC / SOH / Glätten ----------
double BerechneSOC(double Q0, double Qt) {
    if (Q0 == 0.0) {
        logNachricht("Q0 ist 0 – SOC nicht berechenbar.");
        return 0.0;
    }
    double soc = (Q0 - Qt) / Q0 * 100.0;
    logNachricht("SOC berechnet: " + QString::number(soc) + "%");
    return soc;
}

double BerechneSOH(double Q0, double Qmin, double QN) {
    if (QN == Qmin) {
        logNachricht("QN == Qmin – SOH nicht berechenbar.");
        return 0.0;
    }
    double soh = (Q0 - Qmin) / (QN - Qmin) * 100.0;
    logNachricht("SOH berechnet: " + QString::number(soh) + "%");
    return soh;
}

QVector<double> GlaetteDaten(const QVector<double>& daten, int fensterGroesse) {
    QVector<double> geglaettet;
    int halbF = fensterGroesse / 2;
    for (int i = 0; i < daten.size(); ++i) {
        double summe = 0.0;
        int count = 0;
        for (int j = -halbF; j <= halbF; ++j) {
            int idx = i + j;
            if (idx >= 0 && idx < daten.size()) {
                summe += daten[idx];
                ++count;
            }
        }
        geglaettet.append(summe / count);
    }
    return geglaettet;
}

// ---------- Log-Funktion ----------
void logNachricht(const QString &nachricht) {
    QFile file("log.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
            << " - " << nachricht << "\n";
    }
}
