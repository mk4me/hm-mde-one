#include "CorePCH.h"
#include <core/StringTools.h>
#include <boost/foreach.hpp>
#include "VisualizerWidget.h"
#include "DataManager.h"
#include "ui_VisualizerWidget.h"
#include "ToolboxMain.h"

using namespace core;

Q_DECLARE_METATYPE(ObjectWrapperConstPtr);

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
    titleBar->addObject(label, IEDRTitleBar::SideType::Left);
    titleBar->addObject(comboType, IEDRTitleBar::SideType::Left);
    titleBar->addObject(buttonSource, IEDRTitleBar::SideType::Left);

    titleBar->addObject(buttonSplitH, IEDRTitleBar::SideType::Right);
    titleBar->addObject(buttonSplitV, IEDRTitleBar::SideType::Right);


    //inicjalizacja pozostalych sygnalow i slotow
    connect( this, SIGNAL(topLevelChanged(bool)), titleBar->actionFloat, SLOT(setChecked(bool)) );

    connect( actionSplitVertically, SIGNAL(triggered()), this, SLOT(splitVertically()));
    connect( actionSplitHorizontally, SIGNAL(triggered()), this, SLOT(splitHorizontally()));

    connect( this, SIGNAL(topLevelChanged(bool)), buttonSplitH, SLOT(setHidden(bool)));
    connect( this, SIGNAL(topLevelChanged(bool)), buttonSplitV, SLOT(setHidden(bool)));

    // niszczymy przy zamykaniu, nie chowamy!
    setAttribute(Qt::WA_DeleteOnClose, true);

    //ALL old visualizer elements

    retranslateUi(this);

    QMetaObject::connectSlotsByName(this);
}

void VisualizerWidget::setActiveVisualizersSwitch(bool active)
{
    comboType->setEnabled(active);
}

bool VisualizerWidget::getActiveVisualizersSwitch() const
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

void VisualizerWidget::closeEvent( QCloseEvent *event )
{
    ToolboxMain* mainwnd = qobject_cast<ToolboxMain*>(this->parentWidget());
    UTILS_ASSERT(mainwnd);
    if ( mainwnd ) {
        mainwnd->visualizerWidgetClosed(this);
    }
    QDockWidget::closeEvent(event);
}

void VisualizerWidget::retranslateUi(QWidget *visualizerWidget)
{
    actionSplitVertically->setText(QApplication::translate("VisualizerWidget", "Split vertically", 0, QApplication::UnicodeUTF8));
    actionSplitHorizontally->setText(QApplication::translate("VisualizerWidget", "Split horizontally", 0, QApplication::UnicodeUTF8));
    label->setText(QString());
    buttonSplitV->setText(QApplication::translate("VisualizerWidget", "split v", 0, QApplication::UnicodeUTF8));
    buttonSplitH->setText(QApplication::translate("VisualizerWidget", "split h", 0, QApplication::UnicodeUTF8));
//    buttonPin->setText(QApplication::translate("VisualizerWidget", "pin", 0, QApplication::UnicodeUTF8));
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
    currentSeriesData.swap(std::map<core::ObjectWrapperConstPtr, core::VisualizerSeriePtr >());
    groupedSeriesData.swap(std::map<core::TypeInfo, std::set<core::ObjectWrapperConstPtr> >());
}

void VisualizerWidget::setCurrentVisualizer( UniqueID id )
{
    if ( !visualizer || visualizer->getID() != id ) {
        setCurrentVisualizer( static_cast<VisualizerManager*>(getVisualizerManager())->createVisualizer(id) );
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
                //stworz nowy, odswiezajacy dane z DM ObjectWrapperCollection
                core::ObjectWrapperCollectionPtr collection(new core::ObjectWrapperCollection(visualizer->getInputType(i), false));
                std::vector<core::ObjectWrapperPtr> dmData;
                DataManager::getInstance()->getObjects(dmData, collection->getTypeInfo());

                for(auto it = dmData.begin(); it != dmData.end(); it++){
                    collection->addObject(*it);
                }

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
                titleBar->addObject(obj, IEDRTitleBar::SideType::Left);
            }

            EDRDockInnerWidget * innerWidget = getInnerWidget();
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
    // akcja zeruj¹ca obiekt - czyœci wszystkie serie danych
    actionNone = menuSource->addAction("none");
    actionNone->setCheckable(true);
    actionNone->setChecked(currentSeriesData.empty() == true ? true : false);
    if(actionNone->isChecked() == true){
        actionNone->setEnabled(false);
    }

    connect(actionNone, SIGNAL(triggered()), this, SLOT(removeAllSeries()) );

    menuSource->addAction(actionNone);   

    std::map<core::TypeInfo, QMenu*> dataTypeMenus;

    for(int i = 0; i < visualizer->getNumInputs(); i++){
        ObjectWrapperCollectionConstPtr objects = visualizer->getObjects(i);
        if(objects != nullptr){
            std::ostringstream str;
        
            int aditional = 0;
            auto it = groupedSeriesData.find(objects->getTypeInfo());
            if(it != groupedSeriesData.end()){
                aditional = it->second.size();
            }

            int total = objects->size() + aditional;
            str << objects->getTypeInfo().name() << " [" << total;

            if(aditional > 0){
                str << "(" << aditional << ")";
            }

            str << "]";

            QMenu* nestedMenu = menuSource->addMenu(toQString(str.str()));
            nestedMenu->setEnabled(total > 0 ? true : false);
            dataTypeMenus[objects->getTypeInfo()] = nestedMenu;
        }
    }

    //uzupelnij brakujace typy z serii danych aktualnie wybranych
    for(auto it = groupedSeriesData.begin(); it != groupedSeriesData.end(); it++){
        if(dataTypeMenus.find(it->first) == dataTypeMenus.end()){
            std::ostringstream str;
            str << it->first.name() << " [" << it->second.size() << "]";

            QMenu* nestedMenu = menuSource->addMenu(toQString(str.str()));
            nestedMenu->setEnabled(true);
            dataTypeMenus[it->first] = nestedMenu;
        }
    }

    std::set<core::ObjectWrapperConstPtr> currentData;

    //uzupelniaj menu danymi
    //najpierw dane podpiete na wejsciu
    for(int i = 0; i < visualizer->getNumInputs(); i++){
        ObjectWrapperCollectionConstPtr objects = visualizer->getObjects(i);
        if(objects != nullptr && objects->empty() == false){            
            QMenu* nestedMenu = dataTypeMenus.find(objects->getTypeInfo())->second;             

            for(int j = 0; j < objects->size(); j++){
                ObjectWrapperConstPtr object = objects->getObject(j);

                currentData.insert(object);

                std::ostringstream str;
                str << object->getName() << " (from " << object->getSource() << ")";
                QAction* action = nestedMenu->addAction(toQString(str.str()));
                connect(action, SIGNAL(triggered()), this, SLOT(sourceSelected()) );
                action->setCheckable(true);
                action->setData(qVariantFromValue(object));

                bool checked = currentSeriesData.find(object) == currentSeriesData.end() ? false : true;

                action->setChecked( checked );
                nestedMenu->addAction(action);
            }
        }
    }

    //teraz dane z serii danych, których nie ma w danych wejsciowych ale s¹ trzymane przez serie danych
    for(auto it = groupedSeriesData.begin(); it != groupedSeriesData.end(); it++){
        QMenu* nestedMenu = dataTypeMenus.find(it->first)->second;
        
        if(nestedMenu->actions().empty() == false){
            nestedMenu->addSeparator()->setText("Expired data");
        }
        
        for(auto iT = it->second.begin(); iT != it->second.end(); iT++){
            std::ostringstream str;
            str << (*iT)->getName() << " (from " << (*iT)->getSource() << ")";
            QAction* action = nestedMenu->addAction(toQString(str.str()));
            connect(action, SIGNAL(triggered()), this, SLOT(sourceSelected()) );
            action->setCheckable(true);
            action->setData(qVariantFromValue(*iT));
            action->setChecked( true );
            nestedMenu->addAction(action);
        }
    }

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
        for(auto it = currentSeriesData.begin(); it != currentSeriesData.end(); it++){
            visualizer->removeSerie(it->second);
        }
    }

    clearDataSeries();
}

void VisualizerWidget::sourceSelected()
{
    UTILS_ASSERT(visualizer);

    QAction* action = qobject_cast<QAction*>(sender());
    UTILS_ASSERT(action);

    if(action->isChecked() == true){

        actionNone->setEnabled(true);
        actionNone->blockSignals(true);
        actionNone->setChecked(false);
        actionNone->blockSignals(false);

        //czy mozna jeszcze utworzyc serie?
        if(visualizer->getMaxSeries() < 0 || visualizer->getMaxSeries() != currentSeriesData.size()){
            //aktualizujemy ostatnia serie danych
            lastSerie.first = action;
            lastSerie.second = action->data().value<ObjectWrapperConstPtr>();

            //dodaj nowa seriê
            VisualizerSeriePtr serie(visualizer->createSerie(lastSerie.second, getLabel(lastSerie.second, true)));

            currentSeriesData[lastSerie.second] = serie;
            groupedSeriesData[lastSerie.second->getTypeInfo()].insert(lastSerie.second);
        }else{
            //nie moge utworzyc serii - przelacz ostatnia serie
            lastSerie.first->blockSignals(true);
            lastSerie.first->setChecked(false);
            lastSerie.first->blockSignals(false);

            auto it = currentSeriesData.find(lastSerie.second);
            VisualizerSeriePtr serie = it->second;
            groupedSeriesData[lastSerie.second->getTypeInfo()].erase(lastSerie.second);
            currentSeriesData.erase(it);

            //aktualizujemy ostatnia serie danych
            lastSerie.first = action;
            lastSerie.second = action->data().value<ObjectWrapperConstPtr>();

            serie->setData(lastSerie.second);
            serie->setName(getLabel(lastSerie.second, true));

            currentSeriesData[lastSerie.second] = serie;
            groupedSeriesData[lastSerie.second->getTypeInfo()].insert(lastSerie.second);
        }
    }else{
        //usuñ seriê
        auto it = currentSeriesData.find(action->data().value<ObjectWrapperConstPtr>());

        groupedSeriesData[it->first->getTypeInfo()].erase(it->first);
        if(groupedSeriesData[it->first->getTypeInfo()].empty() == true){
            groupedSeriesData.erase(it->first->getTypeInfo());
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