/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:12
	filename: 	IClonable.h
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__ICLONABLE_H__
#define HEADER_GUARD_CORE__ICLONABLE_H__

namespace core {

//! Wzorzec interfejsu zapewniaj¹cego klonowanie obiektu.
//! W³aœnoœæ metod virtualnych pozwala klasom pochodnym od klasy dziedzicz¹cej ten interfejs zwracaæ wskaŸniki na w³asny typ

template<class T>
class IClonable
{
public:
    //! \return Klon obiektu
    virtual T* createClone() const = 0;

    virtual ~IClonable() 
    {
    }
};

}

#endif