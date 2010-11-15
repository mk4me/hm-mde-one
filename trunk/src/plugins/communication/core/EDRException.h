/**
@author Marek Daniluk
@brief Klasa EDRException Jest klasa obslugujaca wyjatki w module komunikacji. Obecnie klasa zawiera tylko pole
odpowiedzialne za podanie komunikatu bledu i metody pozwalajace wyswietlac komunikat.
*/

#ifndef _EDREXCEPTION_H_
#define _EDREXCEPTION_H_

#include <ctime>
#include <fstream>
#include <string>

namespace EDRCM {

	class EDRException {
	protected:
		std::string exception;
	public:
		/**
		Konstruktor klasy EDRException.
		*/
		EDRException();
		/**
		Konstruktor klasy EDRException.
		@param exception informacja o bledzie
		*/
		EDRException(const std::string& exception);
		/**
		Wirtualny destruktor klasy EDRException.
		*/
		virtual ~EDRException();
		/**
		Setter exception
		@param exception informacja o bledzie
		*/
		void setExceptionMessage(const std::string& exception);
		/**
		Getter exception
		@return informacja o bledzie
		*/
		const std::string& getExceptionMessage() const;
		/**
		Statyczna metoda zapisujaca informacje do pliku dziennika
		@param exception informacja do zapisu
		*/
		static void log(const std::string& information);
	};
}
#endif
