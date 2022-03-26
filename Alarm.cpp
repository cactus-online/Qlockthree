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
#include "Alarm.h"

//#define DEBUG
#include "Debug.h"

/*
 * Eigene Weckmelodien
 * *** 1. Eigene Tonfolgen definieren (in einem Feld melodyCustomX[].
 * *** 2. Gesamtzahl der eigenen Melodien in _numberOfCustomMelodies eintragen.
 * *** 3. Eigene Tonfolgen in Funktion registerAllCustomMelodies() registrieren. 
 */
 
// BEGINN der Definition der eigenen Melodien
    /*
     *  *** 1 ***
     */
    // Eigene Melodien werden nach folgendem Schema angegeben:
    // Dauer: Ton aus (in ms), Dauer: Ton aus (in ms), wie oft die beiden vorherigen Werte gespielt werden (= Wiederholungen + 1)
    // Beispiel: 500, 200, 4 (500ms Ton an, 200ms kein Ton, Tonfolge wird 3 Mal wiederholt, also 4 Mal gespielt)
    const unsigned int melodyCustom1[] PROGMEM = {500, 200, 1, 100, 100, 3, 0, 1000, 1};
    const unsigned int melodyCustom2[] PROGMEM = {500, 500, 10, 250, 250, 25, 100, 100, 1000};

    /*
     * *** 2 ***
     */
    const byte _numberOfCustomMelodies = 2; // Anzahl der eigenen Melodien eintragen

    /*
     * *** 3 ****
     */
    void Alarm::registerAllCustomMelodies() { // Hier die eigenen Melodien registrieren
        registerCustomMelody(melodyCustom1, sizeof(melodyCustom1) / sizeof(unsigned int));
        registerCustomMelody(melodyCustom2, sizeof(melodyCustom2) / sizeof(unsigned int));
    }
// ENDE der Definition der eigenen Melodien



const unsigned int *_pCustomMelodiesArray[_numberOfCustomMelodies];
byte _lengthOfCustomMelodies[_numberOfCustomMelodies];

/*
 * Standardmelodien
 */
const unsigned int melodies[][7] PROGMEM = {
     /*
      * { 
      *   Dauer: Ton an (wiederholt) in ms,
      *   Dauer: Ton aus (wiederholt) in ms,
      *   Anzahl Wiederholungen der beiden vorherigen Werte,
      *   Dauer: Ton an (wiederholt) in ms,
      *   Dauer: Ton aus (wiederholt) in ms,
      *   Anzahl Spielhäufigkeit der beiden vorherigen Werte (= Wiederholungen + 1),
      *   Anzahl Spielhäufigkeit der kompletten Tonfolge (= Gesamtwiederholungen + 1),
      * }
      */ 
     // Ton wird schneller
     {200, 2800, 1, 0, 0, 1,  5},
     {200, 1300, 1, 0, 0, 1, 10},
     {200, 800,  1, 0, 0, 1, 15},
     {200, 300,  1, 0, 0, 1, 30},
     {100, 200,  1, 0, 0, 1, 40},
     {100, 100,  1, 0, 0, 1, 60},
     {0, 1000, 1, 200, 800, 1, 1}

     // Alternative Definition: Ton kommt immer öfter
//     {200, 100,  1, 0, 1700, 1, 10},
//     {200, 100,  2, 0, 1500, 1, 10},
//     {200, 100,  3, 0, 1100, 1, 10},
//     {200, 100,  4, 0,  800, 1, 10},
//     {200, 100,  1, 0,    0, 1, 30},
//     {0, 1000, 1, 200, 1800, 1, 1}

};

/**
 * Konstruktor.
 *
 * @param speakerPin Der Pin, an dem der Lautsprecher oder Buzzer haengt.
 */
Alarm::Alarm(byte minutes, byte hours) : TimeStamp(minutes, hours) {
    _lastLedChange = 0;
    _posCustomMelodiesArray = 0;
    registerAllCustomMelodies();
    deactivate();
}

void Alarm::speakerPin(byte speakerPin) {
    _speakerPin = speakerPin;
    pinMode(_speakerPin, OUTPUT);
}

/**
 * Eigene Melodien in die Liste verfügbarer
 * und auswählebarer Melodien mit aufnehmen
 */
void Alarm::registerCustomMelody(const unsigned int *pCustomMelody, byte length) {
    _pCustomMelodiesArray[_posCustomMelodiesArray] = pCustomMelody;
    _lengthOfCustomMelodies[_posCustomMelodiesArray] = length;
    _posCustomMelodiesArray++;
}

/**
 * Steuert das Leucht-/Blinkverhalten der Alarm-LED
 */
boolean Alarm::pollLed(boolean isStdModeAlarm) { 
    byte _ledBlinkMode;
    
    if (!_isEnable)
        _ledBlinkMode = 0;
    else
        _ledBlinkMode = 1;
    if (isStdModeAlarm)
        _ledBlinkMode = 2;
    else if (_isSnooze)
        _ledBlinkMode = 3;
    
    switch (_ledBlinkMode) {
        case 0: // Alarm-LED dauerhaft aus
                _ledState = false;
                break;
        case 1: // Alarm-LED dauerhaft an
                _ledState = true;
                break;
        case 2: // Langsames Blinken waehrend Alarmzeit eingestellt wird
                if (millis() - _lastLedChange >= (unsigned long) 1000 / ALARM_LED_FREQ_SET_ALARM) {
                    _ledState = !_ledState;
                    _lastLedChange = millis();
                }
                break;
        case 3: // Schnelles Blinken waehrend Snooze aktiv
                if (millis() - _lastLedChange >= (unsigned long) 1000 / ALARM_LED_FREQ_SNOOZE) {
                    _ledState = !_ledState;
                    _lastLedChange = millis();
                }
                break;
    }
    return _ledState;
}

void Alarm::toggleEnable() {
    setEnable(!_isEnable);
}

void Alarm::setEnable(boolean isEnable) {
    _isEnable = isEnable;
    if (!_isEnable)
        deactivate();
}

boolean Alarm::isEnable() {
    return _isEnable;
}

/**
 * Steuert das Verhalten des Alarms:
 * Aktivierung und Deaktivierung des Alarms und
 * Umschalten auf Snooze
 */
void Alarm::pollAlarm(unsigned int rtcMinutesOfDay) {
    // Wenn Alarm aktiv, spiele Weckmelodie
    if (_isActive) {
        playAlarmMelody();
        // Gesamtalarmdauer überschritten -> Alarm abschalten
        if (millis() - _alarmDuration >= (unsigned long) MAX_BUZZ_TIME_IN_MINUTES * 60000) {
            deactivate();
        }
    } else {
        if (!_isSnooze)
            // Prüfen, ob Weckzeit erreicht ist -> Alarm aktivieren
            if ((getMinutesOfDay() == rtcMinutesOfDay) || (getMinutesOf12HoursDay() == rtcMinutesOfDay))
                activate();
    }
    // Wenn Snooze-Modus aktiv, prüfe ob Snooze-Zeit abgelaufen ist 
    // EEPROM.read(18) ist etwas hacky, aber da hier eh nicht mehr weiterentwickelt wird der geringste Aufwand
    if (_isSnooze && (millis() - _snoozeTimer >= (unsigned long) EEPROM.read(18) * 60 * 1000)) {
        activate();   
        DEBUG_PRINTLN(F("Snooze-Alarm"));
    }
}

/**
 * Hilfsfunktion für die Funktion playAlarmMelody()
 */
void Alarm::alarmMelodySubRepeat(const unsigned int *pMelody) {
    if ( !((_melodyStep + 1) % 3) ) {
          _melodySubIteration++;
          if (_melodySubIteration >= pgm_read_word_near(&pMelody[_melodyStep])) {
              _melodyStep++;
              _melodySubIteration = 0;
          } else {
              _melodyStep -= 2;
          }
    }    
}

/**
 * Spielt den aktuellen Ton der ausgewählten Melodie
 * oder macht entsprechend lange Pause
 */
void Alarm::playAlarmMelody() {
    unsigned int _buzzStep;

    if (_melody)
        _localMelody = _melody - 1;

    // Abspielen der Standardmelodien
    if (_localMelody < sizeof(melodies)/sizeof(melodies[0])) {
        // Einzelwiederholung (Ton an, Ton aus)
        alarmMelodySubRepeat(melodies[_localMelody]);
        
        // Gesamtwiederholung der kompletten Tonfolge
        if (_melodyStep == (sizeof(melodies[0])/sizeof(melodies[0][0])-1)) {
            // Übergang zur nächsten Melodie, wenn a) Melodie 0 ausgewählt und b) Anzahl an Wiederholungen erfüllt
            if (!_melody) {
                _melodyIteration++;
                if (_melodyIteration >= pgm_read_word_near(&melodies[_localMelody][_melodyStep])) {
                    _localMelody++;
                    if (_localMelody >= sizeof(melodies)/sizeof(melodies[0]))
                        _localMelody--;
                    _melodyIteration = 0;
                }
            }
            _melodyStep = 0;
        }  
        _buzzStep = pgm_read_word_near(&melodies[_localMelody][_melodyStep]);
    }
    // Abspielen der eigenen Melodien
    else {
        if (_numberOfCustomMelodies) {                
            _posCustomMelodiesArray = _localMelody - sizeof(melodies)/sizeof(melodies[0]);
            // Einzelwiederholung (Ton an, Ton aus)
            alarmMelodySubRepeat(_pCustomMelodiesArray[_posCustomMelodiesArray]);
            
            if (_melodyStep >= _lengthOfCustomMelodies[_posCustomMelodiesArray])
                _melodyStep = 0;
            _buzzStep = pgm_read_word_near(&_pCustomMelodiesArray[_posCustomMelodiesArray][_melodyStep]);

/* 
 * Alternative für statische Angabe von individuellen Melodien 
 * (kein register erforderlich, damit etwas weniger Code, dafür aber mehr Aufwand für Anwender)
 */
//        const byte _caseStart = sizeof(melodies)/sizeof(melodies[0]);
//        switch(_melody) {
//            case (_caseStart + 0): 
//                    alarmMelodySubRepeat(melodyCustom1);
//
//                    if (_melodyStep >= sizeof(melodyCustom1)/sizeof(melodyCustom1[0]))
//                        _melodyStep = 0;
//                    _buzzStep = melodyCustom1[_melodyStep];
//                    break;
//        }

        }
    }

    if (_turnBuzzOnNextCycle) {
        _turnBuzzOnNextCycle = false;
        if (_buzzStep > 0)
            buzz(true);
    }
    
    if (millis() - _buzzCycle >= _buzzStep) {
        _lastBuzzState = !_lastBuzzState;
        if (_lastBuzzState)
            _turnBuzzOnNextCycle = true;
        else
            buzz(false);            
        _buzzCycle = millis();
        _melodyStep++;
    }
}

byte Alarm::getAlarmMelody() {
    return _melody;
}

void Alarm::setAlarmMelody(byte melody) {
    _melody = melody;
    if (_melody > (sizeof(melodies)/sizeof(melodies[0]) + _numberOfCustomMelodies))
        _melody = 0;
}

void Alarm::incAlarmMelody() {
    if (_melody < (sizeof(melodies)/sizeof(melodies[0]) + _numberOfCustomMelodies))
        _melody++;
}

void Alarm::decAlarmMelody() {
    if (_melody > 0)
        _melody--;
}

/**
 * Aktiviert den Schlummer-Modus ausgelöst durch Näherungssensor
 * 
 */
void Alarm::activateSnooze() {
    deactivate();
    _snoozeTimer = millis();
    _isEnable = true;
    _isSnooze = true;
}

/**
 * Ist der Wecker im Schlummermodus?
 *
 * @return TRUE, wenn der Schlummermodus aktiv ist.
 *         FALSE, wenn der Schlummermodus ausgeschaltet ist.
 */
boolean Alarm::isSnooze() {
    return _isSnooze;
}

/**
 * Den Weckton einschalten.
 *
 * @param: on - TRUE, der Ton wird eingeschaltet.
 *              FALSE, der Ton wird ausgeschaltet.
 */
void Alarm::buzz(boolean on) {
#ifdef SPEAKER_IS_BUZZER
    if (on)
        digitalWrite(_speakerPin, HIGH);
    else
        digitalWrite(_speakerPin, LOW);
#else
    if (on)
        tone(_speakerPin, SPEAKER_FREQUENCY);
    else
        noTone(_speakerPin);
#endif
}

/**
 * Ist der Wecker aktiv?
 *
 * @return TRUE, wenn der Wecker aktiv ist.
 *         FALSE, wenn der Wecker ausgeschaltet ist.
 */
boolean Alarm::isActive() {
    return _isActive;
}

/**
 * Den Wecker einschalten.
 */
void Alarm::activate() {
    _isActive = true;
    _isSnooze = false;
    _turnBuzzOnNextCycle = true;
    _lastBuzzState = true;
    _melodyIteration = 0;
    _melodySubIteration = 0;
    _melodyStep = 0;
    _localMelody = 0;
    _buzzCycle = millis();
    _alarmDuration = _buzzCycle;
    DEBUG_PRINTLN(F("Alarm aktiviert"));
}

/**
 * Den Wecker ausschalten.
 */
void Alarm::deactivate() {
    _isEnable = false;
    _isActive = false;
    _isSnooze = false;
    _lastBuzzState = false;
    buzz(false);
    DEBUG_PRINTLN(F("Alarm deaktiviert"));
}

void Alarm::updateShowAlarmTimer(boolean isModeAlarm) {
    if (isModeAlarm) {
        if (showAlarmTimer > 0)
            showAlarmTimer--;
    }
}

boolean Alarm::getShowAlarmTimer() {
    return showAlarmTimer;
}

void Alarm::resetShowAlarmTimer() {
    showAlarmTimer = ALARM_SHOW_TIME_IN_SECONDS;
}
