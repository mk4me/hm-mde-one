#ifndef HEADER_GUARD_COMMUNICATION_PCH_H__
#define HEADER_GUARD_COMMUNICATION_PCH_H__

#ifndef DISABLE_PRECOMPILED_HEADERS

#include <tinyxml2.h>
#include <OpenThreads/Thread>
#include <utils/ObserverPattern.h>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <stack>
#include <iostream>
#include <boost/foreach.hpp>
#include <curl/curl.h>
#pragma warning( disable : 4251 )
#include <wsdlparser/WsdlInvoker.h>
#pragma warning( default : 4251 )

#include <corelib/IPlugin.h>

#include <webserviceslib/ShallowCopyParser.h>
#include <webserviceslib/MetadataParser.h>

#endif
#endif
