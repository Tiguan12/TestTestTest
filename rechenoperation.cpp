#include "rechenoperation.h"

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

        qDebug() << "Ladung Q =" << Q << " C";
        //logNachricht("Gesamtladung Q = " + QString::number(Q) + " C");

    return Q;
}

// Extrema (Maxima/Minima) finden, je nach Suche
// QVector<int> findeExtrema(const QVector<double>& daten, bool sucheMax){
//     QVector<int> extrempunkte;

//     int idx;

//     for (int i = 1; i + 1 < daten.size(); ++i) {
//         if (sucheMax) {
//             if (daten[i] > daten[i - 1] && daten[i] > daten[i + 1]) {
//                 qDebug() << "🔍 MAX bei i=" << i << ": " << daten[i-1] << daten[i] << daten[i+1];
//                 extrempunkte.append(i);
//             }
//         } else {
//             if (daten[i] < daten[i - 1] && daten[i] < daten[i + 1]) {
//                 qDebug() << "🔍 MIN bei i=" << i << ": " << daten[i-1] << daten[i] << daten[i+1];
//                 extrempunkte.append(i);
//             }
//         }
//     }
//     qDebug() << "→ alle gefundenen Extrema-Indizes:" << extrempunkte;
//    // return extrempunkte;
//     return daten;
// }

QVector<int> findeExtrema(const QVector<double>& daten, bool sucheMax){
    QVector<int> extrempunkte;

    for (int i = 1; i + 1 < daten.size(); ++i) {
        if (sucheMax) {
            if (daten[i] > daten[i - 1] && daten[i] > daten[i + 1]) {
                qDebug() << "🔍 MAX bei i=" << i << ": " << daten[i-1] << daten[i] << daten[i+1];
                extrempunkte.append(i);
            }
        } else {
            if (daten[i] < daten[i - 1] && daten[i] < daten[i + 1]) {
                qDebug() << "🔍 MIN bei i=" << i << ": " << daten[i-1] << daten[i] << daten[i+1];
                extrempunkte.append(i);
            }
        }
    }
    qDebug() << "→ alle gefundenen Extrema-Indizes:" << extrempunkte;
    return extrempunkte;  // Korrekt: Rückgabe der Indizes
}





// Gemeinsame Hilfsfunktion für Q0, QN und Qmin
double findeQzyklenwert(const Tabelle& tabelle,int nZyklen,bool sucheMax) {
    QVector<double> werte;
    for (const auto& zeile : tabelle)
        werte.append(zeile[3]);

    QVector<double> geglaettet = GlaetteDaten(werte, 5);
    QVector<int> ext = findeExtrema(geglaettet, sucheMax);


    if (ext.isEmpty()) {
        logNachricht("❗ Kein Extrempunkt gefunden.");
        return 0.0;
    }

    int idx;
    if (nZyklen <= 0) {
        idx = 0;
    } else {
        if (nZyklen <= ext.size()) {
            idx = ext[nZyklen - 1];
        } else {
            idx = ext.last();
        }
    }

    return werte[idx];

    qDebug() << "→ findeQzyklenwert – ext.indizes:" << ext;
    if (!ext.isEmpty()) {
        qDebug() << "→ Ausgewählter idx=" << idx << ", werte[idx]=" << werte[idx];
    }


    qDebug() << "findeQzyklenwert: alle Extrema-Indizes =" << ext;
    qDebug() << "findeQzyklenwert: verwendeter idx =" << idx
             << ", werte[idx]=" << werte[idx];

}

double BerechneQ0(const Tabelle& tabelle, int nZyklen) {
    return findeQzyklenwert(tabelle, nZyklen, true); // Maxima
}

double BerechneQN(const Tabelle& tabelle, int nZyklen) {
    return findeQzyklenwert(tabelle, nZyklen, true); // Maxima
}

double BerechneQmin(const Tabelle& tabelle, int nZyklen) {
    return findeQzyklenwert(tabelle, nZyklen, false); // Minima
}

// SOC und SOH, aufgeräumt ohne ternäre Operatoren
double BerechneSOC(double Q0, double Qt) {
    if (Q0 == 0.0) {
        logNachricht("❗ Q0 ist 0 – SOC nicht berechenbar.");
        return 0.0;
    } else {
        double soc = (Q0 - Qt) / Q0 * 100.0;
        logNachricht("SOC berechnet: " + QString::number(soc) + "%");
        return soc;
    }
}

double BerechneSOH(double Q0, double Qmin, double QN) {
    if (QN == Qmin) {
        logNachricht("❗ QN und Qmin gleich – SOH nicht berechenbar.");
        return 0.0;
    } else {
        double soh = (Q0 - Qmin) / (QN - Qmin) * 100.0;
        logNachricht("SOH berechnet: " + QString::number(soh) + "%");
        return soh;
    }
}


QVector<double> GlaetteDaten(const QVector<double>& daten, int fensterGroesse = 5)
{
    QVector<double> geglaettet;
    int halbF = fensterGroesse / 2;

    for (int i = 0; i < daten.size(); ++i) {
        double summe = 0.0;
        int count = 0;

        for (int j = -halbF; j <= halbF; ++j) {
            int index = i + j;
            if (index >= 0 && index < daten.size()) {
                summe += daten[index];
                count++;
            }
        }

        geglaettet.append(summe / count);
    }

    return geglaettet;
}





//************* für LogNachricht, damit Fehlermeldung weg ist

// In rechenoperation.cpp
#include "rechenoperation.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>

void logNachricht(const QString &nachricht) {
    QFile file("log.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
            << " - " << nachricht << "\n";
        file.close();
    }
}


//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
//******************************************************************************************************************************************
