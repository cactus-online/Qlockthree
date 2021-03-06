/**
 * Button
 * Kleine Klasse zum Entprellen der Tasten.
 *
 * @mc       Arduino/RBBB
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.7c
 * @created  18.2.2011
 * @updated  04.06.2016 (Ergänzungen und Änderungen von A. Mueller)
 *
 * Versionshistorie:
 * V 1.1:  - Kompatibilitaet zu Arduino-IDE 1.0 hergestellt.
 * V 1.2:  - Optimierung hinsichtlich Speicherbedarf.
 * V 1.3:  - Verbessertes Debugging.
 * V 1.4:  - Doppel-Tasten-Abfrage ermoeglicht.
 * V 1.5:  - Ueberlauf in millis() beruecksichtigt.
 * V 1.6:  - Schalten gegen LOW ermoeglicht.
 * V 1.7:  - Unterstuetzung fuer die alte Arduino-IDE (bis 1.0.6) entfernt.
 * V 1.7a: - Berücksichtigung des Überlaufs in millis() wieder rückgängig gemacht.
 * V 1.7b: - Kurzzeitiges Sperren von Buttons ermöglicht.
 * V 1.7c: - Seltenen Initialsierungsfehler behoben.
 */
#include "Button.h"

// #define DEBUG
#include "Debug.h"

#include "Configuration.h"

/**
 * Initialisierung mit dem Pin, an dem der Taster haengt.
 *
 * @param  pin: der Pin, an dem der Taster haengt
 *         pressedAgainst: wogegen schaltet der Taster? (HIGH/LOW)
 */
Button::Button(byte pin, byte pressedAgainst) {
    _pin1 = pin;
    _doubleMode = false;
    _lastPressTime = 0;
    _pressedAgainst = pressedAgainst;
    if (_pressedAgainst == HIGH) {
        pinMode(_pin1, INPUT);
    } else {
        pinMode(_pin1, INPUT_PULLUP);
    }
}

/**
 * Initialisierung mit den zwei Pins, an denen die Taster haengen, die man gleichzeitig abfragen moechte.
 *
 * @param  pin1, pin2: die Pins, an dem der virtuelle Taster haengt
 *         pressedAgainst: wogegen schalten die Taster? (HIGH/LOW)
 */
Button::Button(byte pin1, byte pin2, byte pressedAgainst) {
    _pin1 = pin1;
    _pin2 = pin2;
    _doubleMode = true;
    _lastPressTime = 0;
    _pressedAgainst = pressedAgainst;
    if (_pressedAgainst == HIGH) {
        pinMode(_pin1, INPUT);
        pinMode(_pin2, INPUT);
    } else {
        pinMode(_pin1, INPUT_PULLUP);
        pinMode(_pin2, INPUT_PULLUP);
    }
}

/**
 * Wurde der Taster gedrueckt?
 */
boolean Button::pressed() {
    boolean _retVal = false;

    if ( (digitalRead(_pin1) == _pressedAgainst) && (!_doubleMode || (digitalRead(_pin2) == _pressedAgainst)) && (millis() - _lastPressTime > BUTTON_THRESHOLD) ) {
        _lastPressTime = millis();
        _retVal = true;
    }

    return _retVal;
}

/**
 * Taster sperren
 */
void Button::lock() {
    _lastPressTime = millis();  
}

