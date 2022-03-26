/**
 * MyDCF77
 * Klasse fuer die Auswertung des DCF77-Zeitsignals.
 * Diese Klasse benötigt eine externe 1-Sekunden-Basis und einen
 * zusätzlichen Timer, der im Default-Fall alle 100ms ein Flag setzt.
 * Dieser feste Takt wird benötigt, um die zwingend erforderliche
 * Driftkorrektur durchführen zu können.
 * Diese Klasse geht nicht von einem 'sauberen' Signal aus,
 * und ermittelt die HIGH/LOWs statistisch.
 * 
 *
 * @mc       Arduino/RBBB
 * @autor    Andreas Mueller
 *           Vorlage von: Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.5
 * @created  21.3.2016
 * @updated  04.06.2016
 *
 * Versionshistorie:
 * V 1.0:   * Signalauswertealgoritmus komplett neu geschrieben! *
 *          Zuverlässigkeit der Zeitsynchronisation erhöht:
 *          - Drifts in der Phase werden durch eine automatische Offsetkorrektur bereinigt.
 *          - Damit ist die Erkennung der Schaltsekunde nach spätestens einer Minute
 *            zuverlässig möglich.
 *            Das Fehlen der korrekten Erkennung dieser Schaltsekunde in früheren Versionen
 *            verhinderte eine zuverlässige Zeitsynchronisation.
 *          - Deutlich exaktere Einstellung der Zeit dank Driftkorrektur möglich.        
 * V 1.1:   - Funktion für EXT_MODE_DCF_DEBUG eingeführt.
 *          - Umschaltung von Timer1 auf Timer2 in Header-Datei möglich. (Entfällt ab V 1.2)
 * V 1.2:   * Die Driftkorrektur benötigt keinen Timer mehr! *
 * V 1.3:   - Mehrere kleine Codeoptimierungen
 * V 1.4:   - Fehlerhafter Speicherzugriff korrigiert
 *          - Größere Codeoptimierung
 *          - Änderung an den Funktionen getDcf77LastSuccessSyncMinutes() und setDcf77LastSuccessSyncMinutes()
 * V 1.5:   - Seltene Initialisierungsfehler behoben.
 */
#ifndef MYDCF77_H
#define MYDCF77_H

#include "Arduino.h"
#include "Configuration.h"
#include "TimeStamp.h"

class MyDCF77 : public TimeStamp {

public:
    MyDCF77(byte signalPin, byte statusLedPin, byte dcf77PonPin);

    void statusLed(boolean on);
    void enable(boolean on);

    boolean poll(boolean signalIsInverted);

    unsigned long getDcf77LastSuccessSyncMinutes();
    void setDcf77SuccessSync(TimeStamp* _rtc);

    byte getDcf77ErrorCorner();

    boolean signal(boolean signalIsInverted);

private:
    static byte DCF77Factors[];

    byte _signalPin;
    byte _statusLedPin;
    byte _dcf77PonPin;
    boolean _enable = true;
    unsigned int _nPolls;    

    byte _bits[MYDCF77_TELEGRAMMLAENGE+1];
    byte _bitsPointer;
    
    unsigned int _bins[MYDCF77_SIGNAL_BINS];
    char _binsPointer;
    char _binsOffset;

    unsigned long _dcf77LastCycle;
    // _toggleSignal wird nur für EXT_MODE_DCF_DEBUG benötigt
    boolean _toggleSignal;
    byte _errorCorner;

#ifdef DCF77_SENSOR_EXISTS
    TimeStamp _dcf77LastSyncTime;
#endif

    boolean newCycle();
    void outputSignal(unsigned int average, unsigned int imax, unsigned int isum);

    byte decodeHelper(byte *checksum, byte startV, byte endV);
    boolean decode();

    void clearBits();
    void clearBins();
};

#endif
