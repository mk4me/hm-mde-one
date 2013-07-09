BEGIN_PROJECT("static")

SET_PRIVATE_HEADERS(
	JobManagerImpl.h
)

SET_PUBLIC_HEADERS(
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
)

SET_SOURCE_FILES(
	ThreadPool.cpp
	JobManager.cpp
	JobManagerImpl.cpp
	Runnable.cpp
	QtThreadFactory.cpp	
)

END_PROJECT()