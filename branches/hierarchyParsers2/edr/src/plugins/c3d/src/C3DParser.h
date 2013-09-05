/********************************************************************
	created:	2011/05/25
	created:	25:5:2011   10:14
	filename: 	C3DParser.h
	author:		Wojciech Knieć
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PLUGINC3D__C3DPARSER_H__
#define HEADER_GUARD_PLUGINC3D__C3DPARSER_H__

#include <utils/SmartPtr.h>
#include <corelib/Filesystem.h>
#include <corelib/IParser.h>
#include <c3dlib/C3DParser.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyHelper.h>
#include <plugins/c3d/C3DCollections.h>
#include <utils/ObjectWrapper.h>

//! klasa pomocnicza przy tworzeniu wykresów z wektora 3-elementowego
class  NewVector3ItemHelper : public core::WrappedItemHelper
{
public:
    //! Konstruktor pobiera obiekt z wrapperem wektora 3D
    //! \param wrapper 
    NewVector3ItemHelper(
        const utils::ObjectWrapperConstPtr& wrapper, 
        const EventsCollectionConstPtr& events = EventsCollectionConstPtr());
public:
    //! zwraca utworzone serie danych
    //! \param visualizer wizualizator, który będzie tworzył serie
    //! \param path ścieżka dla timeline
    //! \param series tutaj trafia stworzone serie
    virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series);
    //! \return wizualizator 2D
    virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
    //! \return typ ScalarChannelReaderInterface
    std::vector<core::TypeInfo> getTypeInfos() const;
private:
    EventsCollectionConstPtr events;
};
typedef utils::shared_ptr<NewVector3ItemHelper> NewVector3ItemHelperPtr;
typedef utils::shared_ptr<const NewVector3ItemHelper> NewVector3ItemHelperConstPtr;


//! Klasa wykorzystuje c3dlib do zasilenia systemu w obiekty domenowe pochodzące z plików C3D
class C3DParser : public plugin::IParser, public plugin::ISourceParserCapabilities
{
    UNIQUE_ID("{D7801231-BACA-42C6-9A8E-706F561A563F}")
	CLASS_DESCRIPTION("C3D parser", "C3D parser")
	typedef utils::shared_ptr<c3dlib::C3DParser> ParserPtr;
private:
    //! Obsolete. Wektor zawiera 4 kanały analogowe z płyt GRF
	std::vector<core::ObjectWrapperPtr> GRFChannels;
    //! Obsolete. Wektor zawiera 16 kanałów analogowych EMG
	std::vector<core::ObjectWrapperPtr> EMGChannels;
    //! Kolekcja z danymi GRF
	core::ObjectWrapperPtr GRFs;
    //! Kolekcja z danymi EMG
	core::ObjectWrapperPtr EMGs;
    //! Kolekcja z markerami
	core::ObjectWrapperPtr markerChannels;
    //! Kolekcja z siłami w stawach - dane są estymowane
	core::ObjectWrapperPtr forceChannels;
    //! Kolekcja z kątami w stawach - dane są estymowane
	core::ObjectWrapperPtr angleChannels;
    //! Kolekcja z momentami sił w stawach - dane są estymowane
	core::ObjectWrapperPtr momentChannels;
    //! Kolekcja z mocami w stawach - dane są estymowane
	core::ObjectWrapperPtr powerChannels;
    //! Kolekcja ze zdarzeniami zapisanymi w pliku C3D (np. stąpnięcie stopy)
    core::ObjectWrapperPtr allEvents;
    //! Offsety video
    core::ObjectWrapperPtr movieDelays;
    //! wskaźnik do właściwego parsera z biblioteki c3dlib
	ParserPtr parserPtr;

    core::HierarchyItemPtr hierarchy;

private:
    void createTree(); 
    template <class CollectionPtr>
    void addCollection(core::IHierarchyItemPtr parent, const CollectionPtr& collection, const QString& name, const QString& desc);

    template <class CollectionPtr>
    void tryAddVectorToTree(const CollectionPtr & collection, const std::string& name, const QIcon& childIcon,  core::IHierarchyItemPtr parentItem, bool createContainerItem = true );


public:
    C3DParser();
    virtual ~C3DParser();

public:
    //! Parsowanie pliku c3d
    //! \param path poprawna ścieżka do pliku
    virtual void parse(const std::string & source);
    //! \return pusty obiekt nowego parsera
    virtual plugin::IParser* create() const;
    //! Zwraca obiekty dostarczone przez parser
    //! \param objects kolekcja z obiektami (set)
    virtual plugin::IParser::ParsedObjectsPtr getObjects();
    //! Zwraca rozszerzenia, które są obsługiwane przez parser (tylko c3d)
    //! \param extensions kolecja z roszerzeniami
    virtual void acceptedExpressions(Expressions & expressions) const;
	//! Zapisauje dane w konkretnym miejscu, nie używane
	//! \param path ścieżka zapisu
	void saveFile(const core::Filesystem::Path& path);
};


#endif
