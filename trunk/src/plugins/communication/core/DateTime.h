/**
@author Marek Daniluk
@brief Klasa DateTime reprezentuje wartosc typu DateTime. Jest typ prosty jeden z typow prostych dla 
dokumentow WSDL, dokladny opis znajduje sie pod adresem http://www.w3.org/TR/xmlschema-2/#dateTime.
Na chwile obecna nie ma zadnej walidacji wprowadzanych danych.
*/

#ifndef _DATETIME_H_
#define _DATETIME_H_

#include <ctime>
#include <string>
#include <sstream>
#include "EDRException.h"

namespace EDRCM {

	class DateTime {
	protected:
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
		/**
		Konstruktor klasy DateTime. Ustawia aktualny czas.
		*/
		DateTime();
		/**
		Konstruktor klasy DateTime.
		*/
		DateTime(int year, int month, int day, int hour, int minutes, int seconds);
		/**
		Konstruktor klasy DateTime.
		*/
		DateTime(const std::string& date);
		/**
		Wirtualny destruktor klasy DateTime.
		*/
		virtual ~DateTime();
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
		/**
		Setter date
		@param date data jako string
		*/
		void setDate(const std::string& date);
		/**
		To string
		@return format jaki przyjmuja dokumenty WSDL
		*/
		virtual const std::string toString() const;
		/**
		Aktualny czas
		*/
		virtual void now();
		/**
		Operator wyjscia dla DateTime pod cout
		*/
		friend std::ostream& operator <<(std::ostream& out, const DateTime& d) {
			return out << d.toString();
		}
	};
}
#endif
