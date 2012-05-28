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
#include <core/IManagersAccessor.h>
#include "ManagerHelper.h"

Q_DECLARE_METATYPE(UniqueID);

//class SceneGraphWidget;
class DataManager;

class VisualizerManager : public core::IVisualizerManager, public utils::Observable<VisualizerManager>, public ManagerHelper<VisualizerManager>
{
    friend class Visualizer;
	friend class IVisualizerChannel;
public:
    //! Lista wizualizator�w.
    typedef std::vector<core::IVisualizerPtr> IVisualizers;
    //! 
    typedef boost::iterator_range<IVisualizers::const_iterator> IVisualizersConstRange;

    typedef std::list<Visualizer*> Visualizers;

    //! Lista typ�w kolejnych �r�de�.
    typedef ObjectSlots::SlotsInfo SourcesTypes;

private:

    //! Niezmienne dane pobrane z wizualizator�w.
    struct IVisualizerPersistantData
    {
        //! Ikona.
        QIcon icon;
        //! Lista typ�w kolejnych �r�de�.
        SourcesTypes sourcesTypes;
    };

	//! Dane zwi�zane z obs�uga kana��w timeline
	struct ChannelData {
		Visualizer * visualzier;
		std::set<core::VisualizerTimeSeriePtr> series;
		std::string path;
		bool managed;
		bool synchRemove;
	};

	//! pomocnicza mapa (typ obiektu domenowego) -> (identyfikator wizualizatora)
	std::map<core::TypeInfo, UniqueID> mapType2ID;

    //! Prototypy wizualizator�w.
    IVisualizers prototypes;

    Visualizers visualizers;

    //! Sta�e dane wizualizator�w.
    std::vector< IVisualizerPersistantData* > visualizersData;
    //! Widget do wizualizacji struktury sceny 3D. Do debuggowania.
    //SceneGraphWidget* debugWidget;

	//! Utworzone kana�y
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
    void registerVisualizer(core::IVisualizerPtr visualizer);

    //! Aktualizacja wizualizator�w.
    void update();

    //! \return Ikona dla danego wizualizatora.
    const QIcon& getIcon(UniqueID id) const;
    //! \return Informacje o wej�ciu dla danego wizualizatora.
    const SourcesTypes& getSourcesTypes(UniqueID id) const;

    //! \return Wyliczenie prototyp�w.
    inline IVisualizersConstRange enumPrototypes() const
    {
        return boost::make_iterator_range(prototypes.begin(), prototypes.end());
    }
    //! \return Liczba zarejestrowanych prototyp�w.
    inline int getNumPrototypes() const
    {
        return static_cast<int>(prototypes.size());
    }
    //! \param i Indeks wizualizatora.
    inline core::IVisualizerConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! Sprawdzenie czy dla danego typu danych istnieje wizualizator
    //! \param type Typ danych dla kt�rego chcemy uzyska� wizualizator
    //! \return Instancja wizualizatora lub wyj�tek je�li nie mo�na utworzy� (bo nie istnieje + inne b��dy)
    inline bool existVisualizerForType(const core::TypeInfo & type) const
    {
        DataManager* dataManager = DataManager::getInstance();
        for (auto it = mapType2ID.begin(); it != mapType2ID.end(); ++it) {
            if (dataManager->isTypeCompatible(type, it->first)) {
                return true;
            }
        }

        return mapType2ID.find(type) != mapType2ID.end();
    }

    //! \param id ID wizualizatora.
    core::IVisualizerConstPtr getPrototype(UniqueID id) const;

    //! Tworzy instancj� wizualizatora.
    //! \param id id wizualizatora.
    //! \return Instancja wizualizatora.
    VisualizerPtr createVisualizer(UniqueID id);

    //! \return Liczba instancji wizualizator�w danego typu.
    int getNumInstances(UniqueID id);

	//! \param serie Seria danych, kt�r� opakowujemy w kana� i dodajemy do timeline
	//! \param visualizer Wizualizator ktorego seria ta dotyczy
	//! \param path �cie�ka w timeline gdzie probujemy doda� kana�
	//! \param synchRemove Czy usuni�cie kana�u z timeline powinno skutkowa� usuni�ciem jego serii danych z wizualizatora
	//! \return Wska�nik typu const void * identyfikuj�cy utworzony kana�, warto�� nullptr oznacza �e kana�u nie utworzono
	const void * createChannel(const core::VisualizerTimeSeriePtr & serie, Visualizer * visualizer, const std::string & path = std::string(), bool synchRemove = true);
	//! \param series Serie danych, kt�re opakowujemy w kana� i dodajemy do timeline
	//! \param visualizer Wizualizator ktorego serie te dotyczy
	//! \param path �cie�ka w timeline gdzie probujemy doda� kana�
	//! \param synchRemove Czy usuni�cie kana�u z timeline powinno skutkowa� usuni�ciem jego serii danych z wizualizatora
	//! \return Wska�nik typu const void * identyfikuj�cy utworzony kana�, warto�� nullptr oznacza �e kana�u nie utworzono
	const void * createChannel(const std::vector<core::VisualizerTimeSeriePtr> & series, Visualizer * visualizer, const std::string & path, bool synchRemove = true);
	//! \param channel Wska�nik typu const void * identyfikuj�cy kana� dla serii danych utworzony dla timeline
	void removeChannel(const void * channel);
	//! \param channel Wska�nik typu const void * identyfikuj�cy kana� dla serii danych utworzony dla timeline
	//! Metoda oznacza �e serie danych kana�u zosta�y ju� usuni�t� z wizualizatora
	void markAsRemovedFromVisualizer(const void * channel);

	//! \param visualizer Wizualizator dla ktorego oznaczamy ze wszystkie serie kanalow zostaly juz obsluzone - nie wymagaja usuwania
	void markAllChannelsAsRemovedFromVisualizer(Visualizer * visualizer);
	//! \param visualizer Wizualizator dla ktorego usuwamy wszystkie kanaly
	void removeAllChannelsFromVisualizer(Visualizer * visualizer);

	//! Wszystkie kanaly sa oznaczone jako obsluzone przez wizualizaotry
	void markAllChannelsAsRemoved();
	//! Usuwa wszystkie kanaly
	void removeAllChannels();

private:
    //! Tworzy instancj� wizualizatora.
    //! \param id id wizualizatora.
    //! \return Instancja wizualizatora.
    VisualizerPtr createVisualizer(const core::IVisualizerConstPtr& prototype);
    //! 
    VisualizerPtr createVisualizer(const Visualizer& prototype);

	//! \param channel Kana� kt�ry w�a�nie jest usuwamy
	//! Jedyny smart pointer jest trzymany przez timeline
	//! Je�li tam go usuwam to uruchamiam jego destruktor kt�ry realizuje czyszczenie w wizualizatorach
	void notifyDestroyed(IVisualizerChannel * channel);

// TODO: create publiczne bez friend Visualizer czy prywatne + jakis inny mechanizm dostepu?
public:
	//! Tworzy instancj� wizualizatora.
	//! \param typeInfo typ, ktory bedzie wyswietlany w wizualizatorze.
	//! \return Instancja wizualizatora.
	VisualizerPtr createVisualizer(const core::TypeInfo& typeInfo);

private:
    //! \return Indeks prototypu.
    int getPrototypeIdx(UniqueID id) const;

    void notifyCreated(Visualizer* visualizer);
    void notifyDestroyed(Visualizer* visualizer);
};



#endif  // HEADER_GUARD_CORE__VISUALIZERMANAGER_H__