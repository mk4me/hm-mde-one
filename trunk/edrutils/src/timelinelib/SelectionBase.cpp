#include <timelinelib/SelectionBase.h>
#include <timelinelib/Channel.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

SelectionBase::SelectionBase(const ChannelPtr & channel, const std::string & name)
    : channel(channel), constChannel(channel), name(name)
{
    UTILS_ASSERT((channel != nullptr), "Nieprawidlowy strumien dla zaznaczenia");
}

SelectionBase::~SelectionBase()
{
}


const std::string & SelectionBase::getName() const
{
    return name;
}

const ChannelConstWPtr & SelectionBase::getChannel() const
{
    return constChannel;
}

const ChannelWPtr & SelectionBase::getChannel()
{
    return channel;
}

double SelectionBase::getLength() const
{
    return getEnd() - getBegin();
}

void SelectionBase::setName(const std::string & name)
{
    if(this->name == name){
        return;
    }

    if(constChannel.lock() == nullptr || constChannel.lock()->findSelection(name) == constChannel.lock()->endSelections()){
        this->name = name;
    }else{
        throw std::invalid_argument("Selection with given name already exist in channel!");
    }    
}

void SelectionBase::shiftSelection(double delta)
{
    UTILS_ASSERT(( getBegin() + delta >= 0.0 && getEnd() + delta <= channel.lock()->getLength(),
        "Zle przesuniecie czasowe - zaznaczenie musi byc w przedziale czasowym strumienia rodzica"));
    setBegin(getBegin() + delta);
    setEnd(getEnd() + delta);
}

void SelectionBase::resetChannel()
{
    constChannel = channel = ChannelPtr();
}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////