/**
 * LedDriverDefault
 * Implementierung auf der Basis 74HC595 und UDN2981A.
 *
 * @mc       Arduino/RBBB
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.4f
 * @created  18.1.2013
 * @updated  03.04.2015 / Michael Joester
 *
 * Versionshistorie:
 * V 1.0:  - Erstellt.
 * V 1.1:  - printSignature() eingefuehrt.
 *         - Benennung verbessert.
 * V 1.2:  - Anpassung Helligkeit 0-100%
 * V 1.3:  - Getter fuer Helligkeit nachgezogen.
 * V 1.4:  - Unterstuetzung fuer die alte Arduino-IDE (bis 1.0.6) entfernt.
 * V 1.4f: - Michael Joester: Überblenden ergänzt.
 */
#include "LedDriverDefault.h"
#include "Configuration.h"

/* Treiberkonfiguration */
#define FADING true // Soll die Uhrzeit weich zur neuen überblenden, als Wert 'true' eintragen. Default: 'false'.
#define FADINGDURATION 10 // Schrittweite für eine Überblendung, größer = schneller. Default = 10.

#define FADINGCOUNTERLOAD 3000 // nicht ändern, bitte: Überblendvariable Startwert

// #define DEBUG
#include "Debug.h"

#define STD_MODE_NORMAL     0

// Hilfsvariable, um das Display auf den Kopf zu stellen
#ifdef UPSIDE_DOWN
  #define DISPLAY_SHIFT  (_linesToWrite-1)-
#else
  #define DISPLAY_SHIFT  0+
#endif

/**
 * Initialisierung.
 * 
 * @param data Pin, an dem die Data-Line haengt.
 * @param clock Pin, an dem die Clock-Line haengt.
 * @param latch Pin, an dem die Latch-Line haengt.
 * @param outputEnable Pin, an dem OutputEnable haengt.
 * @param linesToWrite Wieviel Zeilen muessen aus dem Bildspeicher uebernommen werden?
 */
LedDriverDefault::LedDriverDefault(byte data, byte clock, byte latch, byte outputEnable, byte linesToWrite) {
  _shiftRegister = new ShiftRegister(data, clock, latch);
  _outputEnablePin = outputEnable;
  pinMode(_outputEnablePin, OUTPUT);
  digitalWrite(_outputEnablePin, HIGH);
  _linesToWrite = linesToWrite;
}

/**
 * init() wird im Hauptprogramm in init() aufgerufen.
 * Hier sollten die LED-Treiber in eine definierten
 * Ausgangszustand gebracht werden.
 */
void LedDriverDefault::init() {
}

void LedDriverDefault::printSignature() {
  Serial.println(F("Default - 74HC595"));
}

/**
 * Den Bildschirm-Puffer auf die LED-Matrix schreiben.
 * 
 * @param onChange: TRUE, wenn es Aenderungen in dem Bildschirm-Puffer gab,
 *                  FALSE, wenn es ein Refresh-Aufruf war.
 */
void LedDriverDefault::writeScreenBufferToMatrix(word matrix[16], boolean onChange) {
  if (onChange) {
    if (_alpha == 0) { //Wenn die obeste linke LED leuchtet, wird die Uhrzeit angezeigt
      if (((helperSeconds == 0) && (mode == STD_MODE_NORMAL)) && FADING) { //Der Treiber wird im Sekundentakt mit onChange = true aufgerufen. Deswegen muss hier noch das Ende eines Minutenfadens bei Uhrzeitanzeige abgewartet werden
        for (byte i = 0; i < _linesToWrite; i++) { 
          _matrixOld[i] = _matrixNew[i]; //Abbild der aktuellen Matrix in Vorversion rüberkopieren
          _matrixNew[DISPLAY_SHIFT i] = matrix[i]; //Abbild der neuen Matrix in aktuelle Matrix einkopieren, um sie beim nächsten Überblenden zu haben
        }
      _alpha = FADINGCOUNTERLOAD; // Überblendvariable mit Startwert laden
      }
      else {
        for (byte i = 0; i < _linesToWrite; i++) {
          _matrixOld[i] = 0; //Aktuelles Abbild ist nicht verwendbar, weil eventuell keine Uhrzeit
          _matrixNew[DISPLAY_SHIFT i] = matrix[i]; //Abbild der neuen Matrix in aktuelle Matrix einkopieren, um sie beim nächsten Überblenden zu haben
        }
      _alpha = 0;
      }
    }
  }
  word row = 1;  

  _delayOldMatrix =  map(_alpha,0,FADINGCOUNTERLOAD,1,((_brightnessInPercent * PWM_DURATION)+132)); //Summand ist Korrektur um die Zeit, die das Einschieben der 32 bit für "PWM dunkel" dauert
  _delayNewMatrix =  map(_alpha,FADINGCOUNTERLOAD,0,1,((_brightnessInPercent * PWM_DURATION)+132));  
/*
  Serial.print(_alpha);
  Serial.print(F(" "));
  Serial.print(FADINGCOUNTERLOAD);
  Serial.print(F(" "));
  Serial.print(_delayOldMatrix);
  Serial.print(F(" "));
  Serial.println(_delayNewMatrix);
 */ 
  // Jetzt die Daten...
  // wir brauchen keine 16, 10 wuerden reichen... dann gehen aber nicht 
  // mehr alle Modi! Also via Variable, die im Modus-Wechsel geaendert wird...
  for (byte k = 0; k < _linesToWrite; k++) { 
#ifdef SKIP_BLANK_LINES
    if(matrix[k] != 0) {
#endif

      // Alter Zeileninhalt
      // Zeile schreiben...
      _shiftRegister->prepareShiftregisterWrite();
      _shiftRegister->shiftOut(~_matrixOld[k]);
      _shiftRegister->shiftOut(row);
      _shiftRegister->finishShiftregisterWrite();
      if (_alpha > 0) { // Über OE einschalten und nach PWM-Anteil wieder ausschalten, wenn das Display aktiv ist
        if (_displayOn == true) {digitalWrite(_outputEnablePin, LOW);}
        delayMicroseconds(_delayOldMatrix);
        if (_displayOn == true) {digitalWrite(_outputEnablePin, HIGH);}
      }
      // Neuer Zeileninhalt
      // Zeile überschreiben...
      _shiftRegister->prepareShiftregisterWrite();
      _shiftRegister->shiftOut(~_matrixNew[k]);
      _shiftRegister->shiftOut(row);
      _shiftRegister->finishShiftregisterWrite();    
      if (_displayOn == true) {digitalWrite(_outputEnablePin, LOW);} // Über OE einschalten und nach PWM-Anteil wieder ausschalten, wenn das Display aktiv ist
      delayMicroseconds(_delayNewMatrix);
      if (_displayOn == true) {digitalWrite(_outputEnablePin, HIGH);} // bleibt danach ausgeschaltet
   
      // hier kann man versuchen, das Taktverhaeltnis zu aendern (Auszeit)...
      // delayMicroseconds mit Werten <= 3 macht Probleme...
      if(_brightnessInPercent < 97) {
        delayMicroseconds((100 - _brightnessInPercent) * PWM_DURATION);
      }

#ifdef SKIP_BLANK_LINES
    }
#endif
    
    row = row << 1;
  }
    if (_alpha >= FADINGDURATION ) {
      _alpha = _alpha - FADINGDURATION;
    }
    else {
      _alpha = 0;
    }
}

/**
 * Die Helligkeit des Displays anpassen.
 * 
 * @param brightnessInPercent Die Helligkeit.
 */
void LedDriverDefault::setBrightness(byte brightnessInPercent) {
  _brightnessInPercent = brightnessInPercent;
}

/**
 * Die aktuelle Helligkeit bekommen.
 */
byte LedDriverDefault::getBrightness() {
  return _brightnessInPercent;
}

/**
 * Anpassung der Groesse des Bildspeichers.
 * 
 * @param linesToWrite Wieviel Zeilen aus dem Bildspeicher sollen 
 *                     geschrieben werden?
 */
void LedDriverDefault::setLinesToWrite(byte linesToWrite) {
    _linesToWrite = linesToWrite;
}

/**
 * Das Display ausschalten.
 */
void LedDriverDefault::shutDown() {
  digitalWrite(_outputEnablePin, HIGH);
  _displayOn = false;
}

/**
 * Das Display einschalten.
 */
void LedDriverDefault::wakeUp() {
  digitalWrite(_outputEnablePin, LOW);
  _displayOn = true;
}

/**
 * Den Dateninhalt des LED-Treibers loeschen.
 */
void LedDriverDefault::clearData() {
  _shiftRegister->prepareShiftregisterWrite();
  _shiftRegister->shiftOut(65535);
  _shiftRegister->shiftOut(0);
  _shiftRegister->finishShiftregisterWrite();        
}

/*######
 * Diplaystatus abfragen.
 ######*/
boolean LedDriverDefault::displayStatus() {
  return _displayOn;        
}
