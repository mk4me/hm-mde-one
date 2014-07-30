/********************************************************************
	created:  2013/07/01
	created:  1:7:2013   13:20
	filename: Runnable.h
	author:   Mateusz Janiak

	purpose:  Interfejs obiektu joba uruchamianego przez wątki i job managera
	*********************************************************************/
#ifndef HEADER_GUARD_THREADINGUTILS__RUNNABLE_H__
#define HEADER_GUARD_THREADINGUTILS__RUNNABLE_H__

#include <threadingUtils/Export.h>
#include <utils/SmartPtr.h>
#include <boost/function.hpp>

namespace threadingUtils {

	//! Interfejs obiektu realizuj�cego przetwarzanie w w�tkach
	class IRunnable
	{
	public:
		//! Wirtualny destruktor
		virtual ~IRunnable() {}
		//! Metoda wirtualna realizuj�ca przetwarzanie w w�tku
		virtual void run() = 0;
	};

	DEFINE_SMART_POINTERS(IRunnable);

	//! Klasa implementująca interfejs Runnable w oparciu o funktor
	class THREADINGUTILS_EXPORT FunctorRunnable : public IRunnable
	{
	public:
		//! Typ funktora bezargumentowego
		typedef boost::function<void()> Functor;

	public:
		//! Konstruktor
		//! \param func Funktor
		FunctorRunnable(const Functor & func);

		virtual ~FunctorRunnable();

		virtual void run();

	private:
		//! Funktor
		Functor func;
	};
}

#endif	//	HEADER_GUARD_THREADINGUTILS__RUNNABLE_H__
