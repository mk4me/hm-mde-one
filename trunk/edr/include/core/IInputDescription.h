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
    //! Struktura opisuj�ca dane wej�ciowe.
    struct InputInfo
    {
        //! Nazwa wej�cia.
        std::string name;
        //! Typy wspierane przez wej�cie.
        std::list<core::TypeInfo> types;
    };

public:
    //! Pobiera informacje o wej�ciu. Gdy danego wej�cia nie ma wynikowa lista typ�w powinna
    //! by� pusta.
    //! \param inputNo Numer wej�cia. 
    //! \param name Nazwa wej�cia.
    //! \param types Typy wspierane na danym wej�ciu.
    virtual void getInputInfo(int inputNo, InputInfo& info) = 0;
};

}

#endif