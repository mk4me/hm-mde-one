#include <timelinelib/TagSelection.h>
#include <timelinelib/Tag.h>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

TagSelection::TagSelection(const TagPtr & begin, const TagPtr & end, const std::string & name)
    : SelectionBase(begin->channel.lock(), name), begin(begin), end(end)
{
    UTILS_ASSERT((begin != nullptr && end != nullptr && begin != end), "Nieprawidlowe tagi dla zaznaczenia");
    UTILS_ASSERT((begin->getChannel().lock() == end->getChannel().lock()), "Znaczniki pochodza z roznych strumieni - nie mozna utowrzyc zaznaczenia");
}

TagSelection::~TagSelection(void)
{
}

double TagSelection::getBegin() const
{
    return begin.lock()->getTime();
}

double TagSelection::getEnd() const
{
    return end.lock()->getTime();
}

const TagConstWPtr & TagSelection::getBeginTag() const
{
    return constBegin;
}

const TagConstWPtr & TagSelection::getEndTag() const
{
    return constEnd;
}

const TagWPtr & TagSelection::getBeginTag()
{
    return begin;
}

const TagWPtr & TagSelection::getEndTag()
{
    return end;
}

void TagSelection::setBegin(double begin)
{
    this->begin.lock()->setTime(begin);
}

void TagSelection::setEnd(double end)
{
    this->end.lock()->setTime(end);
}

void TagSelection::setBeginTag(const TagPtr & begin)
{
    if(this->begin.lock() == begin){
        return;
    }

    if(begin == end.lock()){
        throw std::invalid_argument("Begin and end tags cannot be the same tag for selection");
    }

    this->begin = begin;
    this->constBegin = begin;
}

void TagSelection::setEndTag(const TagPtr & end)
{
    if(this->end.lock() == end){
        return;
    }

    if(begin.lock() == end){
        throw std::invalid_argument("Begin and end tags cannot be the same tag for selection");
    }

    this->end = end;
    this->constEnd = end;
}

TagSelectionPtr TagSelection::getTagSelection(const SelectionPtr & selection)
{
    return boost::dynamic_pointer_cast<TagSelection>(selection);
}

TagSelectionConstPtr TagSelection::getTagSelection(const SelectionConstPtr & selection)
{
    return boost::dynamic_pointer_cast<const TagSelection>(selection);
}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////