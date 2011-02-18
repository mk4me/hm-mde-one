#ifndef I_DATA_MANAGER_H
#define I_DATA_MANAGER_H

#include <core/IParser.h>
#include <vector>
#include <boost/filesystem.hpp>

class IDataManager
{
public:
    typedef boost::filesystem::path Path;
    typedef std::vector<Path> LocalTrial;

	virtual ~IDataManager() {};

    virtual void findResources() = 0;
    virtual void findLocalTrials() = 0;

 	virtual void loadResources() = 0;
    virtual void loadTrial(const LocalTrial& trial) = 0;

	virtual const LocalTrial& getLocalTrial(int i) const = 0;

	virtual const LocalTrial& getCurrentLocalTrial() const = 0;

	virtual void clear() = 0;

    virtual const std::string& getApplicationSkinsFilePath(int i) = 0;
    virtual int getApplicationSkinsFilePathCount() = 0;

	virtual bool isLoadLocalTrialData() const = 0;
	virtual void setLoadLocalTrialData(bool load) = 0;

	virtual const Path& getResourcesPath() const = 0;
	virtual const Path& getTrialsPath() const = 0;

    //! Rejestruje zadany parser.
    //! \param parser Parser do rejestracji.
    virtual void registerParser(core::IParserPtr parser) = 0;

    //! \return Liczba us�ug.
    virtual int getNumParsers() const = 0;

    //! \param idx Indeks parsera.
    //! \return Parser o zadanym indeksie.
	virtual core::IParserPtr getParser(int idx) = 0;
    //! \param filename nazwa pliku parsera.
    //! \return Odnaleziony parser b�d� NULL.
    virtual core::IParserPtr getParser(const std::string& filename) = 0;
    
    //! Liczba pr�b pomiarowych w zasobach
	virtual int getLocalTrialsCount() const = 0;
    //! \param i za�aduj i-t� pr�b� pomiarow� z zasob�w
    virtual void loadLocalTrial(int i) = 0;
    //! \param path Za�aduj pr�b� pomiarow� z podanej �cie�ki.
    virtual void loadLocalTrial(const Path& path) = 0;
};

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////
    
    //! Metoda wyszukuj�ca wszystkie parsery danego typu (np. implementuj�ce
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
    
    //! Metoda wyszukuj�ca wszystkie parsery danego typu (np. implementuj�ce
    //! dany interfejs).
    template <class T>
    shared_ptr<T> queryParsers(IDataManager* manager, const std::string& filename)
    {
        return manager->getParser(filename);
    }
    
    //! Metoda wyszukuj�ca wszystkie parsery danego typu (np. implementuj�ce
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

    //! Metoda wyszukuj�ca wszystkie dane okre�lonego typu.
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

    //! Metoda wyszukuj�ca wszystkie dane okre�lonego typu.
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