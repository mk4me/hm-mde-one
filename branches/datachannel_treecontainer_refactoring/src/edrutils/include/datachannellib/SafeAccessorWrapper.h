/********************************************************************
	created:  2016/01/26
	filename: SafeAccessorWrapper.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__SAFEACCESSORWRAPPER_H__
#define __HEADER_GUARD_DATAACCESSOR__SAFEACCESSORWRAPPER_H__

#include <utils/ValueCarrier.h>

namespace dataaccessor
{
	//! \tparam Impl Implementacja kt�ra bazuje na damnym akcesorze
	//! \tparam Accessor Typ akcesora kt�ry b�dziemy przechowywa� bezpiecznie
	template<typename Impl, typename T = typename Impl::accessor_type>
	//! Klasa pomocnicza realizuj�ca bezpieczny wrapper - trzyma dane na kt�rych chcemy operaowa�
	class SafeAccessorWrapper : public Impl, private utils::ValueCarrier<T>
	{
	public:
		//! \tparam U Typ akcesora
		//! \tparam Args Typy argument�w przekazywane do akcesora
		template<typename U, class... Args>
		//! \param value Warto��
		//! \param arguments Argumenty przenoszone do implementacji wrappera
		SafeAccessorWrapper(U && value, Args&&... arguments)
			: utils::ValueCarrier<T>(std::forward<U>(value)),
			Impl(utils::ValueCarrier<T>::ref(), std::forward<Args>(arguments)...)
		{
		}

		//! Destruktor wirtualny
		virtual ~SafeAccessorWrapper() {}
	};
}

#endif  // __HEADER_GUARD_DATAACCESSOR__SAFEACCESSORWRAPPER_H__