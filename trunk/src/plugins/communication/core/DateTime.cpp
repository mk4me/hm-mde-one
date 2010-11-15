/**
@author Marek Daniluk
*/

#include "DateTime.h"

using namespace EDRCM;

DateTime::DateTime() {
	now();
}

DateTime::DateTime(int year, int month, int day, int hour = 0, int minutes = 0, int seconds = 0) :
year(year), month(month), day(day), hour(hour), minutes(minutes), seconds(seconds)
{ }

DateTime::DateTime(const std::string& date) {
	setDate(date);
}

DateTime::~DateTime() {}

void DateTime::setYear(int year) {
	this->year = year;
}

void DateTime::setMonth(int month) {
	if(month > 12 || month < 0)
		throw new EDRException("Incorrect date format.");
	this->month = month;
}
//nalezy zrobic walidacje wzgledem miesiaca?
void DateTime::setDay(int day) {
	if(day > 31 || day < 0)
		throw new EDRException("Incorrect date format.");
	this->day = day;
}

void DateTime::setHour(int hour) {
	if(day > 24 || day < 0)
		throw new EDRException("Incorrect date format.");
	this->hour = hour;
}

void DateTime::setMinutes(int minutes) {
	if(day > 60 || day < 0)
		throw new EDRException("Incorrect date format.");
	this->minutes = minutes;
}

void DateTime::setSeconds(int seconds) {
	this->seconds = seconds;
}

const int DateTime::getYear() const {
	return year;
}

const int DateTime::getMonth() const {
	return month;
}

const int DateTime::getDay() const {
	return day;
}

const int DateTime::getHour() const {
	return hour;
}

const int DateTime::getMinutes() const {
	return minutes;
}

const int DateTime::getSeconds() const {
	return seconds;
}
//walidacja formatu!
void DateTime::setDate(const std::string& date) {
	if(date.size() != 19 && date.size() != 23) {
		throw new EDRException("Incorrect date format.");
	}
	std::istringstream iis(date.substr(0, 4));
	iis >> this->year;
	iis.clear();
	iis.str(date.substr(5, 2));
	iis >> this->month;
	iis.clear();
	iis.str(date.substr(8, 2));
	iis >> this->day;
	iis.clear();
	iis.str(date.substr(11, 2));
	iis >> this->hour;
	iis.clear();
	iis.str(date.substr(14, 2));
	iis >> this->minutes;
	iis.clear();
	iis.str(date.substr(17));
	iis >> this->seconds;
}

const std::string DateTime::toString() const {
	std::ostringstream sstream;
	std::string temp;
	sstream << year;
	temp = (month > 9) ? "-" : "-0";
	sstream << temp <<  month;
	temp = (day > 9) ? "-" : "-0";
	sstream << temp << day;
	temp = (hour > 9) ? "T" : "T0";
	sstream << temp << hour;
	temp = (minutes > 9) ? ":" : ":0";
	sstream << temp << minutes;
	temp = (seconds > 9) ? ":" : ":0";
	sstream << temp << seconds;
	return sstream.str();
}

void DateTime::now() {
	time_t date_now;
	tm* t;

	time(&date_now);
	t = localtime(&date_now);
	this->year = t->tm_year + 1900;
	this->month = t->tm_mon + 1;
	this->day = t->tm_mday;
	this->hour = t->tm_hour;
	this->minutes = t->tm_min;
	this->seconds = t->tm_sec;
}
