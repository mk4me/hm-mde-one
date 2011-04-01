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


//! èrÛd≥o obiektÛw.
class ObjectSource : public core::IObjectSource
{
public:
    //! èrÛd≥o faktyczne danych.
    typedef std::pair<core::ObjectWrapperPtr, core::ObjectWrapperConstPtr> Source;
    //! Lista ürÛde≥ danych.
    typedef std::vector<Source> Sources;
    //! Lista typÛw kolejnych ürÛde≥.
    typedef std::vector< std::pair<std::string, core::ObjectWrapper::Types> > SourcesTypes; 

private:
    //! Lista ürÛde≥ danych.
    Sources sources;
    //! Lista typÛw danych dla ürÛde≥.
    //! Wczeúniej by≥a lokalna kopia, teraz to tylko referencja - w≥aúciwe dane trzyma VisualizerManager.
    const SourcesTypes& sourcesTypes;

public:
    //! \param numSources Liczba ürÛde≥.
    // ObjectSource(const SourcesTypes& types);
    //! \param visualizer Wizualizator z ktÛrego mozna pobraÊ informacje o wejúciach.
    ObjectSource(core::IVisualizer* visualizer);
    //! Konstruktor kopiujπcy.
    ObjectSource(const ObjectSource& source);

public:
    //! Czy dany obiekt moøna przypisaÊ danemu ürÛd≥u?
    bool isAssignable(int sourceNo, const core::ObjectWrapperPtr& object) const;
    //! Czy dany obiekt moøna przypisaÊ danemu ürÛd≥u?
    bool isAssignable(int sourceNo, const core::ObjectWrapperConstPtr& object) const;

    //! Dodaje zmienny obiekt.
    void setObject(int sourceNo, const core::ObjectWrapperPtr& object);
    //! Dodanie niezmienny obiekt.
    void setObject(int sourceNo, const core::ObjectWrapperConstPtr& object);

    //! \return Lista typÛw dla danego slotu.
    const core::ObjectWrapper::Types& getTypes(int sourceNo) const;
    //! \return Nazwa danego slotu.
    const std::string& getName(int sourceNo) const;

// IObjectSource
public:
    virtual int getNumObjects() const;
    virtual core::ObjectWrapperPtr getObject(int idx, boost::false_type);
    virtual core::ObjectWrapperConstPtr getObject(int idx, boost::true_type);

private:
    //! Czy dany obiekt moøna przypisaÊ danemu ürÛd≥u?
    bool isAssignable(int sourceNo, const core::ObjectWrapper* object) const;
};




#endif  // HEADER_GUARD_CORE__OBJECTSOURCE_H__