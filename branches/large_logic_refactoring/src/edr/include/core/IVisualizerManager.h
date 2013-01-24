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
		//! Typ agregatu prototyp�w
		typedef std::list<const plugin::IVisualizer *> VisualizerPrototypes;

		//! Typ operacji rejestrowania, wyrejestrowywania wizualizator�w
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
		//! \param prototypes [out] Wszystkie prototypy wizualizator�w
		virtual void visualizerPrototypes(VisualizerPrototypes & prototypes) = 0;
		//! \param id ID �r�d�a do wyszukania.
		//! \return Odnalezione �r�d�o b�d� NULL.
		virtual const plugin::IVisualizer * getVisualizerPrototype(UniqueID id) = 0;
		//! \param type Typ dla kt�rego poszukujeemy wizualizator�w
		//! \param prototypes [out] Prototypy wizualizator�w potrafi�cych obs�u�y� zadany typ
		virtual void getVisualizerPrototypesForType(core::TypeInfo & type, VisualizerPrototypes & prototypes, bool exact) = 0;
		
		//! \param visualizer Wizualizator rejestrowany do aktualizacji
		virtual void registerForUpdate(plugin::IVisualizer * visualizer) = 0;
		//! \param visualizer Wizualizator wy�anczany z aktualizacji
		virtual void unregisterForUpdate(plugin::IVisualizer * visualizer) = 0;

		//! \param visualizer Wizualizator rejestrowany do aktualizacji
		virtual void notifyCreation(plugin::IVisualizer * visualizer) = 0;
		//! \param visualizer Wizualizator wy�anczany z aktualizacji
		virtual void notifyDestruction(plugin::IVisualizer * visualizer) = 0;

		//! \param observer Obserwator tworzonych wizualziator�w w��czony do obserwacji
		virtual void registerObserver(IVisualizerManagerObserver * observer) = 0;
		//! \param observer Obserwator tworzonych wizualziator�w wy�anczany z obserwacji
		virtual void unregisterObserver(IVisualizerManagerObserver * observer) = 0;
    };
} // namespace core

#endif  // HEADER_GUARD_CORE__IVISUALIZERMANAGER_H__
