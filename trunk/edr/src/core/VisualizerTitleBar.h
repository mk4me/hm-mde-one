/********************************************************************
    created:  2011/03/22
    created:  22:3:2011   18:42
    filename: VisualizerTitleBarContextBased.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZERWIDGETTITLEBAR_H__
#define HEADER_GUARD_CORE__VISUALIZERWIDGETTITLEBAR_H__

#include <QtGui/QWidget>
#include <core/ObjectWrapper.h>
#include "Visualizer.h"
#include "ui_VisualizerTitleBar.h"

class VisualizerManager;
class DataManager;
class VisualizerWidget;

//! Kontener na title bar dla wizualizatora.
class VisualizerTitleBar : public QWidget, private Ui::VisualizerTitleBar
{
    Q_OBJECT
    friend class VisualizerWidget;
private:
    //! Widget dostarczony przez implementacj� wizualizatora.
    QWidget* widgetCustomTitleBar;
    //! Bie��cy wizualizator.
    VisualizerPtr visualizer;
    //! Widget dostarczaj�cy mo�liwo�ci wyboru �r�d�a.
    QWidget* widgetSources;

public:
    //! \param parent
    VisualizerTitleBar(QWidget* parent = nullptr);

public:
// 
//     template <class Iter>
//     void addVisualizers(Iter first, Iter last)
//     {
//         addVisualizers(first, last, first);
//     }
// 
//     template <class Iter>
//     void addVisualizers(Iter first, Iter last, Iter current)
//     {
//         bool blocked = comboType->signalsBlocked();
//         int toMakeCurrent = -1;
//         comboType->blockSignals(true);
//         while ( first != last ) {
//             addVisualizer( core::toQString((*first)->getName()), (*first)->getID() );
//             if ( first == current ) {
//                 toMakeCurrent = comboType->count() - 1;
//             }
//             ++first;
//         }
//         comboType->blockSignals(blocked);
//         if ( toMakeCurrent >= 0 ) {
//             comboType->setCurrentIndex(-1);
//             comboType->setCurrentIndex(toMakeCurrent);
//         } else {
//             UTILS_FAIL("Jak to mo�liwe?");
//         }
//     }

    //! Dodaje wizualizator do listy obs�ugiwanych.
    void addVisualizer(const QString& label, UniqueID id);
    //! Dodaje wizualizator do listy obs�ugiwanych.
    void addVisualizer(const QIcon& icon, const QString& label, UniqueID id);

    //! \return Liczba zarejestrowanych wizualizator�w.
    int getNumVisualizers() const;
    //! Usuwa wszystkie wizualizatory.
    void removeAllVisualizers();

    //! Przywraca stan pocz�tkowy.
    void clear();

    //! \return Bie��cy wizualizator.
    inline VisualizerPtr getCurrentVisualizer()
    {
        return visualizer;
    }

signals:
    //! \param visualizer
    void currentVisualizerChanged(const VisualizerPtr& visualizer);

public slots:
    //! \param id Id bie��cego wizualizatora.
    void setCurrentVisualizer(UniqueID id);
    //! \param visualizer Bie��cy wizualizator.
    void setCurrentVisualizer(const VisualizerPtr& visualizer);
    //! \param idx Indeks bie��cego wizualizatora.
    void setCurrentVisualizer(int idx);

protected:
    virtual void paintEvent(QPaintEvent *paintEvent);

private:
    //! \param widgetSources Widget przejmowany na w�asno��.
    void setWidgetSources(QWidget* widgetSources);
};

#endif  // HEADER_GUARD_CORE__VISUALIZERWIDGETTITLEBAR_H__