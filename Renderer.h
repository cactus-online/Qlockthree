/**
 * Renderer
 * Diese Klasse rendert die Woerter auf die Matrix.
 *
 * @mc       Arduino/RBBB
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.6d
 * @created  21.1.2013
 * @updated  12.06.2016 (Ergänzungen und Änderungen von A. Mueller)
 *
 * Versionshistorie:
 * V 1.0:  - Erstellt.
 * V 1.1:  - Spanisch hinzugefuegt.
 * V 1.2:  - setMinutes - hours auf char umgestellt, damit Zeitverschiebung geht...
 * V 1.3:  - Alle Deutsch-Varianten zusammengefasst, um Platz zu sparen.
 *         - Fehler im Italienischen behoben.
 * V 1.4:  - Stundenbegrenzung (die ja wegen der Zeitverschiebungsmoeglichkeit existiert) auf den Bereich 0 <= h <= 24 ausgeweitet, dank Tipp aus dem Forum.
 * V 1.5:  - Unterstuetzung fuer die alte Arduino-IDE (bis 1.0.6) entfernt.
 * V 1.6:  - Stundenbegrenzung (die ja wegen der Zeitverschiebungsmoeglichkeit existiert) auf den Bereich 0 <= h <= 24 auch in setHours eingefuehrt, siehe http://diskussion.christians-bastel-laden.de/viewtopic.php?f=17&t=2028
 * V 1.6a: - Fehler bei Stundenbegrenzung in setMinutes korrigiert und Stundenbegrenzung aus setHours wieder entfernt, siehe http://diskussion.christians-bastel-laden.de/viewtopic.php?f=17&t=2028
 *         - Kleinere Aufräumarbeiten
 * V 1.6b: - Kleine Codeoptimierungen
 *         - Zusätzliche Option, für jede Eckled nur die dazugehörige Kathode und nicht alle einzuschalten. Dies Verhindert das Glimmen ausgeschalteter Eckleds. (Standard: ausgeschaltet)
 * V 1.6c: - Zeitzone wird direkt in die RTC geschrieben, damit kann die Wertebereichanpassung der Variable hours entfallen.
 *         - Funktionen für das Blinken der LEDs am Nachmittag herher verlagert.
 * V 1.6d: - Portugiesisch hinzugefügt.
 *         - Renderer-Funktionen optimiert.
 */
#ifndef RENDERER_H
#define RENDERER_H

#include "Arduino.h"
#include "Configuration.h"

#define LANGUAGE_DE_DE 0
#define LANGUAGE_DE_SW 1
#define LANGUAGE_DE_BA 2
#define LANGUAGE_DE_SA 3
#define LANGUAGE_CH    4
#define LANGUAGE_EN    5
#define LANGUAGE_FR    6
#define LANGUAGE_IT    7
#define LANGUAGE_NL    8
#define LANGUAGE_ES    9
#define LANGUAGE_PT    10
#define LANGUAGE_COUNT 10

class Renderer {
public:
    Renderer();

    boolean pollDisplayBlinkAfternoon();
    void clearScreenIfNeeded_DisplayOnBlinking(word matrix[16]);
    void setAfternoon(byte hours);

    void setMinutes(byte hours, byte minutes, byte language, word matrix[16]);
    void setCorners(byte minutes, boolean cw, word matrix[16]);
    void activateAlarmLed(word matrix[16]);

    void cleanIntroWords(byte language, word matrix[16]);

    void scrambleScreenBuffer(word matrix[16]);
    void clearScreenBuffer(word matrix[16]);
    void setAllScreenBuffer(word matrix[16]);

private:
    void setHours(byte hours, byte glatt_vor_nach, byte language, word matrix[16]);

    unsigned long _lastDisplayBlinkChange;
    boolean _isAfternoon;
    boolean _isDisplayOn_DisplayBlinking;
};

#endif
