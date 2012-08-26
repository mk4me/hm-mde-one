#ifndef HEADER_GUARD_CORE_IPARSER_H__
#define HEADER_GUARD_CORE_IPARSER_H__

#include <map>
#include <core/IIdentifiable.h>
#include <core/SmartPtr.h>
#include <core/ObjectWrapper.h>
#include <core/Filesystem.h>

namespace core
{
    class IDataManager;

    //! Parser danych. Parser stanowi pomost pomiędzy plikiem a odwzorowaniem pliku
    //! w programie. Cykl na cyjl życia parsera składa się stworzenie (metodą clone),
    //! przypisanie do pliku (parse), odwołania do obiektu danych (getObject),
    //! zniszczenie (destruktor). W tym cyklu nigdy nie następują zmiany, poza sytuacją,
    //! gdy parser z jakiegoś powodu po utworzeniu nie jest przypisany do pliku, tylko
    //! od razu niszczony.
    class IParser : public IIdentifiable
    {
    public:
        //! Typ opisujący rozszerzenie
        struct ExtensionDescription
        {
            std::string description;
            std::set<TypeInfo> types;
        };

        //! Słownik rozszerzeń wpsieranych przez parser z ich opisem
        typedef std::map<std::string, ExtensionDescription> Extensions;

    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IParser() {}
        //! \return Instancja tego samego typu.
        virtual IParser* create() = 0;
        //! Przyporządkowanie parsera do konkretnego pliku.
        //! Na każdej instancji parsera ta metoda wywoływana jest maksymalnie jeden raz.
        //! \param path Ścieżka do pliku.
        virtual void parseFile(const Filesystem::Path& path) = 0;
        //! \retrun Lista rozszerzeń, które parser obsługuje. Muszą być oddzielone średnikiem.
        //!         Obsługiwany format rozszerzenia: [ { *. | . } ]ext
        virtual void getSupportedExtensions(Extensions & extensions) const = 0;

        //! \return Obiekt danych parsera.
        virtual void getObjects(Objects& objects) = 0;
    };

    typedef shared_ptr<IParser> IParserPtr;
    typedef shared_ptr<const IParser> IParserConstPtr;
    typedef weak_ptr<IParser> IParserWeakPtr;
}

#endif
