/*
 * Clockthree
 * Die Firmware der Selbstbau-QLOCKTWO.
 *
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  3.4.10
 * @created  01.11.2011
 * @updated  30.01.2022 TF basierend auf Ergänzungen und Änderungen von A. Mueller
 *
 * Versionshistorie:
 * V 1.1:   - DCF77 auf reine Zeit ohne Strings umgestellt.
 *          - Ganzes Datum wird in die DS1307 geschrieben (wieder verworfen).
 * V 1.2:   - Schnelles Schreiben der Shift-Register eingefuehrt.
 * V 1.3:   - Auf eigene DCF77-Bibliothek umgestellt (MyDCF77) und DCF77Helper verbessert.
 * V 1.4:   - Glimmen/Nachleuchten dank Tipp von Volker beseitigt (http://www.mikrocontroller.net/topic/209988).
 *          - Erneute Fehlerbereinigung in der DCF77-Bibliothek.
 * V 1.5:   - Die Helligkeit laesst sich ueber die Variable 'brightness' einstellen (Bereich 1-9) bzw.
 *              ueber den Compiler-Schalter 'ENABLE_LDR' die Regelung einschalten.
 * V 1.6:   - Fehler in der DCF77Helper-Bibliothek behoben.
 * V 1.7:   - SKIP_BLANK_LINES eingefuehrt, damit hellere LEDs (Danke an Johannes).
 * V 1.8:   - UPSIDE_DOWN eingefuehrt, damit man die Kabel beim Anoden-Multiplexer nicht kreuzen muss.
 * V 1.9:   - Diverse Verbesserungen und Fehlerbehebungen, die das Flackern drastisch reduzieren.
 * V 1.9.1: - Fehler behoben, der die Nutzung eines analogen Pins fuer den DCF77-Empfaenger verhindert hat.
 * V 1.9.2: - Timing-Parameter fuer die PWM ueber DEFINE anpassbar.
 *          - LDR-Grenzwerte automatisch anpassbar.
 *          - Schreibfehler in setHoures()->setHours() behoben.
 *          - Kompatibilitaet zu Arduino-IDE 1.0 hergestellt.
 * V 1.9.3: - Glimmen wieder behoben.
 * V 1.9.4: - Modus zum Einstellen der Helligkeit eingefuehrt.
 * V 2.0:   - Sprachrelevante Dinge ausgelagert, so kann man mehr Sprachen haben und einfach durch einkommentieren aktivieren.
 *          - setWords in setMinutes und setMinutes in setCorners umbenannt, weil es mehr Sinn ergibt.
 *          - setCorners in eigene Datei ausgelagert, weil viele Bastler sich vertun und in der Routine Aenderungen vornehmen muessen.
 *          - LDR in eigene Klasse ausgelagert und Werte geglaettet. Am Anfang werden 1000 Werte gelesen, damit er sich einpegelt.
 *          - Signal vom DCF77-Empfaenger geglaettet, damit nicht ein einzelner falscher Peak das Telegramm zerstoert.
 *          - Status-LEDs koennen ueber DEFINEs ausgeschaltet werden.
 *          - DCF77- und SQW-LED blinken am Anfang drei Mal als 'Hello', damit man ohne Serial-Monitor sehen kann, ob der ATMEGA anlaeuft.
 *          - Serial-Output ist immer an, kleine Statusmeldung waehrend der Initialisierung, damit man beim Bespielen des ATMEGA ohne weitere
 *              Elektronik sehen kann, ob das Programm drauf ist und laeuft.
 *          - Aenderung, die ein Stellen der Zeit nach dem DCF77-Signal auch im Modus 'BLANK' ermoeglicht - in diesem Modus ist der Empfang
 *              weniger gestoert, da die LED-Matrix abgeschaltet ist.
 *          - Zeitgesteuertes Abschalten des Displays eingefuehrt (Stromsparen in der Nacht/Schlafzimmer/besserer Empfang des DCF77-Empfaengers).
 *          - Speaker auf D13 eingefuehrt.
 *          - Alarm-Mode eingefuehrt.
 *          - Bayrische Sprachvariante (DE_BA): 'viertel nach Zehn', aber 'dreiviertel Elf'.
 * V 2.0.1: - DCF77-Tresholds angepasst.
 * V 2.0.2: - Strings in PROGMEM (flash) ausgelagert.
 * V 2.0.3: - SPEAKER_IS_BUZZER eingefuehrt, damit kann man sagen, ob man einen Lautsprecher oder Piezo-Buzzer als Alarm verwendet.
 * V 2.0.4: - falsches BREAK dank Lars behoben.
 * V 2.0.5: - Franzoesisch hinzugefuegt, Woerter_??.h's anschaulicher gemacht. Weitere Sprachen angelegt, aber noch nicht die Logik implementiert (Hilfe?!).
 * V 2.0.6: - cleanWordsForAlarmSettingMode() eingefuehrt, damit das Stellen des Alarms sprachenunabhaengig ist.
 *          - Das DCF77-Signal kann per Compiler-Schalter invertiert werden.
 * V 2.0.7: - Neuer Modus: Umschaltung LDR auto/manuell dank Alexander.
 * V 2.0.8: - Genauigkeit verbessert, dank Peter (Interrupt auf FALLING). @Peter: Das Zurueckscheiben in die DS1307 passiert im Normalbetrieb ja
                nur beim Update ueber den DCF77-Empfaenger, daher habe ich es weggelassen.
 * V 2.0.9: - SplitSideDown rausgenommen, diesen seltenen Fehler braucht kein Mensch.
 * V 2.1:   - Neue Sprachen hinzugefuegt: Englisch und Italiensch. Spanisch ist angelegt, aber nicht implementiert, weil ich kein Wort Spanisch spreche.
 *          - Man kann jetzt einfach relativ weit oben im Code ueber Compiler-Schalter festlegen, welche Sprache man haben moechte.
 * V 2.1.1: - Bitmaps in den PROGMEM ausgelagert. So kann man alle Buchstaben halten.
 * V 2.1.2: - Bug im Alaram-Modus behoben.
 * V 2.1.3: - Zusaetzlich zu den Ziffern auch Staben eingefuehrt, A/M-Modus darauf umgestellt (Vorbereitung auf Konfigurationsmoeglichkeit
 *              fuer alle Sprachen in einer Firmware)
 * V 2.1.4: - Optimierung hinsichtlich Speicherbedarf.
 * V 3.0:   - LedDriver-Klasse erstellt, welche die Shift-Register und UDN2981A als Led-Treiber kapseln. Dadurch kann man auch andere
 *              LED-Treiber nehmen (MAX7219, AS1130 etc.) und einfach eine passende Klasse von der LedDriver-Klasse vererben und in den Code einklinken.
 *          - Alle Sprachen im 'Renderer' zusammengefasst, so dass sie ohne die Firmware neu zu flashen umschaltbar werden.
 *          - Niederlaendisch hinzugefuegt (Danke an Wekog24 und Rudolf K.).
 *          - Eine kleine Debug-Klasse eingefuehrt, damit die #debug-Statements nicht so stark den Code zerhacken und er besser lesbar wird.
 *          - Beim Starten wird der freie Speicher und die Version auf der Console ausgegeben.
 *          - Eine Alarm-Klasse eingefuehrt, das macht den 'Hauptcode' nochmal besser lesbar.
 *          - Configuration.h eingefueht, in der alle wichtigen Compiler-Schalter zusammengefuehrt sind, dadurch gibt es eine zentrale Stelle
 *              fuer die Anpassungen (Danke fuer das Konzept an Markus K.).
 *          - Settings.h eingefuehrt, in der die Spracheinstellung gehalten wird und im EEPROM gespeichert wird (Danke fuer das Konzept an Markus K.).
 *          - Modes zum Einstellen der Sprache und des Eck-Modus eingefuehrt.
 *          - Extended Modes eingefuehrt.
 *          - Saechsisch als Sprache aufgenommen, danke an Sven.
 *          - Bug im setzten der Eck-LEDs fuer den Ueberpixel behoben.
 *          - Brightness ist normaler Mode, aber nur wenn ldr_auto == FALSE.
 *          - Automatische Alarm-Abschaltung nach MAX_BUZZ_TIME_IN_MINUTES.
 *          - Stellen der Uhr nur im erweiterten Modus moeglich (sonst verstellt man sie leicht bei der Verwendung als Wecker).
 *          - LDR-Modus wird im EEPROM abgelegt.
 *          - kleinere Korrekturen, die nur in erweiterten Compiler-Ausgaben zu Warnings gefuehrt haben.
 *          - Kleinere Optimierungen, Danke an Michael K.
 *          - Bug aufgrund eines moeglichen Ueberlaufs von millis() in Button.cpp behoben.
 *          - Verbesserungen in der Helligkeitssteuerung.
 *          - ENABLE_LDR aus Configuration.h entfernt, da sich der LDR ueber das erweiterte Setup ausschalten laesst.
 * V 3.0.1: - Schweizer Layout angepasst.
 * V 3.1:   - Spanisch implementiert.
 *          - Der LDR skaliert jetzt selbst, dann flackert es weniger bei unguenstigen Lichtverhaeltnissen.
 * V 3.1.1. - Die Displayhelligkeit wird beim starten gesetzt.
 *          - Kleiner Fehler im Spanischen behoben.
 *          - Die alte Shiftregistermethode ist wieder aktiv, die schnelle kann in Configuration.h gesetzt werden.
 * V 3.1.2. - Led-Driver-Klassen sagen, wer sie sind (fuer die Ausgabe der Konfiguration beim Start).
 *          - Klassen-Namen verbessert.
 * V 3.2.   - Led-Driver fuer das Licht-Monster implementiert.
 *          - LDR-Klasse auf 0-100% umgebaut.
 *          - Brightness wird im EEPROM gesichert.
 *          - EnableAlarm und DCF77SignalIsInverted ins EEPROM ausgelagert und ueber das erweiterte Setup einstellbar.
 *          - Die Modi SCRAMBLE und ALL entfernt, dafuer den Modus TEST eingefuehrt. ALL ist fuer manche DisplayDriver gefaehrlich wegen des Stromverbrauchs. TEST schalte nacheinander alle LEDs einmal durch.
 *          - Moeglichkeit zur Zeitverschiebung eingefuehrt.
 * V 3.2.1. - Alle Deutsch-Varianten zusammengefasst, um Platz zu sparen.
 *          - Fehler im Italienischen behoben.
 *          - Fehler mit Doppelbelegung einer Variable im Qlockthree.ino behoben.
 *          - Der LDR wird nur alle 250ms gelesen, dann flackert er weniger bei unguenstigen Verhaeltnissen.
 * V 3.2.2. - DS1307 Multi-MCU-Faehig gemacht.
 *          - Initialisierung der DS1307 verbessert.
 * V 3.2.3. - A0-Hack: Verbindet man die freie A0-Leitung mit PON vom DCF77-Empfaenger, schaltet diese den Empfaenger korrekt
 *              ein und aus, je nach Displayzustand. Ist PON fest mit GND verbunden, passiert nichts (schadet aber auch  nicht).
 * V 3.2.4. - Der DCF-Empfaenger wird ueber A0 am Anfang eingeschaltet, dann nicht mehr ausgeschaltet.
 *          - Wird in den Blank-Modus geschaltet (Display aus), schaltet sich nach dem erfolgreichen Empfang der Zeit ueber DCF77
 *              das Display wieder ein (hilft beim Debuggen).
 *          - Der Nachtabschaltmodus ist ein eigener, nicht manuell erreichbarer Modus.
 * V 3.3.0. - LedDriverNeoPixel fuer den Aufbau mit WS2812B-Streifen eingefuehrt (Verkabelungsschema ist beim LedDriverNeoPixel)...
 *          - ...dazu die Klasse Button angepasst, so dass sie auch gegen GND schalten kann (damit Unterstuetzung fuer das BBRTCAD).
 *          - Achtung! Wenn die NeoPixel-Library im Librarys-Ordner ist, kann ich keinen Pointer auf den Stripe (in LedDriverNeoPixel.h)
 *              definieren (ich weiss leider nicht warum, es kommt eine seltsame Compiler-Meldung). Wenn sie im Qlockthree-Ordner ist, geht es.
 *          - IR-Fernbedienungs-Unterstuetzung eingefuehrt. Achtung! Wegen Platzmangels alle IRs ausser der Sparkfun (NEC) auskommentiert. Siehe weitere
 *              Infos weiter unten und bei der IR-Fernbedienung!
 *          - Achtung! Weil ich die IR-Receiver-Library (https://github.com/shirriff/Arduino-IRremote/) entschlacken musste, ist sie hier
 *              im Qlockthree-Ordner. Wenn sie auch im Librarys-Ordner ist, schlaegt die Compilierung fehl! Wer andere Codes als die von der
 *              SparkFun-Fernbedienung (NEC) nehmen mechte, muss die entsprechenden Typen in der Datei IRremote.cpp so ab Zeile 427 wieder
 *              einkommentieren. Die Implementierung hier ist beispielhaft zu sehen, um eigene Ideen verwirklichen zu koennen.
 *          - Test-Modus geaendert, damit er mit der IR-Fernbedienung klappt. Die Eck-LEDs zaehlen im Sekunden-Takt hoch, die Spalten werden
 *              im Sekunden-Takt nach rechts durchgeschaltet.
 *          - DCF-Empfang verbessert, dank Tipp aus dem Forum (unguenstiger INTERNAL_PULLUP beim Signal-Pin; PULL-Widerstaende muessen in Hardware
 *              je nach verwendetem Empfaenger realisiert werden).
 *          - Flackern in Ext-Menues und Manual-Brightness behoben, falsch durchdachtes linesToWrite-Statement (interessiert nur DefaultLedDriver,
 *              alle anderen ignorieren es eh).
 *          - Fehler in der DS1307-Klasse behoben. Nicht immer kommen die angeforderten 7 Bytes, dann verwerfen und neu anfordern (und nach
 *              8 Versuchen abbrechen). Der I2C-Bus ist wohl speziell bei LED-Stripes gerne mal gestoert, dann kommen die beruehmten '85' Sekunden.
 *          - Mode fuer die Visualisierung des DCF77-Empfangs (im Ext-Menue nach dem LED-Test) eingefuehrt. Man kann damit abschaetzen, wie gut der
 *              DCF77-Empfang ist. Mit der gelben LED muessen die Ecken durchschalten. Schalten sie wild durcheinander, gibt es Fehlsignale.
 *              Das Problem ist, dass ja die LED-Matrix den Empfang stoert. So stoert natuerlich potentiell auch die Visualisierung den Empfang.
 *              Mit einem WortWecker hat das dennoch ganz gut geklappt. Mit dem experimentellen LedDriverNeoPixel nicht.
 *          - Kleinen Fehler im Italienischen behoben, danke an die diversen Hinweisgeber!
 * V 3.3.1. - Stundenbegrenzung im Renderer (die ja wegen der Zeitverschiebungsmoeglichkeit existiert) auf den Bereich 0 <= h <= 24 ausgeweitet,
 *              dank Tipp aus dem Forum.
 * V 3.4.0. - Die DCF77-Decoding-Klasse vollstaendig umgestellt. Alle Beispiele im Internet gehen davon aus, dass man keine Vergleichszeit hat.
 *              Da wir hier eine Echtzeituhr haben, kann man statistische Methoden zur Auswertung verwenden. Wenn das Signal optisch brauchbar aussieht (gelbe LED),
 *              Synchronisiert die Zeit in weniger als 4 Minuten. Aber es muss eben brauchbar aussehen. Kurzes Aufblinken 1 Mal pro Sekunde, eine
 *              Pause pro Minute.
 * V 3.4.1. - Die Adafruit- und IR-Library umbenannt ("My" vorangestellt), damit sie nicht mit den offiziellen Librarys kollidieren. Warum sie hier lokal liegen,
 *              steht weiter oben.
 * V 3.4.2. - LDR-Bugs behoben und -Klasse optimiert. Dazu auch Getter fuer die Helligkeit in den Display-Drivern eingefuehrt.
 * V 3.4.3. - Vergessene DEBUGs auskommentiert, Check der DS1307 beim Start verbessert (Danke an Erich M.).
 * V 3.4.4. - Verbesserungen in der RTC (setzten der Zeit durch Compile-Zeit moeglich).
 *          - Adafruits NeoPixel-Library muss jetzt im Librarys-Ordner liegen.
 * V 3.4.5. - DS1307 nach MyRTC umbenannt, weil es nicht mehr nur um die DS1307 geht.
 *          - Einschalten des Rechtecksignals fuer DS1307 und DS3231 getrennt (Danke an Erich M.) - Konfiguration in der Configuration.h.
 *          - Unterstuetzung verschiedener IR-Fernbedienungen vereinfacht. Codes von NEC-compatiblen Fernbedienungen werden angezeigt, wenn eine Fernbedienung in
 *              der Configuration.h eingeschaltet ist (muss nicht die richtige sein) und #DEBUG an ist. Drei Beispiel-Remotes sind angelegt: Sparkfun,
 *              Mooncandles, Lunartec. Bei der Verwendung von Nicht-NEC-Fernbedienungen muss der entsprechende Part in MyIRremote.cpp einkommentiert werden,
 *              ein guter Start ist Zeile 428.
 *          - Im Zusammenhang mit den Fernbedienungen setColor() fuer die LED-Driver eingefuehrt. Damit kann man die Farben von RGB-Stripes aendern. Bei
 *              dem klassischen Aufbau mit einfarbeigen LEDs (75HC595; MAX7219 etc.) passiert natuerlich nichts.
 *          - Unterstuetzung fuer APA102-Stripes (DotStars) eingefuehrt, die Adafruit APA102-Library muss dazu im librarys-Ordner liegen.
 *          - FPS werden bei eingeschaltetem #DEBUG angezeigt.
 *          - Die Helligkeitsregelung laeuft jetzt sanfter. Timing-Werte fuer die verschiedenen LedDriver in der Configuration.h hinterlegt.
 * V 3.4.6. - Unterstuetzung fuer LPD8806-Stripes eingefuehrt, die Adafruit LPD8806-Library muss dazu im librarys-Ordner liegen. Achtung! Zur Zeit hat die LPD8806-Library
 *              von Adafruit einen Bug. Es fehlt das klassische #ifndef __CLASSNAME__ #define __CLASSNAME__ [Klasssendefinition] #endif um die Klasse im .h-File.
 * V 3.4.7. - Unterstuetzung fuer Buttons auf rein analoge Ports hinzugefuegt, wie sie der ATMEGA328 in SMD-Ausfuehrung hat.
 *          - Unterstuetzung fuer die alte Arduino-IDE (bis 1.0.6) entfernt, da sich die Firmware damit eh nicht mehr kompilieren laesst.
 *          - Library fuer MAX7219 (LedControl) ausgelagert, sie muss jetzt im Librarys-Ordner liegen.
 * V 3.4.8. - HelperSeconds-Behandlung in Interrupt-Funktion verschoben, damit die nicht aufgrund von Tastendruecken hochgezaehlt werden, danke an Meikel.
 * V 3.4.8a - Zusätzliche Features hinzugefügt:
 *            * Datumsanzeige
 *            * Automatischer Rücksprung auf Uhr (Rücksprung standardmäßig von Sekunden-, Datumsanzeige und Helligkeitsanzeige)
 *            * Nachschaltung einstellbar via Menü
 *            * Nachschaltung abbrechbar via Mode-Taste
 *            * Nachschaltung wieder reaktivierbar via gleichzeitigem Tastendruck von Hour- und Minute-Taste
 *            * Anzeige der vergangenen Stunden und Minuten seit der letzten erfolgreichen DCF-Synchronisation (im Menü)
 * V 3.4.8b - Neue DCF-Decoding-Klasse mit automatische Driftkorrektur, die eine fehlerhafte Drift zwischen RTC und DCF auszugleichen vermag und
 *            dadurch robuster arbeitet. 
 *            - Zuverlässigkeit der Zeitsynchronisation erhöht:
 *             -- Drifts in der Phase werden durch eine automatische Offsetkorrektur bereinigt.
 *             -- Damit ist die Erkennung der Schaltsekunde nach spätestens einer Minute zuverlässig möglich.
 *                Das Fehlen der korrekten Erkennung dieser Schaltsekunde in früheren Versionen verhinderte eine zuverlässige Zeitsynchronisation.
 *             -- Deutlich exaktere Einstellung der Zeit dank Driftkorrektur möglich.
 *            Die neue Klasse benötigt Timer1. (Ab V 3.4.9b3 kein Timer mehr erforderlich!)
 * V 3.4.8b2- Anzeige der vergangenen Stunden und Minuten seit der letzten erfolgreichen DCF-Synchronisation in eigenes Menü gelegt.
 * V 3.4.9. - Stundenbegrenzung (0 <= h <= 24) auch in setHourse eingefuehrt, siehe http://diskussion.christians-bastel-laden.de/viewtopic.php?f=17&t=2028
 * V 3.4.9a - Fehler bei Stundenbegrenzung in setMinutes korrigiert und Stundenbegrenzung aus setHours wieder entfernt, siehe http://diskussion.christians-bastel-laden.de/viewtopic.php?f=17&t=2028
 *          - Große Aufräumarbeiten, Code optimiert, Basisklasse TimeStamp eingeführt und Alarm, MyDCF77, sowie MyRTC davon abgeleitet, um Speicher einzusparen.
 * V 3.4.9b - Zusätzliche Features hinzugefügt:
 *            * Für besondere Ereignisse (einzutragen in neue Datei Ereignisse.h) kann ein Symbol hinterlegt werden, das statt dem Datum in der Datumsanzeige angezeigt wird.
 *            * Countdownfunktion eingeführt (nutzt ebenfalls Ereignisse.h), die einen Countdown zu festgelegten Ereignissen und nach Ablauf des Countdowns das definierte Symbol blinkend anzeigt.
 *            Das Aussehen der großen Null in Zahlen.h wurde geändert (die Null hat keinen diagonalen Strich mehr) und passt damit besser zur kleinen Null (wenn vier Zahlen angezeigt werden).
 * V 3.4.9b2- Kleine Codeoptimierungen
 *          - Schalter für Zusatzfunktionen nach Configuration.h verschoben.
 *          - Zusätzliches Feature:
 *            * Zusätzliche Option, für jede Eckled nur die dazugehörige Kathode und nicht alle einzuschalten. Dies Verhindert das Glimmen ausgeschalteter Eckleds. (Standard: ausgeschaltet)
 *              Hinweis: Diese Option kann nur bei richtiger Verdrahtung verwendet werden. 
 * V 3.4.9b3- Kleine Codeoptimierungen
 *          - * Automatische Driftkorrektur beim DCF77-Empfang benötigt KEINEN Timer mehr! *
 *          - Menü in drei Kategorien unterteilt: MAIN, TIME und TEST, um Navigation zu erleichtern.
 *            Mit der Mode-Taste läuft man durch alle Menüseiten (wie bisher), man kann aber eine Kategorie (MAIN, TIME oder TEST) durch Drücken der Minuten- oder Stundentaste überspringen.
 *            Kurzüberblick welche Modi sich hinter den Kategorien verbergen:
 *            MAIN: LDR, Eck-LED-Richtung, Alarm ein-/ausschalten, DCF normal/invertiert, Sprache
 *            TIME: Uhrzeiteinstellung, Zeitzone, Timeout für automatischen Rücksprung, Nachtschaltung Beginn (Mo-Fr), Nachtschaltung Ende (Mo-Fr), Nachtschaltung Beginn (Sa,So), Nachtschaltung Ende (Sa,So)
 *            TEST: Testmodus (LEDs schalten durch), letzte erfolgreiche DCF-Synchronisation, DCF-Debug (vier Eck-LEDs schalten bei gutem Empfang sekundenweise durch)        
 *            Hinweis: Möchte man durch alle Menüseiten durchblättern, einfach wie gewohnt die Mode-Taste benutzen.
 *          - Zusätzliches Feature:
 *            * Nachtschaltung kennt jetzt zwei Zeitbereiche, ist also für Mo bis Fr und Sa, So separat einstellbar.
 * V 3.4.9b4- Größere Codeoptimierung bei der Nachtschaltung (knapp 800 Byte Einsparung)
 *          - Kleinere Codeoptimierungen in AnalogButton, Button und Qlockthree:
 *            * In allen drei Dateien/Klassen wurde ein Überlauf von millis() abgefangen. Dies ist aber unnötig, wenn nur Differenzen mit einem früheren Wert von millis() betrachtet werden.
 *              Ein Überlauf ändert nämlich die Differenz nicht. Da sich alle erforderlichen Vergleiche auf Differenzen haben umschreiben lassen, ist das Abfangen des Überlaufs entfernt worden.
 *              Beispiel:   Folgende IF-Abfrage
 *                  _lastPressTime + BUTTON_THRESHOLD < millis()  // <-- Bei einem Überlauf von millis() schlägt dieser Vergleich fehl
 *                          lässt sich wie folgt umschreiben:
 *                  millis() - _lastPressTime > BUTTON_THRESHOLD  // <-- Jetzt spielt der Überlauf von millis() keine Rolle mehr, weil nur Differenz (zwischen zwei unsigned Variablen) betrachtet wird.
 *          - Fehler in der Nachtschaltung behoben, der das Aufwecken der Uhr verhindert hat.
 * V 3.4.9b5- Kleinere Codeoptimierungen
 *          - Änderungen:
 *            * Verhalten der Nachtschaltung leicht geändert.
 *          - Zusätzliches Feature:
 *            * Die einleitenden Wörter wie "ES IST" sind jetzt im Menü abschaltbar, so wie bei der originalen Wortuhr auch.
 * V 3.4.9b6- Größere Codeumbauten und -optimierungen
 *          - Änderungen:
 *            * Die Option den Alarmmodus ein- oder auszuschalten ist entfallen (wird zur Compilezeit festgelegt).
 *            * Beim Sprung ins Menü durch doppelten Tastendruck werden die beiden Einzeltasten kurz gesperrt, das verhindert versehentliches Verstellen des ersten Menüpunkts.
 *            * Das Ein- und Ausschalten des Weckers erfolgt durch 5-sekündiges Warten in der Weckzeiteinstellung.
 *              (Früher wurde der Wecker bei jedem beliebigen Tastendruck ausgeschaltet und musste stets neu eingeschaltet werden.)
 *          - Zusätzliche Features:
 *            * Wortweckerfunktionen (Wortwecker mit 5 Knöpfen) eingefügt.
 *              ** Snooze-Modus ist mehrfach hintereinander aktivierbar und ändert die eingestellte Weckzeit NICHT mehr.
 *              ** Ist der A0-Hack aktiviert, wird der DCF-Empfänger während den eingestellen Nachtzeiten aktiviert.
 *                 Neu ist, dass das auch dann klappt, wenn der Nachtmodus manuell (durch Tastendruck) verlassen wurde und danach LDR-gesteuert wieder beginnt.
 *            * Weckmodus / Alarmzeiteinstellung komplett umgebaut
 *              ** Uhr bleibt jetzt bei eingeschaltetem Alarm (Wecker klingelt noch nicht) vollständig bedienfähig.
 *              ** Weckzeit und -status (ein- oder ausgeschaltet) bleibt auch nach Stromausfall erhalten.
 *              ** Viele Weckmelodien: 8 wählbare Weckmelodien + 1 sanfter Weckmodus
 *              ** Zusätzliche eigene Weckmelodien sind sehr leicht einfügbar.
 *              ** Alarm-LED kann ab sofort unterschiedlich schnell blinken:
 *                  langsames Blinken: Im Alarm-Menü, d.h. Weckzeit wird gerade eingestellt.
 *                  schnelles Blinken: Snooze-Modus ist aktiviert (nur für 5-Knöpfe-Wortwecker relevant)
 *            * Puristenmodus: "Es ist" wird standardmäßig nur noch zur vollen und halben Stunde angezeigt (wie beim Original); ist auch weiterhin dauerhaft einschaltbar.
 *            * LDR-gesteuerter Nachtmodus: Display an oder aus, je nach gemessener Helligkeit.
 *            * Minutenanzeige im Sekundenmodus: Die Eck-LEDs leuchten jetzt auch im Sekundenmodus (wie beim Original).
 *            * Helligkeitsanzeige: Umstellbar von Balken- auf Prozentanzeige
 * V 3.4.9b7- Viele Codeoptimierungen
 *          - Zwei Fehlerbehebungen in den Alarmeinstellungen und im DCF77-Auswertealgorithmus
 *          - Änderungen:
 *            * Die Menüs für die Zeiten der Nachtschaltung werden ausgeblendet, wenn der LDR dafür verwendet wird.
 *            * Die Helligkeitseinstellung (in der Balkenansicht) zeigt bei 0% einen Punkt links unten an (früher keine LED), danach geht für jede Erhöhung um 10% in der Helligkeit
 *              eine weitere LED an. Ist der größte Balken also 4 LEDs hoch, ist die Helligkeit zwischen 40% und 49% eingestellt. Vorher war diese Zuordnung nicht in 10er-Schritten.
 *            * Die Helligkeitseinstellung ist nun auch bei aktiviertem LDR zugänglich. Gleichzeitig erfolgt in der Helligkeitseinstellung keine Nachtschaltung mehr.
 *              Grund: In der Helligkeitseinstellung möchte man eventuell mit der (Umgebungs-)Beleuchtung spielen, um den richtigen Schwellwert für die Nachtschaltung zu finden.
 *            * Die Nachtschaltung erfolgt zusätzlich beim Wortwecker auch nicht mehr in dem Modus "alle LEDs ein".
 *              Grund: Der Modus "alle LEDs ein" kann als Art Lampe dienen und sollte anbleiben bis man ihn selbst wieder ausschaltet.
 *            * Wird der Alarm deaktiviert, sei es über eine Taste oder über den Näherungssensor, wird ab sofort das Display eingeschaltet.
 *            * Im Blank-Modus werden die LEDs bei erfolgreicher DCF77-Synchronisation nicht mehr eingeschaltet.
 *              Diese Funktion übernimmt jetzt ein neuer Menüpunkt (siehe unten)
 *          - Neue Features:
 *            * Die Zeiteinstellung der Uhr und des Weckers erfolgt jetzt mit vollen 24 statt nur mit 12 Stunden.
 *              Um anzuzeigen, dass die Zeit gerade zwischen 12:00 Uhr und 23:59 Uhr liegt, blinken die LEDs.
 *                 0 - 11:59 Uhr: LEDs dauerhaft an (Vormittag)
 *                12 - 23:59 Uhr: LEDs blinken (Nachmittag)
 *              Das bedeutet, man kann den Wecker nach dem Klingeln für den nächsten Morgen wieder einschalten ohne dass er 12 Stunden später noch einmal klingelt.
 *            * Der Schlummermodus (Snooze) ist jetzt auch für all jene nutzbar, die keinen Näherungssensor eingebaut haben.
 *              Klingelt der Wecker, wird beim ersten Tastendruck (beliebige Taste) zunächst der Schlummermodus aktiviert, erst beim zweiten Tastendruck erfolgt das vollständige Deaktivieren des Alarms.
 *            * Die Anzeige der letzten erfolgreichen DCF77-Synchronisation in Stunden und Minuten im Menü EXT_MODE_DCF_SYNC kennt keinen Überlauf mehr.
 *              War noch keine Zeitsynchronisation erfolgreich oder ist die vergangene Dauer seit der letzten Synchronisation größer als anzeigbar, wird standardmäßig
 *              die größtmögliche Dauer: 99:59 (für 99 Stunden und 59 Minuten) angezeigt.
 *              Man kann also ab jetzt problemlos erkennen, ob die DCF77-Synchronisation innerhalb der letzten 4 Tage geklappt hat.
 *            * Ein neuer Menüpunkt (allerletzter Menüpunkt) ermöglicht bei jeder Konfiguration die DCF77-Synchronisation.
 *              Er funktioniert ähnlich wie früher der BLANK-Modus, nur dass zusätzlich ein eventuell vorhandener Näherungssensor ausgeschaltet wird (sofern A0-Hack vorhanden).
 *              Sobald die DCF-Synchronisation erfolgreich war, schaltet die Uhr wieder auf den Normal-Modus zurück.
 *            * Ein zusätzlicher Schalter (beim Compilieren) erlaubt die DCF-Funktionalität komplett abzuschalten.
 *            * Die minimalen und maximalen Werte des LDR (automatisch im LDR_AUTOSCALE-Modus ermittelt) werden ab sofort gespeichert.
 *              Nach einem Stromverlust übernimmt die Uhr die früheren Werte und der LDR muss nicht mehr durch Wischen mit dem Finger initialisiert werden.
 *              (Die Werte werden zurückgesetzt, wenn im Menü der "LDR-Modus" von A auf M gestellt wird.)
 * V 3.4.9b8- Änderungen:
 *            * Der Wecker bleibt auch im (erweiterten) Menü eingeschaltet und ist mit den Tasten der Fernbedienung deaktivierbar.
 *            * Die Einstellung "Auto. Rücksprung" wurde aus dem Menü entfernt. Wird jetzt beim Compilieren fest eingestellt.
 *            * Beim Wortwecker kann der zeitgesteuerte Nachtmodus wieder manuell erreicht werden (zweimaliges, langsames Drücken der Blank-Taste).
 *            * Programmcode vereinfacht und optimiert.
 *          - Neue Features:
 *            * Code für DCF77-Empfänger kann in Configuration.h komplett abgeschaltet werden.
 *            * Datumsanzeige kann in Configuration.h ausgeschaltet werden.
 *            * Datum
 *              ** Datum kann manuell im Menü eingestellt werden.
 *              ** Berechnung des Wochentages erfolgt automatisch auf Basis des Datums und wird dementsprechend eingestellt.
 *              ** Eine eingestellte Zeitverschiebung (Zeitzone) wird mit der Uhrzeit (und Datum) verrechnet und in die RTC geschrieben. (RTC hat immer die lokale Zeit und Datum.)
 *              ** Die eingestellte Zeitzone wird beim Empfang des DCF77-Signals berücksichtigt und einberechnet.
 *              --> Mit diesen neuen Features berücksichtigen jetzt alle Funktionen, die intern mit Datum und Uhrzeit rechnen wie bspw.
 *                  Datumsanzeige, Wecker, Ereignisanzeige, Countdown, Nachtschaltung unter der Woche und am Wochende die am Ort geltende Zeitzone.
 *          - Fehlerbehebungen:
 *            * Datumsfunktionen hatten Zeitzone nicht berücksichtigt (siehe oben: "Neue Features").
 *            * Verhalten der LDR- und zeitgesteuerten Nachtschaltung einheitlicher gestaltet. (Algorithmus neu geschrieben)
 *            * Selten auftretender Initialisierungsfehler bei den Buttons und dem Wecker behoben.
 *            * Unerwartetes Verhalten bei Verwendung einer Fernbedienung korrigiert.
 * V 3.4.9b9- Änderungen:
 *            * ACHTUNG! Countdownberechnung hat sich geändert. Es wird jetzt in Ereignisse.h nicht mehr die Startzeit eines Countdowns, sondern die Zielzeit angegeben.
 *              Beispiel: Wollte man für Neujhar einen Countdown von 15 Minuten haben, musste man vorher den 31.12. 23:45 Uhr angeben, jetzt gibt man 01.01. 0:00 Uhr an.
 *              Diese Änderung wurde auf mehrfachen Wunsch durchgeführt und erspart das Rechnen, wann der Countdown starten muss.
 *            * Renderer-Programmcode (Anzeige der Wörter) optimiert.
 *            * Die Datumsanzeige zeigt standardmäßig nur noch den Tag und nicht mehr zusätzlich noch den Monat an.
 *              Hinweis: Die Anzeige des Monats kann aber in der Configuration.h wieder eingeschaltet werden.
 *          - Neue Features:
 *            * Die neue Jahreszahl für den Neujahrscountdown kann automatisch berechnet und muss nicht in der Ereignisse.h für jedes Jahr neu hinterlegt werden. (Standard: ausgeschaltet)
 *            * Neue Sprache "Portugiesisch" hinzugefügt.
 *            * Nichtbenötigte Sprachen sind beim Compilieren abschaltbar.
 *            * Die Null mit dem Strich lässt sich beim Compilieren wieder aktivieren.
 *          - Fehlerbehebungen:
 *            * Die Gültigkeit eines Datums wird beim Verändern eines TimeStamps geprüft, damit werden auch Fehler durch falsche Daten beim Countdown abgefangen.
 *            * Kleinere Schönheitsfehler behoben.
 * V 3.4.9b9TF- Änderungen
 *            * Version mit 12h-Weckverhalten
 * V 3.4.9b9TF2- Änderungen           
 *            * Version Matrix an während der Wecker klingelt oder im Snooze ist
 * V 3.4.9b9TF3- Änderungen           
 *            * Taste Alarm off schaltet Alarm aus, ohne das die Weckzeit noch verstellt werden kann, wenn Alarm an ist, aber der Wecker nicht klingelt (wie bei V 3.4.9b9TF)
 * V 3.4.10   Neue Features
 *            * 12/24 h Weckzeitmodus im Menue einstellbar 
 *            * Snooze-Dauer im Menue einstellbar
 *            * getestet mit 5-Tasten-Wecker
 */
#include <Wire.h> // Wire library fuer I2C
#include <avr/pgmspace.h>
#include <SPI.h>
#include <LedControl.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_DotStar.h>
#include <LPD8806.h>
#include "Configuration.h"
#include "LedDriver.h"
#include "LedDriverDefault.h"
#include "LedDriverUeberPixel.h"
#include "LedDriverPowerShiftRegister.h"
#include "LedDriverNeoPixel.h"
#include "LedDriverDotStar.h"
#include "LedDriverLPD8806.h"
#include "IRTranslator.h"
#include "IRTranslatorSparkfun.h"
#include "IRTranslatorMooncandles.h"
#include "IRTranslatorLunartec.h"
#include "MyIRremote.h"
#include "MyRTC.h"
#ifdef DCF77_SENSOR_EXISTS
    #include "MyDCF77.h"
    #include "DCF77Helper.h"
#endif
#include "Button.h"
#include "AnalogButton.h"
#include "LDR.h"
#include "Renderer.h"
#include "Staben.h"
#include "Settings.h"
#include "Zahlen.h"
#include "ZahlenKlein.h"
#ifdef EVENTDAY
#include "Ereignisse.h"
#endif

#define FIRMWARE_VERSION "V 3.4.10 vom 30.01.2022"

/*
 * Den DEBUG-Schalter gibt es in allen Bibiliotheken. Wird er eingeschaltet, werden ueber den
 * 'Serial-Monitor' der Arduino-Umgebung Informationen ausgegeben. Die Geschwindigkeit im
 * Serial-Monitor muss mit der hier angegeben uebereinstimmen.
 * Default: ausgeschaltet
 */
//#define DEBUG
#include "Debug.h"
// Die Geschwindigkeit der seriellen Schnittstelle. Default: 57600. Die Geschwindigkeit brauchen wir immer,
// da auch ohne DEBUG Meldungen ausgegeben werden!
#define SERIAL_SPEED 57600

/*
 * Die persistenten (im EEPROM gespeicherten) Einstellungen.
 */
Settings settings;

/**
 * Der Renderer, der die Woerter auf die Matrix ausgibt.
 */
Renderer renderer;

/**
 * Der LED-Treiber fuer 74HC595-Shift-Register. Verwendet
 * von der Drei-Lochraster-Platinen-Version und dem
 * NachBau_V02-Board sowie dem WortWecker nach Christian.
 *
 * Data: 10; Clock: 12; Latch: 11; OutputEnable: 3
 * LinesToWrite: 10
 */
#ifdef LED_DRIVER_DEFAULT
LedDriverDefault ledDriver(10, 12, 11, 3, 10);

#define PIN_MODE 7
#define PIN_M_PLUS 5
#define PIN_H_PLUS 6
#define PIN_ALARM 16 // WW

#define BUTTONS_PRESSING_AGAINST HIGH

#define PIN_IR_RECEIVER A1

#define PIN_LDR A3
#define IS_INVERTED true

#define PIN_SQW_SIGNAL 2
#define PIN_DCF77_SIGNAL 9

#define PIN_DCF77_PON A0

#define PIN_SQW_LED 4
#define PIN_DCF77_LED 8

#define PIN_SPEAKER 13
#endif

/**
 * Der LED-Treiber fuer 4 MAX7219-Treiber wie im Ueberpixel.
 * Data: 10; Clock: 11; Load: 12
 */
#ifdef LED_DRIVER_UEBERPIXEL
LedDriverUeberPixel ledDriver(5, 6, 7);

#define PIN_MODE 8
#define PIN_M_PLUS 3
#define PIN_H_PLUS 4

#define BUTTONS_PRESSING_AGAINST HIGH

#define PIN_IR_RECEIVER A1

#define PIN_LDR A3
#define IS_INVERTED true

#define PIN_SQW_SIGNAL 2
#define PIN_DCF77_SIGNAL 9

#define PIN_DCF77_PON A0

#define PIN_SQW_LED 10
#define PIN_DCF77_LED 11

#define PIN_SPEAKER 13
#endif

/**
 * Der LED-Treiber fuer Power-Shift-Register.
 * Data: 10; Clock: 11; Load: 12
 */
#ifdef LED_DRIVER_POWER_SHIFT_REGISTER
LedDriverPowerShiftRegister ledDriver(10, 12, 11, 3);

#define PIN_MODE 7
#define PIN_M_PLUS 5
#define PIN_H_PLUS 6

#define BUTTONS_PRESSING_AGAINST HIGH

#define PIN_IR_RECEIVER A1

#define PIN_LDR A3
#define IS_INVERTED true

#define PIN_SQW_SIGNAL 2
#define PIN_DCF77_SIGNAL 9

#define PIN_DCF77_PON A0

#define PIN_SQW_LED -1
#define PIN_DCF77_LED -1

#define PIN_SPEAKER -1
#endif

/**
 * Der LED-Treiber fuer NeoPixel-Stripes an einem BBRTCAD.
 * Data: 6
 */
#ifdef LED_DRIVER_NEOPIXEL
LedDriverNeoPixel ledDriver(6);

#define PIN_MODE 11
#define PIN_M_PLUS 13
#define PIN_H_PLUS 12

#define BUTTONS_PRESSING_AGAINST LOW

#define PIN_IR_RECEIVER A1

#define PIN_LDR A0
#define IS_INVERTED true

#define PIN_SQW_SIGNAL 2
#define PIN_DCF77_SIGNAL 3

#define PIN_DCF77_PON 4

#define PIN_SQW_LED 9
#define PIN_DCF77_LED 10

#define PIN_SPEAKER -1
#endif

/**
 * Der LED-Treiber fuer DotStars-Stripes an einem BBRTCAD.
 * Data: 6
 * Clock: 7
 */
#ifdef LED_DRIVER_DOTSTAR
LedDriverDotStar ledDriver(6, 7);

#define PIN_MODE 11
#define PIN_M_PLUS 13
#define PIN_H_PLUS 12

#define BUTTONS_PRESSING_AGAINST LOW

#define PIN_IR_RECEIVER A1

#define PIN_LDR A0
#define IS_INVERTED false

#define PIN_SQW_SIGNAL 2
#define PIN_DCF77_SIGNAL 3

#define PIN_DCF77_PON 4

#define PIN_SQW_LED 9
#define PIN_DCF77_LED 10

#define PIN_SPEAKER -1
#endif

/**
 * Der LED-Treiber fuer LPD8806-Stripes an einem BBRTCAD.
 * Data: 6
 * Clock: 7
 */
#ifdef LED_DRIVER_LPD8806
LedDriverLPD8806 ledDriver(6, 7);

#define PIN_MODE 11
#define PIN_M_PLUS 13
#define PIN_H_PLUS 12

#define BUTTONS_PRESSING_AGAINST LOW

#define PIN_IR_RECEIVER A1

#define PIN_LDR A0
#define IS_INVERTED false

#define PIN_SQW_SIGNAL 2
#define PIN_DCF77_SIGNAL 3

#define PIN_DCF77_PON 4

#define PIN_SQW_LED 9
#define PIN_DCF77_LED 10

#define PIN_SPEAKER -1
#endif

/**
 * Der IR-Remote-Control-Receiver.
 */
#ifndef REMOTE_NO_REMOTE
IRrecv irrecv(PIN_IR_RECEIVER);
decode_results irDecodeResults;
#endif
#ifdef REMOTE_SPARKFUN
IRTranslatorSparkfun irTranslator;
#endif
#ifdef REMOTE_MOONCANDLES
IRTranslatorMooncandles irTranslator;
#endif
#ifdef REMOTE_LUNARTEC
IRTranslatorLunartec irTranslator;
#endif

/**
 * Die Real-Time-Clock mit der Status-LED fuer das SQW-Signal.
 */
MyRTC rtc(0x68, PIN_SQW_LED);
volatile byte helperSeconds;

/**
 * Der Funkempfaenger (DCF77-Signal der PTB Braunschweig).
 */
#ifdef DCF77_SENSOR_EXISTS
    MyDCF77 dcf77(PIN_DCF77_SIGNAL, PIN_DCF77_LED, PIN_DCF77_PON);
    DCF77Helper dcf77Helper;
#endif

/**
 * Variable fuer den Alarm.
 */
#ifdef ALARM_OPTION_ENABLE
    Alarm* alarm = (Alarm*) settings.getNightTimeStamp(4);
#endif

/**
 * Der Helligkeitssensor
 */
LDR ldr(PIN_LDR, IS_INVERTED, &settings);
unsigned long lastBrightnessCheck;

/**
 * Die Tasten.
 */
#ifdef WW_5_BUTTONS
    Button alarmButton(PIN_ALARM, BUTTONS_PRESSING_AGAINST);
#endif
Button minutesPlusButton(PIN_M_PLUS, BUTTONS_PRESSING_AGAINST);
Button hoursPlusButton(PIN_H_PLUS, BUTTONS_PRESSING_AGAINST);
Button extModeDoubleButton(PIN_M_PLUS, PIN_H_PLUS, BUTTONS_PRESSING_AGAINST);
Button modeChangeButton(PIN_MODE, BUTTONS_PRESSING_AGAINST);

/**
 * Die Standard-Modi.
 */
#define STD_MODE_NORMAL     0
#define STD_MODE_ALARM      1
#ifdef WW_5_BUTTONS
    #define STD_MODE_WW_START   2
#endif
#define STD_MODE_SECONDS    2
#define STD_MODE_COUNTDOWN  3
#define STD_MODE_DATE       4
#ifdef WW_5_BUTTONS
    #define STD_MODE_WW_END     5
#endif
#define STD_MODE_BRIGHTNESS 5
#define STD_MODE_BLANK      6
#define STD_MODE_COUNT      7
// nicht manuell zu erreichender Modus...
#define STD_MODE_NIGHT      8
// Modus "alle LEDs an" für WW
#ifdef WW_5_BUTTONS
    #define STD_MODE_ALLON  9
#endif

/**
 * Die erweiterten Modi.
 */
#define EXT_MODE_START            20

#define EXT_MODE_MAIN_SETTINGS_START  20
#define EXT_MODE_LDR_MODE         21
#define EXT_MODE_LDR_NIGHT_MODE   22
#define EXT_MODE_LDR_NIGHT_THRESH 23
#define EXT_MODE_CORNERS          24
#define EXT_MODE_ES_IST           25
#define EXT_MODE_ALARM_MELODY     26
#define EXT_MODE_DCF_IS_INVERTED  27
#define EXT_MODE_LANGUAGE         28

#define EXT_MODE_TIME_SETTINGS_START  29
#define EXT_MODE_TIME_SHIFT       30
#define EXT_MODE_YEARSET          31
#define EXT_MODE_MONTHSET         32
#define EXT_MODE_DATESET          33
#define EXT_MODE_TIMESET          34
#define EXT_MODE_OFFTIME_MOFR     35
#define EXT_MODE_ONTIME_MOFR      36
#define EXT_MODE_OFFTIME_SASO     37
#define EXT_MODE_ONTIME_SASO      38
#define EXT_MODE_24               39
#define EXT_MODE_SNOOZE           40 

#define EXT_MODE_TEST_DEBUG_START     41
#define EXT_MODE_TEST             42
#define EXT_MODE_DCF_SYNC         43
#define EXT_MODE_DCF_DEBUG        44
#define EXT_MODE_DCF_BLANK        45
#define EXT_MODE_COUNT            46

// Startmode...
byte mode = STD_MODE_NORMAL;

// Ueber die Wire-Library festgelegt:
// Arduino analog input 4 = I2C SDA
// Arduino analog input 5 = I2C SCL

// Die Matrix, eine Art Bildschirmspeicher
word matrix[16];

// Hilfsvariable, da I2C und Interrupts nicht zusammenspielen
volatile boolean needsUpdateFromRtc = true;

// Fuer den Bildschirm-Test
byte x, y;

// Fuer fps-Anzeige
#ifdef FPS_SHOW_DEBUG
    word frames;
    unsigned long lastFpsCheck;
#endif

// Eigene Variablendeklaration
#ifdef EVENTDAY
    // Fuer die Anzeige eines Symbols bei einem Ereignis
    char eventdaySymbol;
#endif

#ifdef COUNTDOWN
    // Fuer den Ereignis-Countdown
    int countdown;
#endif

// Fuer LDR-Nachtmodus
byte LdrThresholdZustand;
// Fuer den Näherungssensor
boolean nightLock;

// 0: Nachtschaltung via Zeitsteuerung erlaubt
// 1: Nachtschaltung via Zeitsteuerung deaktiviert + (Nur bei 5-Tasten-Wortwecker: DCF77-Empfänger ein- (und Näherungssensor ausgeschaltet))
// 2: (Nur bei 5-Tasten-Wortwecker: Wie 1 + DCF77-Empfänger aus- (und Näherunssensor eingeschaltet))
byte nightByTimeLock;

// Fuer automatischen Rücksprung zur Standardanzeige
byte jumpToTime;

/**
 * Automatisches Zurückschalten von einer definierten Anzeige auf die Zeitanzeige nach einer
 * festgelegten Zeitspanne jumpToTime. Ist dieser Wert == 0, so findet kein
 * automatischer Rücksprung statt.
 * Achtung!: Als Modi (von denen aus zurückgesprungen werden soll) nur Standardmodi verwenden.
 */
static void updateJumpToTime() {
    if (TIMEOUT_JUMP_TO_TIME > 0) {
        switch (mode) {
            case STD_MODE_SECONDS:
            case STD_MODE_DATE:
                jumpToTime--;
                if (!jumpToTime) {
                    mode = STD_MODE_NORMAL;
                }
                break;
        }
    }
}

/* 
 *  Es wird geprüft, ob aktuell ein Countdown geplant ist
 *  und dieser berechnet.
 */
#ifdef COUNTDOWN
    void CheckCountdown() {
        countdown = -1;
        eventdaySymbol = -1;
        for (byte i = 0; i < sizeof(eventdayObject)/sizeof(EventdayObject); i++) {
            byte countdownDay = pgm_read_byte_near(&eventdayObject[i].countdownDay);
            byte countdownMonth = pgm_read_byte_near(&eventdayObject[i].countdownMonth);
            if (countdownDay && countdownMonth) {
                TimeStamp TSTemp( pgm_read_byte_near(&eventdayObject[i].countdownMinute),
                                  pgm_read_byte_near(&eventdayObject[i].countdownHour),
                                  countdownDay,
                                  0,
                                  countdownMonth,
                                  rtc.getYear());
                long diff_0 = TSTemp.getMinutesOfCentury() - rtc.getMinutesOfCentury();
                long diff_1;
                byte countdownMinutes = pgm_read_byte_near(&eventdayObject[i].countdownMinutes);
                if ( (diff_0 >= 0) && (diff_0 <= countdownMinutes) ) {
                    diff_1 = diff_0;
                } else {
                    // Nur für Silvester / Neujahr (Jahreswechsel) erforderlich
                    TSTemp.incYear(1, true);
                    diff_1 = TSTemp.getMinutesOfCentury() - rtc.getMinutesOfCentury();
                }
                if ( (diff_1 >= 0) && (diff_1 <= countdownMinutes) ) {
                    countdown = diff_1 * 60 - rtc.getSeconds();
                    eventdaySymbol = i;
                    break;
                }
            }
        }
    }
    
    static void updateCountDown() {
        if (mode == STD_MODE_COUNTDOWN)
            countdown--;
    }
#endif


/**
 * Aenderung der Anzeige als Funktion fuer den Interrupt, der ueber das SQW-Signal
 * der Real-Time-Clock gesetzt wird. Da die Wire-Bibliothek benutzt wird, kann man
 * den Interrupt nicht direkt benutzen, sondern muss eine Hilfsvariable setzen, die
 * dann in loop() ausgewertet wird.
 */
void updateFromRtc() {
    // Automatischer Rücksprung nach jumpToTime Sekunden auf Zeitanzeige
    updateJumpToTime();
    #ifdef COUNTDOWN
        // Countdown aktualisieren
        updateCountDown();
    #endif
    #ifdef ALARM_OPTION_ENABLE
        alarm->updateShowAlarmTimer(mode == STD_MODE_ALARM);
    #endif
    needsUpdateFromRtc = true;
    // die Zeit verursacht ein kurzes Flackern. Wir muessen
    // sie aber nicht immer lesen, im Modus 'normal' alle 60 Sekunden,
    // im Modus 'seconds' alle Sekunde, sonst garnicht.
    helperSeconds++;
    if (helperSeconds >= 60) {
        helperSeconds = 0;
    }
}

/**
 * Den freien Specher abschaetzen.
 * Kopiert von: http://playground.arduino.cc/Code/AvailableMemory
 */
int freeRam() {
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

/**
 * Initialisierung. setup() wird einmal zu Beginn aufgerufen, wenn der
 * Arduino Strom bekommt.
 */
void setup() {
    Serial.begin(SERIAL_SPEED);
    Serial.println(F("Qlockthree is initializing..."));
    DEBUG_PRINTLN(F("... and starting in debug-mode..."));
    Serial.flush();

    pinMode(PIN_DCF77_PON, OUTPUT);
    #ifdef DCF77_SENSOR_EXISTS
        dcf77.enable(false);
    #endif

    // LED-Treiber initialisieren
    ledDriver.init();
    // Inhalt des Led-Treibers loeschen...
    ledDriver.clearData();
    // und Inhalt des Bildspeichers loeschen
    renderer.clearScreenBuffer(matrix);
    // wir brauchen nur 10 Zeilen...
    ledDriver.setLinesToWrite(10);

    // starte Wire-Library als I2C-Bus Master
    Wire.begin();

    // RTC-Interrupt-Pin konfigurieren
    pinMode(PIN_SQW_SIGNAL, INPUT);
    digitalWrite(PIN_SQW_SIGNAL, HIGH);

    #ifdef ALARM_OPTION_ENABLE
        alarm->speakerPin(PIN_SPEAKER);
    #endif

    // DCF77-LED drei Mal als 'Hello' blinken lassen
    // und Speaker piepsen kassen, falls ENABLE_ALARM eingeschaltet ist.
    #ifdef DCF77_SENSOR_EXISTS
        for (byte i = 0; i < 3; i++) {
            dcf77.statusLed(true);
            delay(100);
            dcf77.statusLed(false);
            delay(100);
        }
    #endif

    Serial.print(F("Compiled: "));
    Serial.print(F(__TIME__));
    Serial.print(F(" / "));
    Serial.println(F(__DATE__));

    /*
    // Uhrzeit nach Compile-Zeit stellen...
    rtc.set(__DATE__, __TIME__);
    rtc.writeTime();
     */

    // RTC starten...
    rtc.readTime();
    if ((rtc.getSeconds() >= 60) || (rtc.getMinutes() >= 60) || (rtc.getHours() >= 24) || (rtc.getYear() < 16)) {
        // Echtzeituhr enthaelt Schrott, neu mit erkennbaren Zahlen beschreiben...
        DEBUG_PRINT(F("Resetting RTC..."));
        rtc.set(11, 11, 1, 1, 1, 16);
        rtc.setSeconds(11);
    }

#ifdef DS1307
    Serial.println(F("Uhrentyp ist DS1307."));
    rtc.enableSQWOnDS1307();
#elif defined DS3231
    Serial.println(F("Uhrentyp ist DS3231."));
    rtc.enableSQWOnDS3231();
#else
    Definition_des_Uhrtyps_fehlt!
    In der Configuration.h muss der Uhrentyp angegeben werden!
#endif

    rtc.writeTime();
    helperSeconds = rtc.getSeconds();
    Serial.print(F("RTC-Time: "));
    Serial.print(rtc.getHours());
    Serial.print(F(":"));
    Serial.print(rtc.getMinutes());
    Serial.print(F(":"));
    Serial.print(rtc.getSeconds());
    Serial.print(F(" RTC-Date: "));
    Serial.print(rtc.getDate());
    Serial.print(F("."));
    Serial.print(rtc.getMonth());
    Serial.print(F("."));
    Serial.println(rtc.getYear());

    // Den Interrupt konfigurieren,
    // nicht mehr CHANGE, das sind 2 pro Sekunde,
    // RISING ist einer pro Sekunde, das reicht.
    // Auf FALLING geandert, das signalisiert
    // den Sekundenwechsel, Danke an Peter.
    attachInterrupt(0, updateFromRtc, FALLING);

    // Werte vom LDR einlesen und vermuellen, da die ersten nichts taugen...
    for (int i = 0; i < 1000; i++) {
        analogRead(PIN_LDR);
    }

    // rtcSQWLed-LED drei Mal als 'Hello' blinken lassen
    // und Speaker piepsen kassen, falls ENABLE_ALARM eingeschaltet ist.
    for (byte i = 0; i < 3; i++) {
        rtc.statusLed(true);
        delay(100);
        rtc.statusLed(false);
        delay(100);
    }

    // ein paar Infos ausgeben
    Serial.println(F("... done and ready to rock!"));

    Serial.print(F("Version: "));
    Serial.println(F(FIRMWARE_VERSION));

    Serial.print(F("Driver: "));
    ledDriver.printSignature();

#ifndef REMOTE_NO_REMOTE
    Serial.print(F("Remote: "));
    irTranslator.printSignature();
    irrecv.enableIRIn();
#else
    Serial.println(F("Remote: disabled."));
#endif

    #ifdef ALARM_OPTION_ENABLE
        Serial.println(F("Alarm is enabled."));
    #endif
    DEBUG_PRINT(F("24h mode = "));
    DEBUG_PRINTLN(settings.get24hMode());
    DEBUG_PRINT(F("Alarmtime is "));
    DEBUG_PRINTLN(alarm->asString());
    DEBUG_FLUSH();

    if (settings.getDcfSignalIsInverted()) {
        Serial.println(F("DCF77-Signal is inverted."));
    }

    Serial.print(F("Free ram: "));
    Serial.print(freeRam());
    Serial.println(F(" bytes."));

    Serial.flush();

    // DCF77-Empfaenger einschalten...
    #ifdef DCF77_SENSOR_EXISTS
        dcf77.enable(true);
    #endif
    // Display einschalten...
    ledDriver.wakeUp();
    ledDriver.setBrightness(settings.getBrightness());
}

/*
 * Schreibroutinen für Buchstaben und Zahlen
 */
void write1xyStab(char char1, byte posx, byte posy) {
    for (byte i = 0; i < 5; i++) {
        matrix[posy + i] |= pgm_read_byte_near(&(staben[char1 - 'A'][i])) << posx;
    }
}

void write2yStaben(char char1, char char2, byte posy) {
    write1xyStab(char1, 11, posy);
    write1xyStab(char2,  5, posy);
}

void write2Staben(char char1, char char2) {
    write2yStaben(char1, char2, 2);
}

void write4Staben(char char1, char char2, char char3, char char4) {
    write2yStaben(char1, char2, 0);
    write2yStaben(char3, char4, 5);
}

void write1xyDigit(byte number, byte posx, byte posy) {
    for (byte i = 0; i < 7; i++) {
        matrix[posy + i] |= pgm_read_byte_near(&(ziffern[number][i])) << posx;
    }
}

void write2yDigits(byte number, byte posy) {
    write1xyDigit(number / 10, 11, posy);
    write1xyDigit(number % 10,  5, posy);
}

void write2ySmallDigits(byte number, byte posy) {
    for (byte i = 0; i < 5; i++) {            
        matrix[posy + i] |= pgm_read_byte_near(&(ziffernKlein[number / 10][i])) << 12;
        matrix[posy + i] |= pgm_read_byte_near(&(ziffernKlein[number % 10][i])) << 7;
    }
}

void write4SmallDigits(byte firstNumber, byte secondNumber) {
    write2ySmallDigits(firstNumber , 0);
    write2ySmallDigits(secondNumber, 5);
}

#ifdef EVENTDAY
    void writeEventSymbol() {
        #ifdef EVENTDAY_CALCULATE_NEW_YEARS_EVE
            if (eventdaySymbol == 0) {
                write4SmallDigits(20, rtc.getYear());
            } else
        #endif
            {
                for (byte i = 0; i < 10; i++) { 
                    matrix[i] |= pgm_read_word_near(&(eventdayObject[(byte) eventdaySymbol].symbol[i])) << 5;
                }
            }
    }
#endif


/**
 * loop() wird endlos auf alle Ewigkeit vom Microcontroller durchlaufen
 */
void loop() {
    static boolean _isAlarmLedOn = false;
    
    /*
     * FPS
     */
#if defined(DEBUG) && defined(FPS_SHOW_DEBUG)
    frames++;
    if (millis() - lastFpsCheck > 1000) {
        DEBUG_PRINT("FPS: ");
        DEBUG_PRINTLN(frames);
        lastFpsCheck = millis();
        frames = 0;
    }
#endif

    /*
     * Dimmung.
     */
    if (settings.getUseLdr()) {
        if (millis() - lastBrightnessCheck > LDR_CHECK_RATE) { // langsam nachsehen...
            byte lv = ldr.value();
            byte currentBrightness = ledDriver.getBrightness();
            if (currentBrightness > lv) {
                ledDriver.setBrightness(settings.changeBrightness(-1));
            } else if (currentBrightness < lv) {
                ledDriver.setBrightness(settings.changeBrightness(1));
            }
            lastBrightnessCheck = millis();
        }
    }

    /*
     * Der optische Naeherungssensor
     */
    #if defined(WW_5_BUTTONS) && defined(WW_5_BUTTONS_NEAR_SENSOR_ENABLE)
        CheckNearSensorIn_Blank_Night();    
    #endif

    /*
     * needsUpdateFromRtc wird via Interrupt gesetzt ueber fallende
     * Flanke des SQW-Signals von der RTC.
     * Oder falls eine Tasten-Aktion eine sofortige Aktualisierung des Displays braucht.
     */
    if (needsUpdateFromRtc) {
        needsUpdateFromRtc = false;

        /*
         * Zeit einlesen...
         */
        switch (mode) {
            default:
                if (helperSeconds == 0) {
                    rtc.readTime();
                }
                break;
//            case STD_MODE_SECONDS:
//            case STD_MODE_BLANK:
//            case STD_MODE_NIGHT:
//                rtc.readTime();
//                helperSeconds = rtc.getSeconds();
//                break;
                // andere Modi egal...
        }

        /*
         * Vorbereitende Aufgaben für Countdown erledigen
         */
        #ifdef COUNTDOWN       
            switch (mode) {
                case STD_MODE_NORMAL:
                    #ifndef COUNTDOWN_AUTO_JUMP_TO
                        break;
                    #endif
                case STD_MODE_COUNTDOWN:
                    if (helperSeconds == 0) {
                        CheckCountdown();
                        if (countdown >= 0) {
                            mode = STD_MODE_COUNTDOWN;
                        }
                    }   
                break;
                // andere Modi egal...
            }
        #endif

        if (helperSeconds == 0) {
            helperSeconds = rtc.getSeconds();
        }

        /*
         * Nachschaltung prüfen...
         * Diese verbessert den DCF77-Empfang bzw. ermoeglicht ein dunkles Schlafzimmer.
         */
        // Vorbedingung: Nur bei Erfüllung dieser Bedinung soll Nachtschaltung erfolgen
        if ( (!nightLock) && (mode < EXT_MODE_START) && (mode != STD_MODE_BRIGHTNESS)
            #ifdef WW_5_BUTTONS 
                && (mode != STD_MODE_ALLON) 
            #endif 
        ) {
            // Wenn im BLANK-Modus, in diesem bleiben
            if (mode != STD_MODE_BLANK) {
                // Display LDR-gesteuert ein- und abschalten
                if (settings.getBlankByLdr()) {
                    if (mode == STD_MODE_NIGHT) {
                        if (checkLdrThreshold() || alarm->isActive() || alarm->isSnooze()) {
                            // Wenn LDR über dem Grenzwert liegt, der Wecker klingelt oder im Snooze ist, wird das Display wieder eingeschaltet.
                            goToNormalDispOn();
                        }
                    } else {
                        if (!checkLdrThreshold() && !alarm->isActive() && !alarm->isSnooze()) {
                            goToNight();
                        }
                    }
                } else {
                    if (mode == STD_MODE_NIGHT) {
                        if (checkNight()) {
                            // Wenn Nacht vorbei, wird Display wieder eingeschaltet
                            goToNormalDispOn();
                        }
                    }   
                    else if (!nightByTimeLock) {
                        if (!checkNight()) {
                            nightByTimeLock = 1;
                            goToNight();
                        }
                    }
                }
            }
            // Sonderfall für den 5-Tasten-Wortwecker mit Näherungssensor und A0-Hack
            // Die Zeitangaben für die Nachtschaltung werden verwendet (auch wenn LDR für Nachtschaltung Verwendung findet),
            // um den Näherungssensor aus- und den DCF77-Empfänger einzuschalten.
            #if defined(WW_5_BUTTONS) && defined(WW_5_BUTTONS_NEAR_SENSOR_ENABLE) && defined(WW_5_BUTTONS_ENABLE_NEARSENSOR_A0)
                if ( (mode == STD_MODE_BLANK) || (mode == STD_MODE_NIGHT) ) {
                    if (!nightByTimeLock)
                        if (!checkNight())
                            nightByTimeLock = 1;
                }
            #endif
        }
        // Ein gesetzter nightByTimeLock bleibt so lange bestehen bis die (zeitgesteuerte) Nacht vorüber ist. (Deckt auch Sonderfall für 5-Tasten-Wortwecker ab.)
        if (nightByTimeLock)
            if (checkNight())
                nightByTimeLock = 0;

        DEBUG_PRINT(F("Mode | nightByTimeLock | nightLock: "));
        DEBUG_PRINT(mode);
        DEBUG_PRINT(F(" | "));
        DEBUG_PRINT(nightByTimeLock);
        DEBUG_PRINT(F(" | "));
        DEBUG_PRINTLN(nightLock);
        DEBUG_FLUSH();
        DEBUG_PRINTLN(rtc.asString());

        /*
         * Variablendeklaration (temporär)
         */
        unsigned long ul_min;
        byte alarm_hours;
        byte rtc_minutes = rtc.getMinutes();
        byte rtc_date = rtc.getDate();
        byte rtc_month = rtc.getMonth();
        byte b_language = settings.getLanguage();
        byte b_brightnessToDisplay; // Für die Anzeige der Helligkeit mit Balken
        char c_TimeShift = settings.getTimeShift();
        char c_abs_TimeShift = abs(c_TimeShift);
        boolean bool_esIst = settings.getEsIst();
        boolean bool_corner = settings.getRenderCornersCw();
        TimeStamp* t_TimeStamp;

        /*
         * Bildschirmpuffer beschreiben...
         */
        renderer.clearScreenBuffer(matrix);
        renderer.setAfternoon(0);
        switch (mode) {
            case EXT_MODE_TIMESET:
                renderer.setAfternoon(rtc.getHours());
            case STD_MODE_NORMAL:
                renderer.setMinutes(rtc.getHours(), rtc_minutes, b_language, matrix);
                renderer.setCorners(rtc_minutes, bool_corner, matrix);
                if (!bool_esIst && ((rtc_minutes / 5) % 6)) {
                    renderer.cleanIntroWords(b_language, matrix); // ES IST weg
                }
                break;
            case EXT_MODE_TIME_SHIFT:
                if (c_TimeShift < 0) {
                    for (byte x = 0; x < 3; x++) {
                        ledDriver.setPixelInScreenBuffer(x, 1, matrix);
                    }
                } else if (c_TimeShift > 0) {
                    for (byte x = 0; x < 3; x++) {
                        ledDriver.setPixelInScreenBuffer(x, 1, matrix);
                    }
                    for (byte y = 0; y < 3; y++) {
                        ledDriver.setPixelInScreenBuffer(1, y, matrix);
                    }
                }
                write1xyDigit(c_abs_TimeShift % 10, 5, 3);
                if (c_abs_TimeShift > 9) {
                    write1xyDigit(1, 10, 3);
                }
                break;
            #ifdef ALARM_OPTION_ENABLE
                case STD_MODE_ALARM:
                   if ((alarm->getShowAlarmTimer() == 0) || alarm->isEnable()) {
                        alarm->toggleEnable();
                        settings.saveToEEPROM();
                        goToNormalDispOn();
                    } else {
                        alarm_hours = alarm->getHours();
                        if (settings.get24hMode())
                           renderer.setAfternoon(alarm_hours);
                        renderer.setMinutes(alarm_hours, alarm->getMinutes(), b_language, matrix);
                        renderer.setCorners(alarm->getMinutes(), bool_corner, matrix);
                        renderer.cleanIntroWords(b_language, matrix); // ES IST weg  
                    }
                    break;
            #endif
            #ifdef DATE_ENABLE
                case STD_MODE_DATE:
                    #ifdef EVENTDAY
                        /**
                         * Es wird geprüft, ob der heutige Tag ein Ereignis ist und das entsprechend
                         * definierte Symbol dazu auf der Datumsanzeige ausgegeben.
                         * Ereignisse werden in der Ereignisse.h definiert.
                         */  
                        eventdaySymbol = -1;
                        for (byte i = 0; i < sizeof(eventdayObject)/sizeof(EventdayObject); i++) {
                            if ( (pgm_read_byte_near(&eventdayObject[i].day) == rtc_date) && (pgm_read_byte_near(&eventdayObject[i].month) == rtc_month) ) {
                                eventdaySymbol = i;
                                break;
                            }
                        }      
                        if (eventdaySymbol != -1) {
                            // Anzeige des Geburtstagssymbols
                            writeEventSymbol();
                        } else
                    #endif
                    {
                        // Anzeige des Datums
                        #ifdef DATE_MONTH_SHOW
                            write4SmallDigits(rtc_date, rtc_month);
                            ledDriver.setPixelInScreenBuffer(10, 4, matrix);
                            ledDriver.setPixelInScreenBuffer(10, 9, matrix);
                        #else
                            write2yDigits(rtc_date, 1);
                        #endif
                    }
                    break;
            #endif
            #ifdef COUNTDOWN
                case STD_MODE_COUNTDOWN:
                    /**
                     * Dieser Modus zeigt den Countdown zu einem Ereignis an.
                     * Ereignisse werden in Ereignisse.h definiert.
                     */ 
                    if (countdown >= 60) {
                        // Anzeige des Countdowns (Minuten und Sekunden)
                        write4SmallDigits(countdown / 60 % 60, countdown % 60);
                        ledDriver.setPixelInScreenBuffer(10, 1, matrix);
                        ledDriver.setPixelInScreenBuffer(10, 3, matrix);
                    } else if (countdown >= 0) {
                        // Anzeige des Countdowns (nur Sekunden)
                        write2yDigits(countdown, 1);
                    } else if (countdown > -COUNTDOWN_BLINK_DURATION) {
                        // Anzeige des Symbols blinkend im Sekundentakt
                        if (countdown % 2) {
                            writeEventSymbol();
                        }
                    } else {
                        // Rücksprung auf Uhrzeit
                        mode = STD_MODE_NORMAL;
                    }
                    break;
            #endif
            case EXT_MODE_MAIN_SETTINGS_START:
                write4Staben('M', 'A', 'I', 'N');
                break;
            case EXT_MODE_TIME_SETTINGS_START:
                write4Staben('T', 'I', 'M', 'E');
                break;
            case EXT_MODE_TEST_DEBUG_START:
                write4Staben('T', 'E', 'S', 'T');
                break;
            case EXT_MODE_YEARSET:
                    write2yStaben('Y', 'Y', 0);
                    write2ySmallDigits(rtc.getYear(), 5);
                    break;
            case EXT_MODE_MONTHSET:
                    write2yStaben('M', 'M', 0);
                    write2ySmallDigits(rtc.getMonth(), 5);
                    break;
            case EXT_MODE_DATESET:
                    write2yStaben('D', 'D', 0);
                    write2ySmallDigits(rtc.getDate(), 5);
                    break;
            case EXT_MODE_OFFTIME_MOFR:
            case EXT_MODE_OFFTIME_SASO:
                ledDriver.setPixelInScreenBuffer(10, 7, matrix);
            case EXT_MODE_ONTIME_MOFR:
            case EXT_MODE_ONTIME_SASO:
                t_TimeStamp = getNightTimeStamp(mode);
                write4SmallDigits(t_TimeStamp->getHours(), t_TimeStamp->getMinutes());
                ledDriver.setPixelInScreenBuffer(10, 1, matrix);
                ledDriver.setPixelInScreenBuffer(10, 3, matrix);
                break; 
            case STD_MODE_SECONDS:
                renderer.setCorners(rtc_minutes, bool_corner, matrix);
                write2yDigits(helperSeconds, 1);
                break;
            case EXT_MODE_LDR_MODE:
                if (settings.getUseLdr()) {
                    write1xyStab('A', 8, 3);
                } else {
                    write1xyStab('M', 8, 3);
                }
                break;
            case EXT_MODE_LDR_NIGHT_MODE:
                if (settings.getBlankByLdr()) {
                    write4Staben('L', 'N', 'E', 'N');
                } else {
                    write4Staben('L', 'N', 'D', 'A');
                }
                break;
            case EXT_MODE_LDR_NIGHT_THRESH:
                write2yDigits(settings.getLdrBlankThreshold(), 3);
                break;
            case STD_MODE_BLANK:
            case STD_MODE_NIGHT:
            case EXT_MODE_DCF_BLANK:
                break;
            #ifdef WW_5_BUTTONS
                case STD_MODE_ALLON:
                    renderer.setAllScreenBuffer(matrix);
                    break;
            #endif
            case STD_MODE_BRIGHTNESS:
                #ifdef BRIGHTNESS_SETTING_IN_PERCENTAGE
                    write2yDigits(settings.getBrightness(), 1);
                #else
                    ledDriver.setPixelInScreenBuffer(0, 9, matrix);
                    b_brightnessToDisplay = settings.getBrightness() / 10;
                    for (byte xb = 0; xb < b_brightnessToDisplay; xb++) {
                        for (byte yb = 0; yb <= xb; yb++) {
                            matrix[9 - yb] |= 1 << (14 - xb);
                        }
                    }
                #endif
                break;
            case EXT_MODE_CORNERS:
                if (bool_corner) {
                    write2Staben('C', 'W');
                } else {
                    write1xyStab('C', 8, 0);
                    write2yStaben('C', 'W', 5);
                }
                break;
            case EXT_MODE_ES_IST:
                if (bool_esIst) {
                    write4Staben('I', 'T', 'E', 'N');
                } else {
                    write4Staben('I', 'T', 'D', 'A');
                }
                break;
            #ifdef ALARM_OPTION_ENABLE
                case EXT_MODE_ALARM_MELODY:
                    write1xyStab('A', 8, 0);
                    write2ySmallDigits(alarm->getAlarmMelody(), 5);
                    break;
                case EXT_MODE_24:
                    if (settings.get24hMode()) {
                       write2ySmallDigits(24, 0);
                    } else {
                       write2ySmallDigits(12, 0);
                    }
                    write1xyStab('H', 4, 5);
                    break;
                case EXT_MODE_SNOOZE:
                    write2yStaben('S', 'N', 0);
                    write2ySmallDigits(settings.getSnooze(), 5);
                    break;
             #endif
            case EXT_MODE_DCF_IS_INVERTED:
                if (settings.getDcfSignalIsInverted()) {
                    write4Staben('R', 'S', 'I', 'N');
                } else {
                    write4Staben('R', 'S', 'N', 'O');
                }
                break;
            case EXT_MODE_LANGUAGE:
                for (byte i = 0; i < 5; i++) {
                    switch (b_language) {
                        #ifdef SPRACHE_DE
                            case LANGUAGE_DE_DE:
                                write2Staben('D', 'E');
                                break;
                            case LANGUAGE_DE_SW:
                                write4Staben('D', 'E', 'S', 'W');
                                break;
                            case LANGUAGE_DE_BA:
                                write4Staben('D', 'E', 'B', 'A');
                                break;
                            case LANGUAGE_DE_SA:
                                write4Staben('D', 'E', 'S', 'A');
                                break;
                        #endif
                        #ifdef SPRACHE_CH
                            case LANGUAGE_CH:
                                write2Staben('C', 'H');
                                break;
                        #endif
                        #ifdef SPRACHE_EN
                            case LANGUAGE_EN:
                                write2Staben('E', 'N');
                                break;
                        #endif
                        #ifdef SPRACHE_FR
                            case LANGUAGE_FR:
                                write2Staben('F', 'R');
                                break;
                        #endif
                        #ifdef SPRACHE_IT
                            case LANGUAGE_IT:
                                write2Staben('I', 'T');
                                break;
                        #endif
                        #ifdef SPRACHE_NL
                            case LANGUAGE_NL:
                                write2Staben('N', 'L');
                                break;
                        #endif
                        #ifdef SPRACHE_ES
                            case LANGUAGE_ES:
                                write2Staben('E', 'S');
                                break;
                        #endif
                        #ifdef SPRACHE_PT
                            case LANGUAGE_PT:
                                write2Staben('P', 'T');
                                break;
                        #endif
                    }
                }
                break;
 
            case EXT_MODE_TEST:
                renderer.setCorners(helperSeconds % 5, bool_corner, matrix);
                renderer.activateAlarmLed(matrix);
                for (byte i = 0; i < 11; i++) {
                    ledDriver.setPixelInScreenBuffer(x, i, matrix);
                }
                x++;
                if (x > 10) {
                    x = 0;
                }
                break;
            #ifdef DCF77_SENSOR_EXISTS
                case EXT_MODE_DCF_SYNC:
                    // Anzeige des letzten erfolgreichen DCF77-Syncs (samplesOK) in Stunden : Minuten
                    ul_min = rtc.getMinutesOfCentury() - dcf77.getDcf77LastSuccessSyncMinutes();
                    if (ul_min > 5999)
                        ul_min = 5999;
                    write4SmallDigits(ul_min / 60, ul_min % 60);
                    ledDriver.setPixelInScreenBuffer(10, 1, matrix);
                    ledDriver.setPixelInScreenBuffer(10, 3, matrix);
                break;
                case EXT_MODE_DCF_DEBUG:
                    needsUpdateFromRtc = true;
                    renderer.setCorners(dcf77.getDcf77ErrorCorner(), bool_corner, matrix);
                    break;
            #endif
        }

        // Leert die Anzeige, wenn eine Zeit >= 12 Uhr mittags angezeigt wird und der Takt dies erfordert.
        renderer.clearScreenIfNeeded_DisplayOnBlinking(matrix);

        // Schaltet die Alarm-LED ein (blinkend oder dauerhaft)
        if (_isAlarmLedOn)
            renderer.activateAlarmLed(matrix);

        // Update mit onChange = true, weil sich hier (aufgrund needsUpdateFromRtc) immer was geaendert hat.
        // Entweder weil wir eine Sekunde weiter sind, oder weil eine Taste gedrueckt wurde.
        ledDriver.writeScreenBufferToMatrix(matrix, true);
    }

    /* 
     * Display blinken lassen bei Zeitanzeige >= 12 Uhr (Nachmittag)
     */
    if (renderer.pollDisplayBlinkAfternoon())
           needsUpdateFromRtc = true;
       
    /*
     * Tasten abfragen (Code mit 3.3.0 ausgelagert, wegen der Fernbedienung)
     */
    // Taste Minuten++ und Stunden++ (brightness) gedrueckt?
    if (extModeDoubleButton.pressed())
        extModeDoublePressed();  

#ifdef WW_5_BUTTONS
    // Taste Alarm gedrueckt?
    if (alarmButton.pressed())
        alarmPressed();
#endif
    
    // Taste Minuten++ (brighness++) gedrueckt?
    if (minutesPlusButton.pressed())
        minutePlusPressed();

    // Taste Stunden++ (brightness--) gedrueckt?
    if (hoursPlusButton.pressed())
        hourPlusPressed();

    // Taste Moduswechsel gedrueckt?
    if (modeChangeButton.pressed())
        modePressed();

    /*
     * Tasten der Fernbedienung abfragen...
     */
#ifndef REMOTE_NO_REMOTE
    if (irrecv.decode(&irDecodeResults)) {
        DEBUG_PRINT(F("Decoded successfully as "));
        DEBUG_PRINTLN2(irDecodeResults.value, HEX);
        needsUpdateFromRtc = true;
        switch (irTranslator.buttonForCode(irDecodeResults.value)) {
            case REMOTE_BUTTON_MODE:
                modePressed();
                break;
            case REMOTE_BUTTON_MINUTE_PLUS:
                minutePlusPressed();
                break;
            case REMOTE_BUTTON_HOUR_PLUS:
                hourPlusPressed();
                break;
            case REMOTE_BUTTON_BRIGHTER:
                setDisplayBrighter();
                break;
            case REMOTE_BUTTON_DARKER:
                setDisplayDarker();
                break;
            case REMOTE_BUTTON_EXTMODE:
                extModeDoublePressed_EnterExtMode();
                break;
            case REMOTE_BUTTON_TOGGLEBLANK:
                toggleDisplay();
                break;
            case REMOTE_BUTTON_BLANK:
                goTo_leaveFrom_Blank(true);
                break;
            case REMOTE_BUTTON_RESUME:
                goTo_leaveFrom_Blank(false);
                break;
            case REMOTE_BUTTON_SETCOLOR:
                ledDriver.setColor(irTranslator.getRed(), irTranslator.getGreen(), irTranslator.getBlue());
                break;
        }
        irrecv.resume();
    }
#endif

    /*
     * DCF77-Empfänger ein-/aus- und Näherungssensor aus-/einschalten via A0-Hack
     */
    #if defined(WW_5_BUTTONS) && defined(WW_5_BUTTONS_NEAR_SENSOR_ENABLE) && defined(WW_5_BUTTONS_ENABLE_NEARSENSOR_A0)
        if (alarm->isActive()) {
            dcf77.enable(false);
        } else {
            switch (mode) {
                case STD_MODE_BLANK:
                case STD_MODE_NIGHT:
                    if (nightByTimeLock == 1) 
                        dcf77.enable(true);
                    else 
                        dcf77.enable(false);
                    break;
                default:
                    dcf77.enable(true);
                    break;
            }
        }
    #endif  

    /*
     * Alarm?
     */ 
    #ifdef ALARM_OPTION_ENABLE
        // Alarm-LED
        if (_isAlarmLedOn != alarm->pollLed(mode == STD_MODE_ALARM)) {
            _isAlarmLedOn = !_isAlarmLedOn;
            needsUpdateFromRtc = true;
        }
        // Alarm
        if ( (mode != STD_MODE_ALARM) && alarm->isEnable() ) { 
           if (settings.get24hMode()) {
               alarm->pollAlarm(rtc.getMinutesOfDay());
           }
           else {
               alarm->pollAlarm(rtc.getMinutesOf12HoursDay());
           }
        }
        #if defined(WW_5_BUTTONS) && defined(WW_5_BUTTONS_NEAR_SENSOR_ENABLE)
            if (alarm->isActive() && (settings.getSnooze() != 0)) {
                if (CheckNearSensor()) {
                    // Näherungssensor betätigt? -> Snooze
                    alarm->activateSnooze();
                }
            }
        #endif 
    #endif

    /*
     * Die Matrix auf die LEDs multiplexen, hier 'Refresh-Zyklen'.
     */
    if (!isCurrentModeDarkMode()) {
        ledDriver.writeScreenBufferToMatrix(matrix, false);
    }

    /*
     * Status-LEDs ausgeben
     */
#if defined(DCF77_SENSOR_EXISTS) && defined(ENABLE_DCF_LED)
    dcf77.statusLed(dcf77.signal(settings.getDcfSignalIsInverted()));
#endif
#ifdef ENABLE_SQW_LED
    rtc.statusLed(digitalRead(PIN_SQW_SIGNAL) == HIGH);
#endif

    /*
     * DCF77-Empfaenger anticken...
     */
    #ifdef DCF77_SENSOR_EXISTS
        if (dcf77.poll(settings.getDcfSignalIsInverted()))
            manageNewDCF77Data();
    #endif
}

/**
 * Was soll ausgefuehrt werden, wenn die H+ und M+ -Taste zusammen gedrueckt wird?
 */
void extModeDoublePressed() {
    // Bedingung erforderlich, um in der Weckzeiteinstellung und im Menü
    // Stunden und Minuten gleichzeitig einstellen zu können.
    if ((mode != STD_MODE_ALARM) && (mode < EXT_MODE_START)) {
        minutesPlusButton.lock();
        hoursPlusButton.lock();
    }
    if (CheckAlarmAndSnoozeOrDeactivate()) {
        DEBUG_PRINT(F("Minutes plus AND hours plus pressed in "));
        // M+ und H+ im STD_MODE_BLANK gedrueckt?
      
        if (mode == STD_MODE_BLANK) {
            DEBUG_PRINTLN(F("mode STD_MODE_BLANK..."));
            DEBUG_FLUSH();
            extModeDoublePressed_EnterExtMode();
        }
        
        #ifdef WW_5_BUTTONS 
            else if ( (mode != STD_MODE_ALARM) && (mode < EXT_MODE_START) ) {
                DEBUG_PRINTLN(F("any mode != STD_MODE_ALARM and < EXT_MODE_START..."));
                DEBUG_FLUSH();
                extModeDoublePressed_EnterBrightnessMode();
            }
        #endif
    }
}

void extModeDoublePressed_EnterExtMode() {
    setDisplayToOn();
    mode = EXT_MODE_START;
    DEBUG_PRINTLN(F("Entering EXT_MODEs"));
    DEBUG_FLUSH();
}

#ifdef WW_5_BUTTONS
    void extModeDoublePressed_EnterBrightnessMode() {
        setDisplayToOn(); 
        mode = STD_MODE_BRIGHTNESS;
    }
#endif

/**
 * Was soll ausgefuehrt werden, wenn die Alarm-Taste gedrueckt wird?
 */
#ifdef WW_5_BUTTONS
    void alarmPressed() {
        if (CheckAlarmAndSnoozeOrDeactivate()) {
            setDisplayToOn();
            if (mode == STD_MODE_ALARM) {
                mode = STD_MODE_NORMAL;
                settings.saveToEEPROM();
            } else {
                alarm->resetShowAlarmTimer();
                mode = STD_MODE_ALARM;
            }
        }
    }
#endif

/**
 * Was soll ausgefuehrt werden, wenn die Mode-Taste gedrueckt wird?
 */
void modePressed() {
    if (CheckAlarmAndSnoozeOrDeactivate()) {
        // Da der aktuelle Modus eventuell durch Tastendruck geändert wird, Display wieder einschalten.
        setDisplayToOn();
    
        #ifdef WW_5_BUTTONS
            if (mode == STD_MODE_ALARM)
                alarm->incMinutes(5); 
            else
        #endif  
                {
                    mode++;
                    settings.saveToEEPROM();
            
                    #ifdef WW_5_BUTTONS
                        if (mode < STD_MODE_WW_START) {
                            mode = STD_MODE_WW_START;
                        }
                    #endif
                }
    
        if (mode == STD_MODE_COUNTDOWN) {
            #ifdef COUNTDOWN
                rtc.readTime();
                CheckCountdown();
                if (countdown < 0)
                    mode++;
            #else
                mode++;
            #endif                    
        }
        // Alarm ueberspringen, wenn Alarmmöglichkeit per #define abgeschaltet wurde
        if (mode == STD_MODE_ALARM) {
            // wenn auf Alarm gewechselt wurde, fuer SHOW_ALARM_TIME_TIMER Sekunden die
            // Weckzeit anzeigen.
            #ifdef ALARM_OPTION_ENABLE
                alarm->resetShowAlarmTimer();
            #else
                mode++;
            #endif
        }
        #if !defined(ALARM_OPTION_ENABLE)
            if (mode == EXT_MODE_ALARM_MELODY) {
                mode++;
            }
        #endif
        #if !defined(DATE_ENABLE)
            if (mode == STD_MODE_DATE) {
                mode++;
            }
        #endif
        if (!settings.getUseLdr()) {
            if (mode == EXT_MODE_LDR_NIGHT_MODE)
                mode++;
        }
        if (!settings.getBlankByLdr() || !settings.getUseLdr()) {
            if (mode == EXT_MODE_LDR_NIGHT_THRESH)
                mode++;
        }
        // Zeiten der Nachtschaltung überpspringen, wenn LDR für Nachtschaltung verwendet wird.
        // Wichtig: Die Zeiten für die Nachtabschaltung müssen aber einstellbar bleiben, wenn der A0-Hack verwendet wird!
        #if ( !defined(WW_5_BUTTONS) || !defined(WW_5_BUTTONS_NEAR_SENSOR_ENABLE) || !defined(WW_5_BUTTONS_ENABLE_NEARSENSOR_A0) )
            if (settings.getBlankByLdr()) {
                if (mode == EXT_MODE_OFFTIME_MOFR)
                    mode++;
                if (mode == EXT_MODE_ONTIME_MOFR)
                    mode++;
                if (mode == EXT_MODE_OFFTIME_SASO)
                    mode++;
                if (mode == EXT_MODE_ONTIME_SASO)
                    mode++;   
            }
        #endif
        // EXT_MODE_DCF_SYNC und EXT_MODE_DCF_DEBUG überspringen, wenn kein DCF77-Empfänger eingebaut
        #if !defined(DCF77_SENSOR_EXISTS)
            if (mode == EXT_MODE_DCF_IS_INVERTED)
                mode++;
            if (mode == EXT_MODE_DCF_SYNC)
                mode++;
            if (mode == EXT_MODE_DCF_DEBUG)
                mode++;
            if (mode == EXT_MODE_DCF_BLANK)
                mode++;
        #endif
    
        // Wenn mode außerhalb zulässigem Bereich (beim Wortwecker), zurück auf STD_MODE_NORMAL
        #ifdef WW_5_BUTTONS
            if ((mode >= STD_MODE_WW_END) && (mode < EXT_MODE_START)) {
                mode = STD_MODE_NORMAL;
            }
        #endif
    
        // Wenn mode außerhalb zulässigem Bereich, zurück auf STD_MODE_NORMAL
        if ( ((mode >= STD_MODE_COUNT) && (mode < EXT_MODE_START)) || (mode >= EXT_MODE_COUNT)) {
            mode = STD_MODE_NORMAL;
        }
    
        // Automatischer Rücksprung nach definierter Wartedauer (hier: Zurücksetzen)
        switch (mode) {
            case STD_MODE_SECONDS:
            case STD_MODE_DATE:
                // Timeout für den automatischen Rücksprung von
                // STD_MODE_SECONDS und STD_MODE_DATE zurücksetzen
                jumpToTime = TIMEOUT_JUMP_TO_TIME;
                break;
        }
       
        DEBUG_PRINT(F("Change mode pressed, mode is now "));
        DEBUG_PRINT(mode);
        DEBUG_PRINTLN(F("..."));
        DEBUG_FLUSH();
    
        // Displaytreiber ausschalten, wenn BLANK oder NIGHT
        if (isCurrentModeDarkMode()) {
            setDisplayToOff();
        }
    }
}

/**
 * Was soll ausgefuehrt werden, wenn die H+-Taste gedrueckt wird?
 */
void hourPlusPressed() {
    if (CheckAlarmAndSnoozeOrDeactivate()) {
        resetCheckLdrThreshold_resetNightLock_needsUpdateFromRtc();
    
        DEBUG_PRINTLN(F("Hours plus pressed..."));
        DEBUG_FLUSH();
          
        switch (mode) {
            #ifdef WW_5_BUTTONS
                case STD_MODE_BLANK:
                    if ( !settings.getBlankByLdr() && !checkNight() ) {
                        // Manueller Wechsel zurück in den zeitgesteuerten Nachtmodus (ohne nightByTimeLock zu verändern)
                        goToNight();
                        // Alternative: nightByTimeLock wird verändert:
                        // nightByTimeLock = 0;
                        // goTo_leaveFrom_Blank(true);
                    } else
                        goToNormalDispOn();
                    break;                   
                case STD_MODE_NIGHT:
                case EXT_MODE_DCF_BLANK:
                    goToNormalDispOn();
                    break;
                default:       
                    goTo_leaveFrom_Blank(true);
                    break;
            #else
                case STD_MODE_NORMAL:
                    // Manueller Wechsel zurück in den zeitgesteuerten Nachtmodus
                    nightByTimeLock = 0;
                    break;
            #endif
            case EXT_MODE_MAIN_SETTINGS_START:
                mode = EXT_MODE_TIME_SETTINGS_START;
                break;
            case EXT_MODE_TIME_SETTINGS_START:
                mode = EXT_MODE_TEST_DEBUG_START;
                break;
            case EXT_MODE_TEST_DEBUG_START:
                mode = STD_MODE_NORMAL;
                break;
            case EXT_MODE_TIMESET:
                rtc.incHours();
                helperSeconds = 59;
                rtc.setSeconds(0);
                rtc.writeTime();
                DEBUG_PRINT(F("H is now "));
                DEBUG_PRINTLN(rtc.getHours());
                DEBUG_FLUSH();
                break;
            case EXT_MODE_YEARSET:
                rtc.incYear(10);
                rtc.writeTime();
                break;
            case EXT_MODE_MONTHSET:
                rtc.incMonth(10);
                rtc.writeTime();
                break;
            case EXT_MODE_DATESET:
                rtc.incDate(10);
                rtc.writeTime();
                break;
            case EXT_MODE_TIME_SHIFT:
                rtc.addSubHoursOverflow(settings.decTimeShift());
                rtc.writeTime();
                break;
            case EXT_MODE_OFFTIME_MOFR:
            case EXT_MODE_ONTIME_MOFR:
            case EXT_MODE_OFFTIME_SASO:
            case EXT_MODE_ONTIME_SASO:
                getNightTimeStamp(mode)->incHours();
                break;
            #ifdef ALARM_OPTION_ENABLE
                case STD_MODE_ALARM:
                    alarm->incHours();
                    alarm->resetShowAlarmTimer();
                    DEBUG_PRINT(F("A is now "));
                    DEBUG_PRINTLN(alarm->asString());
                    DEBUG_FLUSH();
                    break;
            #endif
            case STD_MODE_BRIGHTNESS:
                setDisplayDarker();
                break;
            case EXT_MODE_LDR_MODE:
                settings.toggleUseLdr();
                DEBUG_PRINT(F("LDR is now "));
                DEBUG_PRINTLN(settings.getUseLdr());
                DEBUG_FLUSH();
                break;
            case EXT_MODE_LDR_NIGHT_MODE:
                settings.toggleBlankByLdr();
                break;
            case EXT_MODE_LDR_NIGHT_THRESH:
                settings.changeLdrBlankThreshold(-1);
                break;
            case EXT_MODE_CORNERS:
                settings.toggleRenderCornersCw();
                break;
            case EXT_MODE_ES_IST:
                settings.toggleEsIst();
                break;
            #ifdef ALARM_OPTION_ENABLE
                case EXT_MODE_ALARM_MELODY:
                    alarm->incAlarmMelody();
                    break;
            case EXT_MODE_24:
                settings.toggle24hMode();
                break;
            case EXT_MODE_SNOOZE:
                settings.incSnooze();
                break;
            #endif
            case EXT_MODE_DCF_IS_INVERTED:
                settings.toggleDcfSignalIsInverted();
                break;
            case EXT_MODE_LANGUAGE:
                settings.decLanguage();
                break;
        }
    }
}

/**
 * Was soll ausgefuehrt werden, wenn die M+-Taste gedrueckt wird?
 */
void minutePlusPressed() {
    if (CheckAlarmAndSnoozeOrDeactivate()) {
        resetCheckLdrThreshold_resetNightLock_needsUpdateFromRtc();
    
        DEBUG_PRINTLN(F("Minutes plus pressed..."));
        DEBUG_FLUSH();

        switch (mode) {
            #ifdef WW_5_BUTTONS
                case STD_MODE_ALLON:
                    mode = STD_MODE_NORMAL;
                    break;
                default:
                    setDisplayToOn();
                    mode = STD_MODE_ALLON;
                    break;
            #else
                case STD_MODE_NORMAL:
                    // Manueller Wechsel zurück in den zeitgesteuerten Nachtmodus
                    nightByTimeLock = 0;
                    break;
            #endif
            case EXT_MODE_MAIN_SETTINGS_START:
                mode = EXT_MODE_TIME_SETTINGS_START;
                break;
            case EXT_MODE_TIME_SETTINGS_START:
                mode = EXT_MODE_TEST_DEBUG_START;
                break;
            case EXT_MODE_TEST_DEBUG_START:
                mode = STD_MODE_NORMAL;
                break;
            case EXT_MODE_TIMESET:
                rtc.incMinutes();
                helperSeconds = 59;
                rtc.setSeconds(0);
                rtc.writeTime();
                DEBUG_PRINT(F("M is now "));
                DEBUG_PRINTLN(rtc.getMinutes());
                DEBUG_FLUSH();
                break;
            case EXT_MODE_YEARSET:
                rtc.incYear();
                rtc.writeTime();
                break;
            case EXT_MODE_MONTHSET:
                rtc.incMonth();
                rtc.writeTime();
                break;
            case EXT_MODE_DATESET:
                rtc.incDate();
                rtc.writeTime();
                break;
            case EXT_MODE_TIME_SHIFT:
                rtc.addSubHoursOverflow(settings.incTimeShift());
                rtc.writeTime();
                break;
            case EXT_MODE_OFFTIME_MOFR:
            case EXT_MODE_ONTIME_MOFR:
            case EXT_MODE_OFFTIME_SASO:
            case EXT_MODE_ONTIME_SASO:
                getNightTimeStamp(mode)->incMinutes();
                break;
            #ifdef ALARM_OPTION_ENABLE
                case STD_MODE_ALARM:
                    alarm->incMinutes();
                    alarm->resetShowAlarmTimer();
                    DEBUG_PRINT(F("A is now "));
                    DEBUG_PRINTLN(alarm->asString());
                    DEBUG_FLUSH();
                    break;
            #endif
            case STD_MODE_BRIGHTNESS:
                setDisplayBrighter();
                break;
            case EXT_MODE_LDR_MODE:
                settings.toggleUseLdr();
                DEBUG_PRINT(F("LDR is now "));
                DEBUG_PRINTLN(settings.getUseLdr());
                DEBUG_FLUSH();
                break;
            case EXT_MODE_LDR_NIGHT_MODE:
                settings.toggleBlankByLdr();
                break;
            case EXT_MODE_LDR_NIGHT_THRESH:
                settings.changeLdrBlankThreshold(1);
                break;
            case EXT_MODE_CORNERS:
                settings.toggleRenderCornersCw();
                break;
            case EXT_MODE_ES_IST:
                settings.toggleEsIst();
                break;
            #ifdef ALARM_OPTION_ENABLE
                case EXT_MODE_ALARM_MELODY:
                    alarm->decAlarmMelody();
                    break;
                case EXT_MODE_24:
                    settings.toggle24hMode();
                    break;
                case EXT_MODE_SNOOZE:
                    settings.decSnooze();
                    break;
            #endif
            case EXT_MODE_DCF_IS_INVERTED:
                settings.toggleDcfSignalIsInverted();
                break;
            case EXT_MODE_LANGUAGE:
                settings.incLanguage();
                break;

        }
    }
}

/**
 * Korrekte Daten (auf Basis der Pruefbits) vom DCF-Empfaenger
 * bekommen. Sicherheitshalber gegen Zeitabstaende der RTC pruefen.
 */
#ifdef DCF77_SENSOR_EXISTS
    void manageNewDCF77Data() {
        DEBUG_PRINT(F("Captured: "));
        DEBUG_PRINTLN(dcf77.asString());
        DEBUG_FLUSH();
    
        rtc.readTime();
        dcf77Helper.addSample(&dcf77, &rtc);
        // Stimmen die Abstaende im Array?
        // Pruefung mit Datum!
        if (dcf77Helper.samplesOk()) {
            helperSeconds = 59;
            rtc.setSeconds(0);
            rtc.set(&dcf77, false);
            rtc.addSubHoursOverflow(settings.getTimeShift());
            rtc.writeTime();
            DEBUG_PRINTLN(F("DCF77-Time (+/- Timeshift) written to RTC."));
            DEBUG_FLUSH();
            dcf77.setDcf77SuccessSync(&rtc);
            // Spezialfall für 5-Tasten-Wortwecker mit A0-Hack: Der DCF77-Empfänger wird nach erfolgreichen Empfang aus-
            // und der Näherungssensor wieder eingeschaltet.
            #if defined(WW_5_BUTTONS) && defined(WW_5_BUTTONS_NEAR_SENSOR_ENABLE) && defined(WW_5_BUTTONS_ENABLE_NEARSENSOR_A0)
                if (nightByTimeLock == 1)
                    nightByTimeLock = 2;
            #endif
            if (mode == EXT_MODE_DCF_BLANK)
                goToNormalDispOn();
        } else {
            DEBUG_PRINTLN(F("DCF77-Time trashed because wrong distances between timestamps."));
            DEBUG_FLUSH();
        }
    }
#endif

boolean CheckAlarmAndSnoozeOrDeactivate() {
    #ifdef ALARM_OPTION_ENABLE
        if (alarm->isActive() && (settings.getSnooze() != 0)) {
            alarm->activateSnooze();
            goToNormalDispOn();
            return false;
        } else
        if (alarm->isActive() || alarm->isSnooze()) {
            alarm->deactivate();
            settings.saveToEEPROM();
            goToNormalDispOn();
            return false;
        }
    #endif
    return true;
}

#if defined(WW_5_BUTTONS) && defined(WW_5_BUTTONS_NEAR_SENSOR_ENABLE)
    boolean CheckNearSensor() {
        return (analogRead(A1) < 512);
    }
    
    void CheckNearSensorIn_Blank_Night() {
        static byte _lastModeNearSensor;
        static unsigned long _lastNearSensorDetect;
        if ( ((mode == STD_MODE_NIGHT) || (mode == STD_MODE_BLANK)) && CheckNearSensor() ) {
            DEBUG_PRINTLN("Sensor detects Object");
            DEBUG_FLUSH();
            _lastNearSensorDetect = millis();
            _lastModeNearSensor = mode;
            mode = STD_MODE_NORMAL;
            nightLock = true;
            needsUpdateFromRtc = true;
            setDisplayToOnWithoutReset();
        }
    
        if ( (nightLock) && (millis() - _lastNearSensorDetect > WW_5_BUTTONS_NEAR_SENSOR_DELAY) ) {
            mode = _lastModeNearSensor;
            nightLock = false;
            setDisplayToOff();
        }
    }
#endif

void resetCheckLdrThreshold_resetNightLock_needsUpdateFromRtc() {
    LdrThresholdZustand = 0;
    needsUpdateFromRtc = true;
    nightLock = false;
}

byte checkLdrThreshold() {
    boolean retVal = 1;
    
    static unsigned long _wait;
    byte _brightness = settings.getBrightness();
    byte _threshold = settings.getLdrBlankThreshold();
    switch (LdrThresholdZustand) {
        case 0: /*
                 * Display an. Helligkeitsschwelle unterschritten?
                 * Dann Wechsel zu Zustand 2.
                 */
            if (_brightness < _threshold) {
                LdrThresholdZustand++;
                _wait = millis();
            }
            break;
        case 1: /*
                 * Display an. Warte feste Zeitspanne, dann Beginn Nachtmodus und Wechsel in Zustand 2.
                 * Wird Helligkeitsschwelle überschritten, falle zurück auf Zustand 0.
                 */
            if (_brightness >= _threshold) {
                LdrThresholdZustand--;
            } else {
                if (millis() - _wait > (unsigned long) LDR_TO_NIGHT_DELAY_IN_SECONDS * 1000) {
                    retVal = 0;
                    LdrThresholdZustand++;
                }
            }
            break;
        case 2: /*
                 * Display aus. Helligkeitsschwelle überschritten?
                 * Dann Wechsel zu Zustand 3.
                 */
            retVal = 0;
            if (_brightness >= _threshold) {
                LdrThresholdZustand++;
                _wait = millis();
            }
            break;
        case 3: /*
                 * Display aus. Warte feste Zeitspanne, dann Ende Nachtmodus und Wechsel in Zustand 1. 
                 * Wird Helligkeitsschwelle unterschritten, falle zurück auf Zustand 2.
                 */
            retVal = 0;
            if (_brightness < _threshold) {
                LdrThresholdZustand--;
            } else {
                if (millis() - _wait > (unsigned long) LDR_TO_DAY_DELAY_IN_SECONDS * 1000) {
                    retVal = 1;
                    LdrThresholdZustand = 0;
                }
            }
            break;
    }
    DEBUG_PRINT(F("Brightness | LdrThresholdZustand | retVal: "));
    DEBUG_PRINTLN(_brightness);
    DEBUG_PRINT(F(" | "));
    DEBUG_PRINT(LdrThresholdZustand);
    DEBUG_PRINT(F(" | "));
    DEBUG_PRINTLN(retVal);
    DEBUG_FLUSH();
    
    return retVal;
}

/*
 * Pointer auf NightTime abhängig vom vorgegebenen (Anzeige-)Modus
 */
TimeStamp* getNightTimeStamp(byte _mode) {
    return settings.getNightTimeStamp(_mode - EXT_MODE_OFFTIME_MOFR);
}

/*
 * Pointer auf NightTime abhängig vom Wochentag
 * _on == 0: offTime
 * _on == 1: onTime
 */
TimeStamp* getOffOnTimeDayOfWeek(byte _dayOfWeek, byte _on) {
    switch (_dayOfWeek) {
        default:  return settings.getNightTimeStamp(0 + _on);
                  break;
        case 6:
        case 7:   return settings.getNightTimeStamp(2 + _on);
    }
}

/**
 * Ist aktuell Nacht?
 * Veraltet: Bei glatt wird 0 nur dann zurückgegeben, wenn die aktuelle Uhrzeit
 *           exakt einer eingestellten Ausschaltzeit entspricht.
 *           Bei !glatt wird auch dann 0 zurückgegeben, wenn die aktuelle
 *           Uhrzeit innerhalb eines passenden Zeitbereichs liegt.
 */
byte checkNight() {
    byte retVal = 1; // no night, 1: Display on, 0: Display off
  
    byte dayOfWeek = rtc.getDayOfWeek();
    unsigned int currentMinutesOfWeek = rtc.getMinutesOfWeek1();
    for (byte i = 0; i <= 2; i++) {
        byte dayOfWeek01 = dayOfWeek + i/2;
        TimeStamp* TT0 = getOffOnTimeDayOfWeek(dayOfWeek01, i % 2);
        TT0->setDayOfWeek(dayOfWeek01);
        unsigned int nightMinutes0 = TT0->getMinutesOfWeek1();
        if (!(i % 2)) {
            TimeStamp* TT1 = getOffOnTimeDayOfWeek(dayOfWeek01, 1);
            TT1->setDayOfWeek(dayOfWeek01);
            unsigned int nightMinutes1 = TT1->getMinutesOfWeek1();
            if (nightMinutes0 > nightMinutes1) {
                nightMinutes0 -= 24 * 60;
            }
        }
          // Falls glatt verwendet wird
//        if ( ((!glatt) && (currentMinutesOfWeek >= nightMinutes0)) ||
//              ((glatt) && (currentMinutesOfWeek == nightMinutes0)) ) {
        if (currentMinutesOfWeek >= nightMinutes0) {
            retVal = i % 2; // 0: offTime, 1: onTime
        }
    }
   
    return retVal;
}

/**
 * In den (zeitgesteuerten) Nachtmodus wechseln
 */
void goToNight() {
    DEBUG_PRINTLN(F("Go To NIGHT by time mode"));
    mode = STD_MODE_NIGHT;
    setDisplayToOff();
}

boolean isCurrentModeDarkMode() {
    if ( (mode == STD_MODE_BLANK) || (mode == STD_MODE_NIGHT) || (mode == EXT_MODE_DCF_BLANK) )
        return true;
    return false;
}

/**
 * LEDs einschalten und in den Modus
 * STD_MODE_NORMAL springen
 */
void goToNormalDispOn() {
    DEBUG_PRINTLN(F("Go To NORMAL mode"));
    mode = STD_MODE_NORMAL;
    setDisplayToOn();
}

/**
 * Das Display ausschalten.
 */
void setDisplayToOff() {
    DEBUG_PRINTLN(F("LED-Driver: ShutDown"));
    DEBUG_FLUSH();
    ledDriver.shutDown();
}

/**
 * Das Display einschalten.
 */
void setDisplayToOn() {
    setDisplayToOnWithoutReset();
    resetCheckLdrThreshold_resetNightLock_needsUpdateFromRtc();
}

void setDisplayToOnWithoutReset() {
    DEBUG_PRINTLN(F("LED-Driver: WakeUp"));
    DEBUG_FLUSH();
    ledDriver.wakeUp();
}

/**
 * Das Display toggeln (aus-/einschalten).
 */
void toggleDisplay() {
    goTo_leaveFrom_Blank(mode != STD_MODE_BLANK);
}

/**
 * In den BLANK-Modus wechseln und wieder zurück.
 */
void goTo_leaveFrom_Blank(boolean toBlank) {
    static boolean _lastModeBlank = STD_MODE_NORMAL;
    if (toBlank && !isCurrentModeDarkMode()) {
        _lastModeBlank = mode;
        mode = STD_MODE_BLANK;
        setDisplayToOff();
    }
    if (!toBlank && isCurrentModeDarkMode()) {
        mode = _lastModeBlank;
        setDisplayToOn();
    }
}

/**
 * Das Display manuell heller machen.
 */
void setDisplayBrighter() {
    changeDisplayBrightness(10);
}

/**
 * Das Display dunkler machen.
 */
void setDisplayDarker() {
    changeDisplayBrightness(-10);
}

/**
 * Das Display dunkler machen.
 */
void changeDisplayBrightness(char change) {
    ledDriver.setBrightness(settings.changeBrightness(change));
}
