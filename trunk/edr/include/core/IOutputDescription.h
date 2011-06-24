/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:14
	filename: 	IOutputDescription.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__IOUTPUTDESCRIPTION_H__
#define HEADER_GUARD_CORE__IOUTPUTDESCRIPTION_H__

#include <set>
#include <core/TypeInfo.h>
#include <map>

namespace core {

//! Interfejs s³u¿¹cy do opisu typów danych wyjœciowcyh oferowanych przez element przetwarzajacy.
class IOutputDescription
{
public:
    //! Struktura opisuj¹ca typy danych wyjœciowe.
    struct OutputInfo
    {
        //! Typ opisuj¹cy zale¿ne elementy wejœciowe, które s¹ wymagane do generowana danego typu wyjœciowego
        typedef std::set<int> DependentInput;
        //! Nazwa wyjœcia.
        std::string name;
        //! Typ wyjœcia.
        TypeInfo type;
        //! Zale¿ne wejœcia
        DependentInput dependentInput;

        //!
        OutputInfo() {}

        //!
        OutputInfo(const std::string& name, const TypeInfo& type, const DependentInput & dependentInput = DependentInput()) :
        name(name), type(type), dependentInput(dependentInput)
        {}

        OutputInfo & operator= (const OutputInfo & other)
        {
            name = other.name;
            type = other.type;
            dependentInput = other.dependentInput;
            return *this;
        }
    };

public:
    //! Pobiera informacje o typach wyjœæ. Kolejnoœæ obowi¹zuje potem w metodzie process.
    //! \param output Obiekt umozliwiaj¹cy zapis danych wyjsciowych - zapis zawsze jest mo¿liwy!!
    virtual void getOutputInfo( std::vector<OutputInfo> & output ) = 0;

    virtual ~IOutputDescription() {}
};

}

#endif