/**
 * LDR
 * Klasse fuer den Zugriff auf einen lichtabhaengigen Widerstand.
 *
 * @mc       Arduino/RBBB
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.8a
 * @created  18.3.2012
 * @updated  28.05.2016 (Ergänzungen und Änderungen von A. Mueller)
 *
 * Versionshistorie:
 * V 1.1:  - Optimierung hinsichtlich Speicherbedarf.
 * V 1.2:  - Verbessertes Debugging.
 * V 1.3:  - Beschraenkund der LDR-Werte bei autoscale == false.
 * V 1.4:  - Der LDR mapped die Werte jetzt selbst, dadurch wird flackern bei unguenstigen Lichtverhaeltnissen vermindert.
 * V 1.5:  - Der LDR gibt Werte zwischen 0 und 100% zurueck, das ist besser verstaendlich.
 * V 1.6:  - Hysterese eingefuert, damit bei kippeligen Lichtverhaeltnissen kein Flackern auftritt.
 * V 1.7:  - isInverted eingefuehrt.
 * V 1.8:  - Unterstuetzung fuer die alte Arduino-IDE (bis 1.0.6) entfernt.
 * V 1.8a: - Klammerung in Zeile 62 korrigiert.
 *         - Speichern der im LDR_AUTOSCALE-Modus ermittelten _min- und _max-Werte in das EEPROM.
 */
#ifndef LDR_H
#define LDR_H

#include "Arduino.h"
#include "Configuration.h"
#include "Settings.h"

class LDR {
public:
    LDR(byte pin, boolean isInverted, Settings* settings);

    byte value();

private:
    byte _pin;
    boolean _isInverted;
    Settings* _settings;
    int _lastValue;
    unsigned int _min;
    unsigned int _max;
    byte _outputValue;
};

#endif
