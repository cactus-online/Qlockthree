/**
 * LedDriverNeoPixel
 * Implementierung auf der Basis von WS2812B-Streifen wie sie die Adafruit-Neo-Pixel verwenden.
 *
 * Es lohnt sich in jedem Fall, den UeberGuide von Adafruit zu lesen:
 * https://learn.adafruit.com/adafruit-neopixel-uberguide/overview
 *
 * @mc       Arduino/RBBB
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.2f
 * @created  5.1.2015
 * @updated  04.2.04.2015 MIchael Joester
 *
 * Versionshistorie:
 * V 1.0:  - Erstellt.
 * V 1.1:  - Getter fuer Helligkeit nachgezogen.
 * V 1.2:  - Unterstuetzung fuer die alte Arduino-IDE (bis 1.0.6) entfernt.
 * V 1.2f: - Michael Joester: Überblenden ergänzt.
 *
 * Verkabelung: Einspeisung oben links, dann schlangenfoermig runter, 
 * dann Ecke unten links, oben links, oben rechts, unten rechts.
 *
 */
#include "LedDriverNeoPixel.h"
#include "Configuration.h"

/* Treiberkonfiguration */
#define FADING true // Soll die Uhrzeit weich zur neuen überblenden, als Wert 'true' eintragen. Default: 'false'.
#define FADINGDURATION 10 // Schrittweite für eine Überblendung, größer = schneller. Default = 10.

#define FADINGCOUNTERLOAD 3000 // nicht ändern, bitte: Überblendvariable Startwert

#define STD_MODE_NORMAL     0

// #define DEBUG
#include "Debug.h"

#define NUM_PIXEL 115 // 114 LEDs für die Wortuhr und die 115te ist die Alarm-LED 


/**
 * Initialisierung.
 * 
 * @param data Pin, an dem die Data-Line haengt.
 */
LedDriverNeoPixel::LedDriverNeoPixel(byte dataPin) {
  _dataPin = dataPin;
  _strip = new Adafruit_NeoPixel(NUM_PIXEL, _dataPin, NEO_GRB + NEO_KHZ800);
  _strip->begin();
  setColor(250, 255, 200);
}

/**
 * init() wird im Hauptprogramm in init() aufgerufen.
 * Hier sollten die LED-Treiber in eine definierten
 * Ausgangszustand gebracht werden.
 */
void LedDriverNeoPixel::init() {
  setBrightness(50);
  clearData();
  wakeUp();
}

void LedDriverNeoPixel::printSignature() {
  Serial.println(F("NeoPixel - WS2812B"));
}

/**
 * Den Bildschirm-Puffer auf die LED-Matrix schreiben.
 * 
 * @param onChange: TRUE, wenn es Aenderungen in dem Bildschirm-Puffer gab,
 *                  FALSE, wenn es ein Refresh-Aufruf war.
 */

void LedDriverNeoPixel::writeScreenBufferToMatrix(word matrix[16], boolean onChange) {

  if(onChange || _dirty || (_alpha > 0)) {
    if (_alpha ==0) {  
      if (((helperSeconds == 0) && (mode == STD_MODE_NORMAL)) && FADING) {
        for (byte i = 0; i < 16; i++) {
          _matrixOld[i] = _matrixNew[i];
          _matrixNew[i] = matrix[i];
        }
      _alpha = FADINGCOUNTERLOAD;
      }
      else {
        for (byte i = 0; i < 16; i++) {
          _matrixOld[i] = 0;
          _matrixNew[i] = matrix[i];
        }
      }
    }
    byte brightness255 = map(_brightnessInPercent,0,100,0, 255);
    if (_alpha > 0) {
      _brightnessOld = map(_alpha, 0, FADINGCOUNTERLOAD, 0, brightness255);
      _brightnessNew  = map(_alpha, FADINGCOUNTERLOAD,0 ,0 , brightness255);
      if (_alpha >= FADINGDURATION ) {
        _alpha = _alpha - FADINGDURATION;
      }
      else {
        _alpha = 0;
      }
    }
    else {
      _brightnessOld = 0;
      _brightnessNew = brightness255;
    }
  
      uint32_t color = _strip->Color(_brightnessScaleColor(brightness255, getRed()), _brightnessScaleColor(brightness255, getGreen()), _brightnessScaleColor(brightness255, getBlue()));
      uint32_t colorNew = _strip->Color(_brightnessScaleColor(_brightnessNew, getRed()), _brightnessScaleColor(_brightnessNew, getGreen()), _brightnessScaleColor(_brightnessNew, getBlue()));
      uint32_t colorOld = _strip->Color(_brightnessScaleColor(_brightnessOld, getRed()), _brightnessScaleColor(_brightnessOld, getGreen()), _brightnessScaleColor(_brightnessOld, getBlue()));

      _dirty = false;
      _strip->clear();

      for(byte y=0; y<10; y++) {
        for(byte x=5; x<16; x++) {
          word t = 1 << x;
          if (((_matrixOld[y] & t) == t) && ((_matrixNew[y] & t) == t)) {
            _setPixel(15-x, y, color);
          }
          else {
            if ((_matrixOld[y] & t) == t) {
              _setPixel(15-x, y, colorOld);
            }
            else if ((_matrixNew[y] & t) == t) {
              _setPixel(15-x, y, colorNew);
            }
          }         
        }
      }
  
      // wir muessen die Eck-LEDs und die Alarm-LED umsetzen...
     word cornerLedCount[] = {1, 0, 3, 2, 4};
     for ( int i=0; i<5; i++) {
       if(((_matrixOld[cornerLedCount[i]] & _matrixNew[cornerLedCount[i]] & 0b0000000000011111) > 0) ){
         _setPixel(110+i, color);
       }
       else {
         if(((_matrixOld[cornerLedCount[i]] & 0b0000000000011111) > 0) ){
           _setPixel(110+i, colorOld);
         }
         else if(((_matrixNew[cornerLedCount[i]] & 0b0000000000011111) > 0) ){
           _setPixel(110+i, colorNew);
         }
       } 
     }
 
    _strip->show();
  }
}

/**
 * Die Helligkeit des Displays anpassen.
 * 
 * @param brightnessInPercent Die Helligkeit.
 */
void LedDriverNeoPixel::setBrightness(byte brightnessInPercent) {
  if(brightnessInPercent != _brightnessInPercent) {
    _brightnessInPercent = brightnessInPercent;
    _dirty = true;
  }
}

/**
 * Die aktuelle Helligkeit bekommen.
 */
byte LedDriverNeoPixel::getBrightness() {
  return _brightnessInPercent;
}

/**
 * Anpassung der Groesse des Bildspeichers.
 * 
 * @param linesToWrite Wieviel Zeilen aus dem Bildspeicher sollen 
 *                     geschrieben werden?
 */
void LedDriverNeoPixel::setLinesToWrite(byte linesToWrite) {
}

/**
 * Das Display ausschalten.
 */
void LedDriverNeoPixel::shutDown() {
  _strip->clear();
  _strip->show();
  _displayOn = false;
}

/**
 * Das Display einschalten.
 */
void LedDriverNeoPixel::wakeUp() {
  _displayOn = true;
}

/**
 * Den Dateninhalt des LED-Treibers loeschen.
 */
void LedDriverNeoPixel::clearData() {
  for (byte i = 0; i < 16; i++) {
    _matrixNew[i] = 0;
    _matrixOld[i] = 0;
  }
  _strip->clear();
  _strip->show();
}

/**
 * Einen X/Y-koordinierten Pixel in der Matrix setzen.
 */
void LedDriverNeoPixel::_setPixel(byte x, byte y, uint32_t c) {
  _setPixel(x + (y * 11), c);
}

/**
 * Einen Pixel im Streifen setzten (die Eck-LEDs und die Alarm-LED sind am Ende).
 */
void LedDriverNeoPixel::_setPixel(byte num, uint32_t c) {
  if(num < 110) {
    if((num / 11) % 2 == 0) {
      _strip->setPixelColor(num, c);
    }
    else {
      _strip->setPixelColor(((num / 11) * 11) + 10 - (num % 11), c);
    }
  }
  else {
    switch(num) {
      case 110:
        _strip->setPixelColor(111, c);  
      break;
      case 111:
        _strip->setPixelColor(112, c);  
      break;
      case 112:
        _strip->setPixelColor(113, c);  
      break;
      case 113:
        _strip->setPixelColor(110, c);  
      break;
      case 114:                         // die Alarm-LED
        _strip->setPixelColor(110, c);  
      break;
    }       
  }
}

/**
 * Funktion fuer saubere 'Regenbogen'-Farben.
 * Kopiert aus den Adafruit-Beispielen (strand).
 */
uint32_t LedDriverNeoPixel::_wheel(byte wheelPos) {  
  if(wheelPos < 85) {
    return _strip->Color(wheelPos * 3, 255 - wheelPos * 3, 0);
  } 
  else if(wheelPos < 170) {
    wheelPos -= 85;
    return _strip->Color(255 - wheelPos * 3, 0, wheelPos * 3);
  } 
  else {
    wheelPos -= 170;
    return _strip->Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
}

/**
 * Hilfsfunktion fuer das Skalieren der Farben.
 */ 
byte LedDriverNeoPixel::_brightnessScaleColor(byte brightness, byte colorPart) {
  return map(brightness, 0, 255, 0, colorPart);
}
// #####

/*######
 * Diplaystatus abfragen.
 ######*/
boolean LedDriverNeoPixel::displayStatus() {
  return _displayOn;        
}
