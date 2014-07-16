/********************************************************************
    created:  2012/02/28
    created:  28:2:2012   22:30
    filename: DateTime.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATETIME_H__
#define HEADER_GUARD___DATETIME_H__

#include <string>

namespace webservices
{
    
    //! Klasa reprezentuje prosta obsługę daty
    class Date{
        private:
		/**
		Pole klasy przechowujace informacje o roku
		*/
		int year;
		/**
		Pole klasy przechowujace informacje o miesiacu
		*/
		unsigned char month;
		/**
		Pole klasy przechowujace informacje o dniu
		*/
		unsigned char day;

    public:

        //! Konstruktor domyślny
        Date();
        //! Konstruktor kopiujący
        Date(const Date & date);
        //! Konstruktor
        //! \param year Rok
        //! \param month Miesiąc
        //! \param day Dzień
        Date(const int year, const unsigned char month = 1, const unsigned char day = 1);
        virtual ~Date();

        //! \return Zwraca dzisiejszą datę wg ustawień komputera
        static const Date today();

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
		void setMonth(const unsigned char month);
		/**
		Getter month
		@return miesiac
		*/
		const unsigned char getMonth() const;
		/**
		Setter day
		@param day dzien
		*/
		void setDay(const unsigned char day);
		/**
		Getter day
		@return dzien
		*/
		const unsigned char getDay() const;

        //! \return Data w formie napisu
        const std::string toString() const;

        //! Operator przypisania
        //! \param date Wartośc przypisywana
        Date & operator=(const Date & date);

        bool operator==(const Date & dater) const;

        bool operator!=(const Date & dater) const;

        bool operator<(const Date & dater) const;

        bool operator>(const Date & dater) const;

        bool operator<=(const Date & dater) const;

        bool operator>=(const Date & dater) const;

        std::ostream& operator <<(std::ostream& out) const;
    };    

    //! Klasa przechowująca czas
    class Time{
    private:
        /**
		Pole klasy przechowujace informacje o godzinie
		*/
		unsigned char hour;
		/**
		Pole klasy przechowujace informacje o minutach
		*/
		unsigned char minutes;
		/**
		Pole klasy przechowujace informacje o sekundach
		*/
		unsigned char seconds;

    public:
        //! Konstruktor kopiujący
        //! \param time Czas do skopiowania
        Time(const Time & time);
        //! Konstruktor domyślny
        //! \param hour Godzina
        //! \param minutes Minuty
        //! \param seconds Sekundy
        Time(const unsigned char hour = 0,
			const unsigned char minutes = 0,
			const unsigned char seconds = 0);
        //! Wirtualny destruktor
        virtual ~Time();

        //! Aktualny czas wg komputera
        static const Time now();

        /**
		Setter hour
		@param hour godzina
		*/
        void setHour(const unsigned char hour);
		/**
		Getter hour
		@return godzina
		*/
		const unsigned char getHour() const;
		/**
		Setter minutes
		@param minutes minuty
		*/
		void setMinutes(const unsigned char minutes);
		/**
		Getter minutes
		@return minuty
		*/
		const unsigned char getMinutes() const;
		/**
		Setter seconds
		@param seconds sekundy
		*/
		void setSeconds(const unsigned char seconds);
		/**
		Getter seconds
		@return sekundy
		*/
		const unsigned char getSeconds() const;

        //! \return Czas w formie tekstu
        const std::string toString() const;

        //! Operator przypisania
        //! \param time Wartośc przypisywana
        Time & operator=(const Time & time);

        bool operator==(const Time & timer) const;

        bool operator!=(const Time & timer) const;

        bool operator>(const Time & timer) const;

        bool operator<(const Time & timer) const;

        bool operator<=(const Time & timer) const;

        bool operator>=(const Time & timer) const;

        std::ostream& operator <<(std::ostream& out) const;
    };

    //! Klasa przechowująca datę i czas
	class DateTime : public Date, public Time {
	public:
        //! \return Aktualna data i czas
        static const DateTime now();

		/**
		Konstruktor klasy DateTime. Ustawia aktualny czas.
		*/
		DateTime();
		/**
		Konstruktor klasy DateTime.
		*/
		DateTime(int year, const unsigned char month = 1,
			const unsigned char day = 1,
			const unsigned char hour = 0,
			const unsigned char minutes = 0,
			const unsigned char seconds = 0);
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
		const std::string toString() const;

        DateTime & operator=(const DateTime & dateTime);

        bool operator==(const DateTime & dateTimer) const;

        bool operator!=(const DateTime & dateTimer) const;

        bool operator<(const DateTime & dateTimer) const;

        bool operator>(const DateTime & dateTimer) const;

        bool operator<=(const DateTime & dateTimer) const;

        bool operator>=(const DateTime & dateTimer) const;

        std::ostream& operator <<(std::ostream& out) const;
	};
}

#endif	//	HEADER_GUARD___DATETIME_H__
