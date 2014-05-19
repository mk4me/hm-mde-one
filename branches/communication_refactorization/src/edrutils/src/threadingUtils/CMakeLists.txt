BEGIN_PROJECT("dynamic")

SET_PRIVATE_HEADERS(
	JobManagerImpl.h
)

SET_PUBLIC_HEADERS(
	Export.h
	JobManager.h
	Synchronized.h
	SynchronizationPolicies.h
	IThread.h
	IThreadFactory.h
	IThreadPool.h
	ThreadPool.h
	IProgress.h
	Runnable.h
	QtThreadFactory.h
	StreamData.h
)

SET_SOURCE_FILES(
	ThreadPool.cpp
	JobManager.cpp
	JobManagerImpl.cpp
	Runnable.cpp
	QtThreadFactory.cpp
	SynchronizationPolicies.cpp
	StreamData.cpp
)

END_PROJECT()