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
#ifndef BUTTON_H
#define BUTTON_H

#include "Arduino.h"

class Button {
public:
    Button(byte pin, byte pressedAgainst);
    Button(byte pin1, byte pin2, byte pressedAgainst);

    boolean pressed();
    void lock();

private:
    byte _pin1;
    byte _pin2;
    boolean _doubleMode;
    byte _pressedAgainst;
    unsigned long _lastPressTime;
};

#endif
