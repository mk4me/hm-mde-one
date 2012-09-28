# przygotowanie do szukania
FIND_INIT(OPEN_THREADS openthreads)
FIND_SHARED(OPEN_THREADS_SHARED "OpenThreads" "<ot12-,?>OpenThreads")
FIND_FINISH(OPEN_THREADS)

# sprawdzenie
if (OPEN_THREADS_SHARED_FOUND)
	set(OPEN_THREADS_FOUND 1)
else()
	set(OPEN_THREADS_FOUND 0)
endif()
