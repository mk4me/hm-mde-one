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

    //! Struktura opisuj�ca typy danych wej�ciowe.
    struct InputInfo
    {
        //! Nazwa wej�cia.
        std::string name;
        //! Typy wspierane przez wej�cie.
        TypeInfo type;
        //! Czy wej�ciew wymagane
        bool required;
        //! Czy dane na wejsciu b�dza modyfikowane
        bool modify;
    };

public:
    //! Pobiera informacje o wej�ciu. Gdy danego wej�cia nie ma wynikowa lista typ�w powinna
    //! by� pusta.
    //! \param inputNo Numer wej�cia. 
    //! \param name Nazwa wej�cia.
    //! \param type Typy wspierane na danym wej�ciu.
    virtual void getInputInfo(std::vector<InputInfo>& info) = 0;
};

}

#endif