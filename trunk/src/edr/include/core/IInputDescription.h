/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:12
	filename: 	IInputDescription.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IINPUTDESCRIPTION_H__
#define HEADER_GUARD_CORE__IINPUTDESCRIPTION_H__

#include <core/TypeInfo.h>
#include <vector>
#include <string>

namespace core {

class IInputDescription
{
public:
    virtual ~IInputDescription() 
    {
    }

    //! Struktura opisuj¹ca typy danych wejœciowe.
    struct InputInfo
    {
        //! Nazwa wejœcia.
        std::string name;
        //! Typy wspierane przez wejœcie.
        TypeInfo type;
        //! Czy wejœciew wymagane
        bool required;
        //! Czy dane na wejsciu bêdza modyfikowane
        bool modify;
    };

public:
    //! Pobiera informacje o wejœciu. Gdy danego wejœcia nie ma wynikowa lista typów powinna
    //! byæ pusta.
    //! \param inputNo Numer wejœcia. 
    //! \param name Nazwa wejœcia.
    //! \param type Typy wspierane na danym wejœciu.
    virtual void getInputInfo(std::vector<InputInfo>& info) = 0;
};

}

#endif