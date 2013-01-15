#ifndef HEADER_GUARD_CORE_IPARSER_H__
#define HEADER_GUARD_CORE_IPARSER_H__

#include <map>
#include <core/IIdentifiable.h>
#include <core/SmartPtr.h>
#include <core/ObjectWrapper.h>
#include <core/Filesystem.h>

namespace plugin
{
	class ISourceParser;
	class IStreamParser;

    //! Parser danych. Parser stanowi pomost pomiędzy plikiem a odwzorowaniem pliku
    //! w programie. Cykl na cyjl życia parsera składa się stworzenie (metodą clone),
    //! przypisanie do pliku (parse), odwołania do obiektu danych (getObject),
    //! zniszczenie (destruktor). W tym cyklu nigdy nie następują zmiany, poza sytuacją,
    //! gdy parser z jakiegoś powodu po utworzeniu nie jest przypisany do pliku, tylko
    //! od razu niszczony.
	//! Musi dziedziczyć po ISourceParserCapabilities lub IStreamParserCapabilities
    class IParser : public core::IIdentifiable
    {
	public:
		//! Typ opisujący wyrażenie
		struct ExpressionDescription
		{
			std::string description;	//! Opis wyrazenia
			core::TypeInfoSet types;			//! Typy możliwe do uzyskania
		};

		//! Słownik rozszerzeń wpsieranych przez parser z ich opisem
		typedef std::map<std::string, ExpressionDescription> Expressions;

	public:
		//! \retrun Lista rozszerzeń, które parser obsługuje. Muszą być oddzielone średnikiem.
		//! Obsługiwany format rozszerzenia: [ { *. | . } ]ext
		virtual void acceptedExpressions(Expressions & expressions) const = 0;
	
        //! \return Instancja tego samego typu.
        virtual IParser* create() const = 0;
        //! \return Obiekt danych parsera.
        virtual void getObjects(core::Objects& objects) = 0;
    };

    typedef core::shared_ptr<IParser> IParserPtr;
    typedef core::shared_ptr<const IParser> IParserConstPtr;
    typedef core::weak_ptr<IParser> IParserWeakPtr;

	//! Interfejs parsera obsługującego we własnym zakresie I/O na podstawie ścieżki źródła
	//! Po tej klasie musi dziedziczyć IParser jeśli wspiera taką funkcjonalność
	class ISourceParserCapabilities
	{
	public:
		//! Przyporządkowanie parsera do konkretnego pliku.
        //! Na każdej instancji parsera ta metoda wywoływana jest maksymalnie jeden raz.
        //! \param source Ścieżka do źródła danych - plik, url, id urządzenia, ...
        virtual void parse(const std::string & source) = 0;
	};

	//! Interfejs parsera działąjącego w oparciu o strumienie STL i ścieżkę opisującą typ danych w strumieniu
	//! np. pamięć współdzielona pomiędzy procesami, dostęp do ukrytych zasobów (SQLCIPHER) implementujących strumienie
	//! Po tej klasie musi dziedziczyć IParser jeśli wspiera taką funkcjonalność
	class IStreamParserCapabilities
	{
	public:
		//! \param stream Strumień wejściowy z którego dostarczamy danych
		//! \param source Ścieżka źródła identyfikująca zawartość strumienia - np. rozszerzenie, protokół, adres
		virtual void parse(const std::istream * stream, const std::string & source) = 0;
	};
}

#endif
