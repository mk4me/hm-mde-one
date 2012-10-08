#ifndef HEADER_GUARD__TIMELINETYPES_H__
#define HEADER_GUARD__TIMELINETYPES_H__

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <vector>
#include <map>

/* Nagłówek z deklaracjami typow - deklaracje wyprzedzajace dla klas
* oraz odpowiednie typy kolekcji i wskaźnikow dla nich
*/

////////////////////////////////////////////////////////////////////////////////
namespace timeline{
////////////////////////////////////////////////////////////////////////////////

class IChannel;
typedef boost::shared_ptr<IChannel> IChannelPtr;
typedef boost::weak_ptr<IChannel> IChannelWPtr;
typedef std::vector<IChannelPtr> IChannels;

typedef boost::shared_ptr<const IChannel> IChannelConstPtr;
typedef boost::weak_ptr<const IChannel> IChannelConstWPtr;
typedef std::vector<IChannelConstPtr> ConstIChannels;

class Channel;
typedef boost::shared_ptr<Channel> ChannelPtr;
typedef boost::weak_ptr<Channel> ChannelWPtr;
typedef std::vector<ChannelPtr> Channels;

typedef boost::shared_ptr<const Channel> ChannelConstPtr;
typedef boost::weak_ptr<const Channel> ChannelConstWPtr;
typedef std::vector<ChannelConstPtr> ConstChannels;

class Tag;
typedef boost::shared_ptr<Tag> TagPtr;
typedef boost::weak_ptr<Tag> TagWPtr;
typedef std::vector<TagPtr> Tags;

typedef boost::shared_ptr<const Tag> TagConstPtr;
typedef boost::weak_ptr<const Tag> TagConstWPtr;
typedef std::vector<TagConstPtr> ConstTags;

class Model;
typedef boost::shared_ptr<Model> ModelPtr;
typedef boost::weak_ptr<Model> ModelWPtr;

typedef boost::shared_ptr<const Model> ModelConstPtr;
typedef boost::weak_ptr<const Model> ModelConstWPtr;

class View;
typedef boost::shared_ptr<View> ViewPtr;
typedef boost::weak_ptr<View> ViewWPtr;

typedef boost::shared_ptr<const View> ViewConstPtr;
typedef boost::weak_ptr<const View> ViewConstWPtr;

class IController;
typedef boost::shared_ptr<IController> ControllerPtr;
typedef boost::shared_ptr<const IController> ControllerConstPtr;
typedef boost::weak_ptr<IController> ControllerWPtr;
typedef boost::weak_ptr<const IController> ControllerConstWPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__TIMELINETYPES_H__
