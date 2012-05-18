#include "CorePCH.h"

#include <iterator>
#include <core/StringTools.h>
#include <boost/foreach.hpp>
#include "VisualizerWidget.h"
#include <core/DataAccessors.h>
#include "MainWindow.h"
#include <core/DataAccessors.h>
#include "ServiceManager.h"
#include <plugins/newTimeline/ITimelineService.h>

using namespace core;

typedef std::pair<core::TypeInfo, core::ObjectWrapperConstPtr> TypeData;

Q_DECLARE_METATYPE(TypeData);

VisualizerWidget::VisualizerWidget(QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/, bool autoRefreshInputs  /*= true*/) : EDRDockWidget(parent, flags), autoRefreshInputs_(autoRefreshInputs)
{
    init();
    // dodajemy wizualizatory
    BOOST_FOREACH(const IVisualizerConstPtr& vis, VisualizerManager::getInstance()->enumPrototypes()) {
        addVisualizer( VisualizerManager::getInstance()->getIcon(vis->getID()),  toQString(vis->getName()), vis->getID() );
    }
}

VisualizerWidget::VisualizerWidget( UniqueID visualizerID, QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/, bool autoRefreshInputs  /*= true*/ ) : EDRDockWidget(parent, flags), autoRefreshInputs_(autoRefreshInputs)
{
    init();
    // blokujemy sygna³y 
    comboType->blockSignals(true);
    // dodajemy wizualizatory
    BOOST_FOREACH(const IVisualizerConstPtr& vis, VisualizerManager::getInstance()->enumPrototypes()) {
        addVisualizer( VisualizerManager::getInstance()->getIcon(vis->getID()),  toQString(vis->getName()), vis->getID() );
    }
    // ustawiamy wizualizator
    comboType->blockSignals(false);
    setCurrentVisualizer(visualizerID);
}

VisualizerWidget::VisualizerWidget( const VisualizerPtr& source, QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/, bool autoRefreshInputs  /*= true*/ ) : EDRDockWidget(parent, flags), autoRefreshInputs_(autoRefreshInputs)
{
    init();
    // blokujemy sygna³y
    comboType->blockSignals(true);
    // dodajemy wizualizatory
    BOOST_FOREACH(const IVisualizerConstPtr& vis, VisualizerManager::getInstance()->enumPrototypes()) {
        addVisualizer( VisualizerManager::getInstance()->getIcon(vis->getID()),  toQString(vis->getName()), vis->getID() );
    }
    // ustawiamy wizualizator
    comboType->blockSignals(false);
    // ustawiamy wizualizator
    setCurrentVisualizer(source);
}

VisualizerWidget::~VisualizerWidget()
{        
    // usuniêcie widgeta
	visualizerWidgetContainer = nullptr;
    clearCurrentVisualizer();
}

void VisualizerWidget::setAutoRefreshInputs(bool autoRefresh)
{
	autoRefreshInputs_ = autoRefresh;
}

bool VisualizerWidget::autoRefreshInputs() const
{
	return autoRefreshInputs_;
}

void VisualizerWidget::clearCurrentVisualizerWidget()
{
    if(visualizerWidget == nullptr || visualizerWidgetContainer == nullptr ){
        return;
    }

    //if(){
        //setWidget(nullptr);
		visualizerWidget->setVisible(false);
		visualizerWidgetContainer->layout()->removeWidget(visualizerWidget);
        visualizerWidget->setParent(nullptr);
        visualizerWidget = nullptr;
		setMinimumSize(0,0);
    //}
}

void VisualizerWidget::init()
{
	//wewnêtrzny widget - realizuje ramkê i padding
	visualizerWidgetContainer = new QWidget();
	visualizerWidgetContainer->setObjectName(QString::fromUtf8("visualizerContainer"));
	visualizerWidgetContainer->setLayout(new QHBoxLayout());
	visualizerWidgetContainer->layout()->setContentsMargins(2,2,2,2);
	setWidget(visualizerWidgetContainer);

    setFocusPolicy(Qt::StrongFocus);
    lastSerie.first = nullptr;    

    //labelka jako ikona z okiem
    label = new QLabel();
    label->setObjectName(QString::fromUtf8("label"));
    label->setPixmap(QPixmap(QString::fromUtf8(":/resources/icons/wizualizacja2.png")));

    visualizerCommonElements[label] = InnerVisualizerElement(true, label, IEDRTitleBar::Left);

    //combo do przechowywania typów wizualizatorów
    comboType = new QComboBox();
    comboType->setObjectName(QString::fromUtf8("comboType"));
    comboType->setInsertPolicy(QComboBox::InsertAlphabetically);

    visualizerCommonElements[comboType] = InnerVisualizerElement(true, comboType, IEDRTitleBar::Left);

    //ustawienie zdarzen na zmiane pozycji w combo (zmiana wizualizatora)
    connect(comboType, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentVisualizer(int)));

    //ustawienie domyslnej pozycji w combo
    comboType->setCurrentIndex(-1);

    //ustawienie menu odpowiedzialnego za wybor Ÿród³a danych
    menuSource = new QMenu(tr("Sources"), this);
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/resources/icons/dane.wejsciowe.png"), QSize(), QIcon::Normal, QIcon::Off);
    menuSource->setIcon(icon2);

    visualizerCommonElements[menuSource] = InnerVisualizerElement(true, menuSource, IEDRTitleBar::Left);

    //dynamiczne ³adowanie menu Ÿróde³ na ich rozwiniêcie
    connect(menuSource, SIGNAL(aboutToShow()), this, SLOT(fillSourcesMenu()));

    //wyczyszczenie wizualizatorow, inicjalizacja wszystkich kontrolek z tym zwiazanych
    clearCurrentVisualizer();

    ActionsGroupManager::GroupID id = visualizerCommonElementsOrder.createGroup("Decoration");
    visualizerCommonElementsOrder.addGroupAction(id, label);

    id = visualizerCommonElementsOrder.createGroup("Data");
    visualizerCommonElementsOrder.addGroupAction(id, comboType);
    visualizerCommonElementsOrder.addGroupAction(id, menuSource);

    //ALL old visualizer elements
    QMetaObject::connectSlotsByName(this);
}

void VisualizerWidget::setSourceVisible(bool visible)
{
    menuSource->setVisible(visible);
    visualizerCommonElements[menuSource].visible = visible;
}

bool VisualizerWidget::isSourceVisible() const
{
    return menuSource->isVisible();
}

void VisualizerWidget::setVisualizerIconVisible(bool visible)
{
    label->setVisible(visible);
    visualizerCommonElements[label].visible = visible;
}

bool VisualizerWidget::isVisualizerIconVisible() const
{
    return label->isVisible();
}

void VisualizerWidget::setVisualizerSwitchEnable(bool active)
{
    comboType->setEnabled(active);
}

bool VisualizerWidget::isVisualizerSwichEnable() const
{
    return comboType->isEnabled();
}

void VisualizerWidget::setVisualizerSwitchVisible(bool visible)
{
    comboType->setVisible(visible);
    visualizerCommonElements[comboType].visible = visible;
}

bool VisualizerWidget::isVisualizerSwichVisible() const
{
    return comboType->isVisible();
}

void VisualizerWidget::splitHorizontally()
{
    // to nie bug!
    split(Qt::Vertical);
}

void VisualizerWidget::splitVertically()
{
    // to nie bug!
    split(Qt::Horizontal);
}

void VisualizerWidget::split( Qt::Orientation orientation )
{
    VisualizerWidget* dockWidget;
    VisualizerPtr visualizer = getCurrentVisualizer();

    QMainWindow* mainWindow = qobject_cast<QMainWindow*>(parent());
    UTILS_ASSERT(mainWindow);

    if ( visualizer ) {
        dockWidget = new VisualizerWidget(VisualizerManager::getInstance()->createVisualizer(visualizer->getID()), mainWindow, windowFlags(), autoRefreshInputs_);
    } else {
        dockWidget = new VisualizerWidget(mainWindow, windowFlags(), autoRefreshInputs_);
    }

    mainWindow->splitDockWidget(this, dockWidget, orientation);
    dockWidget->setPermanent(false);
}

void VisualizerWidget::addVisualizer( const QString& label, UniqueID id )
{
    comboType->addItem( label, qVariantFromValue(id) );
}

void VisualizerWidget::addVisualizer( const QIcon& icon, const QString& label, UniqueID id )
{
    comboType->addItem( icon, label, qVariantFromValue(id) );
}

const ActionsGroupManager & VisualizerWidget::getVisualizerActionsManager() const
{
    return visualizerImplementationCustomElements;
}

const ActionsGroupManager & VisualizerWidget::getGenericVisualizerActionsManager() const
{
    return visualizerCommonElementsOrder;
}

void VisualizerWidget::getVisualizerTitleBarElements(VisualizerTitleBarElements & titleBarElements) const
{
    for(auto groupIT = visualizerCommonElementsOrder.begin(); groupIT != visualizerCommonElementsOrder.end(); groupIT++){
        
        std::map<int, QObject *> allGroupObjects;
        (*groupIT).getAllObjects(allGroupObjects);
        
        for(auto actionIT = allGroupObjects.begin(); actionIT != allGroupObjects.end(); actionIT++){
            auto elementIT = visualizerCommonElements.find(actionIT->second);
            if(elementIT != visualizerCommonElements.end()){
                if(elementIT->second.visible == true){
                    titleBarElements.push_back(VisualizerTitleBarElement(elementIT->second.object, elementIT->second.side));
                }
            }else{
                titleBarElements.push_back(VisualizerTitleBarElement(actionIT->second, IEDRTitleBar::Left));
            }
        }
    }

    for(auto groupIT = visualizerImplementationCustomElements.begin(); groupIT != visualizerImplementationCustomElements.end(); groupIT++){
        std::map<int, QObject *> allGroupObjects;
        (*groupIT).getAllObjects(allGroupObjects);

        for(auto actionIT = allGroupObjects.begin(); actionIT != allGroupObjects.end(); actionIT++){
            titleBarElements.push_back(VisualizerTitleBarElement(actionIT->second, IEDRTitleBar::Left));
        }
    }
}

void VisualizerWidget::clearCurrentVisualizer()
{
    if(visualizer == nullptr){
        return;
    }

	VisualizerManager::getInstance()->markAllChannelsAsRemovedFromVisualizer(visualizer.get());
	VisualizerManager::getInstance()->removeAllChannelsFromVisualizer(visualizer.get());
    
    //wyczyœæ menu wyboru Ÿróde³ i kana³ów
    clearSources();

    //usun wszystkie serie danych wizualizatora
    innerRemoveAllSeries();

    // usuniêcie wizualizatora
    visualizer.reset();

    //usuñ wizualizator widget z innerWidget!!
    clearCurrentVisualizerWidget();
}

void VisualizerWidget::clearDataSeries()
{
    currentSeriesData.swap(std::map<core::ObjectWrapperConstPtr, core::VisualizerSeriePtr >());
    groupedSeriesData.swap(std::map<core::TypeInfo, std::set<core::ObjectWrapperConstPtr> >());
}

void VisualizerWidget::setCurrentVisualizer( UniqueID id )
{
    if ( !visualizer || visualizer->getID() != id ) {
        setCurrentVisualizer( VisualizerManager::getInstance()->createVisualizer(id) );
    }
}

void VisualizerWidget::setCurrentVisualizer( const VisualizerPtr& visualizer )
{    
    if ( this->visualizer != visualizer ) {
        
        // wyczyszczenie aktuyalnego - zamieniamy go
        clearCurrentVisualizer();

        // pobranie nowego wizualizatora
        this->visualizer = visualizer;
        // dodanie g³ównego widgetu
        if ( visualizer ) {

			if(autoRefreshInputs_ == true){

				//ustaw dane
				for(int i = 0; i < visualizer->getNumInputs(); i++){
					bool exact = false;
					//pobieram dane

					//stworz nowy OWC, odswiezajacy dane z DM ObjectWrapperCollection
					core::ObjectWrapperCollectionPtr collection(new core::ObjectWrapperCollection(visualizer->getInputType(i), exact));

					DataManager::getInstance()->getObjects(*collection);

					visualizer->setObjects(i, collection);
				}
			}

            // czy indeks siê zgadza?
            // tradycyjne wyszukiwanie nie dzia³a przy copy by value
            int idx = -1;
            UniqueID id = visualizer->getID();
            for ( int i = 0; i < comboType->count(); ++i ) {
                if ( id == comboType->itemData(i).value<UniqueID>() ) {
                    idx = i;
                    break;
                }
            }
            UTILS_ASSERT(idx >= 0);
            comboType->setCurrentIndex(idx);

            visualizerWidget = visualizer->getOrCreateWidget();

            visualizerImplementationCustomElements = visualizer->getGenericActions();
            if(visualizerWidget != nullptr){
                //visualizerWidget->setObjectName(QString::fromUtf8("visualizerWidget"));
                //setWidget(visualizerWidget);
				visualizerWidgetContainer->layout()->addWidget(visualizerWidget);
				setMinimumSize(max(visualizerWidget->minimumWidth(), 50), max(visualizerWidget->minimumHeight(), 50));
            }else{
				setMinimumSize(0, 0);
			}

            setWindowTitle(visualizer->getUIName());

            MainWindow::getInstance()->setCurrentVisualizerActions(this);
        }
        else{
            setWindowTitle( "Empty visualizer" );
        }
    }
}

void VisualizerWidget::setCurrentVisualizer( int idx )
{
    if ( sender() != comboType ) {
        if ( comboType->currentIndex() != idx ) {
            // przekierowanie slotu
            comboType->setCurrentIndex(idx);
        }
    } else {
        if ( idx >= 0 ) {
            UniqueID id = comboType->itemData(idx).value<UniqueID>();
            setCurrentVisualizer(id);
        } else {
            setCurrentVisualizer( VisualizerPtr() );
        }
    }
}

void VisualizerWidget::removeAllVisualizers()
{
    while (comboType->count()) {
        comboType->removeItem(0);
    }
}

int VisualizerWidget::getNumVisualizers() const
{
    return comboType->count();
}

void VisualizerWidget::fillSourcesMenu()
{
    if ( visualizer == nullptr ) {
        return;
    }

    clearSources();

    //odbuduj menu Ÿróde³ danych
    //dostepne dane
    QMenu * avaiableDataMenu = menuSource->addMenu("Available data");
    //aktywne dane w seriach
    QMenu * activeDataMenu = menuSource->addMenu("Active data");
    activeDataMenu->setEnabled(visualizer->getDataSeries().empty() == false);

    // akcja zeruj¹ca obiekt - czyœci wszystkie serie danych
    actionNone = menuSource->addAction("none");
    actionNone->setCheckable(true);
    actionNone->setChecked(visualizer->getDataSeries().empty() == true ? true : false);
    if(actionNone->isChecked() == true){
        clearDataSeries();
        actionNone->setEnabled(false);
    }

    connect(actionNone, SIGNAL(triggered()), this, SLOT(removeAllSeries()) );

    std::set<core::ObjectWrapperConstPtr> currentData;
    std::map<core::TypeInfo, QMenu*> typeMenus;

    bool allNotInitialized = true;

    for(int i = 0; i < visualizer->getNumInputs(); i++){
        auto objects = visualizer->getObjects(i);
        if(objects != nullptr){
            int total = 0;
            int aditional = 0;

            QMenu * activeMenu = nullptr;
        
            auto iT = groupedSeriesData.find(objects->getTypeInfo());
            if(iT != groupedSeriesData.end()){
                aditional = iT->second.size();

                if(aditional > 0){
                    std::ostringstream str;
                    str << objects->getTypeInfo().name() << " [" << aditional << "]";
                    activeMenu = activeDataMenu->addMenu(str.str().c_str());
                }
            }

            std::ostringstream str;


            if(aditional > 0){
                std::set<core::ObjectWrapperConstPtr> current(objects->begin(), objects->end());
                std::vector<core::ObjectWrapperConstPtr> result((std::max)(static_cast<int>(objects->size()), aditional));

                auto stopIT = std::set_difference(iT->second.begin(), iT->second.end(), current.begin(), current.end(), result.begin());
                total = current.size();
                aditional = std::distance(result.begin(), stopIT);
            }

            //znajdŸ wspólne elementy
            //znajdŸ ró¿nicê

            total = objects->size() + aditional;
            str << objects->getTypeInfo().name() << " [" << total;

            if(aditional > 0){
                str << "(" << aditional << ")";
            }

            str << "]";

            if(total > 0){
                allNotInitialized = false;
            }

            QMenu* nestedMenu = avaiableDataMenu->addMenu(toQString(str.str()));
            nestedMenu->setEnabled(total > 0 ? true : false);

            typeMenus[objects->getTypeInfo()] = nestedMenu;

            for(auto it = objects->begin(); it != objects->end(); it++){
                if(currentData.insert(*it).second == true){

                    std::ostringstream str;
                    str << (*it)->getName() << " (from " << (*it)->getSource() << ")";

					std::string s(str.str());
					if(s.size() > 80){
						s.erase(0, s.size() - 80);
						s[0] = s[1] = s[2] = '.';
					}else if( s.size() < 80){
						s.insert(0, 80 - s.size(), ' ');
					}

                    QAction* action = nestedMenu->addAction(toQString(s));
                    connect(action, SIGNAL(triggered()), this, SLOT(sourceSelected()) );
                    action->setCheckable(true);
                    action->setData(qVariantFromValue(TypeData(objects->getTypeInfo(),*it)));

                    bool checked = currentSeriesData.find((*it)) == currentSeriesData.end() ? false : true;

                    action->setChecked( checked );
                    nestedMenu->addAction(action);
                }
            }

            if(iT != groupedSeriesData.end()){

                bool separator = false;

                for(auto it = iT->second.begin(); it != iT->second.end(); it++){

                    std::ostringstream str;
                    str << (*it)->getName() << " (from " << (*it)->getSource() << ")";

					std::string s(str.str());
					if(s.size() > 80){
						s.erase(0, s.size() - 80);
						s[0] = s[1] = s[2] = '.';
					}else if( s.size() < 80){
						s.insert(0, 80 - s.size(), ' ');
					}

                    if(currentData.find(*it) == currentData.end()){
                        if(separator == false){
                            separator = true;
                            if(objects->empty() == false){
                                nestedMenu->addSeparator();
                            }
                        }

                        
                        QAction* action = nestedMenu->addAction(toQString(s));
                        connect(action, SIGNAL(triggered()), this, SLOT(sourceSelected()) );
                        action->setCheckable(true);
                        action->setData(qVariantFromValue(TypeData(objects->getTypeInfo(),*it)));
                        action->setChecked( true );
                    }

                    QAction* action = activeMenu->addAction(toQString(s));
                    connect(action, SIGNAL(triggered()), this, SLOT(sourceSelected()) );
                    action->setCheckable(true);
                    action->setData(qVariantFromValue(TypeData(objects->getTypeInfo(),*it)));
                    action->setChecked( true );
                }
            }
        }
    }

    avaiableDataMenu->setEnabled( !(allNotInitialized == true && visualizer->getDataSeries().empty() == true) );

    if(menuSource->isVisible() == false){
        menuSource->show();
    }
}

void VisualizerWidget::removeAllSeries()
{
    if(actionNone->isChecked() == false){
        actionNone->blockSignals(true);
        actionNone->setChecked(true);
        actionNone->blockSignals(false);
    }else{
        innerRemoveAllSeries();
        actionNone->setEnabled(false);
    }
}

void VisualizerWidget::innerRemoveAllSeries()
{
    if(visualizer != nullptr){
        visualizer->clearAllSeries();
    }

    clearDataSeries();
}

void VisualizerWidget::sourceSelected()
{
    UTILS_ASSERT(visualizer);

    QAction* action = qobject_cast<QAction*>(sender());
    UTILS_ASSERT(action);

    TypeData td = action->data().value<TypeData>();

    if(action->isChecked() == true){

        actionNone->setEnabled(true);
        actionNone->blockSignals(true);
        actionNone->setChecked(false);
        actionNone->blockSignals(false);

        //czy mozna jeszcze utworzyc serie?
        if(visualizer->getMaxSeries() < 0 || visualizer->getMaxSeries() != currentSeriesData.size()){
            //aktualizujemy ostatnia serie danych

            lastSerie.first = action;
            lastSerie.second = td.second;

            //dodaj nowa seriê
            VisualizerSeriePtr serie(visualizer->createSerie(lastSerie.second, getLabel(lastSerie.second, true)));

            VisualizerTimeSeriePtr timeSerie(core::dynamic_pointer_cast<IVisualizer::TimeSerieBase>(serie));

            if(timeSerie != nullptr){
                const void * tChannel = VisualizerManager::getInstance()->createChannel(timeSerie, visualizer.get());
				if(tChannel != nullptr){
					timelineChannels[td.second] = tChannel;
				}
            }

            currentSeriesData[lastSerie.second] = serie;
            groupedSeriesData[td.first].insert(lastSerie.second);
        }else{
            //TODO
            //co jeœli mam ju¿ komplet? moge odznaczaæ serie, ale nie dodawaæ - mo¿e jakoœ kolorem menu?


            ////nie moge utworzyc serii - przelacz ostatnia serie
            //lastSerie.first->blockSignals(true);
            //lastSerie.first->setChecked(false);
            //lastSerie.first->blockSignals(false);

            //auto it = currentSeriesData.find(lastSerie.second);
            //VisualizerSeriePtr serie = it->second;
            //groupedSeriesData[lastSerie.first->data().value<TypeData>().first].erase(lastSerie.second);
            //currentSeriesData.erase(it);

            ////aktualizujemy ostatnia serie danych
            //lastSerie.first = action;
            //lastSerie.second = td.second;

            //serie->setData(lastSerie.second);
            //serie->setName(getLabel(lastSerie.second, true));

            //currentSeriesData[lastSerie.second] = serie;
            //groupedSeriesData[td.first].insert(lastSerie.second);
        }
    }else{
        //usuñ seriê
        auto it = currentSeriesData.find(td.second);

        groupedSeriesData[td.first].erase(it->first);
        if(groupedSeriesData[td.first].empty() == true){
            groupedSeriesData.erase(td.first);
        }

		auto tChannel = timelineChannels.find(td.second);
		if(tChannel != timelineChannels.end()){
			try{
				VisualizerManager::getInstance()->removeChannel(tChannel->second);
			}catch(...){

			}

			timelineChannels.erase(tChannel);
		}

        visualizer->removeSerie(it->second);

        currentSeriesData.erase(it);

        if(currentSeriesData.empty() == true){
            actionNone->setEnabled(false);
            actionNone->blockSignals(true);
            actionNone->setChecked(true);
            actionNone->blockSignals(false);
        }
    }

    visualizerWidget->update();
}

void VisualizerWidget::clearSources()
{
    menuSource->clear();
}

std::string VisualizerWidget::getLabel( const ObjectWrapperConstPtr& object, bool noSource )
{
    if ( !object ) {
        return std::string("none");
    } else {
        if ( noSource ) {
            return object->getName();
        } else {
            // TODO: uzupe³niæ
            return object->getName();
        }
    }
}