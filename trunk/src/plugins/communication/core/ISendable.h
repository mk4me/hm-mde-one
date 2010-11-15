/**
@author Marek Daniluk
@brief Interfejs ISendable przykrywa uslugi do wysylania i pobierania danych wraz z autentykacja.
*/

#ifndef _ISENDABLE_H_
#define _ISENDABLE_H_

#include "IAuthenticable.h"

namespace EDRCM {

	class ISendable : public IAuthenticable {
	public:
		/**
		Wirtualny destruktor klasy ISendable.
		*/
		virtual ~ISendable() {}
		/**
		Metoda wysylajaca.
		@param filename nazwa pliku ktory ma wziac udzial w operacji przesylania
		*/
		virtual void put(const std::string& filename) = 0;
		/**
		Metoda pobierajaca.
		@param filename nazwa pliku ktory ma wziac udzial w operacji przesylania
		@return nazwa pliku ktory ma wziac udzial w operacji przesylania
		*/
		virtual void get(const std::string& filename) = 0;
	};
}
#endif
