#include "CorePCH.h"
#include <utils/Debug.h>
#include "FlexiTabWidget.h"

#include <QtGui/QFrame>
#include <QtGui/QScrollArea>

#include <iterator>
#include "FlexiTabSectionWidget.h"
#include "FlexiWidgetTabContentWidget.h"


FlexiTabWidget::FlexiTabWidget(QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f), nextGroupID(0), nextSectionID(0), currentGroupID(-1), hiddenTabContentPos(-1)
{
    setupUi(this);
}

FlexiTabWidget::~FlexiTabWidget()
{

}

FlexiTabWidget::GUIID FlexiTabWidget::currentGroup() const
{
    return currentGroupID;
}

void FlexiTabWidget::setCurrentGroup(GUIID groupID)
{
    auto groupIT = getGroup(groupID);

    if(groupIT->second->visible == true){
        innerFlexiTabWidget->setCurrentIndex(groupIT->second->tabPos);
    }else{
        innerFlexiTabWidget->setCurrentIndex(-1);
    }

    currentGroupID = groupID;
}

void FlexiTabWidget::setGroupName(GUIID groupID, const QString & name)
{
    auto groupIT = getGroup(groupID);

    groupIT->second->name = name;

    if(groupIT->second->visible == true){
        //innerFlexiTabWidget->setTabText(groupIT->second->tabPos, name);
        refreshTabsTexts();
    }
}

void FlexiTabWidget::setGroupIcon(GUIID groupID, const QIcon & icon)
{
    auto groupIT = getGroup(groupID);

    groupIT->second->icon = icon;

    if(groupIT->second->visible == true){
        innerFlexiTabWidget->setTabIcon(groupIT->second->tabPos, icon);
    }
}

void FlexiTabWidget::setSectionName(GUIID sectionID, const QString & name)
{
    auto sectionIT = getSection(sectionID);

    sectionIT->second->name = name;
    sectionIT->second->sectionWidget->setSectionName(name);
}

void FlexiTabWidget::setSectionIcon(GUIID sectionID, const QIcon & icon)
{
    auto sectionIT = getSection(sectionID);

    sectionIT->second->icon = icon;

    //TODO
}

void FlexiTabWidget::setSectionConfig(GUIID sectionID, QDialog * cfgDialog)
{
    auto sectionIT = getSection(sectionID);

    sectionIT->second->cfgDialog = cfgDialog;

    //TODO
}

void FlexiTabWidget::setSectionWidget(GUIID sectionID, QWidget * widget)
{
    auto sectionIT = getSection(sectionID);

    sectionIT->second->sectionWidget->layout()->removeWidget(sectionIT->second->contentWidget);
    sectionIT->second->contentWidget = widget;
    sectionIT->second->sectionWidget->layout()->addWidget(widget);
}

FlexiTabWidget::GUIID FlexiTabWidget::addGroup(const QString & name, const QIcon & icon, bool visible)
{
    GroupData *gData = new GroupData();
    gData->name = name;
    gData->tabArea = new QScrollArea();
    gData->tabArea->setFrameShape(QFrame::NoFrame);
    gData->tabArea->setLineWidth(0);
    gData->tabArea->setContentsMargins(0,0,0,0);
    gData->tabArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gData->tabArea->setWidgetResizable(true);
    auto tabWidget = new FlexiWidgetTabContentWidget();
    gData->contentWidget = tabWidget;
    gData->tabArea->setWidget(gData->contentWidget);
    gData->contentWidget = tabWidget->trueContent;

    auto layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    //layout->setSizeConstraint(QLayout::SetMinimumSize);
    gData->contentWidget->setLayout(layout);

    gData->visible = visible;
    gData->flexiPos = groupDataByID.size();
    gData->tabPos = innerFlexiTabWidget->count();

    groupDataByID[nextGroupID] = gData;
    groupDataByIndex.push_back(gData);

    if(visible == true){
        innerFlexiTabWidget->addTab(gData->tabArea, icon, name);
        refreshTabsTexts();
    }

    if(groupDataByID.size() == 1){
        currentGroupID = nextGroupID;
    }

    return nextGroupID++;
}

FlexiTabWidget::GUIID FlexiTabWidget::insertGroup(unsigned int pos, const QString & name, const QIcon & icon, bool visible)
{
    UTILS_ASSERT(pos < groupDataByID.size());

    GroupData *gData = new GroupData();
    gData->name = name;
    gData->tabArea = new QScrollArea();
    gData->tabArea->setFrameShape(QFrame::NoFrame);
    gData->tabArea->setLineWidth(0);
    gData->tabArea->setContentsMargins(0,0,0,0);
    gData->tabArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gData->tabArea->setWidgetResizable(true);
    auto tabWidget = new FlexiWidgetTabContentWidget();
    gData->contentWidget = tabWidget;
    gData->tabArea->setWidget(gData->contentWidget);
    gData->contentWidget = tabWidget->trueContent;

    auto layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    //layout->setSizeConstraint(QLayout::SetMinimumSize);
    gData->contentWidget->setLayout(layout);
    gData->visible = visible;
    gData->tabPos = gData->flexiPos = pos;

    if(visible == true){
        innerFlexiTabWidget->insertTab(pos, gData->tabArea, icon, name);
    }

    shiftPosRight(pos, groupDataByID.size());

    groupDataByID[nextGroupID] = gData;

    auto newIT = groupDataByIndex.begin();
    std::advance(newIT, pos);

    groupDataByIndex.insert(newIT, gData);

    if(visible == true){
        refreshTabsTexts();
    }

    if(groupDataByID.size() == 1){
        currentGroupID = nextGroupID;
    }

    return nextGroupID++;
}

void FlexiTabWidget::moveGroup(GUIID groupID, unsigned int newPos)
{
    UTILS_ASSERT(newPos < groupDataByID.size());

    auto srcGroupIT = getGroup(groupID);

    if(newPos == srcGroupIT->second->flexiPos){
        return;
    }

    if(srcGroupIT->second->visible == true){
        innerFlexiTabWidget->removeTab(srcGroupIT->second->tabPos);
        innerFlexiTabWidget->insertTab(newPos, srcGroupIT->second->tabArea, srcGroupIT->second->icon, srcGroupIT->second->name);
    }

    auto srcPos = srcGroupIT->second->flexiPos;
    srcGroupIT->second->tabPos = groupDataByIndex[newPos]->tabPos;
    srcGroupIT->second->flexiPos = groupDataByIndex[newPos]->flexiPos;

    if(newPos < srcPos){
        shiftPosRight(newPos, srcPos);
    }else{
        shiftPosLeft(srcPos+1, newPos+1);
    }

    auto newIT = groupDataByIndex.begin();
    std::advance(newIT, srcPos);

    groupDataByIndex.erase(newIT);

    std::advance(newIT, newPos - srcPos);

    groupDataByIndex.insert(newIT, srcGroupIT->second);

    if(srcGroupIT->second->visible == true){
        refreshTabsTexts();
    }

    if(currentGroupID > -1){
        setCurrentGroup(currentGroupID);
    }
}

unsigned int FlexiTabWidget::countGroups() const
{
    return groupDataByID.size();
}

unsigned int FlexiTabWidget::getGroupPos(GUIID groupID) const
{
    return getGroup(groupID)->second->flexiPos;
}

FlexiTabWidget::GUIID FlexiTabWidget::getGroupID(unsigned int pos) const
{
    return 0;
}

void FlexiTabWidget::moveGroupToFront(GUIID groupID)
{
    moveGroup(groupID, 0);
}

void FlexiTabWidget::moveGroupToEnd(GUIID groupID)
{
    moveGroup(groupID, groupDataByID.size()-1);
}

void FlexiTabWidget::removeAllGroups()
{

}

void FlexiTabWidget::removeAllGroupSections(GUIID groupID)
{
    auto groupIT = getGroup(groupID);

    for(auto sectionIT = groupIT->second->sections.begin(); sectionIT != groupIT->second->sections.end(); ++sectionIT){

        groupIT->second->tabArea->setWidget(nullptr);

        delete sectionIT->second->sectionWidget;
        delete sectionIT->second;

        sectionDataByID.erase(sectionIT->first);
    }

    SectionDataByID().swap(groupIT->second->sections);
}

void FlexiTabWidget::removeGroup(GUIID groupID)
{
    auto groupIT = getGroup(groupID);

    for(auto sectionIT = groupIT->second->sections.begin(); sectionIT != groupIT->second->sections.end(); ++sectionIT){

        groupIT->second->contentWidget->layout()->removeWidget(sectionIT->second->sectionWidget);

        delete sectionIT->second->sectionWidget;
        delete sectionIT->second;

        sectionDataByID.erase(sectionIT->first);
    }

    auto newIT = groupDataByIndex.begin();
    std::advance(newIT, groupIT->second->flexiPos);

    groupDataByIndex.erase(newIT);

    if(groupIT->second->visible == true){
        innerFlexiTabWidget->removeTab(groupIT->second->tabPos);
    }

    shiftPosLeft(groupIT->second->flexiPos, groupDataByIndex.size());

    groupIT->second->tabArea->setWidget(nullptr);

    delete groupIT->second->contentWidget;
    delete groupIT->second->tabArea;

    if(groupIT->second->visible == true){
        refreshTabsTexts();
    }

    delete groupIT->second;

    groupDataByID.erase(groupIT);

    //TODO
    //poprawic mechanizm widzialności grup/ich aktywności
    if(currentGroupID == groupID){
        innerFlexiTabWidget->setCurrentIndex(-1);
        currentGroupID = -1;
    }
}

bool FlexiTabWidget::isGroupVisible(GUIID groupID) const
{
    return getGroup(groupID)->second->visible;
}

void FlexiTabWidget::setGroupVisible(GUIID groupID, bool visible)
{
    auto groupIT = getGroup(groupID);

    if(groupIT->second->visible == visible){
        return;
    }

    if(groupIT->second->visible == true){
        innerFlexiTabWidget->removeTab(groupIT->second->tabPos);

        shiftTabPosLeft(groupIT->second->flexiPos+1, groupDataByIndex.size());

        if(currentGroupID == groupID){
            innerFlexiTabWidget->setCurrentIndex(-1);
        }

    }else{
        innerFlexiTabWidget->insertTab(groupIT->second->tabPos, groupIT->second->tabArea, groupIT->second->icon, groupIT->second->name);

        shiftTabPosRight(groupIT->second->flexiPos+1, groupDataByIndex.size());

        if(currentGroupID == groupID){
            innerFlexiTabWidget->setCurrentIndex(groupIT->second->tabPos);
        }
    }

    groupIT->second->visible = visible;
    refreshTabsTexts();
}

FlexiTabWidget::GUIID FlexiTabWidget::addSection(GUIID groupID, QWidget * widget, const QString & name, const QIcon & icon, bool visible, QDialog * cfgDialog)
{
    UTILS_ASSERT(widget != nullptr);

    auto groupIT = getGroup(groupID);

    SectionData * sData = new SectionData();
    sData->contentWidget = widget;
    sData->groupID = groupID;
    sData->cfgDialog = cfgDialog;
    sData->name = name;
    sData->icon = icon;
    sData->visible = visible;
    sData->sectionWidget = new FlexiTabSectionWidget(name);
    sData->sectionWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    sectionDataByID[nextSectionID] = sData;

    groupIT->second->sections[nextSectionID] = sData;
    groupIT->second->contentWidget->layout()->addWidget(sData->sectionWidget);
    //sData->sectionWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    sData->sectionWidget->setWidget(sData->contentWidget);

    sData->sectionWidget->setVisible(visible);

    return nextSectionID++;
}

void FlexiTabWidget::removeSection(GUIID sectionID)
{
    auto sectionIT = getSection(sectionID);

    auto groupIT = getGroup(sectionIT->second->groupID);

    groupIT->second->sections.erase(sectionID);

    groupIT->second->tabArea->layout()->removeWidget(sectionIT->second->sectionWidget);

    delete sectionIT->second->sectionWidget;
    delete sectionIT->second;

    sectionDataByID.erase(sectionID);
}

void FlexiTabWidget::setSectionVisible(GUIID sectionID, bool visible)
{
    auto sectionIT = getSection(sectionID);
    sectionIT->second->sectionWidget->setVisible(visible);
}

bool FlexiTabWidget::isSectionVisible(GUIID sectionID)
{
    getSection(sectionID);
    return getSection(sectionID)->second->sectionWidget->isVisible();
}

void FlexiTabWidget::moveSection(GUIID sectionID, GUIID newGroupID)
{
    auto sectionIT = getSection(sectionID);

    auto srcGroupIT = getGroup(sectionIT->second->groupID);
    auto destGroupIT = getGroup(newGroupID);

    sectionIT->second->groupID = newGroupID;

    srcGroupIT->second->sections.erase(sectionID);

    srcGroupIT->second->tabArea->layout()->removeWidget(sectionIT->second->sectionWidget);
    destGroupIT->second->tabArea->layout()->addWidget(sectionIT->second->sectionWidget);
    destGroupIT->second->sections[sectionID] = sectionIT->second;
}

FlexiTabWidget::GroupDataByID::iterator FlexiTabWidget::getGroup(GUIID groupID)
{
    auto groupIT = groupDataByID.find(groupID);

    if(groupIT == groupDataByID.end()){
        throw std::runtime_error("Group with given identifier not found");
    }

    return groupIT;
}

FlexiTabWidget::GroupDataByID::const_iterator FlexiTabWidget::getGroup(GUIID groupID) const
{
    const auto groupIT = groupDataByID.find(groupID);

    if(groupIT == groupDataByID.end()){
        throw std::runtime_error("Group with given identifier not found");
    }

    return groupIT;
}

FlexiTabWidget::SectionDataByID::iterator FlexiTabWidget::getSection(GUIID sectionID)
{
    auto sectionIT = sectionDataByID.find(sectionID);

    if(sectionIT == sectionDataByID.end()){
        throw std::runtime_error("Section with given identifier not found");
    }

    return sectionIT;
}

FlexiTabWidget::SectionDataByID::const_iterator FlexiTabWidget::getSection(GUIID sectionID) const
{
    const auto sectionIT = sectionDataByID.find(sectionID);

    if(sectionIT == sectionDataByID.end()){
        throw std::runtime_error("Section with given identifier not found");
    }

    return sectionIT;
}

void FlexiTabWidget::shiftPosLeft(unsigned int begin, unsigned int end)
{
    for(auto i = begin; i < end; ++i){
        groupDataByIndex[i]->flexiPos--;
        groupDataByIndex[i]->tabPos--;
    }
}

void FlexiTabWidget::shiftPosRight(unsigned int begin, unsigned int end)
{
    for(auto i = begin; i < end; ++i){
        groupDataByIndex[i]->flexiPos++;
        groupDataByIndex[i]->tabPos++;
    }
}

void FlexiTabWidget::shiftTabPosLeft(unsigned int begin, unsigned int end)
{
    for(auto i = begin; i < end; ++i){
        groupDataByIndex[i]->tabPos--;
    }
}

void FlexiTabWidget::shiftTabPosRight(unsigned int begin, unsigned int end)
{
    for(auto i = begin; i < end; ++i){
        groupDataByIndex[i]->tabPos++;
    }
}

void FlexiTabWidget::refreshTabsTexts()
{
    int size = groupDataByIndex.size();

    for(int i = 0; i < size; ++i){
        auto groupData = groupDataByIndex[i];
        if(groupData->visible == true){
            innerFlexiTabWidget->setTabText(groupData->tabPos, groupData->name);
        }
    }
}
