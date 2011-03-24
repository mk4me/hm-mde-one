/********************************************************************
    created:  2011/03/14
    created:  14:3:2011   15:08
    filename: Visualizer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZER_H__
#define HEADER_GUARD_CORE__VISUALIZER_H__

#include <core/SmartPtr.h>
#include <core/IVisualizer.h>
#include <boost/range.hpp>
#include "ObjectSource.h"

//! Wizualizator. Jego zadaniem jest stworzy� widget (gdzie on b�dzie osadzony - nie jego sprawa),
//! zadeklarowa� ile �r�de� i jakiego typu jest w stanie obs�u�y� oraz przyj�� �r�d�a danych.
class Visualizer
{
private:
    //! Faktyczna implementacja wizualizatora.
    core::scoped_ptr<core::IVisualizer> impl;
    //! Faktyczny widget.
    QWidget* widget;
    //! �r�d�o danych dla wizualizatora.
    ObjectSource source;

public:
    //! \param impl Implementacja wizualizatora. Obiekt przejmowany na w�asno��.
    Visualizer( core::IVisualizer* impl );
    //! Niepolimorficzny (!) destruktor 
    ~Visualizer();

public:
    //! \return Widget wizualizatora.
    QWidget* getOrCreateWidget();
    //! \return Widget wizualizatora. Mo�e by� nullptr, je�eli jeszcze nie jest stworzony.
    inline QWidget* getWidget()
    {
        return widget;
    }
    //! IVisualizer::update
    inline void update(double deltaTime)
    {
        impl->update(deltaTime);
    }
    //! \see IVisualizer::getSupportedTypes
    inline const core::ObjectWrapper::Types& getSupportedTypes(int idx) const
    {
        return source.getTypes(idx);
    }
    //! \see IVisualizer::getSupportedTypes
    inline const std::string& getName(int idx) const
    {
        return source.getName(idx);
    }


    //! Dodaje niezmienny obiekt na wej�cie.
    //! \param slot
    //! \param object
    void setObject(int slot, const core::ObjectWrapperConstPtr& object)
    {
        source.setObject(slot, object);
    }
    //! Dodaje zmienny obiekt na wej�cie.
    //! \param slot
    //! \param object
    void setObject(int slot, const core::ObjectWrapperPtr& object)
    {
        source.setObject(slot, object);
    }
    //! \return Liczba obiekt�w.
    int getNumObjects() const
    {
        return source.getNumObjects();
    }
    //!
    core::ObjectWrapperConstPtr getObject(int slot)
    {
        return source.getObject(slot, boost::true_type());
    }



    //! \return
    inline QWidget* createWidget()
    {
        return impl->createWidget();
    }

    //! 
    void setUp();
    //! \param source �r�d�o danych.
    bool trySetUp();
};

typedef core::shared_ptr<Visualizer> VisualizerPtr;
typedef core::shared_ptr<const Visualizer> VisualizerConstPtr;
typedef core::weak_ptr<Visualizer> VisualizerWeakPtr;
typedef core::weak_ptr<const Visualizer> VisualizerConstWeakPtr;

#endif  // HEADER_GUARD_CORE__VISUALIZER_H__