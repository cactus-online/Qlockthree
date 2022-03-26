/**
 * Alarm
 * Klasse fuer die Weckfunktion.
 *
 * @mc       Arduino/RBBB
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.1c
 * @created  22.1.2013
 * @update   04.06.2016 (Ergänzungen und Änderungen von A. Mueller)
 *
 * Versionshistorie:
 * V 1.0:  - Erstellt.
 * V 1.1:  - Unterstuetzung fuer die alte Arduino-IDE (bis 1.0.6) entfernt.
 * V 1.1a: - Zusätzliche Weckmelodien.
 *         - Snooze-Modus für 5-Tasten-Wortwecker
 *         - Blinkgeschwindigkeit der Alarm-LED variabel.
 * V 1.1b: - Kleinere Codeoptimierungen
 *         - Abfrage, ob Schlummermodus aktiv wurde hinzugefügt.
 * V 1.1c: - Funktionen für AlarmShowTimer hierher verlagert.
 */
#ifndef ALARM_H
#define ALARM_H

#include "Arduino.h"
#include "Configuration.h"
#include "TimeStamp.h"
#include <EEPROM.h>

class Alarm : public TimeStamp {
public:
    Alarm(byte minutes, byte hours);

    void speakerPin(byte speakerPin);
    
    boolean pollLed(boolean isStdModeAlarm = false);
    void pollAlarm(unsigned int rtcMinutesOfDay); 
    
    byte getAlarmMelody();
    void setAlarmMelody(byte melody);
    void incAlarmMelody();
    void decAlarmMelody();

    void toggleEnable();
    void setEnable(boolean isEnable);
    boolean isEnable();
    void deactivate();
    void activateSnooze();
    boolean isSnooze();
    
    boolean isActive();

    void buzz(boolean on);

    void updateShowAlarmTimer(boolean isModeAlarm);
    boolean getShowAlarmTimer();
    void resetShowAlarmTimer();

private:
    void registerAllCustomMelodies();
    void registerCustomMelody(const unsigned int *pCustomMelody, byte length);
    void alarmMelodySubRepeat(const unsigned int *pMelody);
    void playAlarmMelody();
    void activate();

    byte _speakerPin;
    boolean _ledState;
    
    boolean _isEnable;
    boolean _isActive;
    boolean _isSnooze;
    
    unsigned long _alarmDuration;
    unsigned long _snoozeTimer;
    unsigned long _lastLedChange;
    unsigned long _buzzCycle;

    boolean _lastBuzzState;
    boolean _turnBuzzOnNextCycle;
    
    byte _melody;
    byte _localMelody;
    byte _melodyStep;
    byte _melodySubIteration;
    byte _melodyIteration; 
    byte _posCustomMelodiesArray;

    byte showAlarmTimer;
};

#endif
