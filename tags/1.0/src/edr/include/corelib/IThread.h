/********************************************************************
    created:  2013/07/05
    created:  5:7:2013   13:38
    filename: IThread.h
    author:   Mateusz Janiak
    
    purpose:  Interfejs w¹tku
*********************************************************************/
#ifndef HEADER_GUARD_CORE__ITHREAD_H__
#define HEADER_GUARD_CORE__ITHREAD_H__

#include <threading/IThread.h>

namespace core
{

	class IThread : public utils::IThread
	{
	public:
		//! Destruktor wirtualny
		virtual ~IThread() {}

		//! \return Nazwa ownera
		virtual const std::string & who() const = 0;
		//! \return Przeznaczenie w¹tku
		virtual const std::string & destination() const = 0;
		//! \param destination Przeznaczenie w¹tku, co realizuje
		virtual void setDestination(const std::string & destination) = 0;

	};

	DEFINE_SMART_POINTERS(IThread)

}

#endif	//	HEADER_GUARD_CORE__ITHREAD_H__
