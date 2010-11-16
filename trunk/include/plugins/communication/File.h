/**
@author Marek Daniluk
@brief Klasa File reprezentuje obiekt File po stronie serwera. Klasa powstala na bazie
dokumentacji i kodu zrodlowego zespolu bazodanowego. Brak szczegolow w dokumentacji nie pozwala na
jednoznaczne ustalenie przydatnosci klasy, jednak jak wspomina zespol bazodanowy, funkcjonalnosc
komunikacji z baza danych wciaz jest w fazie testow.
*/

#ifndef _FILE_H_
#define _FILE_H_

#include "Entity.h"

namespace communication {

	class File : public Entity {
	protected:
		std::string filename;
		std::string fileDescription;
	public:
		/**
		Konstruktor klasy File.
		*/
		File(int fileID = 0, const std::string& filename = "", const std::string& fileDescription = "");
		/**
		Wirtualny destruktor klasy File.
		*/
		virtual ~File();
		/**
		Setter filename
		@param filename
		*/
		void setFilename(const std::string& filename);
		/**
		Setter fileDescription
		@param fileDescription
		*/
		void setFileDescription(const std::string& fileDescription);
		/**
		Getter filename
		@return filename
		*/
		const std::string& getFilename() const;
		/**
		Getter fileDescription
		@return fileDescription
		*/
		const std::string& getFileDescription() const;
		/**
		Metoda toString
		@return prezentuje obiekt w formie tekstowej
		**/
		virtual const std::string toString() const;
	};
}
#endif
