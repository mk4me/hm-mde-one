/********************************************************************
    created:  2011/03/14
    created:  14:3:2011   16:43
    filename: ObjectSource.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__OBJECTSOURCE_H__
#define HEADER_GUARD_CORE__OBJECTSOURCE_H__

#include <vector>
#include <core/IObjectSource.h>
#include <core/ObjectWrapper.h>
#include <core/IVisualizer.h>


//! �r�d�o obiekt�w.
class ObjectSource : public core::IObjectSource
{
public:
    //! �r�d�o faktyczne danych.
    typedef std::pair<core::ObjectWrapperPtr, core::ObjectWrapperConstPtr> Source;
    //! Lista �r�de� danych.
    typedef std::vector<Source> Sources;
    //! Lista typ�w kolejnych �r�de�.
    typedef std::vector< std::pair<std::string, core::ObjectWrapper::Types> > SourcesTypes; 

private:
    //! Lista �r�de� danych.
    Sources sources;
    //! Lista typ�w danych dla �r�de�.
    //! Wcze�niej by�a lokalna kopia, teraz to tylko referencja - w�a�ciwe dane trzyma VisualizerManager.
    const SourcesTypes& sourcesTypes;

public:
    //! \param numSources Liczba �r�de�.
    // ObjectSource(const SourcesTypes& types);
    //! \param visualizer Wizualizator z kt�rego mozna pobra� informacje o wej�ciach.
    ObjectSource(core::IVisualizer* visualizer);
    //! Konstruktor kopiuj�cy.
    ObjectSource(const ObjectSource& source);

public:
    //! Czy dany obiekt mo�na przypisa� danemu �r�d�u?
    bool isAssignable(int sourceNo, const core::ObjectWrapperPtr& object) const;
    //! Czy dany obiekt mo�na przypisa� danemu �r�d�u?
    bool isAssignable(int sourceNo, const core::ObjectWrapperConstPtr& object) const;

    //! Dodaje zmienny obiekt.
    void setObject(int sourceNo, const core::ObjectWrapperPtr& object);
    //! Dodanie niezmienny obiekt.
    void setObject(int sourceNo, const core::ObjectWrapperConstPtr& object);

    //! \return Lista typ�w dla danego slotu.
    const core::ObjectWrapper::Types& getTypes(int sourceNo) const;
    //! \return Nazwa danego slotu.
    const std::string& getName(int sourceNo) const;

// IObjectSource
public:
    virtual int getNumObjects() const;
    virtual core::ObjectWrapperPtr getObject(int idx, boost::false_type);
    virtual core::ObjectWrapperConstPtr getObject(int idx, boost::true_type);

private:
    //! Czy dany obiekt mo�na przypisa� danemu �r�d�u?
    bool isAssignable(int sourceNo, const core::ObjectWrapper* object) const;
};




#endif  // HEADER_GUARD_CORE__OBJECTSOURCE_H__