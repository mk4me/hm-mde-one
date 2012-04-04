/**
@author Marek Daniluk
*/

#include <iostream>
#include <string>
#include <webserviceslib/DateTime.h>
#include <utils/Debug.h>

namespace webservices
{

Date::Date(const Date & date) : year(date.year), month(date.month), day(date.day)
{

}

Date::Date() : year(0), month(0), day(0)
{

}

Date::Date(int year, int month, int day)
    : year(year), month(month), day(day)
{
    UTILS_ASSERT((month > 0 && month < 13), "B��dny miesi�c");
    UTILS_ASSERT((day > 0 && day < 32), "B��dny dzie�");
}

Date::~Date()
{

}

void Date::setYear(int year)
{
    this->year = year;
}

void Date::setMonth(int month)
{
    if(month > 12 || month < 1)
        throw std::exception("Incorrect date format.");
    this->month = month;
}

void Date::setDay(int day)
{
    if(day > 31 || day < 1)
        throw std::exception("Incorrect date format.");
    this->day = day;
}

const int Date::getYear() const
{
    return year;
}

const int Date::getMonth() const
{
    return month;
}

const int Date::getDay() const
{
    return day;
}

const std::string Date::toString() const
{
    std::stringstream sstream;

    sstream.fill('0');

    sstream.flags(std::ios::right);
    sstream << year << "-";
    sstream.width(2); sstream << month;
    sstream << "-";
    sstream.width(2); sstream << day;

    return sstream.str();
}

const Date Date::today()
{
    Date ret;

    time_t date_now;
    tm* t;

    time(&date_now);
    t = localtime(&date_now);
    ret.year = t->tm_year + 1900;
    ret.month = t->tm_mon + 1;
    ret.day = t->tm_mday;

    return ret;
}

Date & Date::operator=(const Date & date)
{
    year = date.year;
    month = date.month;
    day = date.day;
    return *this;
}

std::ostream& Date::operator <<(std::ostream& out) const
{
    return out << toString();
}

//! Operator por�wnania
//! \param date Data kt�r� por�wnujemy
bool Date::operator==(const Date & dater) const
{
    bool ret = false;

    if(year == dater.year && month == dater.month && day == dater.day){
        ret = true;
    }

    return ret;
}

bool Date::operator!=(const Date & dater) const
{
    return !(*this == dater);
}

bool Date::operator<(const Date & dater) const
{
    bool ret = false;

    if(year < dater.year){
        ret = true;
    }else if(month < dater.month){
        ret = true;
    }else if(day < dater.day){
        ret = true;
    }

    return ret;
}

bool Date::operator>(const Date & dater) const
{
    return !(*this < dater) && !(*this == dater);
}

bool Date::operator<=(const Date & dater) const
{
    return !(*this > dater);
}

bool Date::operator>=(const Date & dater) const
{
    return !(*this < dater);
}

Time::Time(const Time & time) : hour(time.hour), minutes(time.minutes), seconds(time.seconds)
{

}

Time::Time(int hour, int minutes, int seconds) : hour(hour), minutes(minutes), seconds(seconds)
{
    UTILS_ASSERT((hour > -1), "B��dna godzina");
    UTILS_ASSERT((minutes > -1 && minutes < 60), "B��dne minuty");
    UTILS_ASSERT((seconds > -1 && seconds < 60), "B��dne sekundy");
}

Time::~Time()
{

}

const Time Time::now()
{
    Time ret;

    time_t date_now;
    tm* t;

    time(&date_now);
    t = localtime(&date_now);
    ret.hour = t->tm_hour;
    ret.minutes = t->tm_min;
    ret.seconds = t->tm_sec;

    return ret;
}

void Time::setHour(int hour)
{
    if(hour > 23 || hour < 0)
        throw std::exception("Incorrect date format.");
    this->hour = hour;
}

void Time::setMinutes(int minutes)
{
    if(minutes > 59 || minutes < 0)
        throw std::exception("Incorrect date format.");
    this->minutes = minutes;
}

void Time::setSeconds(int seconds)
{
    if(seconds > 59 || seconds < 0)
        throw std::exception("Incorrect date format.");
    this->seconds = seconds;
}

const int Time::getHour() const
{
    return hour;
}

const int Time::getMinutes() const
{
    return minutes;
}

const int Time::getSeconds() const
{
    return seconds;
}

const std::string Time::toString() const
{
    std::stringstream sstream;

    sstream.fill('0');
    sstream.width(2);
    sstream.flags(std::ios::right);
    sstream << hour << ":";
    sstream.width(2); sstream << minutes;
    sstream << ":";
    sstream.width(2); sstream << seconds;

    return sstream.str();
}

Time & Time::operator=(const Time & time)
{
    hour = time.hour;
    minutes = time.minutes;
    seconds = time.seconds;
    return *this;
}

std::ostream& Time::operator <<(std::ostream& out) const
{
    return out << toString();
}

bool Time::operator==(const Time & timer) const
{
    bool ret = false;

    if(hour == timer.hour && minutes == timer.minutes && seconds == timer.seconds){
        ret = true;
    }

    return ret;
}

bool Time::operator!=(const Time & timer) const
{
    return !(*this == timer);
}

bool Time::operator>(const Time & timer) const
{
    bool ret = false;

    if(hour > timer.hour && minutes > timer.minutes && seconds > timer.seconds){
        ret = true;
    }

    return ret;
}

bool Time::operator<(const Time & timer) const
{
    return !(*this > timer) && !(*this == timer);
}

bool Time::operator<=(const Time & timer) const
{
    return !(*this > timer);
}

bool Time::operator>=(const Time & timer) const
{
    return !(*this < timer);
}

DateTime::DateTime()
{

}

DateTime::DateTime(int year, int month, int day, int hour, int minutes, int seconds) :
Date(year, month, day), Time(hour, minutes, seconds)
{

}

DateTime::DateTime(const DateTime & dateTime) :
Date(dateTime), Time(dateTime)
{

}

DateTime::~DateTime()
{

}

const DateTime DateTime::now()
{
	time_t date_now;
	tm* t;

	time(&date_now);
	t = localtime(&date_now);

    return DateTime(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

const std::string DateTime::toString() const
{
    std::string ret(Date::toString());
    ret += " " + Time::toString();
    return ret;
}

DateTime & DateTime::operator=(const DateTime & dateTime)
{
    (Date &)(*this) = dateTime;
    (Time &)(*this) = dateTime;
    return *this;
}

/**
Operator wyjscia dla DateTime pod cout
*/
std::ostream& DateTime::operator <<(std::ostream& out) const
{
	return out << toString();
}

bool DateTime::operator==(const DateTime & dateTimer) const
{
    bool ret = false;

    if((const Date &)(*this) == (const Date &)(dateTimer) && (const Time &)(*this) == (const Time &)(dateTimer)){
        ret = true;
    }

    return ret;
}

bool DateTime::operator!=(const DateTime & dateTimer) const
{
    return !(*this == dateTimer);
}

bool DateTime::operator<(const DateTime & dateTimer) const
{
    bool ret = false;

    if((const Date &)(*this) < (const Date &)(dateTimer) && (const Time &)(*this) < (const Time &)(dateTimer)){
        ret = true;
    }

    return ret;
}

bool DateTime::operator>(const DateTime & dateTimer) const
{
    return !(*this < dateTimer) && (*this != dateTimer);
}

bool DateTime::operator<=(const DateTime & dateTimer) const
{
    return !(*this > dateTimer);
}

bool DateTime::operator>=(const DateTime & dateTimer) const
{
    return !(*this < dateTimer);
}

}