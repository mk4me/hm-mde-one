/********************************************************************
created:  2010/04/21
created:  21:4:2010   17:47
filename: Enum.h
author:	  Piotr Gwiazdowski

purpose:
*********************************************************************/
#ifndef __MOTIONUTIL_ENUM_H__
#define __MOTIONUTIL_ENUM_H__

//------------------------------------------------------------------------------
#include <utils/Config.h>
#include <vector>
#include <string>
#include <sstream>
#include <map>
//#include <boost/regex.hpp>
#include <utils/Macros.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
////////////////////////////////////////////////////////////////////////////////

/**
* Makro kojarz�ce z wyliczeniem dodatkowe informacje. Nie mo�e by� u�ywane w
* nag��wkach (tworzy zmienn� statyczn�).
*
* Przyk�ad u�ycia:
* enum Color {
*  ColorRed  = 0x01,
*  ColorBlue,
*  ColorPink = ColorRed | ColorBlue,
* };
* #define COLOR_ENUM_DEF \
*  ColorRed  = 0x01,\
*  ColorBlue,\
*  ColorPink = ColorRed | ColorBlue
* DECLARE_ENUM_ENCHANCEMENT(Color, COLOR_ENUM_DEF)
*
* Inaczej:
* #define COLOR_ENUM_DEF \
*  ColorRed  = 0x01,\
*  ColorBlue,\
*  ColorPink = ColorRed | ColorBlue
* enum Color { COLOR_ENUM_DEF };
* DECLARE_ENUM_ENCHANCEMENT(Color, COLOR_ENUM_DEF);
*
* \param TYPE      Typ ju� istniej�cego wyliczenia.
* \param ENUM_DEF  Definicja wyliczenia podana w postaci makra. Definicja musi
*                  zawiera� tylko to, co znajduje si� mi�dzy klamrami.
*                  Mog� w niej wyst�powa� komentarze w stylu C (komentarzy w stylu C++ nie
*                  da si� umie�ci� w makrze).
*/
#define DECLARE_ENUM_ENCHANCEMENT(TYPE, ENUM_DEF)                   \
namespace utils {                                                   \
template <>                                                         \
class __EnumInitializer<TYPE>                                       \
{                                                                   \
public:                                                             \
    /*Dane enuma.*/                                                 \
    __EnumData<TYPE> data;                                          \
    __EnumInitializer()                                             \
    {                                                               \
        /*pobieramy warto�ci enuma*/                                \
        std::vector<__EnumInitHelper::value_type> values;           \
        /*inicjalizacja*/                                           \
        __EnumInitHelper::pValues = &values;                        \
        __EnumInitHelper::lastValue = __EnumInitHelper::initValue;  \
        /*w�a�ciwe dane*/                                           \
        __EnumInitHelper ENUM_DEF;                                  \
        /*zerujemy dla bezpiecze�stwa*/                             \
        __EnumInitHelper::lastValue = __EnumInitHelper::initValue;  \
        __EnumInitHelper::pValues = NULL;                           \
        /**/                                                        \
        /*inicjalizacja danych*/                                    \
        const char * raw = COMA_AWARE_STRINGIZE(ENUM_DEF);          \
        data.init(values, raw);                                     \
    }                                                               \
};                                                                  \
}

//------------------------------------------------------------------------------

//! Klasa u�ywana do inicjalizacji. U�ywana jedynie w drodze specjalizacji.
template <class T>
class __EnumInitializer;

//------------------------------------------------------------------------------

//! Pomocnicza klasa s�u��ca rekonstrukcji warto�ci wylicze�. Konstrukcja
//! tych obiekt�w odbywa si� przez C++ runtime.
struct __EnumInitHelper
{
    //! Typ u�ywanych warto�ci.
    typedef unsigned value_type;

    //! Warto��.
    value_type value;
    //! Ostatnia u�yta warto��.
    static value_type lastValue;
    //! Wska�nik na kontener warto�ci.
    static std::vector<value_type> * pValues;
    //! Warto�� "zeruj�ca"
    static const value_type initValue = value_type(-1);

    //! Inkrementacja wzgl�dem poprzedniej warto�ci.
    __EnumInitHelper() { init(lastValue+1); }
    //! Jawnie podana warto��.
    __EnumInitHelper(value_type val) { init(val); }
    //! Kopia.
    __EnumInitHelper(const __EnumInitHelper& val) { init(val.value); }
    //! Rzutowanie na typ warto�ci.
    operator value_type() { return value; }

    //! Metoda inicjuj�ca.
    void init(value_type val)
    {
        lastValue = value = val;
        pValues->push_back(value);
    }
};

//------------------------------------------------------------------------------

//! Klasa zawieraj�ca informacje odno�nie wylicze�. Singleton.
template <class T>
class __EnumData
{
public:
    //! Tym wyliczenia.
    typedef T EnumType;
    //! Typ s�ownika z kluczem b�d�cym warto�ci�.
    typedef std::multimap<EnumType, std::string> DictValues;
    //! Typ s�ownika z kluczem b�d�cym �a�cuchem
    typedef std::map<std::string, EnumType> DictNames;

    //! Typ rozmiaru (nazwa w stylu STLa)
    typedef typename DictValues::size_type size_type;
    //!
    typedef typename DictValues::value_type value_type;

public:
    //! Nazwy enum�w.
    DictValues byValues;
    //! Warto�ci enum�w.
    DictNames byNames;

public:
    //! \param Warto�� dla kt�rej zwr�ci� nazw�.
    //! \return Nazwa.
    //! \throws std::invalid_argument
    const std::string& getName(EnumType value) const
    {
        typename DictValues::const_iterator it = byValues.find(value);
        if ( it != byValues.end() ) {
            return it->second;
        } else {
            std::ostringstream msg;
            msg << "Value not found: " << value;
            throw std::invalid_argument(msg.str());
        }
    }

    //! \param Warto�� dla kt�rej zwr�ci� nazw�.
    //! \return Nazwa.
    //! \throws std::invalid_argument
    EnumType parse(const std::string& name) const
    {
        typename DictNames::const_iterator it = byNames.find(name);
        if ( it != byNames.end() ) {
            return it->second;
        } else {
            std::ostringstream msg;
            msg << "Unable to parse: " << name;
            throw std::invalid_argument(msg.str());
        }
    }

    //! \return Liczba enum�w.
    size_type size() const
    {
        return byNames.size();
    }

    //! Parsuje �a�cuch.
    //! \param values Warto�ci wylicze�.
    //! \param input �a�cuch reprezentuj�cy wyliczenie.
    //! \return true je�li si� uda�o.
    bool init(const std::vector<__EnumInitHelper::value_type>& values, std::string input)
    {
//         // usuwamy komentarze (w stylu C) oraz bia�e znaki
//         boost::regex removeComments("/\\*(.|[\r\n])*?\\*/|[\\s\n\r]");
//         input = boost::regex_replace(input, removeComments, std::string(""));
// 
//         // teraz szukamy poszczeg�lnych warto�ci
//         boost::regex enrtySearch("(\\w+).*?(,|$)");
// 
//         boost::sregex_iterator it(input.begin(), input.end(), enrtySearch);
//         boost::sregex_iterator end;
//         for (size_t i = 0; it != end; ++it, ++i) {
//             // pobieramy nazw� i warto�
//             const std::string & name = (*it)[1];
//             EnumType value = static_cast<EnumType>(values[i]);
//             // uzupe�niamy s�owniki
//             byNames[name] = value;
//             byValues.insert( std::make_pair(value, name) );
//         }
//         return true;

        // odpowiednik wyszukiwania z u�yciem klasycznych metod
        // usuwamy bia�e znaki
        std::string::iterator lastSign = std::remove_if(input.begin(), input.end(), isspace);
        input.resize( std::distance(input.begin(), lastSign) );

        // ustawiamy zmienne stanu
        size_t entryBegin = 0;
        size_t entryEnd = input.find_first_of(',');
        size_t equalPos = input.find_first_of('=');
        size_t entryNo = 0;

        while (entryEnd != std::string::npos) {
            // koniec tokenu
            size_t nameEnd = entryEnd;
            if ( equalPos != std::string::npos && equalPos < entryEnd ) {
                // przechodzimy na kolejny znak r�wno�ci
                nameEnd = equalPos;
                equalPos = input.find_first_of('=', equalPos + 1);
            }
            // pobieramy token
            std::string name = input.substr(entryBegin, nameEnd - entryBegin);
            EnumType value = static_cast<EnumType>(values[entryNo]);
            byNames[name] = value;
            byValues.insert( std::make_pair(value, name) );
            
            // przechodzimy na nast�pny wpis
            entryBegin = entryEnd + 1;

            // sprawdzamy, czy to nie przypadkiem koniec �a�cucha
            if ( entryEnd != input.size() ) {
                entryEnd = input.find_first_of(",\0", entryBegin);
                if ( entryEnd == std::string::npos ) {
                    entryEnd = input.size();
                }
            } else {
                entryEnd = std::string::npos;
            }
            ++entryNo;
        }
        return true;
    }
};

//------------------------------------------------------------------------------

template <class T>
class Enum {
public:
    typedef __EnumInitializer<T> EnumInitializer;
    //! Typ wyliczenia.
    typedef T EnumType;
    //! Typ danych.
    typedef __EnumData<T> EnumData;
    //! Typ rozmiaru.
    typedef typename EnumData::size_type size_type;

public:
    //! Definicja sta�ego iteratora (w stylu STL).
    typedef typename EnumData::DictValues::const_iterator const_iterator;
    //! Definicja sta�ego odwrotnego iteratora    (w stylu STL).
    typedef typename EnumData::DictValues::const_reverse_iterator const_reverse_iterator;
    //! Definicja warto�ci interator�w (w stylu STL).
    typedef typename EnumData::DictValues::value_type value_type;

public:
    //! Same metody statyczne.
    Enum() {}

public:
    //! \param Warto�� dla kt�rej zwr�ci� nazw�.
    //! \return Nazwa.
    //! \throws std::invalid_argument
    static EnumType parse(const std::string& name)
    {
        return getInitializer().data.parse(name);
    }
    //! \param Warto�� dla kt�rej zwr�ci� nazw�.
    //! \return Nazwa.
    //! \throws std::invalid_argument
    static const std::string& getName(EnumType value)
    {
        return getInitializer().data.getName(value);
    }
    //! \return Liczba enum�w.
    static size_type size()
    {
        return getInitializer().data.size();
    }

    //! \return Iterator.
    const_iterator begin()
    {
        return getInitializer().data.byValues.begin();
    }
    //! \return Iterator.
    const_iterator end()
    {
        return getInitializer().data.byValues.end();
    }
    //! \return Iterator.
    const_reverse_iterator rbegin()
    {
        return const_reverse_iterator(end());
    }
    //! \return Iterator.
    const_reverse_iterator rend()
    {
        return const_reverse_iterator(begin());
    }

private:
    static EnumInitializer& getInitializer()
    {
        static EnumInitializer initializer = EnumInitializer();
        return initializer;
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif  // __MOTIONUTIL_ENUM_H__
