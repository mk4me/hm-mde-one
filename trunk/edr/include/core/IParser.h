#ifndef IPARSER_H
#define IPARSER_H

#include <core/BaseDataTypes.h>
#include <core/IIdentifiable.h>
#include <core/SmartPtr.h>
#include <core/ObjectWrapper.h>
#include <boost/filesystem.hpp>

namespace core
{
    //! Parser danych. Parser stanowi pomost pomiêdzy plikiem a odwzorowaniem pliku
    //! w programie. Cykl na cyjl ¿ycia parsera sk³ada siê stworzenie (metod¹ clone),
    //! przypisanie do pliku (parse), odwo³ania do obiektu danych (getObject),
    //! zniszczenie (destruktor). W tym cyklu nigdy nie nastêpuj¹ zmiany, poza sytuacj¹,
    //! gdy parser z jakiegoœ powodu po utworzeniu nie jest przypisany do pliku, tylko
    //! od razu niszczony.
    class IParser : public IIdentifiable
    {
    public:

        //! Pusty polimorficzny destruktor.
        virtual ~IParser() {}
        
        //! Przyporz¹dkowanie parsera do konkretnego pliku.
        //! \param path Œcie¿ka do pliku.
        virtual void setPath(const boost::filesystem::path& path) = 0;
        
        //! Na ka¿dej instancji parsera ta metoda wywo³ywana jest maksymalnie jeden raz.
        virtual void parse() = 0;

        //! \return Informacja czy parser ju¿ zosta³ zainicjalizowany.
        virtual bool isParsed() = 0;

        //! \return Instancja tego samego typu.
        virtual IParser* create() = 0;

        //! \retrun Lista rozszerzeñ, które parser obs³uguje. Musz¹ byæ oddzielone œrednikiem.
        //!         Obs³ugiwany format rozszerzenia: [ { *. | . } ]ext
        virtual std::string getSupportedExtensions() const = 0;

        //! \return Obiekt danych parsera.
        virtual ObjectWrapperPtr getObject() = 0;
    };

    typedef shared_ptr<IParser> IParserPtr;
    typedef shared_ptr<const IParser> IParserConstPtr;
    typedef weak_ptr<IParser> IParserWeakPtr;
}
#endif
