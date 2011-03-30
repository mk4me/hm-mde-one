#include <timelinelib/Model.h>
#include <timelinelib/Channel.h>
#include <timelinelib/Tag.h>
#include <timelinelib/SimpleSelection.h>
#include <timelinelib/TagSelection.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

//! Konstruktor zerujacy
Model::Model(const std::string & name) : root(new TChannel(name)), constRoot(root)
{

}

Model::~Model()
{

}

const Model::Mask & Model::getMask() const
{
    return constRoot->getData()->getMask();
}

double Model::getMaskBegin() const
{
    return constRoot->getData()->getMaskBegin();
}

double Model::getMaskEnd() const
{
    return constRoot->getData()->getMaskEnd();
}

double Model::getLocalOffset() const
{
    return constRoot->getData()->getLocalOffset();
}

double Model::getGlobalOffset() const
{
    return constRoot->getData()->getGlobalOffset();
}

double Model::getLength() const
{
    return constRoot->getData()->getLength();
}

double Model::getTime() const
{
    return constRoot->getData()->getTime();
}

double Model::getLocalTimeScale() const
{
    return constRoot->getData()->getLocalTimeScale();
}

double Model::getGlobalTimeScale() const
{
    return constRoot->getData()->getGlobalTimeScale();
}

bool Model::isActive() const
{
    return constRoot->getData()->isActive();
}

void Model::setMask(const Channel::Mask & mask)
{
    root->getData()->setMask(mask);

    notify();
}

void Model::setMaskBegin(double maskBegin)
{
    root->getData()->setMaskBegin(maskBegin);

    notify();
}

void Model::setMaskEnd(double maskEnd)
{
    root->getData()->setMaskEnd(maskEnd);

    notify();
}

void Model::setTime(double time)
{
    double t = time + root->getData()->getGlobalOffset();

    propagateTime(constRoot, t);

    notify();
}

void Model::propagateTime(const TChannelConstPtr & child, double time)
{
    getWritableChannel(child)->setTime(time);

    if(child->getData()->getTime() == time){
        for(auto it = child->begin(); it != child->end(); it++){
            propagateTime(toChannel(*it), time);
        }
    }
}

void Model::setLocalTimeScale(double timeScale)
{
    setChannelLocalTimeScale(constRoot, timeScale);

    notify();
}

void Model::setGlobalTimeScale(double timeScale)
{
    setChannelGlobalTimeScale(constRoot, timeScale);

    notify();
}

void Model::setLocalOffset(double offset)
{
    setChannelLocalOffset(constRoot, offset);

    notify();
}


void Model::setGlobalOffset(double offset)
{
    setChannelGlobalOffset(constRoot, offset);

    notify();
}

void Model::setActive(bool active)
{
    root->getData()->setActive(active);

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

void Model::addTag(const TChannelConstPtr & channel, double time, const std::string & name)
{
    if(verifyChannel(channel) == false) {
        throw std::invalid_argument("Channel belongs to another model!");
    }

    ChannelPtr wChannel(getWritableChannel(channel));
    TagPtr tag(new Tag(wChannel, time, name));
    
    wChannel->addTag(tag);

    allTags.push_back(tag);
    constAllTags.push_back(tag);

    notify();
}

void Model::removeTag(const TagConstPtr & tag)
{
    if(verifyChannel(tag->getChannel().lock())){
        throw std::invalid_argument("Tag belongs to another model!");
    }
    
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

void Model::addSelection(const TChannelConstPtr & channel, double begin, double end, const std::string & name)
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
    root->addChild(path);
    TChannelPtr child(getWritableTChannel(root->getChild(path)));
    child->setData(ChannelPtr(new Channel(channel)));
    if(channel != nullptr && channel->getLength() != 0){
        updateParentLength(toChannel(child->getParent().lock()), child);
    }
}

void Model::removeChannel(const TChannelConstPtr & channel)
{
    if(root == channel){
        throw std::invalid_argument("Timeline can NOT delete itself!");
    }

    if(verifyChannel(channel) == false){
        throw std::invalid_argument("TChannel not exist in this model!");
    }

    //TODO
    //zebrac wszystkie podkanaly, tagi, zaznaczenia i usunac je
    for(auto it = channel->getData()->beginSelections(); it != channel->getData()->endSelections(); it++){

    }    

    //Usunac ten kanal z rodzica oraz z mapowania!!
    TChannelPtr parent(getWritableTChannel(channel->getParent().lock()));
    parent->removeChild(getWritableTChannel(channel));
    channelToTChannel.erase(channel->getData());
    updateParentLength(parent,channel);
}

void Model::removeChannel(const std::string & path)
{
    removeChannel(findChannel(path));
}

Model::channel_const_iterator Model::beginChannels() const
{
    return constRoot->begin();
}

Model::channel_const_iterator Model::endChannels() const
{
    return constRoot->end();
}

Model::TChannelConstPtr Model::getChannel(channel_size_type idx) const
{
    return toChannel(constRoot->getChild(idx));
}

Model::channel_size_type Model::sizeChannels() const
{
    return constRoot->size();
}

Model::TChannelConstPtr Model::getChannel(const std::string & path) const
{
    return toChannel(constRoot->getChild(path));
}

void Model::setChannelLocalOffset(const Model::TChannelConstPtr & channel, double offset)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    ChannelPtr wChannel(getWritableChannel(channel));

    double d = offset - wChannel->getLocalOffset();

    //aktualizuj globalne offsety dzieci
    updateChildrenOffset(channel, d);

    if(channel->isRoot() == true){
        wChannel->setLocalOffset(offset);
        wChannel->setGlobalOffset(offset);

    }else{
        TChannelConstPtr parent(toChannel(channel->getParent().lock()));
        wChannel->setLocalOffset(offset);
        if(offset < 0) {

            updateParentOffset(parent, channel);

        }else{
            //aktualizuj moje ofsety
            wChannel->setLocalOffset(offset);
            wChannel->setGlobalOffset(wChannel->getGlobalOffset() + d);

            //aktualizuj dlugosc rodzica
            updateParentLength(parent, channel);
        }
    }

    notify();
}

void Model::setChannelGlobalOffset(const Model::TChannelConstPtr & channel, double offset)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(channel->isRoot() == true){
        setChannelLocalOffset(channel, offset);
    }else{
        setChannelLocalOffset(channel, offset - toChannel(channel->getParent().lock())->getData()->getGlobalOffset());
    }
}

void Model::setChannelLocalTimeScale(const Model::TChannelConstPtr & channel, double scale)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    ChannelPtr wChannel(getWritableChannel(channel));

    //ile zmianila sie skala
    double scaleRatio = scale / wChannel->getLocalTimeScale();

    //aktualizuj lokalna skale
    wChannel->setLocalTimeScale(scale);

    //aktualizuj dlugosc kanalu
    wChannel->setLength(wChannel->getLength() * scaleRatio);

    //aktualizuj globalna skale
    if(channel->isRoot() == true){
        //root ma lokalna i globalna skale taka sama
        wChannel->setGlobalTimeScale(wChannel->getLocalTimeScale());
    }else{

        wChannel->setGlobalTimeScale(wChannel->getGlobalTimeScale() * scaleRatio);

        //aktualizuj rodzica ze wzgledu na dlugosc
        updateParentLength(toChannel(channel->getParent().lock()), channel);
    }

    //Aktrualizuj pozycje Tagow i Selekcji!!
    //Aktualizuj maske!!
    updateForScaleRatio(wChannel, scaleRatio);

    //Aktualizuj skale dzieci
    updateChildrenScale(channel, scaleRatio);

    notify();
}

void Model::setChannelGlobalTimeScale(const Model::TChannelConstPtr & channel, double scale)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(channel->isRoot() == true){
        setChannelLocalTimeScale(channel, scale);
    }else{
        setChannelLocalTimeScale(channel, scale / toChannel(channel->getParent().lock())->getData()->getGlobalTimeScale());
    }
}

void Model::setChannelName(const TChannelConstPtr & channel, const std::string & name)
{    
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(channel->getName() == name){
        return;
    }

    getWritableTChannel(channel)->setName(name);

    notify();
}

void Model::setChannelMask(const TChannelConstPtr & channel, const Channel::Mask & mask)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(channel->getData()->getMask() == mask){
        return;
    }

    getWritableChannel(channel)->setMask(mask);

    notify();
}

void Model::setChannelMaskBegin(const TChannelConstPtr & channel, double maskBegin)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(channel->getData()->getMaskBegin() == maskBegin){
        return;
    }

    getWritableChannel(channel)->setMaskBegin(maskBegin);

    notify();
}

void Model::setChannelMaskEnd(const TChannelConstPtr & channel, double maskEnd)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(channel->getData()->getMaskEnd() == maskEnd){
        return;
    }

    getWritableChannel(channel)->setMaskEnd(maskEnd);

    notify();
}

void Model::setChannelActive(const TChannelConstPtr & channel, bool active)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    if(channel->getData()->isActive() == active){
        return;
    }

    getWritableChannel(channel)->setActive(active);

    notify();
}

void Model::clearChannelTags(const TChannelConstPtr & channel)
{
    if(verifyChannel(channel) ==  false){
        throw std::invalid_argument("Channel not exist in this model!");
    }

    ChannelPtr wChannel(getWritableChannel(channel));

    for(auto it = wChannel->beginTags(); it != wChannel->endTags(); it++) {
        if((*it)->size() != 0){
            throw std::runtime_error("Can not delete tags when some selections are based on them!");
        }
    }

    for(auto it = wChannel->beginTags(); it != wChannel->endTags(); it++) {
        allTags.resize(std::distance(allTags.begin(), std::remove(allTags.begin(), allTags.end(), *it)));
        constAllTags.resize(std::distance(constAllTags.begin(), std::remove(constAllTags.begin(), constAllTags.end(), *it)));
    }

    wChannel->clearTags();

    notify();
}

void Model::clearChannelSelections(const TChannelConstPtr & channel)
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

void Model::clearChannel(const TChannelConstPtr & channel)
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

}

void Model::splitChannel(const TChannelConstPtr & channel, double time, const std::string & nameA, const std::string & nameB)
{
    //TODO
    UTILS_ASSERT((false), "Do implementacji!");
}

void Model::mergeChannels(const TChannelConstPtr & channelA, const TChannelConstPtr & channelB, const std::string & name)
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

Model::TChannelConstPtr Model::toChannel(const NamedTreeBaseConstPtr & channel)
{
    TChannelConstPtr ret(boost::dynamic_pointer_cast<const TChannel>(channel));
    return ret;
}

Model::TChannelPtr Model::findChannel(const std::string & path)
{
    return boost::const_pointer_cast<TChannel>(toChannel(root->getChild(path)));
}

bool Model::verifyChannel(const TChannelConstPtr & channel) const
{
    return channel->getRoot().lock() == constRoot;
}

bool Model::verifyChannel(const ChannelConstPtr & channel) const
{
    TChannelConstPtr tChannel(getTChannelForChannel(channel));
    if(tChannel == nullptr || tChannel->getRoot().lock() != root){
        return false;
    }

    return true;
}

Model::TChannelConstPtr Model::getTChannelForChannel(const ChannelConstPtr & channel) const
{
    auto it = channelToTChannel.find(channel);
    if(it != channelToTChannel.end()){
        return it->second;
    }

    return TChannelConstPtr();
}

Model::TChannelPtr Model::getWritableTChannel(const Model::TChannelConstPtr & channel)
{
    return boost::const_pointer_cast<TChannel>(channel);
}

Model::TChannelPtr Model::getWritableTChannel(const NamedTreeBaseConstPtr & channel)
{
    return getWritableTChannel(toChannel(channel));
}

ChannelPtr Model::getWritableChannel(const Model::TChannelConstPtr & channel)
{
    return boost::const_pointer_cast<Channel>(channel->getData());
}

ChannelPtr Model::getWritableChannel(const ChannelConstPtr & channel)
{
    return boost::const_pointer_cast<Channel>(channel);
}

TagPtr Model::getWritableTag(const TagConstPtr & tag)
{
    return boost::const_pointer_cast<Tag>(tag);
}

SelectionPtr Model::getWritableSelection(const SelectionConstPtr & selection)
{
    return boost::const_pointer_cast<SelectionBase>(selection);
}

void Model::updateChildrenScale(const Model::TChannelConstPtr & child, double ratio)
{
    for(auto it = child->begin(); it != child->end(); it++){
        ChannelPtr channel(getWritableChannel(toChannel(*it)));
        //channel->setLocalTimeScale(channel->getLocalTimeScale() * ratio);
        channel->setGlobalTimeScale(channel->getGlobalTimeScale() * ratio);
        channel->setLength(channel->getLength() * ratio);
        updateForScaleRatio(channel, ratio);
        updateChildrenScale(toChannel(*it), ratio);
    }
}

void Model::updateForScaleRatio(const ChannelPtr & channel, double ratio)
{
    //aktualizuj czas maski
    channel->setMaskBegin(channel->getMaskBegin() * ratio);
    channel->setMaskEnd(channel->getMaskEnd() * ratio);

    //aktualizuj zaznaczenia - czesc automatycznie zaktualizuje tagi
    //tutaj zapamierujemy te tagi by ich ponownie nie modyfikowac

    std::vector<TagConstPtr> updatedTags;

    for(auto it = channel->beginSelections(); it != channel->endSelections(); it++){
        TagSelectionConstPtr tagSel(TagSelection::getTagSelection(*it));
        if(tagSel != nullptr){
            if(std::find(updatedTags.begin(), updatedTags.end(), tagSel->getBeginTag().lock()) == updatedTags.end()){
                updatedTags.push_back(tagSel->getBeginTag().lock());
            }

            if(std::find(updatedTags.begin(), updatedTags.end(), tagSel->getEndTag().lock()) == updatedTags.end()){
                updatedTags.push_back(tagSel->getEndTag().lock());
            }
        }

        (*it)->setBegin((*it)->getBegin() * ratio);
        (*it)->setEnd((*it)->getEnd() * ratio);
    }

    for(auto it = channel->beginTags(); it != channel->endTags(); it++){
        if(std::find(updatedTags.begin(), updatedTags.end(), *it) == updatedTags.end()){
            (*it)->setTime((*it)->getTime() * ratio);
        }
    }
}

void Model::updateParentLength(const Model::TChannelConstPtr & parent, const TChannelConstPtr & child)
{
    bool goUp = false;
    //czy dziecko dluzsze od rodzica
    if(child->getData()->getLocalOffset() + child->getData()->getLength() > parent->getData()->getLength()){
        //rozszerz rodzica, idz wyzej
        getWritableChannel(parent)->setLength(child->getData()->getLocalOffset() + child->getData()->getLength());
        goUp = true;
    }else{
        goUp = refreshChannelLength(parent);
    }

    if(goUp == true && parent->isRoot() == false){
        updateParentLength(toChannel(parent->getParent().lock()), parent);
    }
}

bool Model::refreshChannelLength(const Model::TChannelConstPtr & channel)
{
    bool ret = false;
    
    double l = 0;
    for(auto it = channel->begin(); it != channel->end(); it++){
        TChannelConstPtr child(toChannel(*it));
        l = std::max(l, child->getData()->getLocalOffset() + child->getData()->getLength());
    }

    if(l != channel->getData()->getLength()){
        getWritableChannel(channel)->setLength(l);
        ret = true;
    }

    return ret;
}

void Model::updateChildrenOffset(const Model::TChannelConstPtr & child, double dOffset)
{
    for(auto it = child->begin(); it != child->end(); it++){
        TChannelConstPtr tChannel(toChannel(*it));
        ChannelPtr channel(getWritableChannel(tChannel));
        channel->setGlobalOffset(channel->getGlobalOffset() + dOffset);
        updateChildrenOffset(tChannel, dOffset);
    }
}

void Model::updateParentOffset(const Model::TChannelConstPtr & parent, const Model::TChannelConstPtr & child)
{
    ChannelPtr wChild(getWritableChannel(child));
    ChannelPtr wParent(getWritableChannel(parent));

    double offset = wChild->getLocalOffset();

    //aktualizuj localne przesuniecia dzieci
    for(auto it = parent->begin(); it != parent->end(); it++){
        ChannelPtr wCh(getWritableChannel(toChannel(*it)));
        wCh->setLocalOffset(wCh->getLocalOffset() - offset);
    }

    //rodzic
    //moglismy przesunac jego dziecko najdalsze, ale koniec oparl sie na kolejnym
    refreshChannelLength(parent);
    wParent->setLocalOffset(wParent->getLocalOffset() + offset);
    wParent->setGlobalOffset(wParent->getGlobalOffset() + offset);

    //specjalny przypadek - dziecko ktore spowodowalo przesuniecie
    //nie moze sie zaczynac wczesniej niz rodzic
    wChild->setGlobalOffset(wParent->getGlobalOffset());

    if(parent->isRoot() == false && wParent->getLocalOffset() < 0){
        updateParentOffset(toChannel(parent->getParent().lock()), parent);
    }
}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////