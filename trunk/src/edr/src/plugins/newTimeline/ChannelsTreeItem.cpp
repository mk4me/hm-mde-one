#include "TimelinePCH.h"
#include "ChannelsTreeItem.h"

ChannelsTreeItem::ChannelsTreeItem(const timeline::Model::TChannelConstPtr & channelPtr, ChannelWidget * channelWidget)
    : channelPtr(channelPtr), channelWidget(channelWidget)
{
    UTILS_ASSERT((channelPtr != nullptr), "B��dny kana� modelu timeline");
    UTILS_ASSERT((channelWidget != nullptr), "B��dny widget wizualizuj�cy kana�");
    setText(1, QString::fromUtf8(channelPtr->getName().c_str()));
}

ChannelsTreeItem::~ChannelsTreeItem()
{

}

const timeline::Model::TChannelConstPtr & ChannelsTreeItem::getChannel() const
{
    return channelPtr;
}

void ChannelsTreeItem::refreshVisualChannelStructure(double globalOffset, double globalLength)
{
    double pom = channelPtr->getData()->getGlobalOffset() - globalOffset;

    channelWidget->setRange(pom / globalLength, (pom + channelPtr->getData()->getLength()) / globalLength);
}

void ChannelsTreeItem::refreshVisualChannelTime(double globalOffset, double globalLength)
{
    channelWidget->setCurrentTime((channelPtr->getData()->getTime() - globalOffset) / globalLength);
}
