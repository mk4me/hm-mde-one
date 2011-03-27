#include <timelinelib/Tag.h>
#include <timelinelib/Channel.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

Tag::Tag(const ChannelPtr & channel, double time, const std::string & name)
    : name(name), channel(channel), constChannel(channel), time(time)
{
    UTILS_ASSERT((channel != nullptr), "Niepoprawny kanal taga");
    UTILS_ASSERT((time >= 0 && time <= channel->getLength()), "Niepoprawny czas wsytapienia taga w kanale");
}

const std::string & Tag::getName() const
{
    return name;
}

const ChannelConstWPtr & Tag::getChannel() const
{
    return constChannel;
}

const ChannelWPtr & Tag::getChannel()
{
    return channel;
}

double Tag::getTime() const
{
    return time;
}

Tag::iterator Tag::begin()
{
    return selections.begin();
}

Tag::iterator Tag::end()
{
    return selections.end();
}

Tag::size_type Tag::size() const
{
    return selections.size();
}

Tag::const_iterator Tag::begin() const
{
    return constSelections.begin();
}

Tag::const_iterator Tag::end() const
{
    return constSelections.end();
}

void Tag::setName(const std::string & name)
{
    if(this->name == name){
        return;
    }

    if(constChannel.lock() == nullptr || constChannel.lock()->findTag(name) == constChannel.lock()->endTags()){
        this->name = name;
    }else{
        throw std::invalid_argument("Tag with given name already exist in channel!");
    }
}

void Tag::setTime(double time)
{
    if(time < 0 || time > channel.lock()->getLength()) {
        throw std::invalid_argument("Wrong tag time in channel!");
    }

    this->time = time;
}

void Tag::addSelection(const SelectionPtr & selection)
{
    if(std::find(selections.begin(), selections.end(), selection) == selections.end()){
        selections.push_back(selection);
        constSelections.push_back(selection);
    }else{
        throw new std::runtime_error("Selection is already dependent on this tag!");
    }
};

void Tag::removeSelection(const SelectionPtr & selection)
{
    Selections::iterator it = std::remove(selections.begin(), selections.end(), selection);
    if(it != selections.end()){
        selections.resize(std::distance(selections.begin(), it));
        constSelections.resize(std::distance(constSelections.begin(),
            std::remove(constSelections.begin(), constSelections.end(), selection)));
    }else{
        throw new std::runtime_error("Selection is not dependent on this tag!");
    }
};

void Tag::resetChannel()
{
    channel = ChannelPtr();
    constChannel = ChannelConstPtr();
}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////