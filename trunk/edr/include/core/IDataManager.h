#ifndef HEADER_GUARD_CORE_IDATAMANAGER_H__
#define HEADER_GUARD_CORE_IDATAMANAGER_H__

#include <core/IParser.h>
#include <vector>
#include <boost/filesystem.hpp>

class IDataManager
{
public:
    typedef boost::filesystem::path Path;
    typedef std::vector<Path> LocalTrial;

    //! Wirtualny destruktor.
	virtual ~IDataManager() {};

    //! Wyszukiwanie zasobów na lokalnym dysku.
    virtual void findResources() = 0;
    //! Wyszukiwanie prób pomiarowych na lokalnym dysku.
    virtual void findLocalTrials() = 0;
    //! £adowanie zasobów znalezionych na dysku.
 	virtual void loadResources() = 0;
    //! £adowanie próby pomiarowej z lokalnego dysku.
    //! \param trial próba pomiarowa do za³adowania.
    virtual void loadTrial(const LocalTrial& trial) = 0;
    //! Zwraca i-t¹ próbê pomiarow¹ z listy kolalnych prób pomiarowych.
    //! \param i indeks.
    //! \return i-ta próba pomiarowa.
	virtual const LocalTrial& getLocalTrial(int i) const = 0;
    //! Czyœci zasoby i próby pomiarowe.
	virtual void clear() = 0;
    //! Zwraca i-t¹ œcie¿kê do skórki UI aplikacji.
    //! \param i indeks.
    //! \return œcie¿ka do srórki aplikacji.
    virtual const std::string& getApplicationSkinsFilePath(int i) = 0;
    //! \return iloœæ skórek aplikacji.
    virtual int getApplicationSkinsFilePathCount() = 0;
    //! \return czy za³adowano próbê pomiarow¹.
	virtual bool isLoadLocalTrialData() const = 0;
    //! \param load ustaw flagê ³adowania lokalnej próby pomiarowej.
	virtual void setLoadLocalTrialData(bool load) = 0;
    //! \return œcie¿ka do katalogu z zasobami
	virtual const Path& getResourcesPath() const = 0;
    //! \return œcie¿ka do katalogu z próbami pomiarowymi
	virtual const Path& getTrialsPath() const = 0;

    //! Rejestruje zadany parser.
    //! \param parser Parser do rejestracji.
    virtual void registerParser(core::IParserPtr parser) = 0;

    //! \return Liczba parserów.
    virtual int getNumParsers() const = 0;

    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie.
	virtual core::IParserPtr getParser(int idx) = 0;
    //! \param filename nazwa pliku parsera.
    //! \return Odnaleziony parser b¹dŸ NULL.
    virtual core::IParserPtr getParser(const std::string& filename) = 0;
    
    //! Liczba prób pomiarowych w zasobach
	virtual int getLocalTrialsCount() const = 0;
    //! \param i za³aduj i-t¹ próbê pomiarow¹ z zasobów
    virtual void loadLocalTrial(int i) = 0;
    //! \param path Za³aduj próbê pomiarow¹ z podanej œcie¿ki.
    virtual void loadLocalTrial(const Path& path) = 0;
};

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////
    
    //! Metoda wyszukuj¹ca wszystkie parsery danego typu (np. implementuj¹ce
    //! dany interfejs).
    template <class T>
    shared_ptr<T> queryParsers(IDataManager* manager, T* dummy = NULL)
    {
        std::vector<shared_ptr<T> > result;
        queryParsers(manager, result);
        if ( result.empty() ) {
            return shared_ptr<T>();
        } else {
            UTILS_ASSERT(result.size()==1, "Multiple parsers found.");
            return result[0];
        }
    }
    
    //! Metoda wyszukuj¹ca wszystkie parsery danego typu (np. implementuj¹ce
    //! dany interfejs).
    template <class T>
    shared_ptr<T> queryParsers(IDataManager* manager, const std::string& filename)
    {
        return manager->getParser(filename);
    }
    
    //! Metoda wyszukuj¹ca wszystkie parsery danego typu (np. implementuj¹ce
    //! dany interfejs).
    template <class T>
    void queryParsers(IDataManager* manager, std::vector<shared_ptr<T>>& target)
    {
        for ( int i = 0; i < manager->getNumParsers(); ++i ) {
            IParserPtr parser = manager->getParser(i);
            shared_ptr<T> casted = dynamic_pointer_cast<T>(parser);
            if ( casted ) {
                target.push_back(casted);
            }
        }
    }

    //! Metoda wyszukuj¹ca wszystkie dane okreœlonego typu.
    template <class T>
    void queryData(IDataManager* manager, std::vector< typename ObjectWrapperT<T>::Ptr >& target, bool exact = false )
    {
        for ( int i = 0; i < manager->getNumParsers(); ++i ) {
            IParserPtr parser = manager->getParser(i);
            ObjectWrapperPtr object = parser->getObject();
            typename ObjectWrapperT<T>::Ptr ptr;
            if ( object->tryGet<T>(ptr, exact) ) {
                target.push_back(ptr);
            }
        }
    }

    //! Metoda wyszukuj¹ca wszystkie dane okreœlonego typu.
    template <class T>
    std::vector< typename ObjectWrapperT<T>::Ptr > queryData(IDataManager* manager, bool exact = false )
    {
        std::vector< typename ObjectWrapperT<T>::Ptr > result;
        queryData<T>(manager, result, exact);
        return result;
    }

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif // I_DATA_MANAGER_H