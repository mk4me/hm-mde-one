#include <threadingUtils/StreamData.h>
#include <atomic>
#include <algorithm>

using namespace threadingUtils;

class ResetableStreamStatusObserver::ResetableStreamStatusImpl
{
public:
	ResetableStreamStatusImpl() : modified_(false) {}
	~ResetableStreamStatusImpl() {}

	const bool modified() const
	{
		std::lock_guard<std::recursive_mutex> lock(sync_);
		bool ret = modified_;
		modified_ = false;
		return ret;
	}

	void update()
	{
		std::lock_guard<std::recursive_mutex> lock(sync_);
		modified_ = true;
	}

private:
	mutable std::atomic<bool> modified_;
	mutable std::recursive_mutex sync_;
};

ResetableStreamStatusObserver::ResetableStreamStatusObserver()
: impl(new ResetableStreamStatusImpl)
{
}

ResetableStreamStatusObserver::~ResetableStreamStatusObserver()
{
}

const bool ResetableStreamStatusObserver::modified() const
{
	return impl->modified();
}

void ResetableStreamStatusObserver::update()
{
	impl->update();
}

StreamBase::StreamBase()
{
}

void StreamBase::notify()
{
	std::lock_guard<std::recursive_mutex> lock(synch_);
	for (auto it = observers.begin(); it != observers.end(); ++it){
		(*it)->update();
	}
}

StreamBase::~StreamBase()
{
}

void StreamBase::attachObserver(StreamStatusObserverPtr observer)
{
	std::lock_guard<std::recursive_mutex> lock(synch_);
	if (observer == nullptr){
		throw std::invalid_argument("Empty observer");
	}

	if (std::find(observers.begin(), observers.end(), observer) == observers.end()){
		observers.push_back(observer);
	}
}

void StreamBase::detachObserver(StreamStatusObserverPtr observer)
{
	std::lock_guard<std::recursive_mutex> lock(synch_);
	observers.remove(observer);
}
