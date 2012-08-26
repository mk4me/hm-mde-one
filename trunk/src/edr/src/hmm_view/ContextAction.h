/********************************************************************
	created:	2012/05/31
	created:	31:5:2012   12:22
	filename: 	ContextAction.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMM__CONTEXTACTION_H__
#define HEADER_GUARD_HMM__CONTEXTACTION_H__

#include <core/Visualizer.h>
#include <QtGui/QAction>
class EDRDockWidgetSet;
class HmmTreeItem;

//! akcja obsługiwana w obrębie drzewa danych w analizach
//! przechowuje dodatkowe informacje, które ułatwiaja tworzenie wizualizatorów i drzewa
class ContextAction : public QAction
{
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param itemHelper helper przypiasany do elementu drzewa
    //! \param parent rodzic akcji
    //! \param vis wizualizator związany z akcja
    //! \param set set, do którego ma trafic wizualizator
    ContextAction(HmmTreeItem* itemHelper, QObject* parent, VisualizerPtr vis = VisualizerPtr(), EDRDockWidgetSet* set = nullptr) :
      item(itemHelper), QAction(parent), visualizer(vis), set(set){}

public:
    //! \return helper przypiasany do elementu drzewa
    HmmTreeItem* getTreeItem() const { return item; }
    //! \return wizualizator związany z akcja
    VisualizerPtr getVisualizer() const { return visualizer.lock(); }
    //! \return set, do którego ma trafic wizualizator lub nullptr jeśli nie jest to sprecyzowane
    EDRDockWidgetSet* getDockSet() const { return set; }

private:
    //!  helper przypiasany do elementu drzewa
    HmmTreeItem* item;
    //! wizualizator związany z akcja
    VisualizerWeakPtr visualizer;
    //! set, do którego ma trafic wizualizator lub nullptr jeśli nie jest to sprecyzowane
    EDRDockWidgetSet* set;
};


#endif
