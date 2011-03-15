#ifndef HEADER_GUARD__TIMELINETYPES_H__
#define HEADER_GUARD__TIMELINETYPES_H__

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <vector>

/* Naglowek z deklaracjami typow - deklaracje wyprzedzajace dla klas
* oraz odpowiednie typy kolekcji i wskaznikow dla nich
*/

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

class IStream;
typedef boost::shared_ptr<IStream> IStreamPtr;
typedef boost::weak_ptr<IStream> IStreamWPtr;
typedef std::vector<IStreamPtr> IStreams;

class Stream;
typedef boost::shared_ptr<Stream> StreamPtr;
typedef boost::weak_ptr<Stream> StreamWPtr;
typedef std::vector<StreamPtr> Streams;

class ISelection;
typedef boost::shared_ptr<ISelection> SelectionPtr;
typedef boost::weak_ptr<Selection> SelectionWPtr;
typedef std::vector<SelectionPtr> Selections;

class Tag;
typedef boost::shared_ptr<Tag> TagPtr;
typedef boost::weak_ptr<Tag> TagWPtr;
typedef std::vector<TagPtr> Tags;

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINETYPES_H__