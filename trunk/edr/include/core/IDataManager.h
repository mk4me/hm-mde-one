#ifndef I_DATA_MANAGER_H
#define I_DATA_MANAGER_H

#include <core/LocalTrial.h>
#include <core/IParser.h>
#include <vector>
#include <boost/filesystem.hpp>

class IDataManager
{
public:
	virtual ~IDataManager() {};

 	virtual void loadResources() = 0;
	virtual void loadTrials() = 0;

	virtual const LocalTrial& getLocalTrial(int i) const = 0;
	virtual int getLocalTrialsCount() const = 0;

	virtual const LocalTrial& getActualLocalTrial() const = 0;
	virtual void setActualLocalTrial(int i) = 0;
	virtual void setActualLocalTrial(const std::string& name) = 0;

	virtual void clear() = 0;

    virtual const std::string& getShaderFilePath(int i) = 0;
    virtual int getShaderFilePathCount() = 0;

    virtual const std::string& getMeshFilePath(int i) = 0;
    virtual int getMeshFilePathCount() = 0;

    virtual const std::string& getApplicationSkinsFilePath(int i) = 0;
    virtual int getApplicationSkinsFilePathCount() = 0;

	virtual bool isLoadLocalTrialData() const = 0;
	virtual void setLoadLocalTrialData(bool load) = 0;

	virtual const std::string& getResourcesPath() const = 0;
	virtual const std::string& getTrialsPath() const = 0;

    //! Rejestruje zadan¹ us³ugê.
    //! \param newService
    virtual void registerParser(core::IParserPtr parser) = 0;

    //! \return Liczba us³ug.
    virtual int getNumParsers() const = 0;
    //! \param idx Indeks us³ugi.
    //! \return Us³uga o zadanym indeksie.
	virtual core::IParserPtr getParser(int idx) = 0;
    //! \param filename nazwa pliku parsera.
    //! \return Odnaleziony parser b¹dŸ NULL.
    virtual core::IParserPtr getParser(const std::string& filename) = 0;

    virtual void loadLocalTrial(int i) = 0;
    //! \param Za³aduj próbê pomiarow¹ o podanej nazwie.
    virtual void loadLocalTrial(const std::string& name) = 0;
};

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////
    
    //! Metoda wyszukuj¹ca wszystkie parsery danego typu (np. implementuj¹ce
    //! dany interfejs).
    template <class T>
    CORE_SHARED_PTR(T) queryParsers(IDataManager* manager, T* dummy = NULL)
    {
        std::vector<CORE_SHARED_PTR(T)> result;
        queryParsers(manager, result);
        if ( result.empty() ) {
            return CORE_SHARED_PTR(T)();
        } else {
            UTILS_ASSERT(result.size()==1, "Multiple parsers found.");
            return result[0];
        }
    }
    
    //! Metoda wyszukuj¹ca wszystkie parsery danego typu (np. implementuj¹ce
    //! dany interfejs).
    template <class T>
    CORE_SHARED_PTR(T) queryParsers(IDataManager* manager, const std::string& filename)
    {
        return manager->getParser(filename);
    }
    
    //! Metoda wyszukuj¹ca wszystkie parsery danego typu (np. implementuj¹ce
    //! dany interfejs).
    template <class T>
    void queryParsers(IDataManager* manager, std::vector<CORE_SHARED_PTR(T)>& target)
    {
        for ( int i = 0; i < manager->getNumParsers(); ++i ) {
            IParserPtr parser = manager->getParser(i);
            CORE_SHARED_PTR(T) casted = dynamic_pointer_cast<T>(parser);
            if ( casted ) {
                target.push_back(casted);
            }
        }
    }

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif // I_DATA_MANAGER_H