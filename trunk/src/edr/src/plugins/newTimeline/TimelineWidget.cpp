#include "TimelinePCH.h"

#include <QtGui/QIcon>
#include <QtGui/QCheckBox>

#include <math.h>

#include "TimelineWidget.h"
#include "ChannelCheckbox.h"
#include "ChannelsTreeItem.h"
#include "ChannelWidget.h"
#include <timelinelib/Controller.h>

TimelineWidget::TimelineWidget(const timeline::ControllerPtr & controller, QWidget * parent, Qt::WindowFlags f)
    : QWidget(parent, f), removeChannelsMenu(new QMenu()), scaleSpinBox(new QDoubleSpinBox()),
    directionComboBox(new QComboBox()), timeEditBox(new QTimeEdit()), timeToBeginAction(new QAction(QString("Begin"), nullptr)),
    timeToEndAction(new QAction(QString("End"), nullptr)), playPauseAction(new QAction(QString("Play"), nullptr)),
    stopAction(new QAction(QString("Stop"), nullptr)), scaleLabel(new QLabel(QString("<font color=\"white\"><b>Scale:</b></font>"))),
    directionLabel(new QLabel(QString("<font color=\"white\"><b>Playback direction:</b></font>"))),
    timeLabel(new QLabel(QString("<font color=\"white\"><b>Time:</b></font>")))
{
    //ustawienie kontrolera
    setController(controller);

    //inicjalizujemy widok drzewa

    //rejestracja obserwacji kontrolera
    controller->attach(this);

    //unicjalizacja UI

    //elementy Toolbara!!

    //ustaw akcje odtwarzania
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/resources/icons/begin.png"), QSize(), QIcon::Normal, QIcon::Off);
    timeToBeginAction->setIcon(icon);

    connect(timeToBeginAction, SIGNAL(triggered()), this, SLOT(toBegin()));


    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/resources/icons/end.png"), QSize(), QIcon::Normal, QIcon::Off);
    timeToEndAction->setIcon(icon1);

    connect(timeToEndAction, SIGNAL(triggered()), this, SLOT(toEnd()));

    
    playPauseAction->setCheckable(true);
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/resources/icons/play.png"), QSize(), QIcon::Normal, QIcon::Off);
    icon2.addFile(QString::fromUtf8(":/resources/icons/pause.png"), QSize(), QIcon::Normal, QIcon::On);

    playPauseAction->setIcon(icon2);

    connect(playPauseAction, SIGNAL(toggled(bool)), this, SLOT(pause(bool)));


    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/resources/icons/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
    stopAction->setIcon(icon3);

    connect(stopAction, SIGNAL(triggered()), this, SLOT(stop()));


    // edycja tekstowa czasu
    timeEditBox->setDisplayFormat(QString("hh:mm:ss:zzz"));
    timeEditBox->setTime(QTime(0,0));
    timeEditBox->setMinimumTime(QTime(0,0));
    timeEditBox->setMaximumTime(QTime(0,0));

    //po³¹czenie sygna³ów i slotów
    connect(timeEditBox, SIGNAL(timeChanged(QTime)), this, SLOT(timeChanged(QTime)));

    //skala
    scaleSpinBox->setDecimals(2);
    scaleSpinBox->setValue(1);
    scaleSpinBox->setMinimum(0.01);
    scaleSpinBox->setMaximum(100);
    scaleSpinBox->setSingleStep(0.2);

    //po³¹czenie sygna³ów i slotów
    connect(scaleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(scaleChanged(double)));


    //kontrolka kierunku odtwarzania
    directionComboBox->addItem("Forward");
    directionComboBox->addItem("Backward");
    directionComboBox->setCurrentIndex(0);

    //po³¹czenie sygna³ów i slotów
    connect(directionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(playbackDirectionChanged(int)));


    //elementy widgeta

    setupUi(this);

    // menu kontekstowe dla drzewa kana³ów z akcj¹ usuwaj¹c¹ zaznaczone kana³y
    removeChannelsMenu->addAction(actionRemoveChannels);
}

void TimelineWidget::loadToolbarElements(std::vector<QObject*> & elements) const
{
    elements.push_back(timeToBeginAction);
    elements.push_back(playPauseAction);
    elements.push_back(stopAction);
    elements.push_back(timeToEndAction);
    elements.push_back(scaleLabel);
    elements.push_back(scaleSpinBox);
    elements.push_back(directionLabel);
    elements.push_back(directionComboBox);
    elements.push_back(timeLabel);
    elements.push_back(timeEditBox);
}

TimelineWidget::~TimelineWidget()
{

}

QTime TimelineWidget::convertToQTime(double timeInSeconds)
{
    int seconds = 0;

    if(time >= 0){
        seconds = floor(timeInSeconds);
    }else{
        seconds = ceil(timeInSeconds);
    }

    return QTime(0,0, seconds, (timeInSeconds - seconds) * 1000.0);
}

double TimelineWidget::convertToTime(const QTime & time)
{
    return (double)(time.hour() * 3600 + time.minute() * 60 + time.second()) + time.msec() / 1000.0;
}

void TimelineWidget::update(const State * state)
{
    QMetaObject::invokeMethod(this, "update");
}

void TimelineWidget::update()
{
    //blokujemy sygna³y modyfikowanych komponenrów
    visualTimeSlider->blockSignals(true);
    timeEditBox->blockSignals(true);
    scaleSpinBox->blockSignals(true);
    directionComboBox->blockSignals(true);
    playPauseAction->blockSignals(true);
    stopAction->blockSignals(true);

    refreshChannelsHierarchy();
    refreshChannelsTimeStructure();
    refreshTimeInChannels();
    //refreshTimeAxis();
    refreshPlayerStatus();
    
    //odblokuj komponenty
    visualTimeSlider->blockSignals(false);
    timeEditBox->blockSignals(false);
    scaleSpinBox->blockSignals(false);
    directionComboBox->blockSignals(false);
    playPauseAction->blockSignals(false);
    stopAction->blockSignals(false);
}

void TimelineWidget::showContextMenu(const QPoint& pnt)
{
    if(channelsWidget->selectedItems().empty() == false){
        removeChannelsMenu->exec(pnt);
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
        channelsWidget->itemAbove(it->second)->removeChild(it->second);
        delete it->second;
    }

    //w³aœciwe usuniêcie z modelu
    getController()->removeChannels(toDelete);
}

void TimelineWidget::timeSliderChanged(int value)
{
    getController()->setNormalizedTime((double)value / (double)visualTimeSlider->maximum());
}


void TimelineWidget::timeChanged(const QTime & time)
{
    getController()->setTime(convertToTime(time));
}

void TimelineWidget::scaleChanged(double scale)
{
    getController()->setTimeScale(scale);
}

void TimelineWidget::playbackDirectionChanged(int idx)
{
    getController()->setPlaybackDirection(idx == 0 ? timeline::IController::PlayForward : timeline::IController::PlayBackward);
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
    getController()->getPlaybackDirection() == timeline::IController::PlayForward ? toBegin() : toEnd();
}


void TimelineWidget::refreshChannelsHierarchy()
{
    std::set<QTreeWidgetItem*> toDeleteItems;
    std::vector<timeline::Model::TChannelConstPtr> missingItems;

    std::set<QTreeWidgetItem*> uiNodes;
    std::set<timeline::Model::TChannelConstPtr> modelNodes;

    for(int i = 0; i < channelsWidget->topLevelItemCount(); i++){    
        uiNodes.insert(channelsWidget->topLevelItem(i));
    }

    //modelNodes.insert(getController()->getModel()->beginChannels(), getController()->getModel()->endChannels());
    for(auto it = getController()->getModel()->beginChannels(); it != getController()->getModel()->endChannels(); it++){
        modelNodes.insert(core::dynamic_pointer_cast<const timeline::Model::TChannel>(*it));
    }

    compareNodes(uiNodes, modelNodes, toDeleteItems, missingItems);

    for(auto it = toDeleteItems.begin(); it != toDeleteItems.end(); it++){
        channelsWidget->takeTopLevelItem(channelsWidget->indexOfTopLevelItem(*(it)));
        delete *it;
    }

    for(int i = 0; i < channelsWidget->topLevelItemCount(); i++){    
        recursiveHierarchyRefresh(channelsWidget->topLevelItem(i));
    }

    for(auto it = missingItems.begin(); it != missingItems.end(); it++){
        ChannelWidget * channelWidget = new ChannelWidget();
        QTreeWidgetItem * item = new ChannelsTreeItem(*it, channelWidget);
        channelsWidget->addTopLevelItem(item);

        channelsWidget->setItemWidget(item, 2, channelWidget);

        ChannelCheckBox * checkBox = new ChannelCheckBox(*it);

        checkBox->setChecked((*it)->getData()->isActive());

        connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(channelsActivityChanged(int)));

        channelsWidget->setItemWidget(item, 0, checkBox);
    }
}

void TimelineWidget::refreshTimeInChannels()
{
    if(getController()->getLength() > 0){
        for(int i = 0; i < channelsWidget->topLevelItemCount(); i++){    
            resursiveTimeInChannelsRefresh(channelsWidget->topLevelItem(i),
                getController()->getModel()->getOffset(), getController()->getLength());
        }
    }
}

void TimelineWidget::resursiveTimeInChannelsRefresh(QTreeWidgetItem* uiNode, double globalOffset, double globalLenght)
{
    reinterpret_cast<ChannelsTreeItem*>(uiNode)->refreshVisualChannelTime(globalOffset,globalLenght);
    for(int i = 0; i < uiNode->childCount(); i++){
        resursiveTimeInChannelsRefresh(uiNode->child(i), globalOffset, globalLenght);
    }
}

void TimelineWidget::refreshTimeAxis()
{

}

void TimelineWidget::refreshChannelsTimeStructure()
{
    if(getController()->getLength() > 0){
        for(int i = 0; i < channelsWidget->topLevelItemCount(); i++){    
            resursiveChannelsTimeStructureRefresh(channelsWidget->topLevelItem(i),
                getController()->getModel()->getOffset(), getController()->getLength());
        }
    }
}

void TimelineWidget::resursiveChannelsTimeStructureRefresh(QTreeWidgetItem* uiNode, double globalOffset, double globalLenght)
{
    reinterpret_cast<ChannelsTreeItem*>(uiNode)->refreshVisualChannelStructure(globalOffset, globalLenght);

    for(int i = 0; i < uiNode->childCount(); i++){
        resursiveChannelsTimeStructureRefresh(uiNode->child(i), globalOffset, globalLenght);
    }
}

void TimelineWidget::refreshPlayerStatus()
{
    if(getController()->isPlaying() == false && playPauseAction->isChecked() == true){
        playPauseAction->setChecked(false);
        playPauseAction->setEnabled(false);
    }

    //ustawiamy slider z czasem
    visualTimeSlider->setValue(getController()->getNormalizedTime() * visualTimeSlider->maximum());

    //ustawiamy czas w oknie edycji textowej
    timeEditBox->setTime(convertToQTime(getController()->getTime()));
    //ustaw czas minimalny
    timeEditBox->setMinimumTime(convertToQTime(getController()->getModel()->getBeginTime()));
    //ustaw czas maxymalny
    timeEditBox->setMaximumTime(convertToQTime(getController()->getModel()->getEndTime()));

    //ustaw skalê czasu
    scaleSpinBox->setValue(getController()->getTimeScale());

    //kierunek odtwarzania
    directionComboBox->setCurrentIndex(getController()->getPlaybackDirection() == timeline::IController::PlayForward ? 0 : 1);
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
        delete *it;
    }

    for(int i = 0; i < uiNode->childCount(); i++){
        recursiveHierarchyRefresh(uiNode->child(i));
    }

    for(auto it = missingItems.begin(); it != missingItems.end(); it++){
        ChannelWidget * channelWidget = new ChannelWidget();
        QTreeWidgetItem * item = new ChannelsTreeItem(*it, channelWidget);

        uiNode->addChild(item);

        channelsWidget->setItemWidget(item, 2, channelsWidget);

        ChannelCheckBox * checkBox = new ChannelCheckBox(*it);

        checkBox->setChecked((*it)->getData()->isActive());

        connect(checkBox, SIGNAL(stateChanged(int)), this, SLOT(channelsActivityChanged(int)));

        channelsWidget->setItemWidget(item, 0, checkBox);
    }
}

void TimelineWidget::channelsActivityChanged(int status)
{
    ChannelCheckBox * checkBox = qobject_cast<ChannelCheckBox *>(sender());
    if(checkBox != nullptr){
        getController()->setChannelActive(checkBox->getChannel()->getAbsolutePath(), status == Qt::Checked ? true : false);
    }
}