/**
 * Settings
 * Die vom Benutzer getaetigten Einstellungen werden hier verwaltet
 * und im EEPROM persistiert.
 *
 * @mc       Arduino/RBBB
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.3e
 * @created  23.1.2013
 * @updated  04.06.2016 (Ergänzungen und Änderungen von A. Mueller)
 *
 * Versionshistorie:
 * V 1.0:  - Erstellt.
 * V 1.1:  - LDR-Modus aufgenommen.
 * V 1.2:  - Brightness aufgenommen.
 *         - EnableAlarm aufgenommen.
 *         - DcfSignalIsInverted aufgenommen.
 *         - TimeShift aufgenommen.
 * V 1.3:  - Unterstuetzung fuer die alte Arduino-IDE (bis 1.0.6) entfernt.
 * V 1.3a: - Einstellung für automatischen Rücksprung und Nachtschaltung aufgenommen.
 * V 1.3b: - Einstellung der Nachtschaltung um Sonderzeiten am Wochenende erweitert.
 * V 1.3c: - Einstellung für einleitende Worte ("ES IST") hinzugefügt.
 * V 1.3d: - Codeoptimierungen (alle speicherbaren Werte sind jetzt byte-Variablen).
 *         - Einstellungen für den LDR-gesteuerten Nachtmodus hinzugefügt.
 *         - Einstellung für verschiedene Weckmelodien hinzugefügt.
 *         - Speichern der Weckeinstellungen.
 *         - Speichern der Auto-Ldr-Werte
 * V 1.3e: - TimeShift-Funktionen angepasst, JumpToTime-Funktionen entfallen
 */
#ifndef SETTINGS_H
#define SETTINGS_H

#include "Arduino.h"
#include "Configuration.h"
#ifdef ALARM_OPTION_ENABLE
    #include "Alarm.h"
#endif
#include "TimeStamp.h"

class Settings {
public:
    Settings();

    void resetLdrLimits();

#ifdef LDR_AUTOSCALE
    unsigned int getLdrAutoMin();
    void setLdrAutoMin(unsigned int ldrauto_min);
    unsigned int getLdrAutoMax();
    void setLdrAutoMax(unsigned int ldrauto_max);
#endif

    byte getLanguage();
    void incLanguage();
    void decLanguage();

    boolean getRenderCornersCw();
    void toggleRenderCornersCw();

    boolean getUseLdr();
    void toggleUseLdr();

    boolean getBlankByLdr();
    void toggleBlankByLdr();

    byte changeSavedValue(char change, byte index);
    byte getLdrBlankThreshold();
    void changeLdrBlankThreshold(char change);
    byte getBrightness();
    byte changeBrightness(char change);
    
    boolean getDcfSignalIsInverted();
    void toggleDcfSignalIsInverted();

    char getTimeShift();
    char incTimeShift();
    char decTimeShift();

    boolean getEsIst();
    void toggleEsIst();

    boolean get24hMode();
    void toggle24hMode();

    unsigned int getSnooze();
    void incSnooze();
    void decSnooze();
    
    TimeStamp* getNightTimeStamp(byte _position);

    void loadFromEEPROM();
    void saveToEEPROM();

private:
    byte _savedValues[19]; // 17 -> 19 -TF4

    TimeStamp* _NightTimesAndAlarm[5];
    #ifdef ALARM_OPTION_ENABLE
        Alarm* _alarm;
    #endif

    void _loadSaveNightTimesAndAlarm(boolean save);
};

#endif
