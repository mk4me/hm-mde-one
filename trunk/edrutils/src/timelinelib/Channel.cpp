#include <timeline/Channel.h>
#include <timeline/Tag.h>
#include <timeline/SelectionBase.h>
#include <timeline/TagSelection.h>
#include <utils/Debug.h>
#include <timeline/IChannel.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

Channel::Channel(const std::string & name, const IChannelPtr & channel) : NamedTree<IChannel, PtrPolicyBoost>(name, channel),
    length(0), mask(0,0), localOffset(0.0), globalOffset(0.0), localScale(1.0), globalScale(1.0), time(0.0), active(true)
{
    if(channel != nullptr){
        length = mask.second = channel->getLength();
    }
}

Channel::~Channel()
{
}

const Channel::Mask & Channel::getMask() const
{ 
    return mask;
}

void Channel::setMask(const Channel::Mask & mask) 
{ 
    UTILS_ASSERT((mask.first >= 0 && mask.second <= getLength() && mask.first<= mask.second), "Nieprawidlowa maska dla kanalu");
    this->mask = mask; 
}    

double Channel::getMaskBegin() const
{ 
    return mask.first;
}

void Channel::setMaskBegin(double maskBegin) 
{ 
    UTILS_ASSERT((maskBegin >= 0 && maskBegin <= mask.second), "Poczatek maski dla kanalu musi byc w przedziale 0-maskEnd");
    this->mask.first = maskBegin; 
}

double Channel::getMaskEnd() const
{ 
    return mask.second;
}

void Channel::setMaskEnd(double maskEnd) 
{ 
    UTILS_ASSERT((maskEnd <= getLength() && maskEnd >= mask.first), "Koniec maski dla kanalu musi byc w przedziale maskBegin-length");
    this->mask.second = maskEnd; 
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

double Channel::getTime() const
{
    return time;
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
        throw new std::runtime_error("Tag not exist");
    }
}

const TagConstPtr & Channel::getTag(const std::string & tagName) const
{
    auto it = findByName(constTags.begin(), constTags.end(), tagName);
    if(it != constTags.end()){
        return *it;
    }else{
        throw new std::runtime_error("Tag not exist");
    }
}

Channel::tag_size_type Channel::getNumTags() const
{
    return constTags.size();
}


Channel::selection_iterator Channel::beginSelections()
{
    return selections.begin();
}

Channel::selection_iterator Channel::endSelections()
{
    return selections.end();
}

Channel::selection_const_iterator Channel::beginSelections() const
{
    return constSelections.begin();
}

Channel::selection_const_iterator Channel::endSelections() const
{
    return constSelections.end();
}

const SelectionPtr & Channel::getSelection(selection_size_type idx)
{
    return selections[idx];
}

const SelectionConstPtr & Channel::getSelection(selection_size_type idx) const
{
    return constSelections[idx];
}

const SelectionPtr & Channel::getSelection(const std::string & selName)
{
    auto it = findByName(selections.begin(), selections.end(), selName);
    if(it != selections.end()){
        return *it;
    }else{
        throw new std::runtime_error("Selection not exist");
    }
}

const SelectionConstPtr & Channel::getSelection(const std::string & selName) const
{
    auto it = findByName(constSelections.begin(), constSelections.end(), selName);
    if(it != constSelections.end()){
        return *it;
    }else{
        throw new std::runtime_error("Selection not exist");
    }
}

Channel::selection_size_type Channel::getNumSelections() const
{
    return constSelections.size();
}

void Channel::setLocalOffset(double offset)
{
    double d = this->localOffset - offset;
    if(isRoot() == true){
        this-> localOffset = this->globalOffset = offset;

        //update globalne offsety dzieci
        updateChildrenOffset(d);
    }else{
        if(offset < 0){
            //aktualizuj offset rodzica - on powinien aktualizowac globalne offsety dzieci
            getChannel(getParent().lock())->updateParentOffset(offset);

            //ustaw lokalny offset
            this->localOffset = 0;
        
        }else{
            //aktualizuj globalne offsety dzieci
            updateChildrenOffset(d);
        }

        //aktualizuj dlugosc rodzica
        getChannel(getParent().lock())->updateParentLength(getChannel(shared_from_this()));
    }
}

void Channel::setGlobalOffset(double offset)
{
    setLocalOffset(this->globalOffset - offset);    
}

void Channel::setTime(double time)
{
    UTILS_ASSERT((time <= getLength() && time >= 0), "Niepoprawny czas kanalu");
    //TODO
    //propaguj czas strumienia lub podstrumieni
}

void Channel::setLocalTimeScale(double scale)
{
    UTILS_ASSERT((scale != 0), "Nieprawidlowa skala czasu");

    //ile zmianila sie skala
    double scaleRatio = this->localScale / scale;

    //aktualizuj lokalna skale
    this->localScale = scale;

    //aktualizuj dlugosc kanalu
    this->length *= scaleRatio;

    //aktualizuj globalna skale
    if(isRoot() == true){
        //root ma lokalna i globalna skale taka sama
        this->globalScale = this->localScale;
    }else{

        this->globalScale *= scaleRatio;

        //aktualizuj rodzica ze wzgledu na dlugosc
        getChannel(getParent().lock())->updateParentLength(getChannel(shared_from_this()));
    }

    //Aktrualizuj pozycje Tagow i Selekcji!!
    //Aktualizuj maske!!
    updateForScaleRatio(scaleRatio);

    //Aktualizuj skale dzieci
    updateChildrenScale(scaleRatio);
}

void Channel::updateForScaleRatio(double ratio)
{
    //aktualizuj czas maski
    mask.first *= ratio;
    mask.second *= ratio;

    //aktualizuj zaznaczenia - czesc automatycznie zaktualizuje tagi
    //tutaj zapamierujemy te tagi by ich ponownie nie modyfikowac

    std::vector<TagConstPtr> updatedTags;

    for(auto it = beginSelections(); it != endSelections(); it++){
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
}

void Channel::updateChildrenScale(double ratio)
{
    for(auto it = begin(); it != end(); it++){
        ChannelPtr channel(getChannel(*it));
        channel->localScale *= ratio;
        channel->globalScale *= ratio;
        channel->length *= ratio;
        channel->updateForScaleRatio(ratio);
        channel->updateChildrenScale(ratio);
    }
}

void Channel::updateParentLength(const ChannelPtr & child)
{
    bool goUp = false;
    //czy dziecko dluzsze od rodzica
    if(child->getLocalOffset() + child->getLength() > length){
        //rozszerz rodzica, idz wyzej
        length += child->getLocalOffset() + child->getLength() - length;
        goUp = true;
    }else{
        double l = 0;
        for(auto it = begin(); it != end(); it++){
            ChannelPtr channel(getChannel(*it));
            l = std::max(l, channel->getLocalOffset() + channel->getLength());
        }

        if(l < length){
            length = l;
            goUp = true;
        }
    }

    if(goUp == true && isRoot() == false){
        getChannel(getParent().lock())->updateParentLength(getChannel(shared_from_this()));
    }
}

void Channel::updateChildrenOffset(double dOffset)
{
    for(auto it = begin(); it != end(); it++){
        ChannelPtr channel(getChannel(*it));
        channel->globalOffset += dOffset;
        channel->updateChildrenOffset(dOffset);
    }
}

void Channel::updateParentOffset(double offset)
{
    if(isRoot() == true || this->localOffset + offset >= 0){
        this->localOffset += offset;
        this->globalOffset += offset;
        if(isRoot() == false){
            updateParentLength(getChannel(shared_from_this()));
        }

        updateChildrenOffset(offset);
    }else{
        getChannel(getParent().lock())->updateParentOffset(this->localOffset + offset);
    }
}

void Channel::setGlobalTimeScale(double scale)
{
    UTILS_ASSERT((scale != 0), "Nieprawidlowa skala czasu");

    if(isRoot() == true){
        setLocalTimeScale(scale);
    }else{
        setLocalTimeScale(scale / getChannel(getParent().lock())->getGlobalTimeScale());
    }
}

ChannelPtr Channel::getChannel(const NamedTreeBasePtr & node)
{
    return boost::dynamic_pointer_cast<Channel>(node);
}

void Channel::setActive(bool active)
{
    this->active = active;
}

void Channel::addTag(const TagPtr & tag)
{
    UTILS_ASSERT((tag->getTime() <= getLength()));
    if(std::find(tags.begin(), tags.end(), tag) == tags.end()){
        tags.push_back(tag);
        constTags.push_back(tag);
    }else{
        throw new std::runtime_error("Tag already exist");
    }
}

void Channel::removeTag(const TagPtr & tag)
{
    auto it = std::remove(tags.begin(), tags.end(), tag);
    if(it != tags.end()){
        tags.resize(std::distance(tags.begin(), it));
        constTags.resize(std::distance(constTags.begin(), std::remove(constTags.begin(), constTags.end(), tag)));
    }else{
        throw new std::runtime_error("Tag not exist");
    }
}

void Channel::addSelection(const SelectionPtr & selection)
{
    UTILS_ASSERT((selection->getBegin() >= 0.0 && selection->getEnd() <= getLength()));
    if(std::find(selections.begin(), selections.end(), selection) == selections.end()){
        selections.push_back(selection);
    }else{
        throw new std::runtime_error("Selection already exist");
    }
}

void Channel::removeSelection(const SelectionPtr & selection)
{
    Selections::iterator it = std::remove(selections.begin(), selections.end(), selection);
    if(it != selections.end()){
        selections.resize(std::distance(selections.begin(), it));
    }else{
        throw new std::runtime_error("Selection not exist");
    }
}

void Channel::clearTags()
{
    tags.swap(Tags());
    constTags.swap(ConstTags());
}

void Channel::clearSelections()
{
    selections.swap(Selections());
    constSelections.swap(ConstSelections());
}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////