/**
 * Ereignisse
 * Definition der Zeichen, die am definierten Tag statt
 * des Datums in der Datumsanzeige angezeigt werden.
 * Die Zeichen sind wie die Woerter Bitmasken fuer die Matrix.
 *
 * @mc       Arduino/RBBB
 * @autor    Andreas Müller / raffix _AT_ web _DOT_ de
 * @version  1.2
 * @created  13.03.2016
 * @updated  05.06.2016
 *
 * Versionshistorie:
 * V 1.1:  - Startzeit und -dauer für einen Countdown hinzugefügt.
 * V 1.2:  - Startzeit eines Countdown geändert in die Zielzeit..
 */
#ifndef EREIGNISSE_H
#define EREIGNISSE_H

#include <avr/pgmspace.h>

/*
 * Beispiel für einen Eintrag:
 * 1,1,1,1,0,0,15
 * -- Symbol in der Datumsanzeige --
 * Die ersten beiden Ziffern (1,1) geben an, dass am 01.01. das
 * definierte Symbol ganztägig in der Datumsanzeige angezeigt wird.
 * Die restlichen 5 Zahlen sind nur für den Countdown relevant.
 * -- Countdown --
 * Neujahrs-Countdown 15 Minuten lang:
 * Die letzten 5 Ziffern (1,1,0,0,15) bedeuten, der Countdown soll
 * 15 Minuten vor dem 01.01. um 0:00 Uhr beginnen. 0:00 Uhr ist also
 * die Zielzeit. Der Countdown startet folglich am 31.12. um 23:45 Uhr.
 * Ist der Countdown beendet, wird das definierte Symbol blinkend
 * COUNTDOWN_BLINK_DURATION Sekunden lang angezeigt.
 * Die Zeit ist in der Configuration.h anpassbar.
 */
struct EventdayObject {
  byte day;
  byte month;
  // countdown* gibt an, wohin der Countdown zählen soll
  byte countdownDay;
  byte countdownMonth;
  byte countdownHour;
  byte countdownMinute;
  // Wie lange soll der Countdown laufen?
  byte countdownMinutes;
  int symbol[10];
};

const EventdayObject eventdayObject[] PROGMEM = {
    { 1,
      1,
      1,
      1,
      0,
      0,
      15,
      {
        // Wird automatisch berechnet, sofern in Configuration.h so definiert.
        0b00000000000,
        0b00000000000,
        0b00000000000,
        0b00000000000,
        0b00000000000,
        0b00000000000,
        0b00000000000,
        0b00000000000,
        0b00000000000,
        0b00000000000,
      }
    }
    ,
    { 2,
      5,
      2,
      5,
      0,
      0,
      15,
      {
        0b00000000000,
        0b00010001000,
        0b00101010100,
        0b01000100010,
        0b01000000010,
        0b00100000100,
        0b00010001000,
        0b00001010000,
        0b00000100000,
        0b00000000000
      }
    }
    ,
    { 6,
	    5,
      6,
      5,
      0,
      0,
      15,
	    {
        0b00000000000,
        0b00000000000,
	      0b00100100100,
       	0b00010101000,
        0b00001110000,
        0b00000100000,
        0b00000100000,
        0b00000100000,
        0b00000000000,
        0b00000000000
	    }
    }
    ,
    { 5,
      6,
      5,
      6,
      0,
      0,
      15,
      {
        0b00000000000,
        0b00000000000,
        0b00100000100,
        0b00110001100,
        0b00111011100,
        0b00111011100,
        0b00011111000,
        0b00001110000,
        0b00000000000,
        0b00000001110
      }
    }
    ,
    { 23,
	    8,
      23,
      8,
      0,
      0,
      15,
	    {
        0b00000000000,
        0b00010101000,
        0b00010101000,
        0b01000000010,
        0b01001110010,
        0b00010001000,
        0b00010001000,
        0b00010001000,
        0b00001110000,
        0b00000000000
	    }
    }
};

#endif
