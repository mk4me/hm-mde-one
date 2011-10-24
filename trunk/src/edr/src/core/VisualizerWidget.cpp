#include "CorePCH.h"

#include <iterator>
#include <core/StringTools.h>
#include <boost/foreach.hpp>
#include "VisualizerWidget.h"
//#include "DataManager.h"
#include <core/DataAccessors.h>
#include "ui_VisualizerWidget.h"
#include "MainWindow.h"
#include <core/DataAccessors.h>
#include "ServiceManager.h"
#include <plugins/newTimeline/ITimelineService.h>

using namespace core;

typedef std::pair<core::TypeInfo, core::ObjectWrapperConstPtr> TypeData;

Q_DECLARE_METATYPE(TypeData);

VisualizerWidget::VisualizerWidget(QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/) : EDRDockWidget(parent, flags)
{
    init();
    // dodajemy wizualizatory
    BOOST_FOREACH(const IVisualizerConstPtr& vis, VisualizerManager::getInstance()->enumPrototypes()) {
        addVisualizer( VisualizerManager::getInstance()->getIcon(vis->getID()),  toQString(vis->getName()), vis->getID() );
    }
}

VisualizerWidget::VisualizerWidget( UniqueID visualizerID, QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/ ) : EDRDockWidget(parent, flags)
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

VisualizerWidget::VisualizerWidget( const VisualizerPtr& source, QWidget* parent /*= nullptr*/, Qt::WindowFlags flags /*= 0*/ ) : EDRDockWidget(parent, flags)
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
    clearCurrentVisualizer();
}

void VisualizerWidget::clearCurrentVisualizerWidget()
{
    if(visualizerWidget == nullptr){
        return;
    }

    //ustaw jego rodzica ale samego widgetu nie usuwaj!!
    QLayout * layout = getInnerWidget()->layoutContent;
    if(layout->count() > 0){
        QLayoutItem * item = layout->takeAt(layout->indexOf(visualizerWidget));
        if(item != nullptr){
            visualizerWidget->setParent(nullptr);
            visualizerWidget = nullptr;
            delete item;
        }
    }
}

void VisualizerWidget::init()
{
    lastSerie.first = nullptr;    

    EDRTitleBar * titleBar = getTitleBar();

    //labelka jako ikona z okiem
    label = new QLabel(titleBar);
    label->setObjectName(QString::fromUtf8("label"));
    label->setPixmap(QPixmap(QString::fromUtf8(":/resources/icons/wizualizacja2.png")));

    //combo do przechowywania typów wizualizatorów
    comboType = new QComboBox(titleBar);
    comboType->setObjectName(QString::fromUtf8("comboType"));
    comboType->setInsertPolicy(QComboBox::InsertAlphabetically);

    //ustawienie zdarzen na zmiane pozycji w combo (zmiana wizualizatora)
    connect(comboType, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentVisualizer(int)));

    //ustawienie domyslnej pozycji w combo
    comboType->setCurrentIndex(-1);

    //ustawienie menu odpowiedzialnego za wybor Ÿród³a danych
    menuSource = new QMenu(titleBar);

    buttonSource = new QToolButton(titleBar);
    buttonSource->setObjectName(QString::fromUtf8("buttonSource"));
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(buttonSource->sizePolicy().hasHeightForWidth());
    buttonSource->setSizePolicy(sizePolicy1);
    buttonSource->setMaximumSize(QSize(80, 22));
    buttonSource->setStyleSheet(QString::fromUtf8(""));
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/resources/icons/dane.wejsciowe.png"), QSize(), QIcon::Normal, QIcon::Off);
    buttonSource->setIcon(icon2);
    buttonSource->setPopupMode(QToolButton::InstantPopup);
    buttonSource->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    buttonSource->setArrowType(Qt::NoArrow);
    buttonSource->setText(QString::fromUtf8("Sources"));

    //dynamiczne ³adowanie menu Ÿróde³ na ich rozwiniêcie
    connect(menuSource, SIGNAL(aboutToShow()), this, SLOT(fillSourcesMenu()));

    //³¹czenie menu z przyciskami
    buttonSource->setMenu(menuSource);

    //wyczyszczenie wizualizatorow, inicjalizacja wszystkich kontrolek z tym zwiazanych
    clearCurrentVisualizer();

    //ustawienie akcji dziel¹cych wizualizator (tworzacych nowy po odpowiedniej stronie)
    actionSplitVertically = new QAction(titleBar);
    actionSplitVertically->setObjectName(QString::fromUtf8("actionSplitVertically"));
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/resources/icons/vertical.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionSplitVertically->setIcon(icon3);
    
    actionSplitHorizontally = new QAction(titleBar);
    actionSplitHorizontally->setObjectName(QString::fromUtf8("actionSplitHorizontally"));
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/resources/icons/horizontal.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionSplitHorizontally->setIcon(icon4);
        
    //ustawienie przyciskow odpowiedzialnych za akcje dizelenia wizualizatora
    buttonSplitV = new QToolButton(titleBar);
    buttonSplitV->setObjectName(QString::fromUtf8("buttonSplitV"));

    buttonSplitH = new QToolButton(titleBar);
    buttonSplitH->setObjectName(QString::fromUtf8("buttonSplitH"));
    
    buttonSplitH->setDefaultAction(actionSplitHorizontally);
    buttonSplitV->setDefaultAction(actionSplitVertically);    

    //dodanie stworzonych elementow do titleBara
    titleBar->addObject(label, IEDRTitleBar::Left);
    titleBar->addObject(comboType, IEDRTitleBar::Left);
    titleBar->addObject(buttonSource, IEDRTitleBar::Left);

    titleBar->addObject(buttonSplitH, IEDRTitleBar::Right);
    titleBar->addObject(buttonSplitV, IEDRTitleBar::Right);


    //inicjalizacja pozostalych sygnalow i slotow
    connect( this, SIGNAL(topLevelChanged(bool)), titleBar->actionFloat, SLOT(setChecked(bool)) );

    connect( actionSplitVertically, SIGNAL(triggered()), this, SLOT(splitVertically()));
    connect( actionSplitHorizontally, SIGNAL(triggered()), this, SLOT(splitHorizontally()));

    connect( this, SIGNAL(topLevelChanged(bool)), buttonSplitH, SLOT(setHidden(bool)));
    connect( this, SIGNAL(topLevelChanged(bool)), buttonSplitV, SLOT(setHidden(bool)));

    // niszczymy przy zamykaniu, nie chowamy!
    setPermanent(false);

    //ALL old visualizer elements

    retranslateUi(this);

    QMetaObject::connectSlotsByName(this);
}

void VisualizerWidget::setSourceVisible(bool visible)
{
    buttonSource->setVisible(visible);
}

bool VisualizerWidget::isSourceVisible() const
{
    return buttonSource->isVisible();
}

void VisualizerWidget::setSplitHVisible(bool visible)
{
    buttonSplitH->setVisible(visible);
}

bool VisualizerWidget::isSplitHVisible() const
{
    return buttonSplitH->isVisible();
}

void VisualizerWidget::setSplitVVisible(bool visible)
{
    buttonSplitV->setVisible(visible);
}

bool VisualizerWidget::isSplitVVisible() const
{
    return buttonSplitV->isVisible();
}

void VisualizerWidget::setVisualizerIconVisible(bool visible)
{
    label->setVisible(visible);
}

bool VisualizerWidget::isVisualizerIconVisible() const
{
    return label->isVisible();
}

void VisualizerWidget::setActiveVisualizerSwitch(bool active)
{
    comboType->setEnabled(active);
}

bool VisualizerWidget::isActiveVisualizerSwich() const
{
    return comboType->isEnabled();
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
        dockWidget = new VisualizerWidget(VisualizerManager::getInstance()->createVisualizer(visualizer->getID()), mainWindow);
    } else {
        dockWidget = new VisualizerWidget(mainWindow);
    }
    mainWindow->splitDockWidget(this, dockWidget, orientation);

}

void VisualizerWidget::retranslateUi(QWidget *visualizerWidget)
{
    actionSplitVertically->setText(QApplication::translate("VisualizerWidget", "Split vertically", 0, QApplication::UnicodeUTF8));
    actionSplitHorizontally->setText(QApplication::translate("VisualizerWidget", "Split horizontally", 0, QApplication::UnicodeUTF8));
    label->setText(QString());
    buttonSplitV->setText(QApplication::translate("VisualizerWidget", "split v", 0, QApplication::UnicodeUTF8));
    buttonSplitH->setText(QApplication::translate("VisualizerWidget", "split h", 0, QApplication::UnicodeUTF8));
    buttonSource->setText(QApplication::translate("VisualizerWidget", "Source", 0, QApplication::UnicodeUTF8));
} // retranslateUi

void VisualizerWidget::addVisualizer( const QString& label, UniqueID id )
{
    comboType->addItem( label, qVariantFromValue(id) );
}

void VisualizerWidget::addVisualizer( const QIcon& icon, const QString& label, UniqueID id )
{
    comboType->addItem( icon, label, qVariantFromValue(id) );
}

void VisualizerWidget::clearCurrentVisualizer()
{
    if(visualizer == nullptr){
        return;
    }
    
    //wyczyœæ menu wyboru Ÿróde³ i kana³ów
    clearSources();

    //usun wszystkie serie danych wizualizatora
    innerRemoveAllSeries();

    // usuniêcie wizualizatora
    visualizer.reset();

    // czyszczenie fragmentu z customowymi akcjami
    EDRTitleBar * titleBar = getTitleBar();
    BOOST_FOREACH(QObject* obj, visualizerCustomElements){
        titleBar->removeObject(obj);
    }

    visualizerCustomElements.swap(std::vector<QObject*>());

    //usuñ wizualizator widget z innerWidget!!
    clearCurrentVisualizerWidget();
}

void VisualizerWidget::clearDataSeries()
{
    if(timelineChannels.empty() == false){
        TimelinePtr timeline = core::queryServices<ITimelineService>(ServiceManager::getInstance());
        if(timeline != nullptr) {

            do{

                VisualizerChannelPtr channel = timelineChannels.begin()->second;

                try{
                    timeline->removeChannel(channel->getSerie()->getName());
                }catch(std::runtime_error e){
                    LOG_WARNING("Could not remove channel from timeline because: " << e.what());
                }catch(...){
                    LOG_WARNING("Could not remove channel from timeline. Unknown reason.");
                }

                channel->releaseChannel();

                timelineChannels.erase(timelineChannels.begin());
            }while(timelineChannels.empty() == false);
        }

        timelineChannels.swap(TimelineChannels());
    }

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

            //ustaw dane
            for(int i = 0; i < visualizer->getNumInputs(); i++){
                bool exact = false;
                //pobieram dane
                //std::vector<core::ObjectWrapperPtr> dmData;
                //DataManager::getInstance()->getObjects(dmData, visualizer->getInputType(i), exact);
                //core::Objects dmData(core::queryData(visualizer->getInputType(i), exact));
                
                //core::Objects dmData;

                //stworz nowy OWC, odswiezajacy dane z DM ObjectWrapperCollection
                core::ObjectWrapperCollectionPtr collection(new core::ObjectWrapperCollection(visualizer->getInputType(i), exact));

                //for(auto it = dmData.begin(); it != dmData.end(); it++){
                //    collection->addObject(*it);
                //}

                core::queryData(*collection);

                visualizer->setObjects(i, collection);
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

            EDRTitleBar * titleBar = getTitleBar();

            visualizerCustomElements = visualizer->getOrCreateGenericActions();

            // dodajemy akcje ogólne widgetu
            BOOST_FOREACH(QObject* obj, visualizerCustomElements) {
                titleBar->addObject(obj, IEDRTitleBar::Left);
            }

            EDRDockInnerWidget * innerWidget = getInnerWidget();
			/*std::string n = visualizer->getName();
			if (n == "Video") {
				innerWidget->setFixedSize(600,400);
			}*/
            visualizerWidget = visualizer->getOrCreateWidget();

            if(visualizerWidget != nullptr){
                innerWidget->layoutContent->addWidget(visualizerWidget);
            }

            innerWidget->setWindowTitle( visualizer->getUIName() );
            setWindowTitle( innerWidget->windowTitle() );
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
    //activeDataMenu->setEnabled(currentSeriesData.empty() == false);
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
                std::vector<core::ObjectWrapperConstPtr> result(std::max(static_cast<int>(objects->size()), aditional));

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
                    QAction* action = nestedMenu->addAction(toQString(str.str()));
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

                    if(currentData.find(*it) == currentData.end()){
                        if(separator == false){
                            separator = true;
                            if(objects->empty() == false){
                                nestedMenu->addSeparator();
                            }
                        }

                        
                        QAction* action = nestedMenu->addAction(toQString(str.str()));
                        connect(action, SIGNAL(triggered()), this, SLOT(sourceSelected()) );
                        action->setCheckable(true);
                        action->setData(qVariantFromValue(TypeData(objects->getTypeInfo(),*it)));
                        action->setChecked( true );
                    }

                    QAction* action = activeMenu->addAction(toQString(str.str()));
                    connect(action, SIGNAL(triggered()), this, SLOT(sourceSelected()) );
                    action->setCheckable(true);
                    action->setData(qVariantFromValue(TypeData(objects->getTypeInfo(),*it)));
                    action->setChecked( true );
                }
            }
        }
    }

    avaiableDataMenu->setEnabled( !(allNotInitialized == true && visualizer->getDataSeries().empty() == true) );

    if(buttonSource->isVisible() == false){
        buttonSource->show();
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
                TimelinePtr timeline = core::queryServices<ITimelineService>(ServiceManager::getInstance());
                if(timeline != nullptr) {
                    
                    VisualizerChannelPtr channel(new VisualizerChannel(timeSerie, this));

                    try{
                        timeline->addChannel(timeSerie->getName(), channel);
                        timelineChannels[timeSerie] = channel;
                    }catch(std::runtime_error e){
                        LOG_WARNING("Could not add channel to timeline because: " << e.what());
                    }catch(...){
                        LOG_WARNING("Could not add channel to timeline. Unknown reason.");
                    }
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

        visualizer->removeSerie(it->second);

        VisualizerTimeSeriePtr timeSerie(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(it->second));

        if(timeSerie != nullptr){

            auto iT = timelineChannels.find(timeSerie);

            if(iT != timelineChannels.end()){
                //usun kanal z timeline
                TimelinePtr timeline = core::queryServices<ITimelineService>(ServiceManager::getInstance());
                if(timeline != nullptr) {
                    try{
                        timeline->removeChannel(iT->first->getName());
                    }catch(std::runtime_error e){
                        LOG_WARNING("Could not remove channel from timeline because: " << e.what());
                    }catch(...){
                        LOG_WARNING("Could not remove channel from timeline. Unknown reason.");
                    }
                }

                iT->second->releaseChannel();

                timelineChannels.erase(iT);
            }
        }

        currentSeriesData.erase(it);

        if(currentSeriesData.empty() == true){
            actionNone->setEnabled(false);
            actionNone->blockSignals(true);
            actionNone->setChecked(true);
            actionNone->blockSignals(false);
        }
    }

    //setPermanent(visualizer->getTimelineDataSeries().empty() == false);

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