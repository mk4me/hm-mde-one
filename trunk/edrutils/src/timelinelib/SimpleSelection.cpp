#include <timelinelib/SimpleSelection.h>
#include <timelinelib/Channel.h>

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

SimpleSelection::SimpleSelection(const ChannelPtr & channel, double begin, double end, const std::string & name)
    : SelectionBase(channel, name), begin(begin), end(end)
{
    UTILS_ASSERT((begin >= 0 && end <= channel->getLength() && begin <= end), "Nieprawidlowe ramy czasowe zaznaczenia");
}

SimpleSelection::~SimpleSelection(void)
{
}

double SimpleSelection::getBegin() const
{
    return begin;
}

double SimpleSelection::getEnd() const
{
    return end;
}

void SimpleSelection::setBegin(double begin)
{
    if(channel.lock() == nullptr || (begin >= 0 && begin <= end)){
        this->begin = begin;
    }else{
        throw std::invalid_argument("Wrong selection begin time!");
    }
}

void SimpleSelection::setEnd(double end)
{
    if(channel.lock() == nullptr || (end >= begin && end <= channel.lock()->getLength())){
        this->end = end;
    }else{
        throw std::invalid_argument("Wrong selection end time!");
    }
}

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////