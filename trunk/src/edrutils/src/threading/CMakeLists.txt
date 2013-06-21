BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	IJob.h
	IJobManager.h
	Synchronized.h
	SynchronizationPolicies.h
	IThread.h
	IThreadFactory.h
	IThreadGroup.h
	IThreadPool.h
	ThreadPool.h
	Thread.h
	ThreadGroup.h
	IProgress.h
	ReusableThread.h
	RerunnableThread.h
)

SET_SOURCE_FILES(
	ThreadPool.cpp
	Thread.cpp
	ThreadGroup.cpp
	ReusableThread.cpp
	RerunnableThread.cpp
)

END_PROJECT()