/********************************************************************
	created:  2014/10/02	12:16:10
	filename: FunctionWrapper.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_THREADINGUTILS__FUNCTIONWRAPPER_H__
#define __HEADER_GUARD_THREADINGUTILS__FUNCTIONWRAPPER_H__

#include <utils/SmartPtr.h>
#include <stdexcept>
#include <functional>

namespace threadingUtils
{
	//! Klasa pozwalaj¹ca opakowaæ funktory z przekazaniem w³asnoœci niezale¿nie od ich sygnatury
	//! tzw. type erasure
	class FunctionWrapper
	{
	private:

		//! Obiekt bazowy do wywo³añ funktorów
		struct ImplBase {
			//! Metoda wirtualna wo³aj¹ca funktor
			virtual void call() = 0;
			//! Destruktor wirtualny
			virtual ~ImplBase() {}
		};

		//! \tparam F Typ funktora który chcemy wrapowaæ
		template<typename F>
		struct ImplType : public ImplBase
		{
			//! Funktor
			F f;
			//! \param f Funktor do wykonania
			ImplType(F&& f) : f(std::move(f)) {}
			//! Metoda wirtualna wo³aj¹ca funktor
			virtual void call() override { f(); }
			//! Destruktor wirtualny
			virtual ~ImplType() {}
		};

	public:
		//! \tparam F Typ funktora który chcemy wrapowaæ
		//! \param f Funktor do wykonania
		template<typename F>
		FunctionWrapper(F&& f) :
			impl(new ImplType<F>(std::move(f)))
		{

		}

		//! Metoda wywo³uj¹ca funktor
		inline void operator()() { if (valid() == false) { throw std::runtime_error("Empty wrapper"); } impl->call(); }

		//! param other Wrapper z którego przejmujemy funktor
		FunctionWrapper(FunctionWrapper&& other) :
			impl(std::move(other.impl))
		{

		}

		//! param other Wrapper z którego przejmujemy funktor
		FunctionWrapper& operator=(FunctionWrapper&& other)
		{
			impl = std::move(other.impl);
			return *this;
		}

		//! Domyslny konsturktor
		FunctionWrapper() = default;
		//! Domyœlny destruktor
		~FunctionWrapper() = default;

		//! \return Czy funktor poprawny
		const bool valid() const { return impl != nullptr; }
		//! Metoda zwalnia funktor
		void reset() { impl.reset(); }


		FunctionWrapper(const FunctionWrapper& Other) = delete;
		FunctionWrapper(FunctionWrapper& Other) = delete;
		FunctionWrapper& operator=(const FunctionWrapper& Other) = delete;

	private:
		//! Obiekt obs³uguj¹cy funktor
		utils::shared_ptr<ImplBase> impl;
	};
}

#endif	// __HEADER_GUARD_THREADINGUTILS__FUNCTIONWRAPPER_H__