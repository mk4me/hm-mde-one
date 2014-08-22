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
#include <boost/filesystem/path.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

    inline QString toQString(const boost::filesystem::path& path)
    {
        return toQString(path.string());
    }

    inline QString toQString(const char* ascii, int size=-1)
    {
        return QString::fromLatin1(ascii, size);
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
        const QByteArray asc = str.toLatin1(); 
        return toStdString(asc.constData(), asc.length()); 
    }

    inline std::string toStdString(const boost::filesystem::path& path)
    {
        return path.string();
    }

    //! Przechowywanie instancji tego typu spowoduje błędy przy wyłuskiwaniu wartości.
    struct toString_t
    {
        //! Forma pośrednia, potrzebna gdy korzystamy z l-wartości.
        std::string strval;
        //! Gdy korzystamy z r-wartości.
        const std::string& strref;

        //! Konstruktor dla r-wartości
        toString_t(const std::string& str) : strval(), strref(str)
        {}
        //! Konstruktor dla l-wartości.
        toString_t(std::string && str) : strval(str), strref(strval)
        {}

        operator const std::string&()
        {
            return strref;
        }
        operator QString()
        {
            return toQString(strref);
        }
        operator boost::filesystem::path()
        {
            return boost::filesystem::path(strref);
        }
    };

    inline toString_t toString(const QString& str)
    {
        return toString_t(str.toStdString());
    }
    inline toString_t toString(const std::string& str)
    {
        return toString_t(str);
    }
    inline toString_t toString(std::string&& str)
    {
        return toString_t(str);
    }
    inline toString_t toString(const boost::filesystem::path& path)
    {
        return toString_t(toStdString(path));
    }



////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_CORE__STRINGTOOLS_H__
