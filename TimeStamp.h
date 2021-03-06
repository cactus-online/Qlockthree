/**
 * TimeStamp
 * Klasse fuer die Kapselung eines Zeitstempels. Dadurch lassen sich
 * Zeiten aus der Echtzeituhr und von dem DCF77-Empfaenger
 * leichter verarbeiten und vergleichen.
 *
 * @mc       Arduino/RBBB
 * @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
 * @version  1.7f
 * @created  2.3.2011
 * @updated  05.06.2016 (Ergänzungen und Änderungen von A. Mueller)
 *
 * Versionshistorie:
 * V 1.1:  - Fehler in toString() behoben.
 * V 1.2:  - Kompatibilitaet zu Arduino-IDE 1.0 hergestellt.
 * V 1.3:  - neuer Konstruktor, neue Methoden.
 * V 1.4:  - getMinutesOf12HoursDay eingefuehrt.
 * V 1.5:  - Optimierung hinsichtlich Speicherbedarf.
 * V 1.6:  - Verbessertes Debugging & leeren Konstruktor entfernt.
 * V 1.7:  - Unterstuetzung fuer die alte Arduino-IDE (bis 1.0.6) entfernt.
 * V 1.7a: - Funktion getMinutesOfCentury() hinzugefügt.
 * V 1.7b: - Funktion getMinutesOfWeek() hinzugefügt.
 * V 1.7c: - Funktion getMinutesOfWeek_07() in getMinutesOfWeek1() umbenannt.
 * V 1.7d: - getMinutesOf12HourDay() entfernt, Funktion zur Einstellung
 *           von Stunden und Minuten geändert und den Konstruktor
 *           mit Standardwerten für die Initialisierung versehen.
 *         - Neue Funktionen addSubHoursOverflow(), incDate(), incMonth() und incDay().
 *           sowie zwei Hilfsfunktionen getDayOfMonth() und CalculateAndSetDayOfWeek() eingefügt.
 *           Die Funktion setTimeDateFromMinutesOfCentury() ist verfügbar (noch auskommentiert).
 * V 1.7e: - Neuer Konstruktor für TimeStamp-Zeiger hinzugefügt.
 * V 1.7f: - Setzen und Ändern des Datums wird mit der Funktion CheckDateValidity() validiert.
 *           //TF getMinutesOf12HourDay() wieder hinzugefügt   
*/
#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "Arduino.h"

class TimeStamp {
public:
    // Initialisierung mit date = 1 und month = 1 für getMinutesOfCentury() notwendig
    TimeStamp(byte minutes = 0, byte hours = 0, byte date = 1, byte dayOfWeek = 0, byte month = 1, byte year = 0);
    TimeStamp(TimeStamp* timestamp);

    byte getMinutes();
    unsigned int getMinutesOfDay();
    unsigned int getMinutesOf12HoursDay();
    unsigned int getMinutesOfWeek1();
    unsigned int getMinutesOfWeek();
    unsigned long getMinutesOfCentury();
    byte getHours();
    byte getDate();
    byte getDayOfWeek();
    byte getMonth();
    byte getYear();   

    void setMinutes(byte minutes);
//    void setTimeDateFromMinutesOfCentury(unsigned long minutesOfCentury);
    void setHours(byte hours);
    void setDate(byte date, boolean overflow = true);
    void setDayOfWeek(byte dayOfWeek);
    void setMonth(byte month, boolean overflow = true);
    void setYear(byte year, boolean overflow = true);
    void set(byte minutes, byte hours, byte date, byte dayOfWeek, byte month, byte year, boolean checkValidity = true);
    void set(TimeStamp* timeStamp, boolean checkValidity = true);

    void incMinutes(byte addMinutes = 1);
    void incHours(byte addHours = 1);
    void addSubHoursOverflow(char addSubHours = 0); 
    void incDate(byte addDate = 1, boolean overflow = false);
    void incMonth(byte addMonth = 1, boolean overflow = false);
    void incYear(byte addYear = 1, boolean overflow = false);

    char* asString();

protected:
    byte getDaysOfMonth(byte month, byte year);
    void CalculateAndSetDayOfWeek();
    void CheckDateValidity(boolean overflow = true);

    byte _minutes;
    byte _hours;

    byte _date;
    byte _dayOfWeek;
    byte _month;
    byte _year;

    char _cDateTime[17];
};

#endif
