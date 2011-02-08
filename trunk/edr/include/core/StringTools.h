/********************************************************************
	created:  2011/02/08
	created:  8:2:2011   11:51
	filename: StringTools.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_CORE__STRINGTOOLS_H__
#define __HEADER_GUARD_CORE__STRINGTOOLS_H__

#include <QtCore/QString>
#include <string>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

    inline QString toQString(const char* ascii, int size=-1)
    {
        return QString::fromAscii(ascii, size);
    }

    inline QString toQString(const std::string& str)
    {
        return toQString(str.c_str(), static_cast<int>(str.size()));
    }

    inline std::string toStdString(const char* ascii, int size=-1)
    {
        if ( size < 0 ) {
            return std::string(ascii);
        } else {
            return std::string(ascii, ascii+size);
        }
    }

    inline std::string toStdString(const QString& str)
    {
        const QByteArray asc = str.toAscii(); 
        return toStdString(asc.constData(), asc.length()); 
    }



////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_CORE__STRINGTOOLS_H__