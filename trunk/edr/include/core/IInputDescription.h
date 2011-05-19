/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:12
	filename: 	IInputDescription.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IINPUTDESCRIPTION_H__
#define HEADER_GUARD_CORE__IINPUTDESCRIPTION_H__

namespace core {

class IInputDescription
{
public:
    virtual ~IInputDescription() 
    {
    }
    static const int maxNumSources = 16;
    //! Struktura opisuj¹ca dane wejœciowe.
    struct InputInfo
    {
        //! Nazwa wejœcia.
        std::string name;
        //! Typy wspierane przez wejœcie.
        std::list<core::TypeInfo> types;
    };

public:
    //! Pobiera informacje o wejœciu. Gdy danego wejœcia nie ma wynikowa lista typów powinna
    //! byæ pusta.
    //! \param inputNo Numer wejœcia. 
    //! \param name Nazwa wejœcia.
    //! \param types Typy wspierane na danym wejœciu.
    virtual void getInputInfo(int inputNo, InputInfo& info) = 0;
};

}

#endif