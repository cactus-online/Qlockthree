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
#include "LDR.h"

// #define DEBUG
#include "Debug.h"

/**
 * Initialisierung mit dem Pin, an dem der LDR haengt.
 * Die Maximalwerte vom LDR koennen automatisch
 * eingemessen werden (LDR_AUTOSCALE).
 * Ansonsten muss man diese Werte im #define-DEBUG-Mode
 * ausmessen und eintragen.
 */
LDR::LDR(byte pin, boolean isInverted, Settings* settings) {
    _pin = pin;
    _isInverted = isInverted;
    _lastValue = 0;
    _outputValue = 0;
    _settings = settings;
#ifndef LDR_AUTOSCALE
    _min = LDR_MANUAL_MIN;
    _max = LDR_MANUAL_MAX;
#endif
}

/**
 * Welchen Wert hat der LDR? In Prozent...
 */
byte LDR::value() {
    unsigned int rawVal, val;
    #ifdef LDR_AUTOSCALE
        _min = _settings->getLdrAutoMin();
        _max = _settings->getLdrAutoMax();
    #endif

    rawVal = analogRead(_pin);
    if (_isInverted) {
        rawVal = 1023 - rawVal;
    }

    if ( (rawVal != _lastValue) && ( (rawVal == 0) || (rawVal == 1023) || (rawVal > (_lastValue + LDR_HYSTERESE)) || (rawVal < (_lastValue - LDR_HYSTERESE)) ) )  {
        val = rawVal;
        _lastValue = val;
#ifdef LDR_AUTOSCALE
        if (val < _min) {
            _min = val;
            _settings->setLdrAutoMin(_min);
        }
        if (val > _max) {
            _max = val;
            _settings->setLdrAutoMax(_max);
        }
#else
        val = constrain(val, _min, _max);
#endif
        unsigned int mapVal = map(val, _min, _max, 0, 100);
        mapVal = constrain(mapVal, LDR_MIN_PERCENT, LDR_MAX_PERCENT);
        DEBUG_PRINT(F("rawVal: "));
        DEBUG_PRINT(rawVal);
        DEBUG_PRINT(F(" val: "));
        DEBUG_PRINT(val);
        DEBUG_PRINT(F(" _min: "));
        DEBUG_PRINT(_min);
        DEBUG_PRINT(F(" _max: "));
        DEBUG_PRINT(_max);
        DEBUG_PRINT(F(" mapValue: "));
        DEBUG_PRINTLN(mapVal);
        DEBUG_FLUSH();
        _outputValue = mapVal;
    }
    return _outputValue;
}
