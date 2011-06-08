#ifndef HEADER_GUARD_CORE_IPARSER_H__
#define HEADER_GUARD_CORE_IPARSER_H__

#include <core/BaseDataTypes.h>
#include <core/IIdentifiable.h>
#include <core/SmartPtr.h>
#include <core/ObjectWrapper.h>
#include <boost/filesystem.hpp>
#include <core/PluginCommon.h>

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
        //! Pusty polimorficzny destruktor.
        virtual ~IParser() {}
        //! \return Instancja tego samego typu.
        virtual IParser* create() = 0;
        //! Przyporz�dkowanie parsera do konkretnego pliku.
        //! Na ka�dej instancji parsera ta metoda wywo�ywana jest maksymalnie jeden raz.
        //! \param path �cie�ka do pliku.
        virtual void parseFile(IDataManager* dataManager, const boost::filesystem::path& path) = 0;
        //! \retrun Lista rozszerze�, kt�re parser obs�uguje. Musz� by� oddzielone �rednikiem.
        //!         Obs�ugiwany format rozszerzenia: [ { *. | . } ]ext
        virtual std::string getSupportedExtensions() const = 0;
        //! \param extension Rozszerzenie dla ktorego chemy pobrac opis formatu pliku
        //! \return Opis formatu danego rozszerzenia
        virtual std::string getExtensionDescription(const std::string & extension) const
        {
            return std::string();
        }

        //! \return Obiekt danych parsera.
        virtual void getObjects(std::vector<ObjectWrapperPtr>& objects) = 0;

        //! \return Obiekt danych parsera dla konretnego rozszerzenia.
        virtual void getExtensionObjects(const std::string & extension, std::vector<ObjectWrapperPtr>& objects)
        {
            //TODO
            //wprowadzic taki opis!! Aktualnie dla kazdego rozszerzenia zwraca te same obiekty!!
            getObjects(objects);
        }
    };

    typedef shared_ptr<IParser> IParserPtr;
    typedef shared_ptr<const IParser> IParserConstPtr;
    typedef weak_ptr<IParser> IParserWeakPtr;
}

#endif
