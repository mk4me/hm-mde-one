#include <threadingUtils/ThreadAfinityVerifier.h>
#include <QtCore/QThread>

using namespace threadingUtils;

class ThreadAfinityVerifier::ThreadAfinityVerifierImpl
{
public:
	ThreadAfinityVerifierImpl() : threadID(QThread::currentThreadId())
	{

	}

	~ThreadAfinityVerifierImpl()
	{

	}

	const bool verifyCurrentThreadAfinity() const
	{
		return threadID == QThread::currentThreadId();
	}

private:
	//! Identyfikator w¹tku tworz¹cego obiekt
	const Qt::HANDLE threadID;
};

ThreadAfinityVerifier::ThreadAfinityVerifier() : impl(new ThreadAfinityVerifierImpl)
{

}
	
const bool ThreadAfinityVerifier::verifyCurrentThreadAfinity() const
{
	return impl->verifyCurrentThreadAfinity();
}

ThreadAfinityVerifier::~ThreadAfinityVerifier()
{

}