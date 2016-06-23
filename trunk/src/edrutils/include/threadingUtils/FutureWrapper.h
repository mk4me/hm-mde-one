/********************************************************************
	created:  2014/10/03	08:58:06
	filename: FutureWrapper.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__FUTUREWRAPPER_H__
#define __HEADER_GUARD_THREADINGUTILS__FUTUREWRAPPER_H__

#include <utils/SmartPtr.h>
#include <threadingUtils/Future.h>

namespace threadingUtils
{
	//! Obiekt pozwalaj�cy wrapowa� obiety typu future by potem na nie czeka�
	//! tzw. type erasure pattern
	class FutureWrapper
	{
		//! Obiekt bazowy do wywo�a� future
		struct ImplBase {			
			//! \return Czy wrapowany future jest poprawny
			virtual bool valid() const = 0;			
			//! Metoda czekaj�ca a� wrapowany future b�dzie gotowy
			virtual void wait() = 0;
			//! Destruktor wirtualny
			virtual ~ImplBase() {}
		};

		//! \tparam T Typ wartosci zwracanej
		//! \tparam FuturePolicy Typ polityki dla future (shared albo zwyk�y)
		template<typename Future>
		struct ImplType : public ImplBase
		{
			//! Future do odpytania
			Future future;
			//! \param f Future do wywo�ania
			ImplType(Future && f) : future(std::move(f)) {}
			//! \return Czy wrapowany future jest poprawny
			virtual bool valid() const override { return future.valid(); }
			//! Metoda czekaj�ca a� wrapowany future b�dzie gotowy
			virtual void wait() override { future.wait(); };
			//! Destruktor wirtualny
			virtual ~ImplType() {}
		};

	public:
		//! \tparam T Typ wartosci zwracanej
		//! \tparam FuturePolicy Typ polityki dla future (shared albo zwyk�y)
		//! \param f Future do odpytania
		template<typename Future>
		FutureWrapper(Future && f) :
			impl(new ImplType<Future>(std::move(f)))
		{

		}
		
		//! \return Czy future poprawny
		bool valid() const { return impl != nullptr && impl->valid(); }
		//! Metoda czekaj�ca a� wrapowany future b�dzie gotowy
		void wait() { if (impl != nullptr) { impl->wait(); } }

		//! Konstrutkor domy�lny
		FutureWrapper() = default;

		//! \param other Wrapper z kt�rego przejmujemy wrapowany future
		FutureWrapper(FutureWrapper&& other) :
			impl(std::move(other.impl))
		{
			
		}

		//! Destruktor
		~FutureWrapper() = default;

		//! \param other Wrapper z kt�rego przejmujemy wrapowany future
		FutureWrapper& operator=(FutureWrapper&& other)
		{
			impl = std::move(other.impl);
			return *this;
		}

		//! \tparam Future Typ future jaki wrapujemy
		//! \prama f Wrapowany future
		template<typename Future>
		FutureWrapper& operator=(Future && f)
		{
			impl.reset(new ImplType<Future>(std::move(f)));
			return *this;
		}

		FutureWrapper(const FutureWrapper&) = delete;
		FutureWrapper& operator=(const FutureWrapper&) = delete;

		//! Metoda przywraca pierwotny stan wrappera - zwalnia future
		void reset() { impl.reset(); }

	private:
		//! Obiekt obs�uguj�cy funktor
		utils::shared_ptr<ImplBase> impl;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__FUTUREWRAPPER_H__