#include <timeline/Model.h>
#include <timeline/Channel.h>
#include <timeline/Tag.h>
#include <timeline/SimpleSelection.h>
#include <timeline/TagSelection.h>
#include <boost/foreach.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

//! Konstruktor zerujacy
Model::Model(const std::string & name) : Channel(name)
{

}

Model::~Model()
{

}

void Model::setMask(const Channel::Mask & mask)
{
    setMask(mask);

    notify();
}

void Model::setMaskBegin(double maskBegin)
{
    setMaskBegin(maskBegin);

    notify();
}

void Model::setMaskEnd(double maskEnd)
{
    setMaskEnd(maskEnd);

    notify();
}

void Model::setTime(double time)
{
    setTime(time);

    //TODO
    //update time

    notify();
}

void Model::setLength(double length)
{
    setLength(length);

    //TODO
    //update children

    notify();
}

void Model::setLocalTimeScale(double timeScale)
{
    setLocalTimeScale(timeScale);

    //TODO
    //update children

    notify();
}

bool Model::isPlaying() const
{
    return state.isPlaying;
}

void Model::setPlaying(bool play)
{
    if(play != state.isPlaying){
        state.isPlaying = play;
        notify();
    }
}

const State & Model::getState() const
{
    return state;
}

void Model::setState(const State & state)
{
    this->state = state;

    notify();
}

void Model::addTag(const ChannelConstPtr & channel, double time, const std::string & name)
{
    if(verifyChannel(channel) == false) {
        throw std::invalid_argument("Channel belongs to another model!");
    }

    ChannelPtr wChannel = getWritableChannel(channel);
    TagPtr tag(new Tag(wChannel, time, name));
    
    wChannel->addTag(tag);

    allTags.push_back(tag);
    constAllTags.push_back(tag);

    notify();
}

void Model::removeTag(const TagConstPtr & tag)
{
    ChannelPtr wChannel(getWritableChannel(tag->getChannel().lock()));

    if(tag->begin() != tag->end()){
        std::runtime_error("Cant remove tag with selections based on it!");
    }

    TagPtr wTag(getWritableTag(tag));

    wTag->resetChannel();

    wChannel->removeTag(wTag);

    allTags.resize(std::distance(allTags.begin(), std::remove(allTags.begin(), allTags.end(), tag)));
    constAllTags.resize(std::distance(constAllTags.begin(), std::remove(constAllTags.begin(), constAllTags.end(), tag)));

    notify();
}

Model::tag_const_iterator Model::beginAllTags() const
{
    return constAllTags.begin();
}

Model::tag_const_iterator Model::endAllTags() const
{
    return constAllTags.end();
}

const TagConstPtr & Model::getAllTags(Model::tag_size_type idx) const
{
    return constAllTags[idx];
}

Model::tag_size_type Model::sizeAllTags() const
{
    return constAllTags.size();
}

void Model::addSelection(const ChannelConstPtr & channel, double begin, double end, const std::string & name)
{
    if(verifyChannel(channel) == false) {
        throw std::invalid_argument("Channel belongs to another model!");
    }

    ChannelPtr wChannel = getWritableChannel(channel);

    SelectionPtr selection(new SimpleSelection(wChannel, begin, end, name));
    wChannel->addSelection(selection);

    allSelections.push_back(selection);
    constAllSelections.push_back(selection);

    notify();
}

void Model::removeSelection(const SelectionConstPtr & selection)
{
    auto it = std::find(constAllSelections.begin(), constAllSelections.end(), selection);
    if(it == constAllSelections.end()){
        throw std::invalid_argument("Selection not exist in this model!");
    }

    SelectionPtr wSelection(getWritableSelection(selection));

    wSelection->resetChannel();

    ChannelPtr wChannel(wSelection->getChannel().lock());

    boost::shared_ptr<TagSelection> tagSel(boost::dynamic_pointer_cast<TagSelection>(wSelection));

    //jesli zaznaczenie na bazie tagow wyrejestruj to zaznaczenie z obu tagow
    if(tagSel != nullptr){
        getWritableTag(tagSel->getBeginTag().lock())->removeSelection(wSelection);
        getWritableTag(tagSel->getEndTag().lock())->removeSelection(wSelection);
    }

    wChannel->removeSelection(getWritableSelection(selection));

    allSelections.resize(std::distance(allSelections.begin(), std::remove(allSelections.begin(), allSelections.end(), selection)));
    constAllSelections.resize(std::distance(constAllSelections.begin(), std::remove(constAllSelections.begin(), constAllSelections.end(), selection)));

    notify();
}

Model::selection_const_iterator Model::beginAllSelections() const
{
    return constAllSelections.begin();
}

Model::selection_const_iterator Model::endAllSelections() const
{
    return constAllSelections.end();
}

const SelectionConstPtr & Model::getAllSelection(Model::selection_size_type idx) const
{
    return constAllSelections[idx];
}

Model::selection_size_type Model::sizeAllSelections() const
{
    return constAllSelections.size();
}

void Model::addChannel(const std::string & path, const IChannelPtr & channel)
{
    addChild(path);
    getChannel(path)->setData(channel);
}

void Model::removeChannel(const ChannelConstPtr & channel)
{
    if(shared_from_this() == channel){
        throw std::invalid_argument("Timeline can NOT delete itself!");
    }

    if(verifyChannel(channel) == false){
        throw std::invalid_argument("TChannel not exist in this model!");
    }

    //TODO
    //zebrac wszystkie podkanaly, tagi, zaznaczenia i usunac je
    //potem usunac ten kanal z rodzica oraz z mapowania!!
}

void Model::removeChannel(const std::string & path)
{
    removeChannel(getChannel(path));
}

void Model::setChannelOffset(const ChannelConstPtr & channel, double offset)
{
    //TODO
    //update channel offset
    //propagate it up

    notify();
}

void Model::setChannelLength(const ChannelConstPtr & channel, double length)
{
    //TODO
    //update channel length
    //propagate it up

    notify();
}

void Model::setChannelTimeScale(const ChannelConstPtr & channel, double timeScale)
{
    //TODO
    //update channel time scale
    //propagate it up

    notify();
}

void Model::setChannelName(const ChannelConstPtr & channel, const std::string & name)
{    
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(channel->getName() == name){
        return;
    }

    getWritableChannel(channel)->setName(name);

    notify();
}

void Model::setChannelMask(const ChannelConstPtr & channel, const Channel::Mask & mask)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(channel->getMask() == mask){
        return;
    }

    getWritableChannel(channel)->setMask(mask);

    notify();
}

void Model::setChannelMaskBegin(const ChannelConstPtr & channel, double maskBegin)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(channel->getMaskBegin() == maskBegin){
        return;
    }

    getWritableChannel(channel)->setMaskBegin(maskBegin);

    notify();
}

void Model::setChannelMaskEnd(const ChannelConstPtr & channel, double maskEnd)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(channel->getMaskEnd() == maskEnd){
        return;
    }

    getWritableChannel(channel)->setMaskEnd(maskEnd);

    notify();
}

void Model::setChannelActive(const ChannelConstPtr & channel, bool active)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(channel->isActive() == active){
        return;
    }

    getWritableChannel(channel)->setActive(active);

    notify();
}

void Model::clearChannelTags(const ChannelConstPtr & channel)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    ChannelPtr wChannel(getWritableChannel(channel));
    for(auto it = wChannel->beginTags(); it != wChannel->endTags(); it++) {
        allTags.resize(std::distance(allTags.begin(), std::remove(allTags.begin(), allTags.end(), *it)));
        constAllTags.resize(std::distance(constAllTags.begin(), std::remove(constAllTags.begin(), constAllTags.end(), *it)));
    }

    wChannel->clearTags();

    notify();
}

void Model::clearChannelSelections(const ChannelConstPtr & channel)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    ChannelPtr wChannel(getWritableChannel(channel));
    for(auto it = wChannel->beginSelections(); it != wChannel->endSelections(); it++) {
        allSelections.resize(std::distance(allSelections.begin(),
            std::remove(allSelections.begin(), allSelections.end(), *it)));

        constAllSelections.resize(std::distance(constAllSelections.begin(),
            std::remove(constAllSelections.begin(), constAllSelections.end(), *it)));
    }

    wChannel->clearSelections();

    notify();
}

void Model::splitChannel(const ChannelConstPtr & channel, double time, const std::string & nameA, const std::string & nameB)
{
    //TODO
    UTILS_ASSERT((false), "Do implementacji!");
}

void Model::mergeChannels(const ChannelConstPtr & channelA, const ChannelConstPtr & channelB, const std::string & name)
{
    //TODO
    UTILS_ASSERT((false), "Do implementacji!");
}

void Model::setTagName(const TagConstPtr & tag, const std::string & name)
{
    //verify tag
    ChannelConstPtr channel(tag->getChannel().lock());

    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(tag->getName() == name){
        return;
    }

    getWritableTag(tag)->setName(name);

    notify();
}

void Model::setTagTime(const TagConstPtr & tag, double time)
{
    //verify tag
    if(std::find(allTags.begin(), allTags.end(), tag) == allTags.end()){
        throw std::invalid_argument("Tag not exist in this model!");
    }

    getWritableTag(tag)->setTime(time);

    notify();
}

void Model::setSelectionName(const SelectionConstPtr & selection, const std::string & name)
{
    //verify tag
    ChannelConstPtr channel(selection->getChannel().lock());

    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(selection->getName() == name){
        return;
    }

    getWritableSelection(selection)->setName(name);

    notify();
}

void Model::setSelectionBegin(const SelectionConstPtr & selection, double beginTime)
{
    //verify tag
    ChannelConstPtr channel(selection->getChannel().lock());

    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(selection->getBegin() == beginTime){
        return;
    }

    getWritableSelection(selection)->setBegin(beginTime);

    notify();
}

void Model::setSelectionEnd(const SelectionConstPtr & selection, double endTime)
{
    //verify tag
    ChannelConstPtr channel(selection->getChannel().lock());

    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(selection->getEnd() == endTime){
        return;
    }

    getWritableSelection(selection)->setBegin(endTime);

    notify();
}

void Model::shiftSelection(const SelectionConstPtr & selection, double dTime)
{
    //verify tag
    ChannelConstPtr channel(selection->getChannel().lock());

    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(dTime == 0){
        return;
    }

    getWritableSelection(selection)->shiftSelection(dTime);

    notify();
}

ChannelPtr Model::getChannel(const std::string & path)
{
    return boost::const_pointer_cast<Channel>(getChannel(getChild(path)));
}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////