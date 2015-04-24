#include <stdarg.h>
#include <utils/Debug.h>
#include <boost/lexical_cast.hpp>

#ifdef __WIN32__
#define NOMINMAX
#include <Windows.h>
#include <strsafe.h>
#define snprintf StringCchPrintf
#define vsnprintf StringCchVPrintf
#define PRINT OutputDebugString
#else // __WIN32__
#include <cstdio>
#define PRINT(x) printf("%s", (x));
#endif // __WIN32__

#define BUFFER_SIZE 256

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

void Debug::print( const char * file, int line, const char * format, ... )
{
    // najpierw nazwa pliku i linia
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE-1, "%s(%d): ", file, line);
    PRINT(buffer);

    // następnie musimy sformatować wiadomość
    va_list vaList;
    va_start(vaList, format);
    vsnprintf(buffer, BUFFER_SIZE-1, format, vaList);
    va_end(vaList);
    PRINT(buffer);
    PRINT("\n");
}


void Debug::formatMessage( char * buffer, int bufferSize, const char * format, ... )
{
    va_list vaList;
    va_start(vaList, format);
    vsnprintf(buffer, bufferSize-1, format, vaList);
    va_end(vaList);
}

void Debug::formatMessage( char * buffer, int /*bufferSize*/ )
{
    buffer[0] = '\0';
}
////////////////////////////////////////////////////////////////////////////////

ScopeTimePrinter::ScopeTimePrinter(const std::string& scopeName) :
scopeName(scopeName)
{
	PRINT((this->scopeName + std::string(" - start \n")).c_str());
	this->tick = boost::posix_time::microsec_clock::local_time();
}

ScopeTimePrinter::ScopeTimePrinter(const std::string& scopeName, const char * file, int line)
{
	typedef std::string st;
	this->scopeName = st(file) + st(":") + boost::lexical_cast<st>(line) + st(" - ") + scopeName;
	PRINT((this->scopeName + std::string(" - start \n")).c_str());
	this->tick = boost::posix_time::microsec_clock::local_time();
}

ScopeTimePrinter::~ScopeTimePrinter()
{
	auto next = boost::posix_time::microsec_clock::local_time();
	boost::posix_time::time_duration msdiff = next - tick;
	std::string time = boost::lexical_cast<std::string>(msdiff.total_milliseconds());
	PRINT((this->scopeName + std::string(" - ") + time + std::string("\n")).c_str());
}

} // namespace utils
////////////////////////////////////////////////////////////////////////////////
