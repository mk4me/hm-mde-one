#include <stdarg.h>
#include <utils/Debug.h>

#ifdef __WIN32__
#include <Windows.h>
#include <strsafe.h>
#define snprintf StringCchPrintf
#define vsnprintf StringCchVPrintf
#define PRINT OutputDebugString
#else // __WIN32__
#include <cstdio>
#define PRINT printf
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

    // nastêpnie musimy sformatowaæ wiadomoœæ
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
} // namespace utils
////////////////////////////////////////////////////////////////////////////////