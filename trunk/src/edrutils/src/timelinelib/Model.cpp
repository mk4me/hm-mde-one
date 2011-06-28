#include <timelinelib/Model.h>
#include <timelinelib/Channel.h>
#include <timelinelib/Tag.h>
//#include <timelinelib/SimpleSelection.h>
//#include <timelinelib/TagSelection.h>
#include <stack>
#include <boost/interprocess/sync/scoped_lock.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

//! Konstruktor zerujacy
Model::Model(const std::string & name) : root(new TChannel(name)), constRoot(root), lockHolder(0)
{

}

Model::~Model()
{

}

void Model::lockModel(void * lockHolder) volatile
{
    if(lockHolder == 0){
        throw std::invalid_argument("Model lock holder address can not be equal to 0");
    }

    Model * wThis(const_cast<Model*>(this));

    wThis->modelTimeMutex.lock();
    wThis->lockHolder = lockHolder;
}

void Model::unlockModel(void * lockHolder) volatile
{
    if(lockHolder == 0){
        throw std::invalid_argument("Model lock holder address can not be equal to 0");
    }

    if(this->lockHolder == lockHolder){
        Model * wThis(const_cast<Model*>(this));
        boost::interprocess::scoped_lock<boost::mutex> lock(wThis->lockCheckMutex);
        wThis->lockHolder = 0;
        wThis->modelTimeMutex.unlock();
    }else{
        throw std::invalid_argument("Given lock holder did not locked the model");
    }
}

bool Model::isLocked() const
{
    Model * wThis(const_cast<Model*>(this));
    boost::interprocess::scoped_lock<boost::mutex> lock(wThis->lockCheckMutex);
    return lockHolder != 0;
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

double Model::getOffset() const
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

double Model::getTimeScale() const
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

void Model::breakTimeUpdate(void * lockHolder) volatile
{
    if(this->lockHolder != 0 && this->lockHolder != lockHolder){
        throw std::invalid_argument("Given lock holder did not locked the model");
    }

    Model * wThis(const_cast<Model*>(this));

    wThis->stopTimeUpdate = true;
}

void Model::setTime(double time, void * lockHolder) volatile
{
    if(this->lockHolder != 0 && this->lockHolder != lockHolder){
        throw std::invalid_argument("Given lock holder did not locked the model");
    }

    Model * wThis(const_cast<Model*>(this));

    boost::interprocess::scoped_lock<boost::mutex> lock(wThis->timeUpdateMutex);

    wThis->stopTimeUpdate = false;
    double t = time + wThis->constRoot->getData()->getGlobalOffset();

    //zapewnia ze root zawsdze ma ostatni czas dla ktorego aktualizowano model, dzieci niekoniecznie sa z nim zgodne
    getWritableChannel(wThis->constRoot)->setTime(t);
    
    std::stack<TChannelConstPtr> path;
    std::stack<NamedTreeBase::size_type> pathPos;
    
    path.push(wThis->constRoot);
    while(stopTimeUpdate == false && path.empty() == false) {
        if(path.top()->getData()->isActive() == true && path.top()->getData()->timeInChannel(t) == true){
            getWritableChannel(path.top())->setTime(t);
            if(path.top()->size() == 0){
                path.pop();

                while((path.empty() == false) && (pathPos.empty() == false) && (path.top()->size() == ++pathPos.top())){
                    pathPos.pop();
                    path.pop();
                }
            }else{
                pathPos.push(0);
            }
        }else{
            path.pop();

            while((path.empty() == false) && (pathPos.empty() == false) && (path.top()->size() == ++pathPos.top())){
                pathPos.pop();
                path.pop();
            }
        }

        if(path.empty() == false){
            path.push(toChannel(path.top()->getChild(pathPos.top())));
        }
    }

    wThis->notify();
}

void Model::setNormalizedTime(double normTime, void * lockHolder) volatile
{
    UTILS_ASSERT((normTime >= 0.0 && normTime <= 1.0), "Czas powinien byc znormalizowany");
    
    Model * wThis(const_cast<Model*>(this));
    setTime(normTime * wThis->getLength(), lockHolder);
}

double Model::getNormalizedTime() const
{
    double length = getLength();
    return length == 0 ? 0 : getTime() - getOffset() / length;
}

void Model::setTimeScale(double timeScale) volatile
{
    Model * wThis(const_cast<Model*>(this));

    boost::interprocess::scoped_lock<boost::mutex> lock(wThis->modelTimeMutex);

    wThis->innerSetChannelLocalTimeScale(wThis->constRoot, timeScale);

    wThis->notify();
}

void Model::setOffset(double offset) volatile
{
    Model * wThis(const_cast<Model*>(this));

    boost::interprocess::scoped_lock<boost::mutex> lock(wThis->modelTimeMutex);

    wThis->innerSetChannelLocalOffset(wThis->constRoot, offset);

    wThis->notify();
}

void Model::setActive(bool active)
{
    root->getData()->setActive(active);

    notify();
}

void Model::addTag(const std::string & path, double begin, double length)
{
    ExtPath exPath = getExtPath(path);

    ChannelPtr wChannel(getWritableChannel(getChannel(exPath.first)));
    TagPtr tag(new Tag(wChannel, exPath.second, begin, length));
    
    wChannel->addTag(tag);

    allTags.push_back(tag);
    constAllTags.push_back(tag);

    notify();
}

const TagConstPtr & Model::getTag(const std::string & path) const
{
    ExtPath exPath = getExtPath(path);

    return getChannel(exPath.first)->getData()->getTag(exPath.second);
}

void Model::removeTag(const std::string & path)
{
    ExtPath exPath = getExtPath(path);

    ChannelPtr wChannel(getWritableChannel(getChannel(exPath.first)));

    TagPtr wTag(getWritableTag(wChannel->getTag(exPath.second)));

    wChannel->removeTag(wTag);

    allTags.resize(std::distance(allTags.begin(), std::remove(allTags.begin(), allTags.end(), wTag)));
    constAllTags.resize(std::distance(constAllTags.begin(), std::remove(constAllTags.begin(), constAllTags.end(), wTag)));

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

void Model::addChannel(const std::string & path, const IChannelPtr & channel) volatile
{
    Model * wThis(const_cast<Model*>(this));

    boost::interprocess::scoped_lock<boost::mutex> lock(wThis->modelTimeMutex);
 
    wThis->root->addChild(path);
    TChannelConstPtr tChild(wThis->getChannel(path));
    ChannelPtr child(getWritableChannel(tChild));
    child->setInnerChannel(channel);
    if(channel != nullptr && channel->getLength() != 0){
        updateParentLength(toChannel(tChild->getParent().lock()), tChild);
    }

    wThis->notify();
}

void Model::removeChannel(const std::string & path) volatile
{
    Model * wThis(const_cast<Model*>(this));

    boost::interprocess::scoped_lock<boost::mutex> lock(wThis->modelTimeMutex);

    wThis->innerRemoveChannel(wThis->getChannel(path));

    wThis->notify();
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

Model::TChannelConstPtr Model::findChannel(const std::string & path) const
{
    NamedTreeBaseConstPtr channel(constRoot->findChildByPath(path));

    if(channel == nullptr){
        return TChannelConstPtr();
    }

    return toChannel(channel);
}

void Model::setChannelLocalOffset(const std::string & path, double offset) volatile
{
    Model * wThis(const_cast<Model*>(this));

    boost::interprocess::scoped_lock<boost::mutex> lock(wThis->modelTimeMutex);
    
    if(wThis->findChannel(path) == wThis->root){
        wThis->innerSetChannelLocalOffset(wThis->constRoot, offset);
    }else{
        wThis->innerSetChannelLocalOffset(wThis->getChannel(path), offset);
    }

    wThis->notify();
}
 
void Model::setChannelGlobalOffset(const std::string & path, double offset) volatile
{
    Model * wThis(const_cast<Model*>(this));

    boost::interprocess::scoped_lock<boost::mutex> lock(wThis->modelTimeMutex);

    if(wThis->findChannel(path) == wThis->root){
        wThis->innerSetChannelLocalOffset(wThis->constRoot, offset);
    }else{
        TChannelConstPtr channel(wThis->getChannel(path));
        wThis->innerSetChannelLocalOffset(channel, offset - toChannel(channel->getParent().lock())->getData()->getGlobalOffset());
    }

    wThis->notify();
}

void Model::setChannelLocalTimeScale(const std::string & path, double scale) volatile
{
    Model * wThis(const_cast<Model*>(this));

    boost::interprocess::scoped_lock<boost::mutex> lock(wThis->modelTimeMutex);
    
    if(wThis->findChannel(path) == wThis->root){
        wThis->innerSetChannelLocalTimeScale(wThis->constRoot, scale);
    }else{
        wThis->innerSetChannelLocalTimeScale(wThis->getChannel(path), scale);
    }

    wThis->notify();
}

void Model::setChannelGlobalTimeScale(const std::string & path, double scale) volatile
{
    Model * wThis(const_cast<Model*>(this));

    boost::interprocess::scoped_lock<boost::mutex> lock(wThis->modelTimeMutex);

    if(wThis->findChannel(path) == wThis->root){
        wThis->innerSetChannelLocalTimeScale(wThis->constRoot, scale);
    }else{
        TChannelConstPtr channel(wThis->getChannel(path));
        wThis->innerSetChannelLocalTimeScale(channel, scale / toChannel(channel->getParent().lock())->getData()->getGlobalTimeScale());
    }

    wThis->notify();
}

void Model::setChannelName(const std::string & path, const std::string & name)
{    
    if(findChannel(path) == root){
        innerSetChannelName(constRoot, name);
    }else{
        innerSetChannelName(getChannel(path), name);
    }

    notify();
}

void Model::setChannelMask(const std::string & path, const Channel::Mask & mask)
{
    if(findChannel(path) == root){
        innerSetChannelMask(constRoot, mask);
    }else{
        innerSetChannelMask(getChannel(path), mask);
    }

    notify();
}

void Model::setChannelMaskBegin(const std::string & path, double maskBegin)
{
    if(findChannel(path) == root){
        innerSetChannelMaskBegin(constRoot, maskBegin);
    }else{
        innerSetChannelMaskBegin(getChannel(path), maskBegin);
    }

    notify();
}

void Model::setChannelMaskEnd(const std::string & path, double maskEnd)
{
    if(findChannel(path) == root){
        innerSetChannelMaskEnd(constRoot, maskEnd);
    }else{
        innerSetChannelMaskEnd(getChannel(path), maskEnd);
    }

    notify();
}

void Model::setChannelActive(const std::string & path, bool active)
{
    if(findChannel(path) == root){
        innerSetChannelActive(constRoot, active);
    }else{
        innerSetChannelActive(getChannel(path), active);
    }

    notify();
}

void Model::clearChannelTags(const std::string & path, bool deeper)
{
    if(findChannel(path) == root){
        innerClearChannelTags(constRoot, deeper);
    }else{
        innerClearChannelTags(getChannel(path), deeper);
    }

    notify();
}

//void Model::resetChannelOffsets(const std::string & path, bool deeper)
//{
//    //TODO
//    UTILS_ASSERT((false), "Do implementacji!");
//}
//
//void Model::resetChannelTimeScales(const std::string & path, bool deeper)
//{
//    //TODO
//    UTILS_ASSERT((false), "Do implementacji!");
//}
//
//void Model::resetChannelOffsetsAndTimeScales(const std::string & path, bool deeper)
//{
//    //TODO
//    UTILS_ASSERT((false), "Do implementacji!");
//}

void Model::splitChannel(const std::string & path, double time, const std::string & nameA, const std::string & nameB) 
{
    //TODO
    UTILS_ASSERT((false), "Do implementacji!");
}

void Model::mergeChannels(const std::string & pathA, const std::string & pathB, const std::string & name)
{
    //TODO
    UTILS_ASSERT((false), "Do implementacji!");
}

void Model::setTagName(const std::string & path, const std::string & name)
{
    TagConstPtr tag(getTag(path));

    if(tag->getName() == name){
        return;
    }

    getWritableTag(tag)->setName(name);

    notify();
}

void Model::setTagTime(const std::string & path, double time)
{
    TagConstPtr tag(getTag(path));

    if(tag->getBeginTime() == time){
        return;
    }

    getWritableTag(tag)->setBeginTime(time);

    notify();
}

void Model::setTagLength(const std::string & path, double length)
{
    TagConstPtr tag(getTag(path));

    if(tag->getLength() == length){
        return;
    }

    getWritableTag(tag)->setLength(length);

    notify();
}

void Model::shiftTag(const std::string & path, double dTime)
{
    TagConstPtr tag(getTag(path));

    if(dTime == 0){
        return;
    }

    getWritableTag(tag)->shiftTag(dTime);

    notify();
}

Model::TChannelConstPtr Model::toChannel(const NamedTreeBaseConstPtr & channel)
{
    return boost::dynamic_pointer_cast<const TChannel>(channel);
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

void Model::updateChildrenScale(const Model::TChannelConstPtr & child, double ratio)
{
    for(auto it = child->begin(); it != child->end(); it++){
        ChannelPtr channel(getWritableChannel(toChannel(*it)));
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

    for(auto it = channel->beginTags(); it != channel->endTags(); it++){
        if(std::find(updatedTags.begin(), updatedTags.end(), *it) == updatedTags.end()){
            (*it)->setBeginTime((*it)->getBeginTime() * ratio);
            (*it)->setLength((*it)->getLength() * ratio);
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

Model::ExtPath Model::getExtPath(const std::string & path)
{
    UTILS_ASSERT((path.empty() == false), "Bledna sciezka");

    ExtPath ret("./",path);

    std::string::size_type pos = path.find_last_of("/");

    if(pos != std::string::npos){
        ret.first = std::string(path.begin(), path.begin() + pos);
        ret.second = std::string(path.begin() + pos, path.end());
    }

    return ret;
}

void Model::innerSetChannelLocalOffset(const TChannelConstPtr & channel, double offset)
{
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
}

void Model::innerSetChannelGlobalOffset(const TChannelConstPtr & channel, double offset)
{
    if(channel->isRoot() == false){
        offset -= toChannel(channel->getParent().lock())->getData()->getGlobalOffset();
    }

    innerSetChannelLocalOffset(channel, offset);
}

void Model::innerSetChannelLocalTimeScale(const TChannelConstPtr & channel, double scale)
{
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
}

void Model::innerRemoveChannel(const TChannelConstPtr & channel)
{
    //Usunac ten kanal z rodzica oraz z mapowania!!
    TChannelPtr parent(getWritableTChannel(channel->getParent().lock()));
    parent->removeChild(getWritableTChannel(channel));
    updateParentLength(parent,channel);
    
    ConstTags tags;
//    ConstSelections selections;
    ConstChannels channels;

    //Zebrac dzieci, tagi i zaznaczenia kanalu i jego dzieci
    //usunac je z list wszystkich tagow, zaznaczen i mapowania kanalow
    getAllChildrenData(channel, tags, /*selections,*/ channels);

    for(auto it = tags.begin(); it != tags.end(); it++) {
        allTags.resize(std::distance(allTags.begin(), std::remove(allTags.begin(), allTags.end(), *it)));
        constAllTags.resize(std::distance(constAllTags.begin(), std::remove(constAllTags.begin(), constAllTags.end(), *it)));
    }

    for(auto it = channels.begin(); it != channels.end(); it++){
        channelToTChannel.erase(*it);
    }
}

void Model::getAllChildrenData(const Model::TChannelConstPtr & channel, ConstTags & tags, Model::ConstChannels & channels)
{
    channels.push_back(channel->getData());
    tags.insert(tags.end(), channel->getData()->beginTags(), channel->getData()->endTags());
    for(auto it = channel->begin(); it != channel->end(); it++){
        getAllChildrenData(toChannel(*it), tags, channels);
    }
}

void Model::innerSetChannelGlobalTimeScale(const TChannelConstPtr & channel, double scale)
{
    if(channel->isRoot() == false){
        scale /= toChannel(channel->getParent().lock())->getData()->getGlobalTimeScale();
    }

    innerSetChannelLocalTimeScale(channel, scale);
}

void Model::innerSetChannelName(const TChannelConstPtr & channel, const std::string & name)
{
    if(channel->getName() == name){
        return;
    }

    getWritableTChannel(channel)->setName(name);
}

void Model::innerSetChannelMask(const TChannelConstPtr & channel, const Channel::Mask & mask)
{
    if(channel->getData()->getMask() == mask){
        return;
    }

    getWritableChannel(channel)->setMask(mask);
}

void Model::innerSetChannelMaskBegin(const TChannelConstPtr & channel, double maskBegin)
{
    if(channel->getData()->getMaskBegin() == maskBegin){
        return;
    }

    getWritableChannel(channel)->setMaskBegin(maskBegin);
}

void Model::innerSetChannelMaskEnd(const TChannelConstPtr & channel, double maskEnd)
{
    if(channel->getData()->getMaskEnd() == maskEnd){
        return;
    }

    getWritableChannel(channel)->setMaskEnd(maskEnd);
}

void Model::innerSetChannelActive(const TChannelConstPtr & channel, bool active)
{
    if(channel->getData()->isActive() == active){
        return;
    }

    getWritableChannel(channel)->setActive(active);
}

void Model::innerClearChannelTags(const TChannelConstPtr & channel, bool deeper)
{
    ChannelPtr wChannel(getWritableChannel(channel));

    ConstTags locTags(wChannel->beginTags(),wChannel->endTags());
    wChannel->clearTags();

    for(auto it = locTags.begin(); it != locTags.end(); it++) {
        allTags.resize(std::distance(allTags.begin(), std::remove(allTags.begin(), allTags.end(), *it)));
        constAllTags.resize(std::distance(constAllTags.begin(), std::remove(constAllTags.begin(), constAllTags.end(), *it)));
    }

    if(deeper == true){
        for(auto it = channel->begin(); it != channel->end(); it++){
            innerClearChannelTags(toChannel(*it), true);
        }
    }
}

void Model::innerSplitChannel(const TChannelConstPtr & channel, double time, const std::string & nameA, const std::string & nameB)
{

}

void Model::innerMergeChannels(const TChannelConstPtr & channelA, const TChannelConstPtr & channelB, const std::string & name)
{

}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////