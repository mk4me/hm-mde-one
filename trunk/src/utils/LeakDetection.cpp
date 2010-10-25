#include <utils/LeakDetection.h>
#include <crtdbg.h>

#ifdef __UTILS_ENABLE_LEAD_DETECTION_CRT

class __CrtLeakDetector
{
public:
    __CrtLeakDetector();
    ~__CrtLeakDetector();
};

__CrtLeakDetector __leakDetector;

__CrtLeakDetector::__CrtLeakDetector()
{
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
}

__CrtLeakDetector::~__CrtLeakDetector()
{
}

#endif

