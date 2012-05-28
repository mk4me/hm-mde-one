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
    //! Lista wizualizatorów.
    typedef std::vector<core::IVisualizerPtr> IVisualizers;
    //! 
    typedef boost::iterator_range<IVisualizers::const_iterator> IVisualizersConstRange;

    typedef std::list<Visualizer*> Visualizers;

    //! Lista typów kolejnych Ÿróde³.
    typedef ObjectSlots::SlotsInfo SourcesTypes;

private:

    //! Niezmienne dane pobrane z wizualizatorów.
    struct IVisualizerPersistantData
    {
        //! Ikona.
        QIcon icon;
        //! Lista typów kolejnych Ÿróde³.
        SourcesTypes sourcesTypes;
    };

	//! Dane zwi¹zane z obs³uga kana³ów timeline
	struct ChannelData {
		Visualizer * visualzier;
		std::set<core::VisualizerTimeSeriePtr> series;
		std::string path;
		bool managed;
		bool synchRemove;
	};

	//! pomocnicza mapa (typ obiektu domenowego) -> (identyfikator wizualizatora)
	std::map<core::TypeInfo, UniqueID> mapType2ID;

    //! Prototypy wizualizatorów.
    IVisualizers prototypes;

    Visualizers visualizers;

    //! Sta³e dane wizualizatorów.
    std::vector< IVisualizerPersistantData* > visualizersData;
    //! Widget do wizualizacji struktury sceny 3D. Do debuggowania.
    //SceneGraphWidget* debugWidget;

	//! Utworzone kana³y
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

    //! Aktualizacja wizualizatorów.
    void update();

    //! \return Ikona dla danego wizualizatora.
    const QIcon& getIcon(UniqueID id) const;
    //! \return Informacje o wejœciu dla danego wizualizatora.
    const SourcesTypes& getSourcesTypes(UniqueID id) const;

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
    inline core::IVisualizerConstPtr getPrototype(int i) const
    {
        UTILS_ASSERT(i < getNumPrototypes());
        return prototypes[i];
    }

    //! Sprawdzenie czy dla danego typu danych istnieje wizualizator
    //! \param type Typ danych dla którego chcemy uzyskaæ wizualizator
    //! \return Instancja wizualizatora lub wyj¹tek jeœli nie mo¿na utworzyæ (bo nie istnieje + inne b³êdy)
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

    //! Tworzy instancjê wizualizatora.
    //! \param id id wizualizatora.
    //! \return Instancja wizualizatora.
    VisualizerPtr createVisualizer(UniqueID id);

    //! \return Liczba instancji wizualizatorów danego typu.
    int getNumInstances(UniqueID id);

	//! \param serie Seria danych, któr¹ opakowujemy w kana³ i dodajemy do timeline
	//! \param visualizer Wizualizator ktorego seria ta dotyczy
	//! \param path Œcie¿ka w timeline gdzie probujemy dodaæ kana³
	//! \param synchRemove Czy usuniêcie kana³u z timeline powinno skutkowaæ usuniêciem jego serii danych z wizualizatora
	//! \return WskaŸnik typu const void * identyfikuj¹cy utworzony kana³, wartoœæ nullptr oznacza ¿e kana³u nie utworzono
	const void * createChannel(const core::VisualizerTimeSeriePtr & serie, Visualizer * visualizer, const std::string & path = std::string(), bool synchRemove = true);
	//! \param series Serie danych, które opakowujemy w kana³ i dodajemy do timeline
	//! \param visualizer Wizualizator ktorego serie te dotyczy
	//! \param path Œcie¿ka w timeline gdzie probujemy dodaæ kana³
	//! \param synchRemove Czy usuniêcie kana³u z timeline powinno skutkowaæ usuniêciem jego serii danych z wizualizatora
	//! \return WskaŸnik typu const void * identyfikuj¹cy utworzony kana³, wartoœæ nullptr oznacza ¿e kana³u nie utworzono
	const void * createChannel(const std::vector<core::VisualizerTimeSeriePtr> & series, Visualizer * visualizer, const std::string & path, bool synchRemove = true);
	//! \param channel WskaŸnik typu const void * identyfikuj¹cy kana³ dla serii danych utworzony dla timeline
	void removeChannel(const void * channel);
	//! \param channel WskaŸnik typu const void * identyfikuj¹cy kana³ dla serii danych utworzony dla timeline
	//! Metoda oznacza ¿e serie danych kana³u zosta³y ju¿ usuniêtê z wizualizatora
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
    //! Tworzy instancjê wizualizatora.
    //! \param id id wizualizatora.
    //! \return Instancja wizualizatora.
    VisualizerPtr createVisualizer(const core::IVisualizerConstPtr& prototype);
    //! 
    VisualizerPtr createVisualizer(const Visualizer& prototype);

	//! \param channel Kana³ który w³aœnie jest usuwamy
	//! Jedyny smart pointer jest trzymany przez timeline
	//! Jeœli tam go usuwam to uruchamiam jego destruktor który realizuje czyszczenie w wizualizatorach
	void notifyDestroyed(IVisualizerChannel * channel);

// TODO: create publiczne bez friend Visualizer czy prywatne + jakis inny mechanizm dostepu?
public:
	//! Tworzy instancjê wizualizatora.
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