/********************************************************************
	created:  2014/09/24	09:58:51
	filename: WrapperT.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_SQLITEUTILS__WRAPPERT_H__
#define __HEADER_GUARD_SQLITEUTILS__WRAPPERT_H__

#include <memory>

namespace sqliteUtils
{
	template<typename T, typename D>
	class WrapperT
	{
	public:
		//! \param wrapper Obs³ugiwany uchwyt
		WrapperT(WrapperT & wrapper) : elem_(wrapper.elem_)
		{

		}

		//! \param elem Obs³ugiwany uchwyt
		//! \param del Obiekt zwalniaj¹cy zasoby uchwuty
		explicit WrapperT(T * elem, D del) : elem_(elem, del)
		{

		}

		//! \param elem Obs³ugiwany uchwyt
		explicit WrapperT(T * elem) : elem_(elem)
		{

		}

		//! Destruktor
		~WrapperT()
		{

		}

		//! \return Prawda jeœli mamy jakiœ uchwyt
		explicit operator bool() const
		{
			return elem_;
		}

		//! \param db Baza danych któr¹ beziemy obs³ugiwaæ
		void reset(T * elem = nullptr)
		{
			elem_.reset(elem);
		}

		//! \return Obs³ugiwany uchwyt bazy danych, powoduje wyzerowanie go
		T * release()
		{
			return elem_.release();
		}

		//! \return Obs³ugiwany uchwyt bazy danych
		T * get() const
		{
			return elem_.get();
		}

		//! \return Obs³ugiwany uchwyt bazy danych
		operator T*() const
		{
			return get();
		}

	private:
		//! Uchwyt bazy
		std::unique_ptr<T, D> elem_;
	};
}

#endif	// __HEADER_GUARD_SQLITEUTILS__WRAPPERT_H__