/**
@author Marek Daniluk
@brief Klasa DateTime reprezentuje wartosc typu DateTime. Jest jednym z typow prostych dla 
dokumentow WSDL, dokladny opis znajduje sie pod adresem http://www.w3.org/TR/xmlschema-2/#dateTime.
Na chwile obecna nie ma zadnej walidacji wprowadzanych danych.
*/

#ifndef _DATETIME_H_
#define _DATETIME_H_

namespace communication {
    
    class Date{
        private:
		/**
		Pole klasy przechowujace informacje o roku
		*/
		int year;
		/**
		Pole klasy przechowujace informacje o miesiacu
		*/
		int month;
		/**
		Pole klasy przechowujace informacje o dniu
		*/
		int day;

    public:

        Date();
        Date(const Date & date);
        Date(int year, int month = 1, int day = 1);
        virtual ~Date();

        static Date today();

        /**
		Setter year
		@param year rok
		*/
		void setYear(int year);
		/**
		Getter year
		@return rok
		*/
		const int getYear() const;
		/**
		Setter month
		@param month miesiac
		*/
		void setMonth(int month);
		/**
		Getter month
		@return miesiac
		*/
		const int getMonth() const;
		/**
		Setter day
		@param day dzien
		*/
		void setDay(int day);
		/**
		Getter day
		@return dzien
		*/
		const int getDay() const;

        std::string toString() const;

        friend std::ostream& operator <<(std::ostream& out, const Date& d) {
            return out << d.toString();
        }

        bool operator==(const Date & date) const
        {
            bool ret = false;

            if(year == date.year && month == date.month && day == date.day){
                ret = true;
            }

            return true;
        }

        bool operator!=(const Date & date) const
        {
            return !((*this) == date);
        }

        bool operator<(const Date & date) const
        {
            bool ret = false;

            if(this->year < date.year){
                ret = true;
            }else if(this->month < date.month){
                ret = true;
            }else if(this->day < date.day){
                ret = true;
            }

            return ret;
        }

        bool operator>(const Date & date) const
        {
            return !(*this < date) && !(*this == date);
        }

        bool operator<=(const Date & date) const
        {
            return !(*this > date);
        }

        bool operator>=(const Date & date) const
        {
            return !(*this < date);
        }
    };

    class Time{
    private:
        /**
		Pole klasy przechowujace informacje o godzinie
		*/
		int hour;
		/**
		Pole klasy przechowujace informacje o minutach
		*/
		int minutes;
		/**
		Pole klasy przechowujace informacje o sekundach
		*/
		int seconds;

    public:

        Time(const Time & time);
        Time(int hour = 0, int minutes = 0, int seconds = 0);
        virtual ~Time();

        static Time now();

        /**
		Setter hour
		@param hour godzina
		*/
        void setHour(int hour);
		/**
		Getter hour
		@return godzina
		*/
		const int getHour() const;
		/**
		Setter minutes
		@param minutes minuty
		*/
		void setMinutes(int minutes);
		/**
		Getter minutes
		@return minuty
		*/
		const int getMinutes() const;
		/**
		Setter seconds
		@param seconds sekundy
		*/
		void setSeconds(int seconds);
		/**
		Getter seconds
		@return sekundy
		*/
		const int getSeconds() const;

        std::string toString() const;

        friend std::ostream& operator <<(std::ostream& out, const Time& d) {
            return out << d.toString();
        }

        bool operator==(const Time & time) const
        {
            bool ret = false;

            if(hour == time.hour && minutes == time.minutes && seconds == time.seconds){
                ret = true;
            }

            return true;
        }

        bool operator!=(const Time & time) const
        {
            return !((*this) == time);
        }

        bool operator>(const Time & time) const
        {
            bool ret = false;

            if(hour > time.hour && minutes > time.minutes && seconds > time.seconds){
                ret = true;
            }

            return ret;
        }

        bool operator<(const Time & time) const
        {
            return !(*this > time) && !(*this == time);
        }

        bool operator<=(const Time & time) const
        {
            return !(*this > time);
        }

        bool operator>=(const Time & time) const
        {
            return !(*this < time);
        }
    };

	class DateTime : public Date, public Time {
	public:

        static DateTime now();

		/**
		Konstruktor klasy DateTime. Ustawia aktualny czas.
		*/
		DateTime();
		/**
		Konstruktor klasy DateTime.
		*/
		DateTime(int year, int month = 1, int day = 1, int hour = 0, int minutes = 0, int seconds = 0);
		/**
		Konstruktor klasy DateTime.
		*/
		DateTime(const DateTime& date);
		/**
		Wirtualny destruktor klasy DateTime.
		*/
		virtual ~DateTime();
		
		/**
		To string
		@return format jaki przyjmuja dokumenty WSDL
		*/
		const std::string toString() const
        {
            return Date::toString().append("-").append(Time::toString());
        }
		/**
		Operator wyjscia dla DateTime pod cout
		*/
		friend std::ostream& operator <<(std::ostream& out, const DateTime& d) {
			return out << d.toString();
		}

        bool operator==(const DateTime & p) const
        {
            bool ret = false;

            if((const Date &)(*this) == (const Date &)p && (const Time &)(*this) == (const Time &)p){
                ret = true;
            }

            return ret;
        }

        bool operator!=(const DateTime & p) const
        {
            return !(*this == p);
        }

        bool operator<(const DateTime & p) const
        {
            bool ret = false;

            if((const Date &)(*this) < (const Date &)p && (const Time &)(*this) < (const Time &)p){
                ret = true;
            }

            return ret;
        }

        bool operator>(const DateTime & p) const
        {
            return !(*this < p);
        }

        bool operator<=(const DateTime & p) const
        {
            return !(*this > p);
        }

        bool operator>=(const DateTime & p) const
        {
            return !(*this < p);
        }
	};
}
#endif
