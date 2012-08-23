#include "TimelinePCH.h"

#include <QtGui/QIcon>
#include <QtGui/QCheckBox>

#include <math.h>
//#include <qwt/qwt_scale_draw.h>
#include "TimelineWidget.h"
#include "ChannelCheckbox.h"
#include "ChannelWidget.h"
#include <timelinelib/Controller.h>


TimelineWidget::TimelineWidget(const timeline::ControllerPtr & controller, QWidget * parent, Qt::WindowFlags f)
    : QWidget(parent, f), removeChannelsMenu(new QMenu()), /*scaleSpinBox(new QDoubleSpinBox()),*/
    playbackDirectionAction(new QAction(QString("Playback direction"), this)), timeToBeginAction(new QAction(QString("Begin"), this)),
    timeToEndAction(new QAction(QString("End"), this)), playPauseAction(new QAction(QString("Play"), this)),
    stopAction(new QAction(QString("Stop"), this)), /*scaleLabel(new QLabel(QString("<font color=\"white\"><b>Scale:</b></font>"))),
    timeLabel(new QLabel(QString("<font color=\"white\"><b>Time:</b></font>"))),*/ rootItem(new QTreeWidgetItem()),
    slider(new TimeSliderWidget()), preciseTimeWidget(new QDateTimeEdit()), timelineTabs(new QWidget()),
    leftTabButton(new QToolBar()), middleTabButton(new QToolBar()), rightTabButton(new QToolBar())
{
    //ustawienie kontrolera
    setController(controller);

    //inicjalizujemy widok drzewa

    //rejestracja obserwacji kontrolera
    controller->attach(this);

    //unicjalizacja UI

    //elementy Toolbara!!

    //ustaw akcje odtwarzania

    //TabWidget z timeline

    leftTabButton->setIconSize(QSize(16,15));
    leftTabButton->setContentsMargins(0,0,0,0);
    middleTabButton->setIconSize(QSize(22,15));;
    middleTabButton->setContentsMargins(0,0,0,0);
    rightTabButton->setIconSize(QSize(16,15));
    rightTabButton->setContentsMargins(0,0,0,0);

    leftTabButton->setFixedHeight(22);
    middleTabButton->setFixedHeight(22);;
    rightTabButton->setFixedHeight(22);

    leftTabButton->setObjectName(QString::fromUtf8("leftControls"));
    middleTabButton->setObjectName(QString::fromUtf8("centerControls"));
    rightTabButton->setObjectName(QString::fromUtf8("rightControls"));

    timelineTabs->setObjectName("timelineGroupedControls");

    //LEWY TAB
    preciseTimeWidget->setDisplayFormat(QString::fromUtf8("mm:ss:zzz"));
    preciseTimeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QIcon icon0;
    icon0.addFile(QString::fromUtf8(":/resources/icons/direction.png"), QSize(), QIcon::Normal, QIcon::Off);
    icon0.addFile(QString::fromUtf8(":/resources/icons/direction2.png"), QSize(), QIcon::Normal, QIcon::On);
    playbackDirectionAction->setIcon(icon0);
    playbackDirectionAction->setCheckable(true);

    leftTabButton->addWidget(preciseTimeWidget);
    leftTabButton->addAction(playbackDirectionAction);

    //SRODKOWY-GLOWNY TAB
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/resources/icons/rew.png"), QSize(), QIcon::Normal, QIcon::Off);
    timeToBeginAction->setIcon(icon1);

    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/resources/icons/forw.png"), QSize(), QIcon::Normal, QIcon::Off);
    timeToEndAction->setIcon(icon2);

    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/resources/icons/play.png"), QSize(), QIcon::Normal, QIcon::Off);
    icon3.addFile(QString::fromUtf8(":/resources/icons/pause.png"), QSize(), QIcon::Normal, QIcon::On);
    playPauseAction->setIcon(icon3);
    playPauseAction->setCheckable(true);
    playPauseAction->setEnabled(false);

    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/resources/icons/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
    stopAction->setIcon(icon4);
    stopAction->setEnabled(false);

    middleTabButton->addAction(timeToBeginAction);
    middleTabButton->addAction(playPauseAction);
    middleTabButton->addAction(stopAction);
    middleTabButton->addAction(timeToEndAction);

    //TODO
    //PRAWY TAB = aktualnie pusty!!

    // toBegin action
    connect(timeToBeginAction, SIGNAL(triggered()), this, SLOT(toBegin()));

    // toEnd action
    connect(timeToEndAction, SIGNAL(triggered()), this, SLOT(toEnd()));

    //play | pause action
    connect(playPauseAction, SIGNAL(triggered(bool)), this, SLOT(pause(bool)));

    // stop action
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stop()));

    // edycja tekstowa czasu
    connect(preciseTimeWidget, SIGNAL(timeChanged(QTime)), this, SLOT(timeChanged(QTime)));

    //skala
    //scaleSpinBox->setDecimals(2);
    //scaleSpinBox->setValue(1);
    //scaleSpinBox->setMinimum(0.01);
    //scaleSpinBox->setMaximum(100);
    //scaleSpinBox->setSingleStep(0.2);

    //po³¹czenie sygna³ów i slotów
    //connect(scaleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(scaleChanged(double)));

    ////po³¹czenie sygna³ów i slotów
    connect(playbackDirectionAction, SIGNAL(triggered(bool)), this, SLOT(playbackDirectionChanged(bool)));

    //elementy widgeta

    setupUi(this);

    QStringList headers;
    headers << tr("Channel") << tr("Active") << tr("Time");
    channelsWidget->setHeaderLabels(headers);

    rootItem->setFlags(Qt::NoItemFlags /* | Qt::ItemIsEnabled*/);

    channelsWidget->addTopLevelItem(rootItem);

    rootItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);

    //nowy slider

    // rev - zmieni³o siê troche API, nie ma ju¿ TopScale
    //slider->setScalePosition(QwtSlider::TopScale);
    slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slider->setHandleSize(QSize(45,10));
    slider->setSpacing(-6);
	//hack bo aktualnie to jest ukryte w klasie slidera
    QwtScaleDraw * scaleDraw = const_cast<QwtScaleDraw*>(slider->scaleDraw());
    scaleDraw->setTickLength(QwtScaleDiv::MinorTick, slider->scaleDraw()->tickLength(QwtScaleDiv::MinorTick) + 4);
    scaleDraw->setTickLength(QwtScaleDiv::MajorTick, slider->scaleDraw()->tickLength(QwtScaleDiv::MajorTick) + 4);
    scaleDraw->setTickLength(QwtScaleDiv::MediumTick, slider->scaleDraw()->tickLength(QwtScaleDiv::MediumTick) + 4);

    connect(slider, SIGNAL(valueChanged(double)), this, SLOT(timeSliderChanged(double)));

    //channelsWidget->setItemWidget(rootItem, 2, slider);
    //slider->setFixedHeight(slider->height() + 6);

    channelsWidget->header()->setMinimumSectionSize(14);
    channelsWidget->header()->resizeSection(1, 14);
    channelsWidget->header()->setResizeMode(1, QHeaderView::Fixed);
    channelsWidget->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    channelsWidget->header()->hide();

    // menu kontekstowe dla drzewa kana³ów z akcj¹ usuwaj¹c¹ zaznaczone kana³y
    removeChannelsMenu->addAction(actionRemoveChannels);

    QHBoxLayout * layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,8);
    //left side
    QWidget * w = new QWidget();
    w->setObjectName(QString::fromUtf8("leftContainer"));
    QHBoxLayout * l = new QHBoxLayout();
    l->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));
    l->addWidget(leftTabButton);
    w->setLayout(l);
    layout->addWidget(w);

    //center
    w = new QWidget();
    w->setObjectName(QString::fromUtf8("centerContainer"));
    l = new QHBoxLayout();
    l->addWidget(middleTabButton);
    w->setLayout(l);
    layout->addWidget(w);

    //right side
    w = new QWidget();
    w->setObjectName(QString::fromUtf8("rightContainer"));
    l = new QHBoxLayout();
    l->addWidget(rightTabButton);
    l->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Fixed));
    w->setLayout(l);
    layout->addWidget(w);

    timelineTabs->setLayout(layout);

    //HACK
    //to powinno byæ stylowane za pomoc¹ CSS, niestety Qwt operuje tylko na paletach
    QPalette sliderPalette(this->palette());
    sliderPalette.setBrush(QPalette::Light, QBrush(QColor(135, 177, 255)));
    sliderPalette.setBrush(QPalette::Mid, QBrush(QColor(255,0,0)));
    sliderPalette.setBrush(QPalette::Dark, QBrush(QColor(135, 177, 255)));
    sliderPalette.setBrush(QPalette::Button, QBrush(QColor(0,0,0)));
    slider->setPalette(sliderPalette);

    QWidget * widget = new QWidget();
    widget->setObjectName("timelineControls");
    widget->setLayout(new QVBoxLayout());
    widget->layout()->setSpacing(4);
    widget->layout()->setContentsMargins(1,0,1,0);
    widget->layout()->addWidget(slider);
    slider->setContentsMargins(4,0,4,0);
    widget->layout()->addWidget(timelineTabs);
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //channelsWidget->setItemWidget(rootItem, 2, widget);
    channelsWidget->setVisible(false);
    this->layout()->addWidget(widget);

    rightTabButton->setVisible(false);

    /*int w = std::max(leftTabButton->sizeHint().width(), rightTabButton->sizeHint().width());
    leftTabButton->setMinimumWidth(w);
    rightTabButton->setMinimumWidth(w);*/
}

void TimelineWidget::setChannelTooltip(const std::string & path, const std::string & tooltip)
{
    auto channel = getController()->findChannel(path);

    if(channel == nullptr){
        throw std::runtime_error("Could not set channel tooltip - channel with given path not exist");
    }

    modelToUIChannels[channel]->setToolTip(0, QString(tooltip.c_str()));
}

std::string TimelineWidget::getChannelTooltip(const std::string & path) const
{
    auto channel = getController()->findChannel(path);

    if(channel == nullptr){
        throw std::runtime_error("Could not get channel tooltip - channel with given path not exist");
    }

    return modelToUIChannels.find(channel)->second->toolTip(0).toStdString();
}

void TimelineWidget::setOnChannelClick(const TimelineService::UIChannelAction & action)
{
    onItemClickAction = action;
}

const TimelineService::UIChannelAction & TimelineWidget::getOnChannelClick() const
{
    return onItemClickAction;
}

void TimelineWidget::setOnChannelDblClick(const TimelineService::UIChannelAction & action)
{
   onItemDblClickAction = action;
}

const TimelineService::UIChannelAction & TimelineWidget::getOnChannelDblClick() const
{
    return onItemDblClickAction;
}

void TimelineWidget::setOnChannelRemove(const TimelineService::UIChannelAction & action)
{
    onItemRemoveAction = action;
}

const TimelineService::UIChannelAction & TimelineWidget::getOnChannelRemove() const
{
    return onItemRemoveAction;
}

void TimelineWidget::loadToolbarElements(core::IActionsGroupManager * actionsManager) const
{

}

TimelineWidget::~TimelineWidget()
{
    delete removeChannelsMenu;
}

void TimelineWidget::update(const State * state)
{
    QMetaObject::invokeMethod(this, "refresh");
}

void TimelineWidget::refresh()
{
    //blokujemy sygna³y modyfikowanych komponenrów
    preciseTimeWidget->blockSignals(true);
    //scaleSpinBox->blockSignals(true);
    slider->blockSignals(true);

    refreshChannelsHierarchy();
    refreshChannels();
    refreshPlayerStatus();

    //odblokuj komponenty
    slider->blockSignals(false);
    preciseTimeWidget->blockSignals(false);
    //scaleSpinBox->blockSignals(false);
}

void TimelineWidget::showChannelsTreeContextMenu(const QPoint& pnt)
{
    if(channelsWidget->selectedItems().empty() == false){
        removeChannelsMenu->exec(mapToGlobal(pnt));
    }
}

void TimelineWidget::removeSelectedChannels()
{
    QList<QTreeWidgetItem*> selectedItems = channelsWidget->selectedItems();

    std::map<std::string, QTreeWidgetItem*> orderedPathsToDelete;

    for(auto it = selectedItems.begin(); it != selectedItems.end(); ++it)
    {
        ChannelsTreeItem * item = dynamic_cast<ChannelsTreeItem*>(*it);
        if(item != nullptr){
            orderedPathsToDelete[item->getChannel()->getAbsolutePath()] = item;
        }
    }

    auto it = orderedPathsToDelete.begin();
    std::string lastNode = it->first;
    std::set<std::string> toDelete;
    toDelete.insert(lastNode);

    ++it;
    while(it != orderedPathsToDelete.end()){
        // jeœli to nie moje dziecko w modelu
        std::string::size_type pos = it->first.find(lastNode);
        if(pos == 0){
            //usuwamy podkana³, zostanie usuniêty wraz ze swoim rodzicem
            it = orderedPathsToDelete.erase(it);
        }else{
            //zapamietaj ostatni wêze³ by weryfikowaæ jego dzieci
            lastNode = it->first;
            toDelete.insert(lastNode);
            ++it;
        }
    }

    //w³aœciwe usuniêcie z drzewa UI
    for(auto it = orderedPathsToDelete.begin(); it != orderedPathsToDelete.end(); ++it){
        it->second->parent()->removeChild(it->second);
        delete it->second;
    }

    if(onItemRemoveAction.empty() == false){
        /*for(auto it = toDelete.begin(); it != toDelete.end(); it++){
            onItemRemoveAction((*it)->get);
        }*/
    }

    //w³aœciwe usuniêcie z modelu
    getController()->removeChannels(toDelete);
}

void TimelineWidget::timeSliderChanged(int value)
{
    // rev maxValue zmienione na maximum (to pewno to samo ;) )
    //getController()->setNormalizedTime((double)value / (slider->maxValue() - slider->minValue()));
    getController()->setNormalizedTime((double)value / (slider->maximum() - slider->minimum()));
}

void TimelineWidget::timeSliderChanged(double time)
{
    getController()->setTime(time);
}


void TimelineWidget::timeChanged(const QTime & time)
{
    getController()->setTime(convertToTime(time));
}

void TimelineWidget::scaleChanged(double scale)
{
    getController()->setTimeScale(scale);
}

void TimelineWidget::playbackDirectionChanged(bool backward)
{
    getController()->setPlaybackDirection(backward == true ? timeline::IController::PlayBackward : timeline::IController::PlayForward);
    if(getController()->isPlaying() == false){
        getController()->setNormalizedTime(backward == true);
    }
}

void TimelineWidget::pause(bool playTimeline)
{
    if(playTimeline == false){
        getController()->pause();
    }else{
        getController()->play();
    }
}

void TimelineWidget::toBegin()
{
    getController()->setNormalizedTime(0);
}

void TimelineWidget::toEnd()
{
    getController()->setNormalizedTime(1);
}

void TimelineWidget::stop()
{
    playPauseAction->setEnabled(true);
    playPauseAction->setChecked(false);
    getController()->pause();
    getController()->getPlaybackDirection() == timeline::IController::PlayForward ? toBegin() : toEnd();
}


void TimelineWidget::refreshChannelsHierarchy()
{
    std::set<QTreeWidgetItem*> toDeleteItems;
    std::vector<timeline::Model::TChannelConstPtr> missingItems;

    std::set<QTreeWidgetItem*> uiNodes;
    std::set<timeline::Model::TChannelConstPtr> modelNodes;

    for(int i = 0; i < rootItem->childCount(); ++i){
        uiNodes.insert(rootItem->child(i));
    }

    for(auto it = getController()->getModel()->beginChannels(); it != getController()->getModel()->endChannels(); ++it){
        modelNodes.insert(core::dynamic_pointer_cast<const timeline::Model::TChannel>(*it));
    }

    compareNodes(uiNodes, modelNodes, toDeleteItems, missingItems);

    for(auto it = toDeleteItems.begin(); it != toDeleteItems.end(); ++it){
        rootItem->removeChild(*it);
        modelToUIChannels.erase(reinterpret_cast<ChannelsTreeItem*>(*it)->getChannel());
        delete *it;
    }

    for(auto it = missingItems.begin(); it != missingItems.end(); ++it){
        ChannelWidget * channelWidget = new ChannelWidget(getController(), *it, nullptr, 0, slider->getLeftMargin(), slider->getRightMargin());
        ChannelsTreeItem * item = new ChannelsTreeItem(*it, channelWidget);

        modelToUIChannels[*it] = item;

        rootItem->addChild(item);

        connect(slider, SIGNAL(marginsChanged(double,double)), channelWidget, SLOT(setMargins(double,double)));

        channelsWidget->setItemWidget(item, 2, channelWidget);

        ChannelCheckBox * checkBox = new ChannelCheckBox(*it);

        checkBox->setChecked((*it)->getData()->isActive());

        connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(channelsActivityChanged(int)));

        channelsWidget->setItemWidget(item, 1, checkBox);
    }

    for(int i = 0; i < rootItem->childCount(); ++i){
        recursiveHierarchyRefresh(rootItem->child(i));
    }

    rootItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);

    channelsWidget->update();
}

void TimelineWidget::refreshChannels()
{
    for(int i = 0; i < rootItem->childCount(); ++i){
        recursiveRefreshChannels(rootItem->child(i));
    }
}

void TimelineWidget::recursiveRefreshChannels(QTreeWidgetItem* uiNode)
{
    reinterpret_cast<ChannelsTreeItem*>(uiNode)->refreschVisualChannel();

    for(int i = 0; i < uiNode->childCount(); ++i){
        recursiveRefreshChannels(uiNode->child(i));
    }
}

void TimelineWidget::refreshPlayerStatus()
{
    //jeœli nie mamy pustego timeline (s¹ kana³y) to umo¿liwiamy odtwarzanie i zatrzymywanie
    if(getController()->getModel()->sizeChannels() > 0){
        playPauseAction->setEnabled(true);
        stopAction->setEnabled(true);
    }else{
        playPauseAction->setEnabled(false);
        stopAction->setEnabled(false);
    }

    if(getController()->isPlaying() == false){
        if(playPauseAction->isChecked() == true){
            playPauseAction->setChecked(false);
            playPauseAction->setEnabled(false);
        }else if((getController()->getPlaybackDirection() == timeline::IController::PlayForward && getController()->getTime() < getController()->getModel()->getEndTime()) ||
            (getController()->getPlaybackDirection() == timeline::IController::PlayBackward && getController()->getTime() > getController()->getModel()->getBeginTime())) {
                playPauseAction->setEnabled(true);
        }
    }

    //ustawiamy slider z czasem

    if(getController()->getLength() > 0){
        // rev - zmiana api
        //slider->setRange(getController()->getModel()->getBeginTime(), getController()->getModel()->getEndTime(), 0.001);
        slider->setScale(getController()->getModel()->getBeginTime(), getController()->getModel()->getEndTime(), 0.001);
        slider->setValue(getController()->getTime());
    }else{
        // rev - zmiana api
        slider->setValue(slider->minimum());
    }

    //ustawiamy czas w oknie edycji textowej
    preciseTimeWidget->setTime(convertToQTime(getController()->getTime()));
    //ustaw czas minimalny
    preciseTimeWidget->setMinimumTime(convertToQTime(getController()->getModel()->getBeginTime()));
    //ustaw czas maxymalny
    preciseTimeWidget->setMaximumTime(convertToQTime(getController()->getModel()->getEndTime()));

    //ustaw skalê czasu
    //scaleSpinBox->setValue(getController()->getTimeScale());

    //kierunek odtwarzania
    playbackDirectionAction->setChecked(getController()->getPlaybackDirection() == timeline::IController::PlayBackward ? true : false);
}

void TimelineWidget::compareNodes(const std::set<QTreeWidgetItem*> & uiNodes, const std::set<timeline::Model::TChannelConstPtr> & modelNodes,
    std::set<QTreeWidgetItem*> & toDeleteUINodes, std::vector<timeline::Model::TChannelConstPtr> & missingModelNodes)
{
    std::set<timeline::Model::TChannelConstPtr> present;
    std::vector<timeline::Model::TChannelConstPtr> missingNodes(modelNodes.size());
    for(auto it = uiNodes.begin(); it != uiNodes.end(); ++it){
        timeline::Model::TChannelConstPtr channel(reinterpret_cast<ChannelsTreeItem*>(*it)->getChannel());
        if(modelNodes.find(channel) == modelNodes.end()){
            toDeleteUINodes.insert(*it);
        }

        present.insert(channel);
    }

    if(present.empty() == false){
        auto it = std::set_difference(modelNodes.begin(), modelNodes.end(), present.begin(), present.end(), missingNodes.begin());
        for(auto iT = missingNodes.begin(); iT != it; ++iT){
            missingModelNodes.push_back(*iT);
        }
    }else{
        for(auto it = modelNodes.begin(); it != modelNodes.end(); ++it){
            missingModelNodes.push_back(*it);
        }
    }
}

void TimelineWidget::recursiveHierarchyRefresh(QTreeWidgetItem* uiNode)
{
    std::set<QTreeWidgetItem*> toDeleteItems;
    std::vector<timeline::Model::TChannelConstPtr> missingItems;

    std::set<QTreeWidgetItem*> uiNodes;
    std::set<timeline::Model::TChannelConstPtr> modelNodes;

    for(int i = 0; i < uiNode->childCount(); ++i){
        uiNodes.insert(uiNode->child(i));
    }

    ChannelsTreeItem* channelsTreeNode = reinterpret_cast<ChannelsTreeItem*>(uiNode);

    for(auto it = channelsTreeNode->getChannel()->begin(); it != channelsTreeNode->getChannel()->end(); ++it){
        modelNodes.insert( core::dynamic_pointer_cast<const timeline::Model::TChannel>(*it));
    }

    compareNodes(uiNodes, modelNodes, toDeleteItems, missingItems);

    for(auto it = toDeleteItems.begin(); it != toDeleteItems.end(); ++it){
        uiNode->removeChild(*it);
        modelToUIChannels.erase(reinterpret_cast<ChannelsTreeItem*>(*it)->getChannel());
        delete *it;
    }

    for(auto it = missingItems.begin(); it != missingItems.end(); ++it){
        ChannelWidget * channelWidget = new ChannelWidget(getController(), *it, nullptr, 0, slider->getLeftMargin(), slider->getRightMargin());
        ChannelsTreeItem * item = new ChannelsTreeItem(*it, channelWidget);

        modelToUIChannels[*it] = item;

        uiNode->addChild(item);

        connect(slider, SIGNAL(marginsChanged(double,double)), channelWidget, SLOT(setMargins(double,double)));

        channelsWidget->setItemWidget(item, 2, channelWidget);

        ChannelCheckBox * checkBox = new ChannelCheckBox(*it);

        checkBox->setChecked((*it)->getData()->isActive());

        connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(channelsActivityChanged(int)));

        channelsWidget->setItemWidget(item, 1, checkBox);
    }

    for(int i = 0; i < uiNode->childCount(); ++i){
        recursiveHierarchyRefresh(uiNode->child(i));
    }
}

void TimelineWidget::channelsActivityChanged(int status)
{
    ChannelCheckBox * checkBox = qobject_cast<ChannelCheckBox *>(sender());
    if(checkBox != nullptr){
        getController()->setChannelActive(checkBox->getChannel()->getAbsolutePath(), status == Qt::Checked ? true : false);
    }
}

QWidget * TimelineWidget::createTreeItemWidget(QWidget * widget)
{
    QWidget * ret = new QWidget();
    QHBoxLayout * layout = new QHBoxLayout(ret);
    ret->setLayout(layout);

    //layout->addSpacing(5);
    layout->addWidget(widget);

    //layout->addSpacing(5);
    ret->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //ret->setStyleSheet(QString("QWidget { background-color: red; }"));
    widget->setVisible(true);
    ret->setVisible(true);
    return ret;
}

void TimelineWidget::onItemClicked(QTreeWidgetItem * item, int column)
{
    if(onItemClickAction.empty() == false && column == 0 && item != rootItem){
        onItemClickAction(reinterpret_cast<ChannelsTreeItem*>(item)->getChannel()->getData()->getInnerChannel());
    }
}

void TimelineWidget::onItemDblClicked(QTreeWidgetItem * item, int column)
{
    bool empty = onItemDblClickAction.empty();
    if(empty == false && column == 0 && item != rootItem){
        onItemDblClickAction(reinterpret_cast<ChannelsTreeItem*>(item)->getChannel()->getData()->getInnerChannel());
    }
}
