/**
@author Marek Daniluk
*/

#include <iostream>
#include <string>
#include <plugins/communication/DateTime.h>
#include <utils/Debug.h>
using namespace communication;

Date::Date(const Date & date) : year(date.year), month(date.month), day(date.day)
{

}

Date::Date() : year(0), month(0), day(0)
{

}

Date::Date(int year, int month, int day)
    : year(year), month(month), day(day)
{
    UTILS_ASSERT((month > 0 && month < 13), "Błędny miesiąc");
    UTILS_ASSERT((day > 0 && day < 32), "Błędny dzień");
}

Date::~Date()
{

}

void Date::setYear(int year) {
    this->year = year;
}

void Date::setMonth(int month) {
    if(month > 12 || month < 1)
        throw std::exception("Incorrect date format.");
    this->month = month;
}

void Date::setDay(int day) {
    if(day > 31 || day < 1)
        throw std::exception("Incorrect date format.");
    this->day = day;
}

const int Date::getYear() const {
    return year;
}

const int Date::getMonth() const {
    return month;
}

const int Date::getDay() const {
    return day;
}

std::string Date::toString() const {
    std::stringstream sstream;

    sstream.fill('0');

    sstream.flags(std::ios::right);
    sstream << year << "-";
    sstream.width(2); sstream << month;
    sstream << "-";
    sstream.width(2); sstream << day;

    return sstream.str();
}

Date Date::today()
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

Time::Time(const Time & time) : hour(time.hour), minutes(time.minutes), seconds(time.seconds)
{

}

Time::Time(int hour, int minutes, int seconds) : hour(hour), minutes(minutes), seconds(seconds)
{
    UTILS_ASSERT((hour > -1), "Błędna godzina");
    UTILS_ASSERT((minutes > -1 && minutes < 60), "Błędne minuty");
    UTILS_ASSERT((seconds > -1 && seconds < 60), "Błędne sekundy");
}

Time::~Time()
{

}

Time Time::now()
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

void Time::setHour(int hour) {
    if(hour > 23 || hour < 0)
        throw std::exception("Incorrect date format.");
    this->hour = hour;
}

void Time::setMinutes(int minutes) {
    if(minutes > 59 || minutes < 0)
        throw std::exception("Incorrect date format.");
    this->minutes = minutes;
}

void Time::setSeconds(int seconds) {
    if(seconds > 59 || seconds < 0)
        throw std::exception("Incorrect date format.");
    this->seconds = seconds;
}

const int Time::getHour() const {
    return hour;
}

const int Time::getMinutes() const {
    return minutes;
}

const int Time::getSeconds() const {
    return seconds;
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

DateTime DateTime::now() {
	time_t date_now;
	tm* t;

	time(&date_now);
	t = localtime(&date_now);

    return DateTime(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}
