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

#include <QtCore/QMetaType>
#include <QtGui/QIcon>

#include <OpenThreads/ReentrantMutex>

#include "Visualizer.h"
#include "VisualizerChannel.h"

#include <core/IVisualizerManager.h>
#include <utils/ObserverPattern.h>

Q_DECLARE_METATYPE(UniqueID);

//class SceneGraphWidget;

namespace core {

class VisualizerManager : public IVisualizerManager, public utils::Observable<VisualizerManager>
{
    friend class Visualizer;
	friend class IVisualizerChannel;
public:
    //! Lista wizualizatorów.
    typedef std::vector<plugin::IVisualizerPtr> IVisualizers;
    //! 
    typedef boost::iterator_range<IVisualizers::const_iterator> IVisualizersConstRange;

    typedef std::list<Visualizer*> Visualizers;

    //! Lista typów kolejnych źródeł.
	//TODO
    //typedef ObjectSlots::SlotsInfo SourcesTypes;

private:

    //! Niezmienne dane pobrane z wizualizatorów.
    struct IVisualizerPersistantData
    {
        //! Ikona.
        QIcon icon;
		//TODO
        //! Lista typów kolejnych źródeł.
        //SourcesTypes sourcesTypes;
    };

	//! Dane związane z obsługa kanałów timeline
	struct ChannelData {
		Visualizer * visualzier;
		std::set<plugin::VisualizerTimeSeriePtr> series;
		std::string path;
		bool managed;
		bool synchRemove;
	};

	//! pomocnicza mapa (typ obiektu domenowego) -> (identyfikator wizualizatora)
	std::map<core::TypeInfo, UniqueID> mapType2ID;

    //! Prototypy wizualizatorów.
    IVisualizers prototypes;

    Visualizers visualizers;

    //! Stałe dane wizualizatorów.
    std::vector< IVisualizerPersistantData* > visualizersData;
    //! Widget do wizualizacji struktury sceny 3D. Do debuggowania.
    //SceneGraphWidget* debugWidget;

	//! Utworzone kanały
	std::map<IVisualizerChannel *, ChannelData> channels;
	std::map<Visualizer *, std::set<IVisualizerChannel *>> visualizerChannels;

	mutable OpenThreads::ReentrantMutex visualizersMutex;

public:
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    VisualizerManager();
    //! Tworzenie i niszczenie tylko przez metody singletonu.
    ~VisualizerManager();

public:

    //! \param visualizer
    void registerVisualizer(plugin::IVisualizerPtr visualizer);

    //! Aktualizacja wizualizatorów.
    void update();

    //! \return Ikona dla danego wizualizatora.
    const QIcon& getIcon(UniqueID id) const;
	//TODO
    //! \return Informacje o wejściu dla danego wizualizatora.
    //const SourcesTypes& getSourcesTypes(UniqueID id) const;

    //! \return Wyliczenie prototypów.
    inline IVisualizersConstRange enumPrototypes() const
    {
        return boost::make_iterator_range(prototypes.begin(), prototypes.end());
    }
    //! \return Liczba zarejestrowanych prototypów.
    inline int getNumPrototypes() const
    {
        return static_cast<int>(prototypes.size());
    }
    //! \param i Indeks wizualizatora.
    inline plugin::IVisualizerConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! Sprawdzenie czy dla danego typu danych istnieje wizualizator
    //! \param type Typ danych dla którego chcemy uzyskać wizualizator
    //! \return Instancja wizualizatora lub wyjątek jeśli nie można utworzyć (bo nie istnieje + inne błędy)
    inline bool existVisualizerForType(const core::TypeInfo & type) const
    {
		/*DataManager* dataManager = DataManager::getInstance();
		for (auto it = mapType2ID.begin(); it != mapType2ID.end(); ++it) {
		if (dataManager->isTypeCompatible(type, it->first)) {
		return true;
		}
		}

		return mapType2ID.find(type) != mapType2ID.end();*/
		UTILS_ASSERT(false, "Poprawić");
		return false;
    }

    //! \param id ID wizualizatora.
    plugin::IVisualizerConstPtr getPrototype(UniqueID id) const;

    //! Tworzy instancję wizualizatora.
    //! \param id id wizualizatora.
    //! \return Instancja wizualizatora.
    VisualizerPtr createVisualizer(UniqueID id);

    //! \return Liczba instancji wizualizatorów danego typu.
    int getNumInstances(UniqueID id);

	//! \param serie Seria danych, którą opakowujemy w kanał i dodajemy do timeline
	//! \param visualizer Wizualizator którego seria ta dotyczy
	//! \param path Ścieżka w timeline gdzie próbujemy dodać kanał
	//! \param synchRemove Czy usunięcie kanału z timeline powinno skutkować usunięciem jego serii danych z wizualizatora
	//! \return Wskaźnik typu const void * identyfikujący utworzony kanał, wartość nullptr oznacza że kanału nie utworzono
	const void * createChannel(const plugin::VisualizerTimeSeriePtr & serie, Visualizer * visualizer, const std::string & path = std::string(), bool synchRemove = true);
	//! \param series Serie danych, które opakowujemy w kanał i dodajemy do timeline
	//! \param visualizer Wizualizator którego serie te dotyczy
	//! \param path Ścieżka w timeline gdzie próbujemy dodać kanał
	//! \param synchRemove Czy usunięcie kanału z timeline powinno skutkować usunięciem jego serii danych z wizualizatora
	//! \return Wskaźnik typu const void * identyfikujący utworzony kanał, wartość nullptr oznacza że kanału nie utworzono
	const void * createChannel(const std::vector<plugin::VisualizerTimeSeriePtr> & series, Visualizer * visualizer, const std::string & path, bool synchRemove = true);
	//! \param channel Wskaźnik typu const void * identyfikujący kanał dla serii danych utworzony dla timeline
	void removeChannel(const void * channel);
	//! \param channel Wskaźnik typu const void * identyfikujący kanał dla serii danych utworzony dla timeline
	//! Metoda oznacza że serie danych kanału zostały już usuniętę z wizualizatora
	void markAsRemovedFromVisualizer(const void * channel);

	//! \param visualizer Wizualizator dla którego oznaczamy ze wszystkie serie kanałów zostały już obsłużone - nie wymagaja usuwania
	void markAllChannelsAsRemovedFromVisualizer(Visualizer * visualizer);
	//! \param visualizer Wizualizator dla którego usuwamy wszystkie kanały
	void removeAllChannelsFromVisualizer(Visualizer * visualizer);

	//! Wszystkie kanały są oznaczone jako obsłużone przez wizualizaotry
	void markAllChannelsAsRemoved();
	//! Usuwa wszystkie kanały
	void removeAllChannels();

private:
    //! Tworzy instancję wizualizatora.
    //! \param id id wizualizatora.
    //! \return Instancja wizualizatora.
    VisualizerPtr createVisualizer(const plugin::IVisualizerConstPtr& prototype);
    //! 
    VisualizerPtr createVisualizer(const Visualizer& prototype);

	//! \param channel Kanał który właśnie jest usuwamy
	//! Jedyny smart pointer jest trzymany przez timeline
	//! Jeśli tam go usuwam to uruchamiam jego destruktor który realizuje czyszczenie w wizualizatorach
	void notifyDestroyed(IVisualizerChannel * channel);

// TODO: create publiczne bez friend Visualizer czy prywatne + jakis inny mechanizm dostępu?
public:
	//! Tworzy instancję wizualizatora.
	//! \param typeInfo typ, który będzie wyświetlany w wizualizatorze.
	//! \return Instancja wizualizatora.
	VisualizerPtr createVisualizer(const TypeInfo& typeInfo);

private:
    //! \return Indeks prototypu.
    int getPrototypeIdx(UniqueID id) const;

    void notifyCreated(Visualizer* visualizer);
    void notifyDestroyed(Visualizer* visualizer);
};

}

#endif  // HEADER_GUARD_CORE__VISUALIZERMANAGER_H__
