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
    EDRTitleBar * titleBar = getTitleBar();

    //labelka jako ikona z okiem
    label = new QLabel(titleBar);
    label->setObjectName(QString::fromUtf8("label"));
    label->setPixmap(QPixmap(QString::fromUtf8(":/data/resources/icons/wizualizacja2.png")));

    //combo do przechowywania typów wizualizatorów
    comboType = new QComboBox(titleBar);
    comboType->setObjectName(QString::fromUtf8("comboType"));
    comboType->setInsertPolicy(QComboBox::InsertAlphabetically);

    //ustawienie zdarzen na zmiane pozycji w combo (zmiana wizualizatora)
    connect(comboType, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentVisualizer(int)));

    //ustawienie domyslnej pozycji w combo
    comboType->setCurrentIndex(-1);

    //ustawienie menu odpowiedzialnego za wybor Ÿród³a danych
    groupSources = new QActionGroup(titleBar);
    menuSource = new QMenu(titleBar);

    //ustawienie menu odpowiedzialnego za wybór kana³u Ÿród³a danych
    groupMenuPin = new QActionGroup(titleBar);
    menuPin = new QMenu(titleBar);
    
    //ustawienie domyslnego kana³u Ÿród³a danych
    currentSlot = -1;

    //ustawienie przyciskow obslugujacych menu wyboru Ÿróde³
    buttonPin = new QToolButton(titleBar);
    buttonPin->setObjectName(QString::fromUtf8("buttonPin"));
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/data/resources/icons/wejscie2.png"), QSize(), QIcon::Normal, QIcon::Off);
    buttonPin->setIcon(icon1);
    buttonPin->setPopupMode(QToolButton::InstantPopup);
    buttonPin->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

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
    icon2.addFile(QString::fromUtf8(":/data/resources/icons/dane.wejsciowe.png"), QSize(), QIcon::Normal, QIcon::Off);
    buttonSource->setIcon(icon2);
    buttonSource->setPopupMode(QToolButton::InstantPopup);
    buttonSource->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    buttonSource->setArrowType(Qt::NoArrow);

    //dynamiczne ³adowanie menu Ÿróde³ na ich rozwiniêcie
    connect(menuSource, SIGNAL(aboutToShow()), this, SLOT(fillSourcesMenu()));

    //³¹czenie menu z przyciskami
    buttonSource->setMenu(menuSource);
    buttonPin->setMenu(menuPin);

    //wyczyszczenie wizualizatorow, inicjalizacja wszystkich kontrolek z tym zwiazanych
    clearCurrentVisualizer();

    //ustawienie akcji dziel¹cych wizualizator (tworzacych nowy po odpowiedniej stronie)
    actionSplitVertically = new QAction(titleBar);
    actionSplitVertically->setObjectName(QString::fromUtf8("actionSplitVertically"));
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/data/resources/icons/vertical.png"), QSize(), QIcon::Normal, QIcon::Off);
    actionSplitVertically->setIcon(icon3);
    
    actionSplitHorizontally = new QAction(titleBar);
    actionSplitHorizontally->setObjectName(QString::fromUtf8("actionSplitHorizontally"));
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/data/resources/icons/horizontal.png"), QSize(), QIcon::Normal, QIcon::Off);
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
    titleBar->addObject(buttonPin, IEDRTitleBar::SideType::Left);
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
    buttonPin->setText(QApplication::translate("VisualizerWidget", "pin", 0, QApplication::UnicodeUTF8));
    buttonSource->setText(QApplication::translate("VisualizerWidget", "source", 0, QApplication::UnicodeUTF8));
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
    clearVisualizerSources();

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

            for ( int i = 0; i < visualizer->getNumObjects(); ++i ) {
                QAction* action = menuPin->addAction( QString("%0 (%1)")
                    .arg(core::toQString(visualizer->getInputName(i)))
                    .arg(getLabel(visualizer->getObject(i), true)), this, SLOT(pinSelected()) );
                action->setData(qVariantFromValue(i));
                action->setCheckable(true);
                if ( i == 0 ) {
                    action->setChecked(true);
                }
                groupMenuPin->addAction(action);
            }

            if ( visualizer->getNumObjects() ) {
                setCurrentSlot(0);
            } else {
                setCurrentSlot(-1);
            }
            EDRInnerWidget * innerWidget = getInnerWidget();
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

    int slotNo = getCurrentSlot();

    // wspierane obiekty w tym slocie
    std::list<TypeInfo> supportedTypes = visualizer->getInputTypes(slotNo);
    UTILS_ASSERT(!supportedTypes.empty());

    // pobranie obiektu w tym slocie
    const core::ObjectWrapperConstPtr& object = visualizer->getObject(slotNo);

    // akcja zeruj¹ca obiekt
    QAction* actionNone = menuSource->addAction("none");
    connect(actionNone, SIGNAL(triggered()), this, SLOT(sourceSelected()) );
    actionNone->setCheckable(true);
    actionNone->setData(qVariantFromValue(ObjectWrapperConstPtr()));
    actionNone->setChecked(!object);
    groupSources->addAction(actionNone);

    bool nested = true;// (++supportedTypes.begin() != supportedTypes.end());

    // dodanie submenu z akcjami wybieraj¹cymi konkretne obiekty
    BOOST_FOREACH(TypeInfo type, supportedTypes) {
        // pobranie obiektów z dataManagera
        // TODO: bardziej generyczne pobranie obiektów
        LOG_DEBUG("TypeInfo " << type.name() << " at slot " << slotNo);
        std::vector<ObjectWrapperPtr> objects;
        DataManager::getInstance()->getObjects(objects, type);

        std::ostringstream str;
        str << type.name() << " [" << objects.size() << "]";

        if ( objects.empty() ) {
            QAction* action = menuSource->addAction(toQString(str.str()));
            action->setEnabled(false);
        } else {
            QMenu* nestedMenu = nested ? menuSource->addMenu(toQString(str.str())) : menuSource;
            BOOST_FOREACH(ObjectWrapperPtr ptr, objects) {
                //titleBar->comboData->addItem( toQString(ptr->getName()) );
                std::ostringstream str;
                str << ptr->getName() << " (from " << ptr->getSource() << ")";
                QAction* action = nestedMenu->addAction(toQString(str.str()));
                connect(action, SIGNAL(triggered()), this, SLOT(sourceSelected()) );
                //connect(action, SIGNAL(triggered()), mapper, SLOT(map()));
                action->setCheckable(true);
                action->setData(qVariantFromValue(ObjectWrapperConstPtr(ptr)));
                action->setChecked( ptr == object );
                groupSources->addAction(action);
            }
        }
    }

    if(buttonSource->isVisible() == false){
        buttonSource->show();
    }
}

void VisualizerWidget::sourceSelected()
{
    UTILS_ASSERT(visualizer);

    QAction* action = qobject_cast<QAction*>(sender());
    UTILS_ASSERT(action);

    int slotNo = getCurrentSlot();
    UTILS_ASSERT(slotNo >= 0);
    ObjectWrapperConstPtr obj = action->data().value<ObjectWrapperConstPtr>();

    QString objName = getLabel(obj, true);
    QString slotName = toQString(visualizer->getInputName(slotNo));

    // ustawienie wygl¹du guzika
    buttonSource->setText(objName);

    menuPin->actions()[slotNo]->setText( QString("%0 (%1)").arg(slotName).arg(objName) );

    // ustawienie wizualizatora
    visualizer->setObject(slotNo, obj);
    visualizer->trySetUp();
}

int VisualizerWidget::getCurrentSlot() const
{
    return currentSlot;
}

void VisualizerWidget::clearSources()
{
    delete groupSources;
    menuSource->clear();
    groupSources = new QActionGroup(this);

    setCurrentSource(-1);
}

void VisualizerWidget::clearSlots()
{
    delete groupMenuPin;
    menuPin->clear();
    groupMenuPin = new QActionGroup(this);

    setCurrentSlot(-1);
}

void VisualizerWidget::clearVisualizerSources()
{
    //usun i wyczysc poprzednie elementy ze Ÿróde³
    clearSources();

    //usun i wyczysc poprzednie elementy ze slotów
    clearSlots();

}

void VisualizerWidget::setCurrentSource( int idx )
{
    if ( idx < 0 ) {
        buttonSource->hide();
    } else {

        UTILS_ASSERT(visualizer);

        QAction* action = qobject_cast<QAction*>(sender());
        UTILS_ASSERT(action);

        ObjectWrapperConstPtr obj = action->data().value<ObjectWrapperConstPtr>();

        QString objName = getLabel(obj, true);

        // ustawienie wygl¹du guzika
        buttonSource->setText(objName);

        if(buttonSource->isVisible() == false){
            buttonSource->show();
        }
    }
}

void VisualizerWidget::setCurrentSlot( int idx )
{
    currentSlot = idx;
    if ( idx < 0 ) {
        buttonSource->hide();
    } else {
        UTILS_ASSERT(visualizer);
        ObjectWrapperConstPtr ptr = visualizer->getObject(idx);
        buttonPin->setText(toQString(visualizer->getInputName(idx)));
        buttonSource->setText( getLabel(ptr, true) );
        buttonSource->show();
        buttonPin->show();
    }
}

void VisualizerWidget::pinSelected()
{
    QAction* action = qobject_cast<QAction*>(sender());
    int idx = action->data().value<int>();
    setCurrentSlot(idx);
}

QString VisualizerWidget::getLabel( const core::ObjectWrapperConstPtr& object, bool noSource )
{
    if ( !object ) {
        return QString("none");
    } else {
        if ( noSource ) {
            return toQString(object->getName());
        } else {
            // TODO: uzupe³niæ
            return toQString(object->getName());
        }
    }
}