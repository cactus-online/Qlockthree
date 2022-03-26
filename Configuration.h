/**
 * Configuration
 * Die Compiler-Einstellungen der Firmware an einer zentralen Stelle.
 *
 * @mc       Arduino/RBBB
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.5h
 * @created  23.1.2013
 * @updated  28.06.2016 (Ergänzungen und Änderungen von A. Mueller)
 *
 * Versionshistorie:
 * V 1.0:  - Erstellt
 * V 1.1:  - ENABLE_LDR und ENABLE_DCF entfernt.
 * V 1.2:  - PWM_DURATION von 50 auf 75 erhoeht.
 * V 1.3:  - MAX_BRIGHTNESS entfernt. Der LDR kann jetzt individuell skalieren.
 *         - PWM_DURATION zurueck auf 50.
 * V 1.4:  - ENABLE_ALARM jetzt im EEPROM.
 *         - PWM_DURATION an neue LDR-Klasse angepasst.
 *         - DCF77_SIGNAL_IS_INVERTED jetzt im EEPROM.
 * V 1.5:  - Diverse Config-Moeglichkeiten fuer die verschiedenen LED-Driver eingefuehrt.
 * V 1.5a: - Optionen für die neue DCF77-Synchronisation hinzugefuegt.
 * V 1.5b: - Schalter für Zusatzoptionen von Qlockthree.ino hierher verlegt
 * V 1.5c: - Schalter für Timer2 entfernt, da kein Timer mehr für die Driftkorrektur benötigt wird
 * V 1.5d: - Schalter für den 5-Tasten-Wortwecker und Konfigurationsmoeglichkeiten dazu
 *         - Diverse Konfigurationsmoeglichkeiten für Weckfunktion und den LDR-gesteuerten Nachtmodus
 * V 1.5e: - Einstellung für die Blinkfrequenz der LEDs bei Einstellen der Uhrzeit/Weckzeit hinzugefügt.
 * V 1.5f: - Option für einen 5-Tasten-Wortwecker ohne Näherungssensor hinzugefügt, AUTO_JUMP_BLANK und
 *           AUTO_JUMP_TIME entfernt. Einige Optionen umbenannt und neu sortiert.
 *           DCF77_SENSOR_EXISTS und DATE_ENABLE neu hinzugefügt.
 * V 1.5g: - Option EVENTDAY_CALCULATE_NEW_YEARS_EVE neu hinzugefügt.
 * V 1.5h: - Optionen hinzugefügt um nichtbenötige Sprachen weglassen zu können.
 *         - Option für die Null mit Strich hinzugefügt.
 */
#ifndef CONFIGURATION_H
#define CONFIGURATION_H


/**
 * Schalter für Sprachen
 * 
 * Leider wird später teileweise LANGUAGE_* verwendet, lieber nicht auskommentieren
 */
#define SPRACHE_DE
#define SPRACHE_CH
#define SPRACHE_EN
#define SPRACHE_FR
#define SPRACHE_IT
#define SPRACHE_NL
#define SPRACHE_ES
#define SPRACHE_PT

/**
 ** Zusätzliche Schalter, um bestimmte Grund- und Zusatzfunktionen ein- bzw. auszuschalten oder einzustellen.
 **/

/* 
 * Grundfunktionen der Wortuhr ein- bzw. ausschalten 
 */
// Alarmfunktion aktivieren / deaktivieren (Option muss beim 5-Tasten-Wortwecker zwingend aktiviert sein!)
// (Standard: eingeschaltet)
#define ALARM_OPTION_ENABLE

// Schaltet für jede Eckled und das Alarmsymbol (Glockensymbol) nur die dazu passende Kathode und
// nicht immer alle ein. Dies verhindert das Glimmen bei richtiger Verdrahtung. (Standard: ausgeschaltet)
//#define USE_INDIVIDUAL_CATHODES

// Ist ein DCF77-Empfänger eingebaut? Dann wird eine Menüseite zur Anzeige der vergangenen Stunden und Minuten
// seit der letzten erfolgreichen DCF-Synchronisation im Menü EXT_MODE_DCF_SYNC angezeigt (Standard: eingeschaltet)
#define DCF77_SENSOR_EXISTS

// Diese Option aktiviert die Datumsanzeige. (Standard: eingeschaltet)
// Hinweis: Diese Einstellung ist nur bei richtig eingestelltem Datum (z.B. via DCF77) sinnvoll.
#define DATE_ENABLE
// Mit dieser Option wird zusätzlich zum Tag noch der Monat angezeigt. (Standard: ausgeschaltet)
//#define DATE_MONTH_SHOW

// Zeitdauer (in s) bis automatisch von der Sekunden- und Datumsanzeige zurück auf die Uhrzeit gesprungen wird.
// Eine Dauer von 0 bedeutet: Kein automatischer Rücksprung. (Standard: 10)
// #define TIMEOUT_JUMP_TO_TIME 10
#define TIMEOUT_JUMP_TO_TIME 30

// Zeigt die Helligkeit in Prozent und nicht mit Balken an. (Standard: ausgeschaltet)
#define BRIGHTNESS_SETTING_IN_PERCENTAGE

// Zeigt die große Null wieder mit schrägem Strich an. (Standard: ausgeschaltet)
// #define NULL_TECHNICAL
#define NULL_TECHNICAL

/*
 * Zusatzfunktionen der Wortuhr ein- bzw. ausschalten und einstellen
 */
// Zeigt bei besonderem Ereignis ein Symbol in der Datumsanzeige statt dem Datum. (Standard: eingeschaltet)
// Hinweis: Diese Einstellung ist nur bei richtig eingestelltem Datum (z.B. via DCF77) sinnvoll. 
#define EVENTDAY

// Ist in der Ereignisse.h als erstes Ereignis (Eintrag 0) der Neujahrscountdown hinterlegt, kann mit deser
// Einstellung die Jahreszahl des neuen Jahres automatisch berechnet und angezeigt werden. (Standard: eingeschaltet)
// Hinweis: Einstellung nur wählen, wenn Eintrag 0 tatsächlich der Neujahrscountdown ist!
#define EVENTDAY_CALCULATE_NEW_YEARS_EVE

// Zeigt einen Countdown (und am Ende ein blinkendes Symbol) zu einem Ereignis an. (Standard: eingeschaltet)
// Hinweis: Diese Einstellung ist nur bei richtig eingestelltem Datum (z.B. via DCF77) sinnvoll. 
// (Achtung!: #define EVENTDAY wird benötigt!) 
#define COUNTDOWN

// Wenn ein Countdown zu einem Ereignis definiert wurde, automatisch (von der Uhrzeit) auf die Countdown-Ansicht springen. (Standard: eingeschaltet).
#define COUNTDOWN_AUTO_JUMP_TO

// Wie lange (in s) soll das definierte Symbol nach Ablauf des Countdowns blinken? (Standard: 20)
#define COUNTDOWN_BLINK_DURATION 20

// FPS im Debug-Modus anzeigen (Standard: eingeschaltet).
//#define FPS_SHOW_DEBUG


/*
 * Wortwecker-Funktionen
 */
// 5-Tasten-Wortwecker-Funktionen aktivieren. (Standard: ausgeschaltet)
// (Achtung!: #define ALARM_OPTION_ENABLE wird benötigt, diese Einstellung befindet weiter oben!) 
//#define WW_5_BUTTONS
#define WW_5_BUTTONS

// Hat der 5-Tasten-Wortwecker einen Näherungssensor eingebaut? (Standard: eingeschaltet)
#define WW_5_BUTTONS_NEAR_SENSOR_ENABLE

// Zeitdauer in ms, die das Display mindestens an bleibt, wenn eine Bewegung vom Näherungssensor detektiert wurde. (Standard: 4000)
// #define WW_5_BUTTONS_NEAR_SENSOR_DELAY 4000
#define WW_5_BUTTONS_NEAR_SENSOR_DELAY 8000

// Schaltet den DCF77-Empfänger aus und den Näherungssensor ein, sobald in den BLANK- oder LDR-BLANK-Modus gewechselt wird.
// Schaltet den Näherungssensor aus und den DCF77-Empfänger ein bei jedem anderen Modus (auch Nachtmodus). Sobald im Nachtmodus ein gültiges
// Zeitzeichensignal empfangen wurde, wird der Näherungssensor wieder eingeschaltet und der DCF77-Empfänger wieder ausgeschaltet. (Standard: eingeschaltet)
// (Achtung!: #define DCF77_SENSOR_EXISTS wird benötigt, diese Einstellung befindet sich weiter oben!) 
#define WW_5_BUTTONS_ENABLE_NEARSENSOR_A0


/*
 * Alarmfunktion eingeschaltet?
 * Dazu muss ein Lautsprecher an D13 und GND und eine weitere 'Eck-LED' an die 5te Reihe.
 * Dann gibt es einen neuen Modus, direkt nach der normalen Zeitanzeige. Die neue LED
 * blinkt nach dem Moduswechsel und der Alarm ist eingeschaltet. Drueckt man jetzt M+ oder H+ stellt man
 * die Alarmzeit ein, angedeutet durch die blinkende Alarm-LED. Drueckt man SHOW_ALARM_TIME_TIMER Sekunden
 * keine Taste, hoert das Blinken auf und die normale Zeit wird wieder angezeigt.
 * Bei erreichen des Alarms piept der Lautpsrecher auf D13. Zum Ausschalten muss der
 * Modus-Taster gedrueckt werden.
 * Weiter unten kommen drei weitere DEFINEs:
 * - SPEAKER ist der Pin, an dem der Lautsprecher haengt.
 * - SPEAKER_FREQUENCY ist die Tonhoehe, wenn der Speaker ein Lautpsrecher ist.
 * - MAX_BUZZ_TIME_IN_MINUTES: Nach dieser Zeitdauer schaltet sich der Wecker definitiv aus. (Standard: 5)
 * - SPEAKER_IS_BUZZER: wenn einkommentiert wird davon ausgegangen, dass am Pin SPEAKER ein Buzzer haengt (Reichelt: SUMMER TDB 05).
 * - ALARM_SHOW_TIME_IN_SECONDS: Zeit bis Alarm ein- oder ausgeschaltet wird und damit auch wie lange die Alarmeinstellung angezeigt wird.
 * - SNOOZE_TIME_IN_MINUTES: Wie lange der Alarm des Weckers schlummert bis er erneut ertönt. (Standard: 5)
 * - ALARM_LED_FREQ_SET_ALARM: Blinkfrequenz in Hz bei Einstellen des Alarms. (Standard: 1)
 * - ALARM_LED_FREQ_SNOOZE: Blinkfrequenz in Hz im Snooze-Modus. (Standard: 2)
 */
#define SPEAKER_FREQUENCY 200000
#define MAX_BUZZ_TIME_IN_MINUTES 10
#define SPEAKER_IS_BUZZER
#define ALARM_SHOW_TIME_IN_SECONDS 5
#define SNOOZE_TIME_IN_MINUTES 5
#define ALARM_LED_FREQ_SET_ALARM 1    
#define ALARM_LED_FREQ_SNOOZE 2       

/*
 * Die Status-LEDs koennen hier durch auskommentieren ausgeschaltet werden.
 * Default: eingeschaltet
 */
#define ENABLE_DCF_LED
#define ENABLE_SQW_LED

/*
 * Mit welcher Frequenz (in Hz) sollen die LEDs blinken,
 * wenn Zeiten >= 12 Uhr (Nachmittag) eingestellt werden?
 *  0 - 12 Uhr: LEDs dauerhaft an
 * 13 - 24 Uhr: LEDs blinken
 */
#define LEDS_FREQ_TIMESET_ON_PM 2

/*
 * Welcher LED-Treiber soll benutzt werden?
 */
#define LED_DRIVER_DEFAULT
// #define LED_DRIVER_UEBERPIXEL
// #define LED_DRIVER_POWER_SHIFT_REGISTER
// #define LED_DRIVER_NEOPIXEL
// #define LED_DRIVER_DOTSTAR
// #define LED_DRIVER_LPD8806

/*
 * Welche Uhr soll benutzt werden?
 */
#define DS1307
// #define DS3231

/*
 * Welche Fernbedienung soll benutzt werden?
 */
   #define REMOTE_NO_REMOTE
// #define REMOTE_SPARKFUN
// #define REMOTE_MOONCANDLES
// #define REMOTE_LUNARTEC


/*
 *
 * Seltener zu aendernde Einstellungen...
 *
 */

// ------------------ Default Display Driver ---------------------
/*
 * Dieser Schalter erhoeht die Helligkeit bei der Verwendung des Default-LED-Treibers (Shift-Register und UDN2981A),
 * indem er nicht beleuchtete Zeilen ueberspringt. (Tipp von Johannes)
 * Default: ausgeschaltet
 */
#define SKIP_BLANK_LINES

/*
 * Die Zeit, die mit LDR::MAX_BRIGHTNESS multipliziert auf
 * eine Zeile aufgeteilt wird in Mikrosekunden.
 * (default = 8)
 */
#define PWM_DURATION 8
/*
 * Dieser Schalter stellt die Anzeige auf den Kopf, falls man die Kabel beim Anoden-
 * multiplexer nicht kreuzen moechte oder es vergessen hat.
 * Default: ausgeschaltet
 */
// #define UPSIDE_DOWN
/*
 * Schnelle Datenausgabe?
 * Default: Eingeschaltet.
 */
#define SHIFTREGISTER_TURBO
/*
 * Noch optimiertes Schreiben von 0 und FFFF?
 * Default: Ausgeschaltet
 */
// #define OPTIMIZED_FOR_DARKNESS

// ------------------ Tasten ---------------------
/*
 * Die Zeit in Millisekunden, innerhalb derer Prellungen der Taster nicht als Druecken zaehlen.
 * (Und damit auch die Tastaturwiederholrate)
 * Default: 300
 */
#define BUTTON_THRESHOLD 300

// ------------------ DCF77-Empfaenger ---------------------
/*
 * Fuer wieviele DCF77-Samples muessen die Zeitabstaende stimmen, damit das DCF77-Telegramm als gueltig zaehlt?
 * Default: 3 (min. 3 Minuten Synchronisationszeit).
 */
#define DCF77HELPER_MAX_SAMPLES 2
/*
 * Bei einer Verwendung eines analogen Pins die naechsten zwei Zeilen einkommentieren.
 * Default: ausgeschaltet
 */
// #define MYDCF77_SIGNAL_IS_ANALOG
// #define MYDCF77_ANALOG_SIGNAL_TRESHOLD 600
/*
 * Die Timing-Werte fuer den DCF77-Empfaenger.
 * Default: 1700, 185, 85.
 * Entfaellt mit der V 3.4.0
 */
// #define MYDCF77_SYNC_PAUSE_TRESHOLD 1700
// #define MYDCF77_200MS_TRESHOLD 185
// #define MYDCF77_100MS_TRESHOLD 85
/*
 * Die Telegrammlaenge.
 * Default: 59
 */
#define MYDCF77_TELEGRAMMLAENGE 59
/*
 * Die Sekunde wird in die entsprechende Zahl
 * von Signalbins unterteilt, um das Wegdriften
 * der internen RTC quantifizieren zu können.
 * Dieser Wert entspricht einer Auflösung.
 * ACHTUNG! Wird diese Zahl zu groß gewählt (> 30), 
 * überschreitet sie die Frequenz der loop().
 * Eine Steigerung der Auflösung ist dann nicht mehr
 * möglich, die Drift wird beliebig und es kann 
 * keine Zeiteinstellung via DCF mehr erfolgen.
 * Werte < 4 sind ebenso sinnlos.
 * Default: 11  (sinnvoll: 5 - 25)
 */
#define MYDCF77_SIGNAL_BINS 11
/*
 * Dieser Wert gibt an wie viele Sekunden
 * die Drift bestimmt, bevor sie korrigiert wird.
 * ACHTUNG! Ein zu großer Wert verhindert eine
 * wirksame Driftkorrektur. Bei einem zu kleinen
 * Wert hingegen wird eine erfolgreiche
 * Zeitsynchronisation praktisch unmöglich.
 * Default: 30  (sinnvoll: 10 - 300)
 */
#define MYDCF77_DRIFT_CORRECTION_TIME 30
/*
 * Ist das Signal invertiert (z.B. ELV-Empfaenger)?
 * Default: ausgeschaltet.
 * Jetzt im EEPROM! Hier hat es keine Bedeutung mehr!
 * #define MYDCF77_SIGNAL_IS_INVERTED
 */

/*
 * Startwerte fuer den Mittelwert.
 */
#ifdef LED_DRIVER_DEFAULT
#define MYDCF77_MEANSTARTVALUE 7
#endif
#ifdef LED_DRIVER_UEBERPIXEL
#define MYDCF77_MEANSTARTVALUE 1300
#endif
#ifdef LED_DRIVER_POWER_SHIFT_REGISTER
#define MYDCF77_MEANSTARTVALUE 10
#endif
#ifdef LED_DRIVER_NEOPIXEL
#define MYDCF77_MEANSTARTVALUE 450
#endif
#ifdef LED_DRIVER_DOTSTAR
#define MYDCF77_MEANSTARTVALUE 1280
#endif
#ifdef LED_DRIVER_LPD8806
#define MYDCF77_MEANSTARTVALUE 1280
#endif
// ------------------ Lichtabhaengiger Widerstand ---------------------
/*
 * Sollen die Grenzwerte vom LDR automatisch angepasst werden? Bei einem Neustart der QlockTwo kann
 * das Display flackern, dann muss man einmal ueber den LDR 'wischen', damit er verschiedene
 * Messwerte bekommt. Ohne AUTOSCALE werden die manuellen Min-/Max-Werte genommen.
 * Default: eingeschaltet.
 */
#define LDR_AUTOSCALE

#define LDR_MANUAL_MIN 0
#define LDR_MANUAL_MAX 1023

/*
 * Verzögerung (in s) bis ein Über- oder Unterschreiten der LDR-Schwelle
 * den Wechsel zum Nacht- oder Tagmodus auslöst. (Standard: 10 | 10)
 */
#define LDR_TO_NIGHT_DELAY_IN_SECONDS 10
#define LDR_TO_DAY_DELAY_IN_SECONDS 10

/*
 * Die Menge der Werte fuer die Glaettung. Mehr Werte bringen bessere Ergebnisse aber
 * brauchen auch mehr Zeit (und mehr Speicher! / Gleitender Mittelwert)
 * Ab der Version 3.4.2 obsolet.
 * Default: 32
 * #define LDR_MEAN_COUNT 32
 */
/*
 * Der Hysterese-Threshold.
 * Default: 50
 */
#define LDR_HYSTERESE 50
/*
 * Die LDR-Werte werden auf Prozent gemappt.
 * Hier koennen diese Werte beschnitten werden,
 * falls man eine minimale oder maximale
 * Helligkeit vorgeben moechte.
 */
#define LDR_MIN_PERCENT 0
#define LDR_MAX_PERCENT 100
/*
 * LDR-Check-Raten. Dieser Wert beeinflusst, wie schnell
 * sich die Displayhelligkeit an neue LDR-Werte anpasst
 * und ist vom LedDriver abhaengig.
 * Der Wert ist der Threshold in Millisekunden, bevor eine
 * neue Anpassung stattfindet.
 */
#ifdef LED_DRIVER_DEFAULT
#define LDR_CHECK_RATE 1
#endif
#ifdef LED_DRIVER_UEBERPIXEL
#define LDR_CHECK_RATE 7
#endif
#ifdef LED_DRIVER_POWER_SHIFT_REGISTER
#define LDR_CHECK_RATE 10
#endif
#ifdef LED_DRIVER_NEOPIXEL
#define LDR_CHECK_RATE 10
#endif
#ifdef LED_DRIVER_DOTSTAR
#define LDR_CHECK_RATE 10
#endif
#ifdef LED_DRIVER_LPD8806
#define LDR_CHECK_RATE 10
#endif

#endif
