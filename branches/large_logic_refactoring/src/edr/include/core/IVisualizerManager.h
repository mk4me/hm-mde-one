/********************************************************************
    created:  2011/04/22
    created:  22:4:2011   11:50
    filename: IVisualizerManager.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IVISUALIZERMANAGER_H__
#define HEADER_GUARD_CORE__IVISUALIZERMANAGER_H__

#include <list>
#include <core/BaseDataTypes.h>
#include <core/TypeInfo.h>

namespace plugin {

	class IVisualizer;

}

namespace core 
{
    class IVisualizerManager
    {
	public:
		//! Typ agregatu prototypów
		typedef std::list<const plugin::IVisualizer *> VisualizerPrototypes;

		//! Typ operacji rejestrowania, wyrejestrowywania wizualizatorów
		enum VisuzalizerOperation {
			Creation,	//! Rejestracja wizualizatora
			Destruction	//! Wyrejestrowywanie wizualizatora
		};

		class IVisualizerManagerObserver
		{
		public:
			virtual void update(VisuzalizerOperation modification, plugin::IVisualizer * visualizer);
		};

    public:
		//! \param prototypes [out] Wszystkie prototypy wizualizatorów
		virtual void visualizerPrototypes(VisualizerPrototypes & prototypes) = 0;
		//! \param id ID Ÿród³a do wyszukania.
		//! \return Odnalezione Ÿród³o b¹dŸ NULL.
		virtual const plugin::IVisualizer * getVisualizerPrototype(UniqueID id) = 0;
		//! \param type Typ dla którego poszukujeemy wizualizatorów
		//! \param prototypes [out] Prototypy wizualizatorów potrafi¹cych obs³u¿yæ zadany typ
		virtual void getVisualizerPrototypesForType(core::TypeInfo & type, VisualizerPrototypes & prototypes, bool exact) = 0;
		
		//! \param visualizer Wizualizator rejestrowany do aktualizacji
		virtual void registerForUpdate(plugin::IVisualizer * visualizer) = 0;
		//! \param visualizer Wizualizator wy³anczany z aktualizacji
		virtual void unregisterForUpdate(plugin::IVisualizer * visualizer) = 0;

		//! \param visualizer Wizualizator rejestrowany do aktualizacji
		virtual void notifyCreation(plugin::IVisualizer * visualizer) = 0;
		//! \param visualizer Wizualizator wy³anczany z aktualizacji
		virtual void notifyDestruction(plugin::IVisualizer * visualizer) = 0;

		//! \param observer Obserwator tworzonych wizualziatorów w³¹czony do obserwacji
		virtual void registerObserver(IVisualizerManagerObserver * observer) = 0;
		//! \param observer Obserwator tworzonych wizualziatorów wy³anczany z obserwacji
		virtual void unregisterObserver(IVisualizerManagerObserver * observer) = 0;
    };
} // namespace core

#endif  // HEADER_GUARD_CORE__IVISUALIZERMANAGER_H__
