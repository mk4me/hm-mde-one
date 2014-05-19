#include <threadingUtils/StreamData.h>

using namespace threadingUtils;

class ResetableStreamStatusObserver::ResetableStreamStatusImpl
{
public:
	ResetableStreamStatusImpl() : modified_(false) {}
	~ResetableStreamStatusImpl() {}

	const bool modified() const
	{
		ScopedLock<RecursiveSyncPolicy> lock(sync_);
		auto ret = modified_;
		modified_ = false;
		return ret;
	}

	void update()
	{
		ScopedLock<RecursiveSyncPolicy> lock(sync_);
		modified_ = true;
	}

private:
	volatile mutable bool modified_;
	mutable RecursiveSyncPolicy sync_;
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
	ScopedLock<RecursiveSyncPolicy> lock(synch_);
	for (auto it = observers.begin(); it != observers.end(); ++it){
		(*it)->update();
	}
}

StreamBase::~StreamBase()
{
}

void StreamBase::attachObserver(StreamStatusObserverPtr observer)
{
	ScopedLock<RecursiveSyncPolicy> lock(synch_);
	if (observer == nullptr){
		throw std::invalid_argument("Empty observer");
	}

	if (std::find(observers.begin(), observers.end(), observer) == observers.end()){
		observers.push_back(observer);
	}
}

void StreamBase::detachObserver(StreamStatusObserverPtr observer)
{
	ScopedLock<RecursiveSyncPolicy> lock(synch_);
	observers.remove(observer);
}