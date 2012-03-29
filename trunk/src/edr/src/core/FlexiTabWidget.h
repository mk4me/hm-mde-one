/********************************************************************
    created:  2011/12/09
    created:  9:12:2011   14:11
    filename: FlexiTabWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_QT_EXT__FLEXITABWIDGET_H__
#define HEADER_GUARD_QT_EXT__FLEXITABWIDGET_H__

#include <QtGui/QTabWidget>
#include <QtGui/QTabBar>
#include <QtGui/QFrame>
#include <QtGui/QScrollArea>
#include "FlexiTabSectionWidget.h"
#include "ui_FlexiTabWidget.h"

class FlexiTabWidget : public QWidget, private Ui::FlexiTabWidget
{
    Q_OBJECT;

public:

    typedef int GUIID;

public:

    FlexiTabWidget(QWidget * parent = 0, Qt::WindowFlags f = 0);

    ~FlexiTabWidget();

    GUIID addGroup(const QString & name = "Default Group", const QIcon & icon = QIcon(), bool visible = true);

    GUIID currentGroup() const;
    void setCurrentGroup(GUIID groupID);
    
    void setGroupName(GUIID groupID, const QString & name);
    void setGroupIcon(GUIID groupID, const QIcon & icon);

    GUIID insertGroup(unsigned int pos, const QString & name = "Default Group", const QIcon & icon = QIcon(), bool visible = true);
    void moveGroup(GUIID groupID, unsigned int newPos);
    void moveGroupToFront(GUIID groupID);
    void moveGroupToEnd(GUIID groupID);
    unsigned int countGroups() const;
    unsigned int getGroupPos(GUIID groupID) const;
    GUIID getGroupID(unsigned int pos) const;
    void setGroupVisible(GUIID groupID, bool visible);
    bool isGroupVisible(GUIID groupID) const;

    void removeAllGroups();
    void removeAllGroupSections(GUIID groupID);

    void removeGroup(GUIID groupID);

    GUIID addSection(GUIID groupID, QWidget * widget, const QString & name = "Default Section", const QIcon & icon = QIcon(), bool visible = true, QDialog * cfgDialog = 0);
    void setSectionName(GUIID sectionID, const QString & name);
    void setSectionIcon(GUIID sectionID, const QIcon & icon);
    void setSectionConfig(GUIID sectionID, QDialog * cfgDialog);
    void setSectionWidget(GUIID sectionID, QWidget * widget);

    void removeSection(GUIID sectionID);

    void setSectionVisible(GUIID sectionID, bool visible = true);
    bool isSectionVisible(GUIID sectionID);

    void moveSection(GUIID sectionID, GUIID newGroupID);

private:

    struct SectionData
    {
        GUIID id;
        QString name;
        QIcon icon;
        bool visible;
        QDialog * cfgDialog;        
        FlexiTabSectionWidget* sectionWidget;
        GUIID groupID;
        QWidget * contentWidget;
    };

    typedef std::map<GUIID, SectionData*> SectionDataByID;

    struct GroupData
    {
        GUIID id;
        QString name;
        QIcon icon;
        bool visible;
        //! Pozycja zak³¹dki w QTabBar
        int tabPos;
        //! Pozycja zak³¹dki w FlexiTab (mo¿e byæ ukryta -> ró¿na od tabPos)
        unsigned int flexiPos;
        QScrollArea * tabArea;
        QWidget * contentWidget;
        SectionDataByID sections;
    };

    

    typedef std::vector<GroupData*> GroupDataByIndex;
    typedef std::map<GUIID, GroupData*> GroupDataByID;
    

private:

    GroupDataByID::iterator getGroup(GUIID groupID);
    GroupDataByID::const_iterator getGroup(GUIID groupID) const;

    SectionDataByID::iterator getSection(GUIID sectionID);
    SectionDataByID::const_iterator getSection(GUIID sectionID) const;

    void shiftPosLeft(unsigned int begin, unsigned int end);
    void shiftPosRight(unsigned int begin, unsigned int end);

    void shiftTabPosLeft(unsigned int begin, unsigned int end);
    void shiftTabPosRight(unsigned int begin, unsigned int end);

    void refreshTabsTexts();

private:

    GUIID currentGroupID;
    int hiddenTabContentPos;

    GUIID nextGroupID;
    GUIID nextSectionID;

    GroupDataByID groupDataByID;
    GroupDataByIndex groupDataByIndex;
    SectionDataByID sectionDataByID;

};


#endif HEADER_GUARD_QT_EXT__FLEXITABWIDGET_H__