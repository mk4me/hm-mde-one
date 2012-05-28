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

    //! Parser danych. Parser stanowi pomost pomi�dzy plikiem a odwzorowaniem pliku
    //! w programie. Cykl na cyjl �ycia parsera sk�ada si� stworzenie (metod� clone),
    //! przypisanie do pliku (parse), odwo�ania do obiektu danych (getObject),
    //! zniszczenie (destruktor). W tym cyklu nigdy nie nast�puj� zmiany, poza sytuacj�,
    //! gdy parser z jakiego� powodu po utworzeniu nie jest przypisany do pliku, tylko
    //! od razu niszczony.
    class IParser : public IIdentifiable
    {
    public:
        //! Typ opisuj�cy rozszerzenie
        struct ExtensionDescription
        {
            std::string description;
            std::set<TypeInfo> types;
        };

        //! S�ownik rozszerze� wpsieranych przez parser z ich opisem
        typedef std::map<std::string, ExtensionDescription> Extensions;

    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IParser() {}
        //! \return Instancja tego samego typu.
        virtual IParser* create() = 0;
        //! Przyporz�dkowanie parsera do konkretnego pliku.
        //! Na ka�dej instancji parsera ta metoda wywo�ywana jest maksymalnie jeden raz.
        //! \param path �cie�ka do pliku.
        virtual void parseFile(const Filesystem::Path& path) = 0;
        //! \retrun Lista rozszerze�, kt�re parser obs�uguje. Musz� by� oddzielone �rednikiem.
        //!         Obs�ugiwany format rozszerzenia: [ { *. | . } ]ext
        virtual void getSupportedExtensions(Extensions & extensions) const = 0;

        //! \return Obiekt danych parsera.
        virtual void getObjects(Objects& objects) = 0;
    };

    typedef shared_ptr<IParser> IParserPtr;
    typedef shared_ptr<const IParser> IParserConstPtr;
    typedef weak_ptr<IParser> IParserWeakPtr;
}

#endif
