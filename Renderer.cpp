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
#include "Renderer.h"

#include "Woerter_DE.h"
// #include "Woerter_DE_MKF.h"
#include "Woerter_CH.h"
#include "Woerter_EN.h"
#include "Woerter_FR.h"
#include "Woerter_IT.h"
#include "Woerter_NL.h"
#include "Woerter_ES.h"
#include "Woerter_PT.h"

// #define DEBUG
#include "Debug.h"

#define STD_MODE_ALARM     1

Renderer::Renderer() {
}

/**
 * Ein Zufallsmuster erzeugen (zum Testen der LEDs)
 */
void Renderer::scrambleScreenBuffer(word matrix[16]) {
    for (byte i = 0; i < 16; i++) {
        matrix[i] = random(65536);
    }
    _isDisplayOn_DisplayBlinking = true;
    _lastDisplayBlinkChange = 0;
}

/**
 * Die Matrix loeschen (zum Stromsparen, DCF77-Empfang
 * verbessern etc.)
 */
void Renderer::clearScreenBuffer(word matrix[16]) {
    for (byte i = 0; i < 16; i++) {
        matrix[i] = 0;
    }
}

/**
 * Die Matrix komplett einschalten (zum Testen der LEDs)
 */
void Renderer::setAllScreenBuffer(word matrix[16]) {
    for (byte i = 0; i < 16; i++) {
        matrix[i] = 65535;
    }
}

/*
 * Lässt die LEDs blinken, wenn die einzustellende
 * Zeit größer oder gleich 12 Uhr (mittags) ist.
 *  0 - 11:59 Uhr: LEDs dauerhaft an
 * 12 - 23:59 Uhr: LEDs blinken.
 */
  boolean Renderer::pollDisplayBlinkAfternoon() {
  boolean change = false;
  if (_isAfternoon) {
      if (millis() - _lastDisplayBlinkChange >= (unsigned long) 1000 / LEDS_FREQ_TIMESET_ON_PM) {
          _lastDisplayBlinkChange = millis();
          change = true;
      }
  } else {
      if (!_isDisplayOn_DisplayBlinking) 
          change = true;
  }
  if (change)
      _isDisplayOn_DisplayBlinking = !_isDisplayOn_DisplayBlinking;
  return change;
}

void Renderer::clearScreenIfNeeded_DisplayOnBlinking(word matrix[16]) {
    if (!_isDisplayOn_DisplayBlinking)
        clearScreenBuffer(matrix);
}

void Renderer::setAfternoon(byte hours) {
    _isAfternoon = (hours >= 12);
}

/**
 * Setzt die Wortminuten, je nach hours/minutes.
 */
void Renderer::setMinutes(byte hours, byte minutes, byte language, word matrix[16]) {
    byte minutes_5 = minutes / 5;

    switch (language) {
        #ifdef SPRACHE_DE
                //
                // Deutsch (Hochdeutsch, Schwaebisch, Bayrisch, Saechsisch)
                //
            case LANGUAGE_DE_DE:
            case LANGUAGE_DE_SW:
            case LANGUAGE_DE_BA:
            case LANGUAGE_DE_SA:
                DE_ESIST;
    
                switch (minutes_5) {
                    case 0:
                        // glatte Stunde
                        setHours(hours, 0, language, matrix);
                        break;
                    case 1:
                        // 5 nach
                        DE_FUENF;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 2:
                        // 10 nach
                        DE_ZEHN;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 3:
                        // viertel nach
                        DE_VIERTEL;
                        if ((language == LANGUAGE_DE_SW) || (language == LANGUAGE_DE_SA))
                            setHours(hours + 1, 3, language, matrix);
                        else
                            setHours(hours, 2, language, matrix);
                        break;
                    case 4:
                        // 20 nach
                        if (language == LANGUAGE_DE_SA) {
                            DE_ZEHN;
                            DE_HALB;
                            setHours(hours + 1, 1, language, matrix);
                        } else {
                            DE_ZWANZIG;
                            setHours(hours, 2, language, matrix);
                        }
                        break;
                    case 5:
                        // 5 vor halb
                        DE_FUENF;
                        DE_HALB;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 6:
                        // halb
                        DE_HALB;
                        setHours(hours + 1, 3, language, matrix);
                        break;
                    case 7:
                        // 5 nach halb
                        DE_FUENF;
                        DE_HALB;
                        setHours(hours + 1, 2, language, matrix);
                        break;
                    case 8:
                        // 20 vor
                        if (language == LANGUAGE_DE_SA) {
                            DE_ZEHN;
                            DE_HALB;
                            setHours(hours + 1, 2, language, matrix);
                        } else {
                            DE_ZWANZIG;
                            setHours(hours + 1, 1, language, matrix);
                        }
                        break;
                    case 9:
                        // viertel vor
                        if ((language == LANGUAGE_DE_SW) || (language == LANGUAGE_DE_BA) || (language == LANGUAGE_DE_SA)) {
                            DE_DREIVIERTEL;
                            setHours(hours + 1, 3, language, matrix);
                        } else {
                            DE_VIERTEL;
                            setHours(hours + 1, 1, language, matrix);
                        }
                        break;
                    case 10:
                        // 10 vor
                        DE_ZEHN;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 11:
                        // 5 vor
                        DE_FUENF;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_CH
                //
                // Schweiz: Berner-Deutsch
                //
            case LANGUAGE_CH:
                CH_ESISCH;
    
                switch (minutes_5) {
                    case 0:
                        // glatte Stunde
                        setHours(hours, 0, language, matrix);
                        break;
                    case 1:
                        // 5 ab
                        CH_FUEF;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 2:
                        // 10 ab
                        CH_ZAEAE;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 3:
                        // viertel ab
                        CH_VIERTU;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 4:
                        // 20 ab
                        CH_ZWAENZG;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 5:
                        // 5 vor halb
                        CH_FUEF;
                        CH_HAUBI;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 6:
                        // halb
                        CH_HAUBI;
                        setHours(hours + 1, 3, language, matrix);
                        break;
                    case 7:
                        // 5 ab halb
                        CH_FUEF;
                        CH_HAUBI;
                        setHours(hours + 1, 2, language, matrix);
                        break;
                    case 8:
                        // 20 vor
                        CH_ZWAENZG;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 9:
                        // viertel vor
                        CH_VIERTU;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 10:
                        // 10 vor
                        CH_ZAEAE;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 11:
                        // 5 vor
                        CH_FUEF;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_EN
                //
                // Englisch
                //
            case LANGUAGE_EN:
                EN_ITIS;
    
                switch (minutes_5) {
                    case 0:
                        // glatte Stunde
                        setHours(hours, 0, language, matrix);
                        break;
                    case 1:
                        // 5 nach
                        EN_FIVE;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 2:
                        // 10 nach
                        EN_TEN;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 3:
                        // viertel nach
                        EN_A;
                        EN_QUATER;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 4:
                        // 20 nach
                        EN_TWENTY;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 5:
                        // 5 vor halb
                        EN_TWENTY;
                        EN_FIVE;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 6:
                        // halb
                        EN_HALF;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 7:
                        // 5 nach halb
                        EN_TWENTY;
                        EN_FIVE;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 8:
                        // 20 vor
                        EN_TWENTY;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 9:
                        // viertel vor
                        EN_A;
                        EN_QUATER;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 10:
                        // 10 vor
                        EN_TEN;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 11:
                        // 5 vor
                        EN_FIVE;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_FR
                //
                // Franzoesisch
                //
            case LANGUAGE_FR:
                FR_ILEST;
    
                switch (minutes_5) {
                    case 0:
                        // glatte Stunde
                        setHours(hours, 0, language, matrix);
                        break;
                    case 1:
                        // 5 nach
                        setHours(hours, 0, language, matrix);
                        FR_CINQ;
                        break;
                    case 2:
                        // 10 nach
                        setHours(hours, 0, language, matrix);
                        FR_DIX;
                        break;
                    case 3:
                        // viertel nach
                        setHours(hours, 0, language, matrix);
                        FR_ET;
                        FR_QUART;
                        break;
                    case 4:
                        // 20 nach
                        setHours(hours, 0, language, matrix);
                        FR_VINGT;
                        break;
                    case 5:
                        // 5 vor halb
                        setHours(hours, 0, language, matrix);
                        FR_VINGT;
                        FR_TRAIT;
                        FR_CINQ;
                        break;
                    case 6:
                        // halb
                        setHours(hours, 0, language, matrix);
                        FR_ET;
                        FR_DEMI;
                        break;
                    case 7:
                        // 5 nach halb
                        setHours(hours + 1, 0, language, matrix);
                        FR_MOINS;
                        FR_VINGT;
                        FR_TRAIT;
                        FR_CINQ;
                        break;
                    case 8:
                        // 20 vor
                        setHours(hours + 1, 0, language, matrix);
                        FR_MOINS;
                        FR_VINGT;
                        break;
                    case 9:
                        // viertel vor
                        setHours(hours + 1, 0, language, matrix);
                        FR_MOINS;
                        FR_LE;
                        FR_QUART;
                        break;
                    case 10:
                        // 10 vor
                        setHours(hours + 1, 0, language, matrix);
                        FR_MOINS;
                        FR_DIX;
                        break;
                    case 11:
                        // 5 vor
                        setHours(hours + 1, 0, language, matrix);
                        FR_MOINS;
                        FR_CINQ;
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_IT
                //
                // Italienisch
                //
            case LANGUAGE_IT:
                switch (minutes_5) {
                    case 0:
                        // glatte Stunde
                        setHours(hours, 0, language, matrix);
                        break;
                    case 1:
                        // 5 nach
                        IT_CINQUE;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 2:
                        // 10 nach
                        IT_DIECI;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 3:
                        // viertel nach
                        IT_UN;
                        IT_QUARTO;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 4:
                        // 20 nach
                        IT_VENTI;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 5:
                        // 5 vor halb
                        IT_VENTI;
                        IT_CINQUE;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 6:
                        // halb
                        IT_MEZZA;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 7:
                        // 5 nach halb
                        IT_VENTI;
                        IT_CINQUE;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 8:
                        // 20 vor
                        IT_VENTI;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 9:
                        // viertel vor
                        IT_UN;
                        IT_QUARTO;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 10:
                        // 10 vor
                        IT_DIECI;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 11:
                        // 5 vor
                        IT_CINQUE;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_NL
                //
                // Niederlaendisch
                //
            case LANGUAGE_NL:
                NL_HETIS;
    
                switch (minutes_5) {
                    case 0:
                        // glatte Stunde
                        setHours(hours, 0, language, matrix);
                        break;
                    case 1:
                        // 5 nach
                        NL_VIJF;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 2:
                        // 10 nach
                        NL_TIEN;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 3:
                        // viertel nach
                        NL_KWART;
                        NL_OVER2;
                        setHours(hours, 3, language, matrix);
                        break;
                    case 4:
                        // 10 vor halb
                        NL_TIEN;
                        NL_HALF;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 5:
                        // 5 vor halb
                        NL_VIJF;
                        NL_HALF;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 6:
                        // halb
                        NL_HALF;
                        setHours(hours + 1, 3, language, matrix);
                        break;
                    case 7:
                        // 5 nach halb
                        NL_VIJF;
                        NL_HALF;
                        setHours(hours + 1, 2, language, matrix);
                        break;
                    case 8:
                        // 20 vor
                        NL_TIEN;
                        NL_HALF;
                        setHours(hours + 1, 2, language, matrix);
                        break;
                    case 9:
                        // viertel vor
                        NL_KWART;
                        NL_VOOR2;
                        setHours(hours + 1, 3, language, matrix);
                        break;
                    case 10:
                        // 10 vor
                        NL_TIEN;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 11:
                        // 5 vor
                        NL_VIJF;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_ES
                //
                // Spanisch
                //
            case LANGUAGE_ES:
                switch (minutes_5) {
                    case 0:
                        // glatte Stunde
                        setHours(hours, 0, language, matrix);
                        break;
                    case 1:
                        // 5 nach
                        ES_CINCO;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 2:
                        // 10 nach
                        ES_DIEZ;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 3:
                        // viertel nach
                        ES_CUARTO;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 4:
                        // 20 nach
                        ES_VEINTE;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 5:
                        // 5 vor halb
                        ES_VEINTICINCO;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 6:
                        // halb
                        ES_MEDIA;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 7:
                        // 5 nach halb
                        ES_VEINTICINCO;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 8:
                        // 20 vor
                        ES_VEINTE;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 9:
                        // viertel vor
                        ES_CUARTO;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 10:
                        // 10 vor
                        ES_DIEZ;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 11:
                        // 5 vor
                        ES_CINCO;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_PT
            //
            // Portugiesisch
            //
            case LANGUAGE_PT:
                switch (minutes / 5) {
                    case 0:
                        // glatte Stunde
                        setHours(hours, 0, language, matrix);
                        break;
                    case 1:
                        // 5 nach
                        PT_CINCO;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 2:
                        // 10 nach
                        PT_DEZ;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 3:
                        // viertel nach
                        PT_UM_QUARTO;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 4:
                        // 20 nach
                        PT_VINTE;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 5:
                        // 25 nach
                        PT_VINTE;
                        PT_E_CINCO;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 6:
                        // halb nach
                        PT_MEIA;
                        setHours(hours, 2, language, matrix);
                        break;
                    case 7:
                        // 25 vor
                        PT_VINTE;
                        PT_E_CINCO;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 8:
                        // 20 vor
                        PT_VINTE;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 9:
                        // viertel vor
                        PT_UM_QUARTO;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 10:
                        // 10 vor
                        PT_DEZ;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                    case 11:
                        // 5 vor
                        PT_CINCO;
                        setHours(hours + 1, 1, language, matrix);
                        break;
                }
                break;
        #endif
    }
}

/**
 * Setzt die Stunden, je nach hours. 'glatt' bedeutet,
 * es ist genau diese Stunde und wir muessen 'UHR'
 * dazuschreiben und EIN statt EINS, falls es 1 ist.
 * (Zumindest im Deutschen).
 * Andere sprachliche Spezialfaelle kommen weiter unten
 * im Code...
 */
void Renderer::setHours(byte hours, byte glatt_vor_nach, byte language, word matrix[16]) {
    byte hours12 = hours % 12;
    switch (language) {
        #ifdef SPRACHE_DE
                //
                // Deutsch (Hochdeutsch, Schwaebisch, Bayrisch, Saechsisch)
                //
            case LANGUAGE_DE_DE:
            case LANGUAGE_DE_SW:
            case LANGUAGE_DE_BA:
            case LANGUAGE_DE_SA:
                switch (glatt_vor_nach) {
                    case 0: DE_UHR; break;
                    case 1: DE_VOR; break;
                    case 2: DE_NACH; break;
                }
    
                switch (hours12) {
                    case 0:
                        DE_H_ZWOELF;
                        break;
                    case 1:
                        if (glatt_vor_nach) {
                            DE_H_EINS;
                        } else {
                            DE_H_EIN;
                        }
                        break;
                    case 2:
                        DE_H_ZWEI;
                        break;
                    case 3:
                        DE_H_DREI;
                        break;
                    case 4:
                        DE_H_VIER;
                        break;
                    case 5:
                        DE_H_FUENF;
                        break;
                    case 6:
                        DE_H_SECHS;
                        break;
                    case 7:
                        DE_H_SIEBEN;
                        break;
                    case 8:
                        DE_H_ACHT;
                        break;
                    case 9:
                        DE_H_NEUN;
                        break;
                    case 10:
                        DE_H_ZEHN;
                        break;
                    case 11:
                        DE_H_ELF;
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_CH
                //
                // Schweiz: Berner-Deutsch
                //
            case LANGUAGE_CH:
                switch (glatt_vor_nach) {
                    case 1: CH_VOR; break;
                    case 2: CH_AB; break;
                }
                switch (hours12) {
                    case 0:
                        CH_H_ZWOEUFI;
                        break;
                    case 1:
                        CH_H_EIS;
                        break;
                    case 2:
                        CH_H_ZWOEI;
                        break;
                    case 3:
                        CH_H_DRUE;
                        break;
                    case 4:
                        CH_H_VIER;
                        break;
                    case 5:
                         CH_H_FUEFI;
                        break;
                    case 6:
                        CH_H_SAECHSI;
                        break;
                    case 7:
                        CH_H_SIEBNI;
                        break;
                    case 8:
                        CH_H_ACHTI;
                        break;
                    case 9:
                        CH_H_NUENI;
                        break;
                    case 10:
                        CH_H_ZAENI;
                        break;
                    case 11:
                        CH_H_EUFI;
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_EN
                //
                // Englisch
                //
            case LANGUAGE_EN:
                switch (glatt_vor_nach) {
                    case 0: EN_OCLOCK; break;
                    case 1: EN_TO; break;
                    case 2: EN_PAST; break;
                }
                switch (hours12) {
                    case 0:
                        EN_H_TWELVE;
                        break;
                    case 1:
                        EN_H_ONE;
                        break;
                    case 2:
                        EN_H_TWO;
                        break;
                    case 3:
                        EN_H_THREE;
                        break;
                    case 4:
                        EN_H_FOUR;
                        break;
                    case 5:
                        EN_H_FIVE;
                        break;
                    case 6:
                        EN_H_SIX;
                        break;
                    case 7:
                        EN_H_SEVEN;
                        break;
                    case 8:
                        EN_H_EIGHT;
                        break;
                    case 9:
                        EN_H_NINE;
                        break;
                    case 10:
                        EN_H_TEN;
                        break;
                    case 11:
                        EN_H_ELEVEN;
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_FR
                //
                // Franzoesisch
                //
            case LANGUAGE_FR:
                /*
                 *  Sprachlicher Spezialfall fuer Franzoesisch.
                 */
                if (hours12 == 1) {
                    FR_HEURE;
                } else if (hours12 != 0) {
                    // MIDI / MINUIT ohne HEURES
                    FR_HEURES;
                }
                switch (hours) {
                    case 0:
                    case 24:
                        FR_H_MINUIT;
                        break;
                    case 12:
                        FR_H_MIDI;
                        break;
                }
                switch (hours12) {
                    case 1:
                        FR_H_UNE;
                        break;
                    case 2:
                        FR_H_DEUX;
                        break;
                    case 3:
                        FR_H_TROIS;
                        break;
                    case 4:
                        FR_H_QUATRE;
                        break;
                    case 5:
                        FR_H_CINQ;
                        break;
                    case 6:
                        FR_H_SIX;
                        break;
                    case 7:
                        FR_H_SEPT;
                        break;
                    case 8:
                        FR_H_HUIT;
                        break;
                    case 9:
                        FR_H_NEUF;
                        break;
                    case 10:
                        FR_H_DIX;
                        break;
                    case 11:
                        FR_H_ONZE;
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_IT
                //
                // Italienisch
                //
            case LANGUAGE_IT:
                switch (glatt_vor_nach) {
                    case 1: IT_E2; break;
                    case 2: IT_MENO; break;
                }
                /*
                 *  Sprachlicher Spezialfall fuer Italienisch.
                 */
                if (hours12 == 1) {
                    IT_E;
                } else {
                    IT_SONOLE;
                }
                switch (hours12) {
                    case 0:
                        IT_H_DODICI;
                        break;
                    case 1:
                        IT_H_LUNA;
                        break;
                    case 2:
                        IT_H_DUE;
                        break;
                    case 3:
                        IT_H_TRE;
                        break;
                    case 4:
                        IT_H_QUATTRO;
                        break;
                    case 5:
                        IT_H_CINQUE;
                        break;
                    case 6:
                        IT_H_SEI;
                        break;
                    case 7:
                        IT_H_SETTE;
                        break;
                    case 8:
                        IT_H_OTTO;
                        break;
                    case 9:
                        IT_H_NOVE;
                        break;
                    case 10:
                        IT_H_DIECI;
                        break;
                    case 11:
                        IT_H_UNDICI;
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_NL
                //
                // Niederlaendisch
                //
            case LANGUAGE_NL:
                switch (glatt_vor_nach) {
                    case 0: NL_UUR; break;
                    case 1: NL_VOOR; break;
                    case 2: NL_OVER; break;
                }
    
                switch (hours12) {
                    case 0:
                        NL_H_TWAALF;
                        break;
                    case 1:
                        NL_H_EEN;
                        break;
                    case 2:
                        NL_H_TWEE;
                        break;
                    case 3:
                        NL_H_DRIE;
                        break;
                    case 4:
                        NL_H_VIER;
                        break;
                    case 5:
                        NL_H_VIJF;
                        break;
                    case 6:
                        NL_H_ZES;
                        break;
                    case 7:
                        NL_H_ZEVEN;
                        break;
                    case 8:
                        NL_H_ACHT;
                        break;
                    case 9:
                        NL_H_NEGEN;
                        break;
                    case 10:
                        NL_H_TIEN;
                        break;
                    case 11:
                        NL_H_ELF;
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_ES
                //
                // Spanisch
                //
            case LANGUAGE_ES:
                switch (glatt_vor_nach) {
                    case 1: ES_MENOS; break;
                    case 2: ES_Y; break;
                }
                /*
                 *  Sprachlicher Spezialfall fuer Spanisch.
                 */
                if (hours12 == 1) {
                    ES_ESLA;
                } else {
                    ES_SONLAS;
                }
                switch (hours12) {
                    case 0:
                        ES_H_DOCE;
                        break;
                    case 1:
                        ES_H_UNA;
                        break;
                    case 2:
                        ES_H_DOS;
                        break;
                    case 3:
                        ES_H_TRES;
                        break;
                    case 4:
                        ES_H_CUATRO;
                        break;
                    case 5:
                        ES_H_CINCO;
                        break;
                    case 6:
                        ES_H_SEIS;
                        break;
                    case 7:
                        ES_H_SIETE;
                        break;
                    case 8:
                        ES_H_OCHO;
                        break;
                    case 9:
                        ES_H_NUEVE;
                        break;
                    case 10:
                        ES_H_DIEZ;
                        break;
                    case 11:
                        ES_H_ONCE;
                        break;
                }
                break;
        #endif
        #ifdef SPRACHE_PT
            //
            // Portugiesisch
            //
            case LANGUAGE_PT:
                switch (glatt_vor_nach) {
                    case 0: if (hours12 == 1) {
                                PT_H_HORA;
                            } else if (hours12 != 0) {
                                // MEIODIA / MEIANOITE ohne HORAS
                                PT_H_HORAS;
                            }
                            break;
                    case 1: PT_MENOS; break;
                    case 2: PT_EEINS; break;
                }
                /*
                 *  Sprachlicher Spezialfall fuer Portugiesisch.
                 */
                if ((hours12 == 0) || (hours12 == 1)) {
                    PT_E;
                } else {
                    PT_SAO;
                }
                
                switch (hours) {
                    case 0:
                    case 24:
                        PT_H_MEIANOITE;
                        break;
                    case 12:
                        PT_H_MEIODIA;
                        break;
                }
                switch (hours12) {
                    case 1:
                        PT_H_UMA;
                        break;
                    case 2:
                        PT_H_DUAS;
                        break;
                    case 3:
                        PT_H_TRES;
                        break;
                    case 4:
                        PT_H_QUATRO;
                        break;
                    case 5:
                        PT_H_CINCO;
                        break;
                    case 6:
                        PT_H_SEIS;
                        break;
                    case 7:
                        PT_H_SETE;
                        break;
                    case 8:
                        PT_H_OITO;
                        break;
                    case 9:
                        PT_H_NOVE;
                        break;
                    case 10:
                        PT_H_DEZ;
                        break;
                    case 11:
                        PT_H_ONZE;
                        break;
                }
                break;
        #endif
    }
}

/**
 * Setzt die vier Punkte in den Ecken, je nach minutes % 5 (Rest).
 *
 * @param ccw: TRUE -> clock wise -> im Uhrzeigersinn.
 *             FALSE -> counter clock wise -> gegen den Uhrzeigersinn.
 */
void Renderer::setCorners(byte minutes, boolean cw, word matrix[16]) {
    byte b_minutes = minutes % 5;
    for (byte i = 0; i < b_minutes; i++) {
        byte j;
        if (cw) {
          // j: 1, 0, 3, 2
          j = (1 - i + 4) % 4;
        } else {
          // j: 0, 1, 2, 3
          j = i;
        }
        #ifdef USE_INDIVIDUAL_CATHODES
            matrix[j] |= (0b0000000000010000 >> j);
        #else
            matrix[j] |= 0b0000000000011111;
        #endif
    }
}

/**
 * Schalte die Alarm-LED ein
 */
void Renderer::activateAlarmLed(word matrix[16]) {
    #ifdef USE_INDIVIDUAL_CATHODES
        matrix[4] |= 0b0000000000000001;
    #else
        matrix[4] |= 0b0000000000011111;
    #endif 
}

/**
 * Im Alarm-Einstell-Modus muessen bestimmte Woerter weg, wie z.B. "ES IST" im Deutschen.
 */
void Renderer::cleanIntroWords(byte language, word matrix[16]) {
    switch (language) {
        #ifdef SPRACHE_DE
            case LANGUAGE_DE_DE:
            case LANGUAGE_DE_SW:
            case LANGUAGE_DE_BA:
            case LANGUAGE_DE_SA:
                matrix[0] &= 0b0010001111111111; // ES IST weg
                break;
        #endif
        #ifdef SPRACHE_CH
            case LANGUAGE_CH:
                matrix[0] &= 0b0010000111111111; // ES ISCH weg
                break;
        #endif
        #ifdef SPRACHE_EN
            case LANGUAGE_EN:
                matrix[0] &= 0b0010011111111111; // IT IS weg
                break;
        #endif
        #ifdef SPRACHE_FR
            case LANGUAGE_FR:
                matrix[0] &= 0b0010001111111111; // IL EST weg
                break;
        #endif
        #ifdef SPRACHE_IT
            case LANGUAGE_IT:
                matrix[0] &= 0b0000100111111111; // SONO LE weg
                matrix[1] &= 0b0111111111111111; // E (L'UNA) weg
                break;
        #endif
        #ifdef SPRACHE_NL
            case LANGUAGE_NL:
                matrix[0] &= 0b0001001111111111; // HET IS weg
                break;
        #endif
        #ifdef SPRACHE_ES
            case LANGUAGE_ES:
                matrix[0] &= 0b1000100011111111; // SON LAS weg
                matrix[0] &= 0b0011100111111111; // ES LA weg
                break;
        #endif
        #ifdef SPRACHE_PT
            case LANGUAGE_PT:
                matrix[0] |= 0b0111111111111111; // E weg
                matrix[0] |= 0b1000111111111111; // SAO weg
                break;
        #endif
    }
}
