#ifndef HEADER_GUARD_CORE_IPARSER_H__
#define HEADER_GUARD_CORE_IPARSER_H__

#include <map>
#include <corelib/BaseDataTypes.h>
#include <corelib/IIdentifiable.h>
#include <utils/SmartPtr.h>
#include <corelib/Filesystem.h>
#include <corelib/IHierarchyItem.h>

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
    class IParser : public ICoreElement
    {
	public:
		//! Typ opisujący wyrażenie
		struct ExpressionDescription
		{
			std::string description;	//! Opis wyrazenia
			core::TypeInfoSet types;			//! Typy możliwe do uzyskania
		};

        struct ParsedObjects
        {
            core::IHierarchyItemPtr hierarchy;
            core::Objects additionalObjects;
        };
        DEFINE_SMART_POINTERS(ParsedObjects);

		//! Słownik rozszerzeń wpsieranych przez parser z ich opisem
		typedef std::map<std::string, ExpressionDescription> Expressions;

	public:

		virtual ~IParser() {}

		//! \retrun Lista rozszerzeń, które parser obsługuje. Muszą być oddzielone średnikiem.
		//! Obsługiwany format rozszerzenia: [ { *. | . } ]ext
		virtual void acceptedExpressions(Expressions & expressions) const = 0;
	
        //! \return Instancja tego samego typu.
        virtual IParser* create() const = 0;
        //! \return Obiekt danych parsera.
        virtual ParsedObjectsPtr getObjects() = 0;
        
    };

    typedef utils::shared_ptr<IParser> IParserPtr;
    typedef utils::shared_ptr<const IParser> IParserConstPtr;
    typedef utils::weak_ptr<IParser> IParserWeakPtr;

	//! Interfejs parsera obsługującego we własnym zakresie I/O na podstawie ścieżki źródła
	//! Po tej klasie musi dziedziczyć IParser jeśli wspiera taką funkcjonalność
	class ISourceParserCapabilities
	{
	public:

		virtual ~ISourceParserCapabilities() {}

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

		virtual ~IStreamParserCapabilities() {}

		//! \param stream Strumień wejściowy z którego dostarczamy danych
		//! \param source Ścieżka źródła identyfikująca zawartość strumienia - np. rozszerzenie, protokół, adres
		virtual void parse(const std::istream * stream, const std::string & source) = 0;
	};
}

#endif
