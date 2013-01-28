/********************************************************************
	created:  2011/03/11
	created:  11:3:2011   14:29
	filename: VisualizerManager.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZERMANAGER_H__
#define HEADER_GUARD_CORE__VISUALIZERMANAGER_H__

#include <vector>
#include <list>

#include <boost/range.hpp>
#include <boost/iterator.hpp>
#include <QtGui/QIcon>
#include <corelib/IVisualizerManager.h>
#include <corelib/IVisualizer.h>
#include <utils/SynchronizationPolicies.h>


//#include <QtCore/QMetaType>
//Q_DECLARE_METATYPE(UniqueID);

//class SceneGraphWidget;

namespace core {

class VisualizerManager : public IVisualizerManager {

private:
	//! Struktura opisująca prototyp wizualizatora
	struct VisualizerPrototypeData {
		//! Prototyp wizualizatora
		VisualizerPtr visualizerPrototype;
		//! Zbiór wspieranych przez niego typów bezpośrednio
		TypeInfoSet basicSupportedTypes;
		//! Zbiór wspieranych przez niego typów
		TypeInfoSet derrivedSupportedTypes;
	};

	//! Lista prototypów vizualizatorów
	typedef std::map<UniqueID, VisualizerPrototypeData> VisualizerPrototypes;
	//! Typ listy obserwatorów
	typedef std::set<IVisualizerManagerObserver*> Observers;
	//! Typ listy aktualnie żyjącyh instancji
	typedef std::map<Visualizer*, plugin::IVisualizer*> VisualizerInstances;
	//! Obsługa synchronizacji
	typedef utils::RecursiveSyncPolicy SyncPolicy;
	typedef utils::ScopedLock<SyncPolicy> ScopedLock;

private:

	VisualizerPrototypes visualizerPrototypes_;
	Observers observers_;
	VisualizerInstances visualizerInstances_;

	//! Obiekt na potrzeby synchronizacji update
	mutable SyncPolicy updateSync;
	//! Obiekt na potrzeby synchronizacji obserwatorów
	mutable SyncPolicy observerSync;

	bool skipNotify;

public:

	VisualizerManager();
	~VisualizerManager();

public:

	//! API IVisualizerManager

	//! \param prototypes [out] Wszystkie prototypy wizualizatorów
	virtual void visualizerPrototypes(IVisualizerManager::VisualizerPrototypes & prototypes);
	//! \param id ID źródła do wyszukania.
	//! \return Odnalezione źródło bądź NULL.
	virtual VisualizerConstPtr getVisualizerPrototype(UniqueID id);
	//! \param type Typ dla którego poszukujeemy wizualizatorów
	//! \param prototypes [out] Prototypy wizualizatorów potrafiących obsłużyć zadany typ
	virtual void getVisualizerPrototypesForType(core::TypeInfo & type, IVisualizerManager::VisualizerPrototypes & prototypes, bool exact);
	
	//! \param observer Obserwator tworzonych wizualziatorów włączony do obserwacji
	virtual void registerObserver(IVisualizerManagerObserver * observer);
	//! \param observer Obserwator tworzonych wizualziatorów wyłanczany z obserwacji
	virtual void unregisterObserver(IVisualizerManagerObserver * observer);

private:

	virtual void registerVisualizer(Visualizer* visualizerImpl, plugin::IVisualizer * visualizer) = 0;
	virtual void unregisterVisualizer(Visualizer* visualizer) = 0;

public:

	void registerVisualizerPrototype(plugin::IVisualizerPtr visualizerPrototype);

	void update(double deltaTime);

private:

	void notify(Visualizer * visualizer, VisuzalizerOperation modyfication);

};

}

#endif  // HEADER_GUARD_CORE__VISUALIZERMANAGER_H__
