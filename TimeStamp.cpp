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
#include "TimeStamp.h"

// #define DEBUG
#include "Debug.h"

TimeStamp::TimeStamp(byte minutes, byte hours, byte date, byte dayOfWeek, byte month, byte year) {
    set(minutes, hours, date, dayOfWeek, month, year);
}

TimeStamp::TimeStamp(TimeStamp* timestamp) {
    set(timestamp);
}

byte TimeStamp::getMinutes() {
    return _minutes;
}

unsigned int TimeStamp::getMinutesOfDay() {
    return _minutes + 60 * _hours;
}
/**
* Minuten des Tages ohne die Beruecksichtigung von 12/24 Stunden 
* (fuer den Wecker)...
*/
unsigned int TimeStamp::getMinutesOf12HoursDay() {
  int ret = _minutes + 60 * _hours;
  
  while(ret > 12 * 60) {
    ret -= 12 * 60;
  }
  
  return ret;
}

unsigned int TimeStamp::getMinutesOfWeek1() {
    return getMinutesOfDay() + _dayOfWeek * 24 * 60;
}

unsigned int TimeStamp::getMinutesOfWeek() {
    return getMinutesOfWeek1() - 24 * 60;
}

unsigned long TimeStamp::getMinutesOfCentury() {
    /* 
     * Exakte Berechnung der vergangenen Minuten des aktuellen Jahrhunderts bis zum TimeStamp.
     * Diese Funktion ist auch für Zeitvergleiche mehrerer TimeStamps über Jahre hinweg geeignet.
     */
    unsigned int days = 0;
    if (_year)
        days += (_year + 3) / 4 + _year * 365;
    for (byte i = 1; i <= _month-1; i++) {
        days += getDaysOfMonth(i, _year);
    }
    days += _date - 1;
    return ( (unsigned long) days * 24 * 60 + getMinutesOfDay() );
}

byte TimeStamp::getHours() {
    return _hours;
}

byte TimeStamp::getDate() {
    return _date;
}

byte TimeStamp::getDayOfWeek() {
    return _dayOfWeek;
}

byte TimeStamp::getMonth() {
    return _month;
}

byte TimeStamp::getYear() {
    return _year;
}

void TimeStamp::setMinutes(byte minutes) {
    _minutes = minutes % 60;
}

//void TimeStamp::setTimeDateFromMinutesOfCentury(unsigned long minutesOfCentury) {
//    /*
//     * Diese Funktion berechnet auf Basis der angegebenen vergangenen Minuten dieses Jahrhunderts,
//     * also seit dem 01.01.2000 das dazugehörige Datum und die Uhrzeit.
//     * Im Anschluss wird aus dem Datum der passende Wochentag berechnet und alle Ergebnisse gespeichert.
//     */
//    unsigned int hoursminutes = minutesOfCentury % 1440;
//    byte hours = hoursminutes / 60;
//    byte minutes = hoursminutes % 60;
//    
//    unsigned int days = minutesOfCentury / 1440;
//    
//    byte years = 0;
//    while (days >= 365) {
//        if (years % 4)
//            days -= 365;
//        else  {
//            if (days >= 366)
//                days -= 366;
//            else 
//                break;
//        }
//        years++;        
//    }
//    
//    byte months = 1;
//    while (days >= getDaysOfMonth(months, years)) {
//        days -= getDaysOfMonth(months, years);
//        months++;
//    }
//    
//    set(minutes, hours, days + 1, 0, months, years);
//    CalculateAndSetDayOfWeek();
//}

void TimeStamp::setHours(byte hours) {
    _hours = hours % 24;
}

void TimeStamp::setDayOfWeek(byte dayOfWeek) {
    _dayOfWeek = dayOfWeek;
}

void TimeStamp::setDate(byte date, boolean overflow) {
    _date = date;
    CheckDateValidity(overflow);
}

void TimeStamp::setMonth(byte month, boolean overflow) {
    _month = month;
    CheckDateValidity(overflow);
}

void TimeStamp::setYear(byte year, boolean overflow) {
    _year = year;
    CheckDateValidity(overflow);
}

void TimeStamp::set(byte minutes, byte hours, byte date, byte dayOfWeek, byte month, byte year, boolean checkValidity) {
    setMinutes(minutes);
    setHours(hours);
    _date = date;
    _dayOfWeek = dayOfWeek;
    _month = month;
    _year = year;
    if (checkValidity)
        CheckDateValidity(true);
}

void TimeStamp::set(TimeStamp* timeStamp, boolean checkValidity) {
    set(timeStamp->_minutes, timeStamp->_hours, timeStamp->_date, timeStamp->_dayOfWeek, timeStamp->_month, timeStamp->_year, checkValidity);
}

/**
 * Die Minuten erhoehen.
 */
void TimeStamp::incMinutes(byte addMinutes) {
    setMinutes(_minutes + addMinutes);
}

/**
 * Die Stunden erhoehen.
 */
void TimeStamp::incHours(byte addHours) {
    setHours(_hours + addHours);
}

/**
 * Den TimeStamp um die vorgebene Zahl von Stunden anpassen
 * (inkl. Tag, Monat und Jahr)
 */
void TimeStamp::addSubHoursOverflow(char addSubHours) {
    char sumHours = _hours + addSubHours;
    // sumHours < 0
    while (sumHours < 0) {
        sumHours += 24;
        _date--;
        if (!_date) {
            _month--;
            if (!_month) {
                _year--;
                _month = 12;
            }
            _date = getDaysOfMonth(_month, _year);
        }
    }
    // sumHours >= 24
    _date += sumHours / 24;

    _hours = sumHours % 24;
    CheckDateValidity(true);
    
//    if (addSubHours != 0)
//        setTimeDateFromMinutesOfCentury(getMinutesOfCentury() + (int) addSubHours * 60);
}

/**
 * Datum, Monat, Jahr erhöhren.
 */
void TimeStamp::incDate(byte addDate, boolean overflow) {
    setDate(_date + addDate, overflow);
}

void TimeStamp::incMonth(byte addMonth, boolean overflow) {
    setMonth(_month + addMonth, overflow);
}

void TimeStamp::incYear(byte addYear, boolean overflow) {
    setYear(_year + addYear, overflow);
}

/**
 * Die Zeit als String bekommen
 */
char* TimeStamp::asString() {
    _cDateTime[0] = 0;
    char temp[5];

    // build the string...
    if (_hours < 10) {
        sprintf(temp, "0%d:", _hours);
        strncat(_cDateTime, temp, strlen(temp));
    } else {
        sprintf(temp, "%d:", _hours);
        strncat(_cDateTime, temp, strlen(temp));
    }

    if (_minutes < 10) {
        sprintf(temp, "0%d ", _minutes);
        strncat(_cDateTime, temp, strlen(temp));
    } else {
        sprintf(temp, "%d ", _minutes);
        strncat(_cDateTime, temp, strlen(temp));
    }

    if (_date < 10) {
        sprintf(temp, "0%d.", _date);
        strncat(_cDateTime, temp, strlen(temp));
    } else {
        sprintf(temp, "%d.", _date);
        strncat(_cDateTime, temp, strlen(temp));
    }

    if (_month < 10) {
        sprintf(temp, "0%d.", _month);
        strncat(_cDateTime, temp, strlen(temp));
    } else {
        sprintf(temp, "%d.", _month);
        strncat(_cDateTime, temp, strlen(temp));
    }

    sprintf(temp, "%d", _year);
    strncat(_cDateTime, temp, strlen(temp));

    return _cDateTime;
}

byte TimeStamp::getDaysOfMonth(byte month, byte year) {
    // Only valid for years from 2000 to 2099
    switch (month) {
        default: return 0;
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12: 
            return 31;
            break;
        case 2:
            if (year % 4)
                return 28;
            else
                return 29;
            break;
       case 4:
       case 6:
       case 9:
       case 11:
            return 30;
            break;
    }
}

void TimeStamp::CalculateAndSetDayOfWeek() {
    // (Adopted) alghorithm by Schwerdtfeger
    // This alghorithm is only valid from 1st March 2000 to 31st December 2099
    byte g = _year;
    if (_month < 3) g--;
    byte e;
    switch (_month) {
        default: e = 0; break;
        case 2: 
        case 6:  e = 3; break;
        case 3:
        case 11: e = 2; break;
        case 4: 
        case 7:  e = 5; break;
        case 8:  e = 1; break;
        case 9: 
        case 12: e = 4; break;
        case 10: e = 6; break;
    }
    byte w = (_date + e + g + g/4) % 7;
    if (!w) w = 7;
    setDayOfWeek(w);
}

void TimeStamp::CheckDateValidity(boolean overflow) {
    if (_month < 1)
        _month = 1;
    while (_month > 12) {
        _month -= 12;
        if (overflow) _year++;
    }

    if (_date < 1)
        _date = 1;
    while (_date > getDaysOfMonth(_month, _year)) {
        _date -= getDaysOfMonth(_month, _year);
        if (overflow) {
            _month++;
            while (_month > 12) {
                _month -= 12;
                _year++;   
            }
        }
    }
    
    _year %= 100;
    
    CalculateAndSetDayOfWeek();
}

