#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <streambuf>

#include "ConsoleWidget.h"
// 
// class ConsoleWidgetOutputBuffer : public std::streambuf 
// {
// public:
//     ConsoleWidgetOutputBuffer(ConsoleWidget *console, std::size_t buff_sz = 256): 
//       _buffer(buff_sz + 1)
//     , _console(console)
//     {
//     char *base = &_buffer.front();
//     setp(base, base + _buffer.size() - 1);
//     }
// 
//     virtual ~ConsoleWidgetOutputBuffer()
//     {
//         _console = NULL; 
//         _buffer.clear(); 
//     }
// private: 
//     virtual int_type overflow(int_type c = traits_type::eof()) 
//     {
//         *pptr() = c;
//         pbump(1);
//         flush_buffer();
//         return c;
//     }
// 
//     virtual int sync()
//     {
//         return flush_buffer() ? 0 : -1;
//     }
// 
//     bool flush_buffer()
//     {
//         std::string buf = std::string(pbase(), pptr()); 
//         _console->print(buf); 
//         char *base = &_buffer.front();
//         setp(base, base + _buffer.size() - 1);
//         return true; 
//     }
// private: 
//     ConsoleWidget *_console; 
//     std::vector<char> _buffer;
// };

#endif