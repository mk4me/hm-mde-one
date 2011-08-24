#include "TimelinePCH.h"

#include <QtGui/QIcon>
#include <QtGui/QCheckBox>

#include <math.h>

#include "TimelineWidget.h"
#include "ChannelCheckbox.h"
#include "ChannelWidget.h"
#include <timelinelib/Controller.h>


TimelineWidget::TimelineWidget(const timeline::ControllerPtr & controlsler, QWidget * parent, Qt::WindowFlags f)
    : QWidget(parent, f), removeChannelsMenu(new QMenu()), scaleSpinBox(new QDoubleSpinBox()),
    playbackDirectionAction(new QAction(QString("Playback direction"), nullptr)), timeEditBox(new QTimeEdit()), timeToBeginAction(new QAction(QString("Begin"), nullptr)),
    timeToEndAction(new QAction(QString("End"), nullptr)), playPauseAction(new QAction(QString("Play"), nullptr)), visualTimeSlider(new QSlider(nullptr)),
    stopAction(new QAction(QString("Stop"), nullptr)), scaleLabel(new QLabel(QString("<font color=\"white\"><b>Scale:</b></font>"))),
    timeLabel(new QLabel(QString("<font color=\"white\"><b>Time:</b></font>"))), rootItem(new QTreeWidgetItem()),
    slider(new TimeSliderWidget()), controls(new TimelineControlsWidget())
{
    //ustawienie kontrolera
    setController(controlsler);

    //inicjalizujemy widok drzewa

    //rejestracja obserwacji kontrolera
    controlsler->attach(this);

    //unicjalizacja UI

    //elementy Toolbara!!

    //ustaw akcje odtwarzania

    // toBegin action
    connect(controls->toBeginButton, SIGNAL(pressed()), this, SLOT(toBegin()));
    /*QIcon icon;
    icon.addFile(QString::fromUtf8(":/resources/icons/toBegin.png"), QSize(), QIcon::Normal, QIcon::Off);
    timeToBeginAction->setIcon(icon);

    connect(timeToBeginAction, SIGNAL(triggered()), this, SLOT(toBegin()));*/


    // toEnd action
    connect(controls->toEndButton, SIGNAL(pressed()), this, SLOT(toEnd()));
    /*QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/resources/icons/toEnd.png"), QSize(), QIcon::Normal, QIcon::Off);
    timeToEndAction->setIcon(icon1);

    connect(timeToEndAction, SIGNAL(triggered()), this, SLOT(toEnd()));*/

    //play | pause action
    controls->playPauseButton->setCheckable(true);
    connect(controls->playPauseButton, SIGNAL(toggled(bool)), this, SLOT(pause(bool)));
    controls->playPauseButton->setEnabled(false);
    /*playPauseAction->setCheckable(true);
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/resources/icons/play.png"), QSize(), QIcon::Normal, QIcon::Off);
    icon2.addFile(QString::fromUtf8(":/resources/icons/pause.png"), QSize(), QIcon::Normal, QIcon::On);

    playPauseAction->setIcon(icon2);

    connect(playPauseAction, SIGNAL(toggled(bool)), this, SLOT(pause(bool)));*/

    //Deaktywujemy, bo nie ma ¿adnych kana³ów
    //playPauseAction->setEnabled(false);

    // stop action
    connect(controls->stopButton, SIGNAL(pressed()), this, SLOT(stop()));
    controls->stopButton->setEnabled(false);
    /*QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/resources/icons/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
    stopAction->setIcon(icon3);

    connect(stopAction, SIGNAL(triggered()), this, SLOT(stop()));*/

    //Deaktywujemy, bo nie ma ¿adnych kana³ów
    //stopAction->setEnabled(false);

    // edycja tekstowa czasu
    controls->timeEditBox->setDisplayFormat(QString("mm:ss:zz"));
    controls->timeEditBox->setTime(QTime(0,0));
    controls->timeEditBox->setMinimumTime(QTime(0,0));
    controls->timeEditBox->setMaximumTime(QTime(0,0));

    connect(controls->timeEditBox, SIGNAL(timeChanged(QTime)), this, SLOT(timeChanged(QTime)));


    //timeEditBox->setDisplayFormat(QString("mm:ss:zzz"));
    //timeEditBox->setTime(QTime(0,0));
    //timeEditBox->setMinimumTime(QTime(0,0));
    //timeEditBox->setMaximumTime(QTime(0,0));

    ////po³¹czenie sygna³ów i slotów
    //connect(timeEditBox, SIGNAL(timeChanged(QTime)), this, SLOT(timeChanged(QTime)));


    //skala
    scaleSpinBox->setDecimals(2);
    scaleSpinBox->setValue(1);
    scaleSpinBox->setMinimum(0.01);
    scaleSpinBox->setMaximum(100);
    scaleSpinBox->setSingleStep(0.2);

    //po³¹czenie sygna³ów i slotów
    connect(scaleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(scaleChanged(double)));


    //akcja kierunku odtwarzania
    controls->playbackDirectionButton->setCheckable(true);

    //playbackDirectionAction->setCheckable(true);

    //QIcon icon4;
    //icon4.addFile(QString::fromUtf8(":/resources/icons/playbackForward.png"), QSize(), QIcon::Normal, QIcon::Off);
    //icon4.addFile(QString::fromUtf8(":/resources/icons/playbackBackward.png"), QSize(), QIcon::Normal, QIcon::On);
    //playbackDirectionAction->setIcon(icon4);

    ////po³¹czenie sygna³ów i slotów
    //connect(playbackDirectionAction, SIGNAL(toggled(bool)), this, SLOT(playbackDirectionChanged(bool)));
    connect(controls->playbackDirectionButton, SIGNAL(toggled(bool)), this, SLOT(playbackDirectionChanged(bool)));

    //elementy widgeta

    setupUi(this);

    QStringList headers;
    headers << "Channel" << "Active" << "Time";
    channelsWidget->setHeaderLabels(headers);
    //channelsWidget->header()->close();
    


    //rootItem->setText(0, QString("Timeline"));
    rootItem->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);

    //slider
    /*visualTimeSlider->setObjectName(QString::fromUtf8("visualTimeSlider"));
    visualTimeSlider->setMaximum(10000);
    visualTimeSlider->setOrientation(Qt::Horizontal);
    visualTimeSlider->setTickInterval(100);
    visualTimeSlider->setTickPosition(QSlider::TicksBelow);
    visualTimeSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);*/

    //connect(visualTimeSlider, SIGNAL(valueChanged(int)), this, SLOT(timeSliderChanged(int)));

    channelsWidget->addTopLevelItem(rootItem);
    //channelsWidget->setItemWidget(rootItem, 2, visualTimeSlider);

    //nowy slider
    slider->setScalePosition(QwtSlider::TopScale);
    slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    slider->setHandleSize(40,10);

    connect(slider, SIGNAL(valueChanged(double)), this, SLOT(timeSliderChanged(double)));

    channelsWidget->setItemWidget(rootItem, 2, slider);
    slider->setFixedHeight(slider->height() + 6);

    channelsWidget->setItemWidget(rootItem, 0, controls);
    

    channelsWidget->header()->setMinimumSectionSize(14);
    channelsWidget->header()->resizeSection(1, 14);
    channelsWidget->header()->setResizeMode(1, QHeaderView::Fixed);
    channelsWidget->header()->setResizeMode(0, QHeaderView::ResizeToContents);
    channelsWidget->header()->hide();

    // menu kontekstowe dla drzewa kana³ów z akcj¹ usuwaj¹c¹ zaznaczone kana³y
    removeChannelsMenu->addAction(actionRemoveChannels);
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

void TimelineWidget::loadToolbarElements(std::vector<QObject*> & elements) const
{
    //elements.push_back(timeToBeginAction);
    //elements.push_back(playPauseAction);
    //elements.push_back(stopAction);
    //elements.push_back(timeToEndAction);
    ////elements.push_back(scaleLabel);
    ////elements.push_back(scaleSpinBox);
    //elements.push_back(timeLabel);
    //elements.push_back(timeEditBox);
    //elements.push_back(playbackDirectionAction);
}

TimelineWidget::~TimelineWidget()
{

}

void TimelineWidget::update(const State * state)
{
    QMetaObject::invokeMethod(this, "refresh");
}

void TimelineWidget::refresh()
{
    //blokujemy sygna³y modyfikowanych komponenrów
    //visualTimeSlider->blockSignals(true);
    controls->timeEditBox->blockSignals(true);
    scaleSpinBox->blockSignals(true);
    controls->playbackDirectionButton->blockSignals(true);
    controls->playPauseButton->blockSignals(true);
    controls->stopButton->blockSignals(true);
    slider->blockSignals(true);

    refreshChannelsHierarchy();
    refreshChannels();
    refreshPlayerStatus();
    
    //odblokuj komponenty
    slider->blockSignals(false);
    //visualTimeSlider->blockSignals(false);
    controls->timeEditBox->blockSignals(false);
    scaleSpinBox->blockSignals(false);
    controls->playbackDirectionButton->blockSignals(false);
    controls->playPauseButton->blockSignals(false);
    controls->stopButton->blockSignals(false);
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

    for(auto it = selectedItems.begin(); it != selectedItems.end(); it++)
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

    it++;
    while(it != orderedPathsToDelete.end()){
        // jeœli to nie moje dziecko w modelu
        std::string::size_type pos = it->first.find(lastNode);
        if(pos == 0){
            //usuwamy podkana³, zostanie usuniêty wraz ze swoim rodzicem
            it = orderedPathsToDelete.erase(it);
        }else{
            //zapamietaj ostatni wezel by weryfikowac jego dzieci
            lastNode = it->first;
            toDelete.insert(lastNode);
            it++;
        }
    }

    //w³aœciwe usuniêcie z drzewa UI
    for(auto it = orderedPathsToDelete.begin(); it != orderedPathsToDelete.end(); it++){
        it->second->parent()->removeChild(it->second);
        delete it->second;
    }

    //w³aœciwe usuniêcie z modelu
    getController()->removeChannels(toDelete);
}

void TimelineWidget::timeSliderChanged(int value)
{
    getController()->setNormalizedTime((double)value / (double)visualTimeSlider->maximum());
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
    controls->playPauseButton->setEnabled(true);
    controls->playPauseButton->setChecked(false);
    getController()->getPlaybackDirection() == timeline::IController::PlayForward ? toBegin() : toEnd();
}


void TimelineWidget::refreshChannelsHierarchy()
{
    std::set<QTreeWidgetItem*> toDeleteItems;
    std::vector<timeline::Model::TChannelConstPtr> missingItems;

    std::set<QTreeWidgetItem*> uiNodes;
    std::set<timeline::Model::TChannelConstPtr> modelNodes;

    for(int i = 0; i < rootItem->childCount(); i++){    
        uiNodes.insert(rootItem->child(i));
    }

    //modelNodes.insert(getController()->getModel()->beginChannels(), getController()->getModel()->endChannels());
    for(auto it = getController()->getModel()->beginChannels(); it != getController()->getModel()->endChannels(); it++){
        modelNodes.insert(core::dynamic_pointer_cast<const timeline::Model::TChannel>(*it));
    }

    compareNodes(uiNodes, modelNodes, toDeleteItems, missingItems);

    for(auto it = toDeleteItems.begin(); it != toDeleteItems.end(); it++){
        rootItem->removeChild(*it);
        modelToUIChannels.erase(reinterpret_cast<ChannelsTreeItem*>(*it)->getChannel());
        delete *it;
    }

    for(auto it = missingItems.begin(); it != missingItems.end(); it++){
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

    for(int i = 0; i < rootItem->childCount(); i++){    
        recursiveHierarchyRefresh(rootItem->child(i));
    }

    channelsWidget->update();
}

void TimelineWidget::refreshChannels()
{
    for(int i = 0; i < rootItem->childCount(); i++){    
        recursiveRefreshChannels(rootItem->child(i));
    }
}

void TimelineWidget::recursiveRefreshChannels(QTreeWidgetItem* uiNode)
{
    reinterpret_cast<ChannelsTreeItem*>(uiNode)->refreschVisualChannel();

    for(int i = 0; i < uiNode->childCount(); i++){    
        recursiveRefreshChannels(uiNode->child(i));
    }
}

void TimelineWidget::refreshPlayerStatus()
{
    //jeœli nie mamy pustego timeline (sa kana³y) to umo¿liwiamy odtwarzanie i zatrzymywanie
    if(getController()->getModel()->sizeChannels() > 0){
        controls->playPauseButton->setEnabled(true);
        controls->stopButton->setEnabled(true);
    }else{
        controls->playPauseButton->setEnabled(false);
        controls->stopButton->setEnabled(false);
    }

    if(getController()->isPlaying() == false){
        if(controls->playPauseButton->isChecked() == true){
            controls->playPauseButton->setChecked(false);
            controls->playPauseButton->setEnabled(false);
        }else if((getController()->getPlaybackDirection() == timeline::IController::PlayForward && getController()->getTime() < getController()->getModel()->getEndTime()) ||
            (getController()->getPlaybackDirection() == timeline::IController::PlayBackward && getController()->getTime() > getController()->getModel()->getBeginTime())) {
                controls->playPauseButton->setEnabled(true);
        }
    }

    //ustawiamy slider z czasem
    //visualTimeSlider->setValue(getController()->getNormalizedTime() * (double)visualTimeSlider->maximum());

    if(getController()->getLength() > 0){
        slider->setRange(getController()->getModel()->getBeginTime(), getController()->getModel()->getEndTime());
        slider->setValue(getController()->getTime());
    }

    //ustawiamy czas w oknie edycji textowej
    controls->timeEditBox->setTime(convertToQTime(getController()->getTime()));
    //ustaw czas minimalny
    controls->timeEditBox->setMinimumTime(convertToQTime(getController()->getModel()->getBeginTime()));
    //ustaw czas maxymalny
    controls->timeEditBox->setMaximumTime(convertToQTime(getController()->getModel()->getEndTime()));

    //ustaw skalê czasu
    scaleSpinBox->setValue(getController()->getTimeScale());

    //kierunek odtwarzania
    controls->playbackDirectionButton->setChecked(getController()->getPlaybackDirection() == timeline::IController::PlayBackward ? true : false);
}

void TimelineWidget::compareNodes(const std::set<QTreeWidgetItem*> & uiNodes, const std::set<timeline::Model::TChannelConstPtr> & modelNodes,
    std::set<QTreeWidgetItem*> & toDeleteUINodes, std::vector<timeline::Model::TChannelConstPtr> & missingModelNodes)
{
    std::set<timeline::Model::TChannelConstPtr> present;
    std::vector<timeline::Model::TChannelConstPtr> missingNodes(modelNodes.size());
    for(auto it = uiNodes.begin(); it != uiNodes.end(); it++){
        timeline::Model::TChannelConstPtr channel(reinterpret_cast<ChannelsTreeItem*>(*it)->getChannel());
        if(modelNodes.find(channel) == modelNodes.end()){
            toDeleteUINodes.insert(*it);
        }

        present.insert(channel);
    }

    if(present.empty() == false){
        auto it = std::set_difference(modelNodes.begin(), modelNodes.end(), present.begin(), present.end(), missingNodes.begin());
        for(auto iT = missingNodes.begin(); iT != it; iT++){
            missingModelNodes.push_back(*iT);
        }
    }else{
        for(auto it = modelNodes.begin(); it != modelNodes.end(); it++){
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

    for(int i = 0; i < uiNode->childCount(); i++){
        uiNodes.insert(uiNode->child(i));
    }

    ChannelsTreeItem* channelsTreeNode = reinterpret_cast<ChannelsTreeItem*>(uiNode);

    for(auto it = channelsTreeNode->getChannel()->begin(); it != channelsTreeNode->getChannel()->end(); it++){
        modelNodes.insert( core::dynamic_pointer_cast<const timeline::Model::TChannel>(*it));
    }    

    compareNodes(uiNodes, modelNodes, toDeleteItems, missingItems);

    for(auto it = toDeleteItems.begin(); it != toDeleteItems.end(); it++){
        uiNode->removeChild(*it);
        modelToUIChannels.erase(reinterpret_cast<ChannelsTreeItem*>(*it)->getChannel());
        delete *it;
    }

    for(auto it = missingItems.begin(); it != missingItems.end(); it++){
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

    for(int i = 0; i < uiNode->childCount(); i++){
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