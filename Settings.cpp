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
#include "Settings.h"
#include <EEPROM.h>
#include "Renderer.h"

//#define DEBUG
#include "Debug.h"

/**
 *  Konstruktor.
 */
Settings::Settings() {
    // START - Einstellungen
    _savedValues[0]  = 0xB8;            // SETTINGS_MAGIC_NUMBER
    _savedValues[1]  = 2;               // SETTINGS_VERSION
    
    _savedValues[2]  = LANGUAGE_DE_DE;  // byte _language = LANGUAGE_DE_DE;
    _savedValues[3]  = true;            // boolean _renderCornersCw = true;
    // Ersetzt durch resetLdrLimits()
//    _savedValues[4]                     // low byte of LDR_AUTOSCALE _min
//    _savedValues[5]                     // high byte of LDR_AUTOSCALE _min
//    _savedValues[6]                     // low byte of LDR_AUTOSCALE _max
//    _savedValues[7]                     // high byte of LDR_AUTOSCALE _max
    resetLdrLimits();
    _savedValues[8]  = true;            // boolean _useLdr = true;
    _savedValues[9]  = false;           // boolean _blankByLdr = false;
    _savedValues[10] = 10;              // byte _ldrBlankTreshold = 10;
    _savedValues[11] = 75;              // byte _brightness = 75;
    _savedValues[12] = false;           // boolean _dcfSignalIsInverted = false;
    _savedValues[13] = 0;               // char _timeShift = 0;
    _savedValues[14] = false;           // boolean _esIst = false;
    _savedValues[15] = false;           // boolean _alarmEnable = false;
    _savedValues[16] = 0;               // byte _alarmMelody = 0
    _savedValues[17] = false;           // boolean _24hmode = false
    _savedValues[18] = 5;               // u_int _SnoozeTime= 5
    
    #ifdef ALARM_OPTION_ENABLE
        byte alarmHours = 6;
        byte alarmMinutes = 0;
    #endif
    byte offTimeMoFrHours = 3;
    byte offTimeMoFrMinutes = 0;
    byte onTimeMoFrHours = 4;
    byte onTimeMoFrMinutes = 0;
    byte offTimeSaSoHours = 3;
    byte offTimeSaSoMinutes = 0;
    byte onTimeSaSoHours = 4;
    byte onTimeSaSoMinutes = 0;
    // ENDE - Einstellungen
    

    _NightTimesAndAlarm[0] = new TimeStamp(offTimeMoFrMinutes, offTimeMoFrHours);
    _NightTimesAndAlarm[1] = new TimeStamp(onTimeMoFrMinutes, onTimeMoFrHours);
    _NightTimesAndAlarm[2] = new TimeStamp(offTimeSaSoMinutes, offTimeSaSoHours);
    _NightTimesAndAlarm[3] = new TimeStamp(onTimeSaSoMinutes, onTimeSaSoHours);
    #ifdef ALARM_OPTION_ENABLE
        _alarm = new Alarm(alarmMinutes, alarmHours);
        _NightTimesAndAlarm[4] = _alarm;
    #endif

    if (!_savedValues[8]) _savedValues[9] = false;

    // Versuche alte Einstellungen zu laden...
    loadFromEEPROM();
}

void Settings::resetLdrLimits() {
    // Die Initialisierung erfolgt absichtlich vertauscht, damit die Grenzen
    // neu ausgemessen werden.
    _savedValues[4]  = (1023 % 256);    // low byte of LDR_AUTOSCALE _min
    _savedValues[5]  = (1023 / 256);    // high byte of LDR_AUTOSCALE _min
    _savedValues[6]  = (0 % 256);       // low byte of LDR_AUTOSCALE _max
    _savedValues[7]  = (0 / 256);       // high byte of LDR_AUTOSCALE _max
}

/*
 * Auslesen und Speichern der automatisch ermittelten LDR-Regelgrenzen
 */
#ifdef LDR_AUTOSCALE
    // ACHTUNG!! Der typecast auf integer erfordert zwei byte, die direkt
    // aufeinander folgen müssen, wie hier: _savedValues[4] und _savedValues[5]
    // _savedValues[4] beinhaltet das low byte und _savedValues[5] beinhaltet das high byte.
    unsigned int Settings::getLdrAutoMin() {
        return ( *((unsigned int*) &_savedValues[4]) );
    }

    void Settings::setLdrAutoMin(unsigned int ldrauto_min) {
        *((unsigned int*) &_savedValues[4]) = ldrauto_min;
    }

    // ACHTUNG!! Der typecast auf integer erfordert zwei byte, die direkt
    // aufeinander folgen müssen, wie hier: _savedValues[6] und _savedValues[7]
    // _savedValues[6] beinhaltet das low byte und _savedValues[7] beinhaltet das high byte.
    unsigned int Settings::getLdrAutoMax() {
        return ( *((unsigned int*) &_savedValues[6]) );
    }

    void Settings::setLdrAutoMax(unsigned int ldrauto_max) {
        *( (unsigned int*) &_savedValues[6] ) = ldrauto_max;
    }
#endif

/**
 * Die Sprache. Werte siehe Renderer.h
 */
byte Settings::getLanguage() {
    return _savedValues[2];
}

void Settings::incLanguage() {
    if (_savedValues[2] == LANGUAGE_COUNT)
        _savedValues[2] = 0;
    else
        _savedValues[2]++;
}

void Settings::decLanguage() {
    if (_savedValues[2] == 0)
        _savedValues[2] = LANGUAGE_COUNT;
    else
        _savedValues[2]--;
}

/**
 * Die Laufrichtung der Eck-LEDs.
 */
boolean Settings::getRenderCornersCw() {
    return _savedValues[3];
}

void Settings::toggleRenderCornersCw() {
   _savedValues[3] = !_savedValues[3];
}

/**
 * Der LDR-Modus.
 */
boolean Settings::getUseLdr() {
    return _savedValues[8];
}

void Settings::toggleUseLdr() {
    _savedValues[8] = !_savedValues[8];
    if (!_savedValues[8]) {
        _savedValues[9] = false;
        resetLdrLimits();
    }
}

/**
 * Automatischer Nachtmodus via LDR.
 */
boolean Settings::getBlankByLdr() {
    return _savedValues[9];
}

void Settings::toggleBlankByLdr() {
    _savedValues[9] = !_savedValues[9];
}

byte Settings::changeSavedValue(char change, byte index) {
    char c = _savedValues[index] + change;
    if (c > 99)
        c = 99;
    if (c < 0)
        c = 0;
    _savedValues[index] = c;
    return _savedValues[index];
}

/**
 * LDR An- und Abschaltschwelle
 * (für die LDR-gesteuerte Nachtschaltung).
 */

byte Settings::getLdrBlankThreshold() {
    return _savedValues[10];
}


void Settings::changeLdrBlankThreshold(char change) {
    changeSavedValue(change, 10);
}

/**
 * Die Helligkeit.
 */
byte Settings::getBrightness() {
    return _savedValues[11];
}

byte Settings::changeBrightness(char change) {
    return changeSavedValue(change, 11);
}

/**
 * Ist das DCF-Signal invertiert?
 */
boolean Settings::getDcfSignalIsInverted() {
    return _savedValues[12];
}

void Settings::toggleDcfSignalIsInverted() {
    _savedValues[12] = !_savedValues[12];
}

/**
 * Zeitverschiebung
 */
char Settings::getTimeShift() {
    return _savedValues[13];
}

char Settings::incTimeShift() {
    if ((char)_savedValues[13] < 13) {
        _savedValues[13]++;
        return 1;
    } else {
        return 0;
    }
}

char Settings::decTimeShift() {
    if ((char)_savedValues[13] > -13) {
        _savedValues[13]--;
        return -1;
    } else {
        return 0;
    }
}

boolean Settings::getEsIst() {
    return _savedValues[14];
}

void Settings::toggleEsIst() {
    _savedValues[14] = !_savedValues[14];
}

boolean Settings::get24hMode() {
    return _savedValues[17];
}

void Settings::toggle24hMode() {
    _savedValues[17] = !_savedValues[17];
}

unsigned int Settings::getSnooze() {
    return _savedValues[18];
}

void Settings::incSnooze() {
    if (_savedValues[18] >= 30)
        _savedValues[18] = 0;
    else
        _savedValues[18] = _savedValues[18]+5;
}

void Settings::decSnooze() {
    if (_savedValues[18] <= 0)
        _savedValues[18] = 30;
    else
        _savedValues[18] = _savedValues[18]-5;
}

/**
 * Die Ab- und Anschaltzeiten fuer das Display werden über ein separates Menü eingestellt. Die Abschaltung des
 * Displays verbessert den Empfang des DCF77-Empfaengers. Und hilft, falls die Uhr im
 * Schlafzimmer haengt. Nach einer Minute kann man das Display manuell wieder ein- / ausschalten.
 */
TimeStamp* Settings::getNightTimeStamp(byte _position) {
    /* 
     * 0: offTimeMoFr
     * 1: onTimeMoFr
     * 2: offTimeSaSo
     * 3: onTimeSaSo
     * 4: Alarm
     */
     return _NightTimesAndAlarm[_position];
}

/**
 * Die Einstellungen laden.
 */
void Settings::loadFromEEPROM() {
    if ((EEPROM.read(0) == _savedValues[0]) && (EEPROM.read(1) == _savedValues[1])) {
        // es sind gueltige Einstellungen vorhanden...
        for (byte i = 0; i < sizeof(_savedValues)/sizeof(_savedValues[0]); i++) {
            _savedValues[i] = EEPROM.read(i);   
        }
        _loadSaveNightTimesAndAlarm(false);
    }
    #ifdef ALARM_OPTION_ENABLE
        _alarm->setEnable(_savedValues[15]);
        _alarm->setAlarmMelody(_savedValues[16]);
    #endif
}

/**
 * Die Einstellungen speichern.
 */
void Settings::saveToEEPROM() {
    #ifdef ALARM_OPTION_ENABLE
        _savedValues[15] = _alarm->isEnable();
        _savedValues[16] = _alarm->getAlarmMelody();
    #endif
    for (byte i = 0; i < sizeof(_savedValues)/sizeof(_savedValues[0]); i++) {
        // _brightness wird nur gespeichert, wenn der LDR _NICHT_ verwendet wird.
        DEBUG_PRINTLN();
        DEBUG_PRINT(F(" - "));
        DEBUG_PRINT(i);
        DEBUG_PRINTLN(F(" - "));
        DEBUG_PRINT(F("EEPROM: "));
        DEBUG_PRINTLN(EEPROM.read(i));
        DEBUG_PRINT(F("   RAM: "));
        DEBUG_PRINT(_savedValues[i]);
        if ( (i != 11) || !_savedValues[8] )
            if (EEPROM.read(i) != _savedValues[i]) {
                EEPROM.write(i, _savedValues[i]);
                DEBUG_PRINT(F(" Speichern!!"));
            }    
    }
    _loadSaveNightTimesAndAlarm(true);
}

void Settings::_loadSaveNightTimesAndAlarm(boolean save) {
    byte forStart = sizeof(_savedValues)/sizeof(_savedValues[0]);
    #ifdef ALARM_OPTION_ENABLE
        #define NIGHTTIMES_FOR_END 2
    #else
        #define NIGHTTIMES_FOR_END 0
    #endif
    byte forEnd = forStart + 8 + NIGHTTIMES_FOR_END;
    byte getValue;
    for (byte i = forStart; i < forEnd; i++) {
        byte value = EEPROM.read(i);
        byte i_forStart   = i - forStart;
        byte i_forStart_2 = i_forStart / 2;
        if (!(i_forStart % 2)) {
            // Hours
            if (save) {
                getValue = _NightTimesAndAlarm[i_forStart_2]->getHours();
            } else {
                _NightTimesAndAlarm[i_forStart_2]->setHours(value);
            }
        } else {
            // Minutes
            if (save) {
                getValue = _NightTimesAndAlarm[i_forStart_2]->getMinutes();
            } else {
                _NightTimesAndAlarm[i_forStart_2]->setMinutes(value);
            }
            
        }
        DEBUG_PRINTLN();
        DEBUG_PRINT(F(" - "));
        DEBUG_PRINT(i);
        DEBUG_PRINTLN(F(" - "));
        DEBUG_PRINT(F("EEPROM: "));
        DEBUG_PRINTLN(EEPROM.read(i));
        DEBUG_PRINT(F("   RAM: "));
        DEBUG_PRINT(getValue);
        if (save && value != getValue) {
            EEPROM.write(i, getValue);
            DEBUG_PRINT(F(" Speichern!!"));
        }
    }
}
