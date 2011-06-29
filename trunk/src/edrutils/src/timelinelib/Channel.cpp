#include <timelinelib/Channel.h>
#include <timelinelib/Tag.h>
//#include <timelinelib/SelectionBase.h>
//#include <timelinelib/TagSelection.h>
#include <timelinelib/IChannel.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

Channel::Channel(const IChannelPtr & channel) : length(0),
    mask(0,0), localOffset(0.0), globalOffset(0.0), localScale(1.0), globalScale(1.0),
    time(0.0), active(true), innerChannel(channel), constInnerChannel(channel)
{
    if(channel != nullptr){
        length = mask.second = channel->getLength();
    }
}

Channel::Channel(const Channel & channel, bool deep)
{
    //TODO
    //kopiowanie propertow kanalu
}

Channel::~Channel()
{
}

bool Channel::timeInChannel(double time) const
{
    if(time >= globalOffset + mask.first && time <= globalOffset + mask.second){
        return true;
    }

    return false;
}

void Channel::setInnerChannel(const IChannelPtr & channel)
{
    constInnerChannel = innerChannel = channel;
    if(globalScale * channel->getLength() > length){
        length = globalScale * channel->getLength();
    }

    setTime(time);
}

const IChannelConstPtr & Channel::getInnerChannel() const
{
    return constInnerChannel;
}

const IChannelPtr & Channel::getInnerChannel()
{
    return innerChannel;
}

const Channel::Mask & Channel::getMask() const
{ 
    return mask;
}

void Channel::setMask(const Channel::Mask & mask) 
{ 
    UTILS_ASSERT((mask.first >= 0 && mask.second >= 0 && mask.first + mask.second <= getLength()), "Nieprawidlowa maska dla kanalu");
    this->mask = mask; 
}    

double Channel::getMaskBegin() const
{ 
    return mask.first;
}

void Channel::setMaskBegin(double maskBegin) 
{ 
    UTILS_ASSERT((maskBegin >= 0 && maskBegin + mask.second <= getLength()), "Poczatek maski sprawi ze maska wyjdzie poza kanal");
    this->mask.first = maskBegin; 
}

double Channel::getMaskLength() const
{
    return mask.second;
}

double Channel::getMaskEnd() const
{ 
    return mask.first + mask.second;
}

void Channel::setMaskEnd(double maskEnd) 
{ 
    UTILS_ASSERT((maskEnd <= getLength() && maskEnd >= mask.first), "Koniec maski dla kanalu musi byc w przedziale maskBegin-length");
    this->mask.second = maskEnd - mask.first; 
}

double Channel::getLocalOffset() const
{
    return localOffset;
}

double Channel::getGlobalOffset() const
{
    return globalOffset;
}

double Channel::getLength() const
{
    return length;
}

void Channel::setLength(double length)
{
    this->length = length;
}

double Channel::getTime() const
{
    return time;
}

void Channel::setTime(double time){
    this->time = time;
    if(active == true && innerChannel != nullptr && time >= globalOffset + mask.first && time <= globalOffset + mask.second){
        innerChannel->setTime((this->time - globalOffset - mask.first) / globalScale);
    }
}

double Channel::getLocalTimeScale() const
{
    return localScale;
}

double Channel::getGlobalTimeScale() const
{
    return globalScale;
}

bool Channel::isActive() const
{
    return active;
}

Channel::tag_iterator Channel::beginTags()
{
    return tags.begin();
}

Channel::tag_iterator Channel::endTags()
{
    return tags.end();
}

Channel::tag_const_iterator Channel::beginTags() const
{
    return constTags.begin();
}

Channel::tag_const_iterator Channel::endTags() const
{
    return constTags.end();
}

Channel::tag_const_iterator Channel::findTag(const std::string & name) const
{
    return std::find_if(constTags.begin(), constTags.end(), [&](TagConstPtr tag){ return tag->getName() == name; });
}

const TagPtr & Channel::getTag(tag_size_type idx)
{
    return tags[idx];
}

const TagConstPtr & Channel::getTag(tag_size_type idx) const
{
    return constTags[idx];
}

const TagPtr & Channel::getTag(const std::string & tagName)
{
    auto it = findByName(tags.begin(), tags.end(), tagName);
    if(it != tags.end()){
        return *it;
    }else{
        throw std::runtime_error("Tag not exist");
    }
}

const TagConstPtr & Channel::getTag(const std::string & tagName) const
{
    auto it = findByName(constTags.begin(), constTags.end(), tagName);
    if(it != constTags.end()){
        return *it;
    }else{
        throw std::runtime_error("Tag not exist");
    }
}

Channel::tag_size_type Channel::getNumTags() const
{
    return constTags.size();
}

void Channel::setLocalOffset(double offset)
{
    localOffset = offset;
}

void Channel::setGlobalOffset(double offset)
{
    globalOffset = offset; 
}

void Channel::setLocalTimeScale(double scale)
{
    UTILS_ASSERT((scale != 0), "Nieprawidlowa skala czasu");

    localScale = scale;
}

void Channel::setGlobalTimeScale(double scale)
{
    globalScale = scale;
}

ChannelPtr Channel::getChannel(const NamedTreeBasePtr & node)
{
    return boost::dynamic_pointer_cast<Channel>(node);
}

ChannelConstPtr Channel::getConstChannel(const NamedTreeBaseConstPtr & node)
{
    return boost::dynamic_pointer_cast<const Channel>(node);
}

void Channel::setActive(bool active)
{
    this->active = active;
}

void Channel::addTag(const TagPtr & tag)
{
    UTILS_ASSERT((tag->getBeginTime() >= 0 && tag->getBeginTime() <= getLength()));
    if(std::find(tags.begin(), tags.end(), tag) == tags.end()){
        tags.push_back(tag);
        constTags.push_back(tag);
    }else{
        throw std::runtime_error("Tag already exist");
    }
}

void Channel::removeTag(const TagPtr & tag)
{
    auto it = std::remove(tags.begin(), tags.end(), tag);
    if(it != tags.end()){
        tags.resize(std::distance(tags.begin(), it));
        constTags.resize(std::distance(constTags.begin(), std::remove(constTags.begin(), constTags.end(), tag)));
    }else{
        throw std::runtime_error("Tag not exist");
    }
}

void Channel::clearTags()
{    
    tags.swap(Tags());
    constTags.swap(ConstTags());
}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////