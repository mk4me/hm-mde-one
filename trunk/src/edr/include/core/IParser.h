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

    //! Parser danych. Parser stanowi pomost pomiêdzy plikiem a odwzorowaniem pliku
    //! w programie. Cykl na cyjl ¿ycia parsera sk³ada siê stworzenie (metod¹ clone),
    //! przypisanie do pliku (parse), odwo³ania do obiektu danych (getObject),
    //! zniszczenie (destruktor). W tym cyklu nigdy nie nastêpuj¹ zmiany, poza sytuacj¹,
    //! gdy parser z jakiegoœ powodu po utworzeniu nie jest przypisany do pliku, tylko
    //! od razu niszczony.
    class IParser : public IIdentifiable
    {
    public:
        //! Typ opisuj¹cy rozszerzenie
        struct ExtensionDescription
        {
            std::string description;
            std::set<TypeInfo> types;
        };

        //! S³ownik rozszerzeñ wpsieranych przez parser z ich opisem
        typedef std::map<std::string, ExtensionDescription> Extensions;

    public:
        //! Pusty polimorficzny destruktor.
        virtual ~IParser() {}
        //! \return Instancja tego samego typu.
        virtual IParser* create() = 0;
        //! Przyporz¹dkowanie parsera do konkretnego pliku.
        //! Na ka¿dej instancji parsera ta metoda wywo³ywana jest maksymalnie jeden raz.
        //! \param path Œcie¿ka do pliku.
        virtual void parseFile(const Filesystem::Path& path) = 0;
        //! \retrun Lista rozszerzeñ, które parser obs³uguje. Musz¹ byæ oddzielone œrednikiem.
        //!         Obs³ugiwany format rozszerzenia: [ { *. | . } ]ext
        virtual void getSupportedExtensions(Extensions & extensions) const = 0;

        //! \return Obiekt danych parsera.
        virtual void getObjects(Objects& objects) = 0;
    };

    typedef shared_ptr<IParser> IParserPtr;
    typedef shared_ptr<const IParser> IParserConstPtr;
    typedef weak_ptr<IParser> IParserWeakPtr;
}

#endif
