/********************************************************************
    created:  2011/07/18
    created:  18:7:2011   13:56
    filename: ChannelsTreeItem.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__CHANNELSTREEITEM_H__
#define HEADER_GUARD_TIMELINE__CHANNELSTREEITEM_H__

#include <plugins/newTimeline/TimelineService.h>
#include <QtGui/QTreeWidgetItem>
#include <timelinelib/Model.h>
#include "ChannelWidget.h"

//! Klasa reprezentuj�ca kana� w drzewie UI
class ChannelsTreeItem : public QTreeWidgetItem
{
private:
    //! Kana� skojarzony z tym wpisem drzewa
    timeline::Model::TChannelConstPtr channelPtr;
    //! Widget z wizualna reprezentacj� czasu kana�u wzgl�dem roota
    ChannelWidget * channelWidget;

public:
    ChannelsTreeItem(const timeline::Model::TChannelConstPtr & channelPtr, ChannelWidget * channelWidget);
    ~ChannelsTreeItem();

    //! \return Kana� timeline skojarzony z tym wpisem
    const timeline::Model::TChannelConstPtr & getChannel() const;

    void refreschVisualChannel();
};

#endif  //  HEADER_GUARD_TIMELINE__CHANNELSTREEITEM_H__