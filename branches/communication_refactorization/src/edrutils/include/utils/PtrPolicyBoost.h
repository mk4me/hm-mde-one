/********************************************************************
	created:    2010/04/07
	created:    7:4:2010     18:08
	filename: PolciesBoost.h
	author:	    Piotr Gwiazdowski

	purpose:
	*********************************************************************/
#ifndef __AVINOUT_POLCIESBOOST_H__
#define __AVINOUT_POLCIESBOOST_H__

#include <utils/SmartPtr.h>
#include <utils/Utils.h>
#include <utils/PtrPolicyHelper.h>

////////////////////////////////////////////////////////////////////////////////
namespace utils {
	////////////////////////////////////////////////////////////////////////////////

	/**
	*	Domyślna implementacja dla wskaźników utils::shared_ptr.
	*/
	struct PtrPolicyBoost
	{
		//! Typ wskaźnika.
		template <typename T> struct Ptr
		{
			typedef utils::shared_ptr<T> Type;
		};
		//! Tylko deklaracja, specjalizacja wyciąga wskaźnik!
		template <typename T> struct Pointed;
		//! Specjalizacja wyciągająca wskaźnik.
		template <typename T> struct Pointed< utils::shared_ptr<T> >
		{
			typedef typename utils::shared_ptr<T>::element_type Type;
		};

			//! Klasa bazowa.
			typedef NullType Base;

			//! Zerowanie wskaźnika.
			//! \param ptr
			template <typename T>
			static void initPtr(T & ptr)
			{}

			//! Ustawienie wartości wskaźnika surowymi danymi.
			//! \param ptr
			//! \param data
			template<typename T, typename Y>
			static void setPtr(utils::shared_ptr<T> & ptr, Y * data)
			{
				ptr.reset(data);
			}

			//! Ustawienie wartości wskaźnika poprzes kopiowanie z innego mądrego wskaźnika.
			//! \param ptr
			//! \param data
			template<typename T, typename Y>
			static void setPtr(utils::shared_ptr<T> & ptr, const utils::shared_ptr<Y> & data)
			{
				ptr = data;
			}

			template<typename T>
			static void swapPtr(utils::shared_ptr<T> & ptr, utils::shared_ptr<T> & data)
			{
				std::swap(ptr, data);
			}

			template<typename T, typename Y>
			static const utils::shared_ptr<Y> dynamicCastPtr(const utils::shared_ptr<T> & ptr)
			{
				return boost::dynamic_pointer_cast<Y>(ptr);
			}

			template<typename T, typename Y>
			static const utils::shared_ptr<Y> constCastPtr(const utils::shared_ptr<T> & ptr)
			{
				return boost::const_pointer_cast<Y>(ptr);
			}

			//! Zwraca surowy wskaźnik
			//! \param ptr
			template<typename T>
			static void* getRawPtr(const utils::shared_ptr<T> & ptr)
			{
				return ptr.get();
			}

			//! Zwraca surowy wskaźnik
			//! \param ptr
			template<typename T>
			static const void* getConstRawPtr(const utils::shared_ptr<T> & ptr)
			{
				return ptr.get();
			}

			//! Zwraca surowy wskaźnik
			//! \param ptr
			template<typename T>
			static T* getPtr(const utils::shared_ptr<T> & ptr)
			{
				return ptr.get();
			}

			//! Zwraca surowy wskaźnik
			//! \param ptr
			template<typename T>
			static const T* getConstPtr(const utils::shared_ptr<T> & ptr)
			{
				return ptr.get();
			}

			//! Czy wskaźnik jest unikatowy?
			//! \param ptr
			template<typename T>
			static const bool isUnique(const utils::shared_ptr<T> & ptr)
			{
				return ptr.unique();
			}

			//! \return Ilość referencji do tego wskaźnika
			template<typename T>
			static const long referenceCount(const utils::shared_ptr<T> & ptr)
			{
				return ptr.use_count();
			}
	};

	template <> struct is_ptr_policy<PtrPolicyBoost> : public __traits::true_type{};

	////////////////////////////////////////////////////////////////////////////////
} // namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif    // __AVINOUT_POLCIESBOOST_H__
