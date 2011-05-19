/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:14
	filename: 	IOutputDescription.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IOUTPUTDESCRIPTION_H__
#define HEADER_GUARD_CORE__IOUTPUTDESCRIPTION_H__

#include <vector>

namespace core {

class IOutputDescription
{
public:
    //! Struktura opisuj�ca dane wyj�ciowe.
    struct OutputInfo
    {
        //! Nazwa wyj�cia.
        std::string name;
        //! Typ wyj�cia.
        core::TypeInfo type;
        //!
        OutputInfo(const std::string& str, const core::TypeInfo& type) :
        name(name), type(type)
        {}
    };

public:
    //! Pobiera informacje o typach wyj��. Kolejno�� obowi�zuje potem w metodzie process.
    //! \param source �r�d�o danych. Wej�cia mog� zawiera� obiekty b�d� by� wyzerowane.
    virtual void getOutputInfo( std::vector<OutputInfo>& info ) = 0;

    virtual ~IOutputDescription() {}
};

}

#endif