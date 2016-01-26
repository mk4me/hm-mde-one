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
	//! \tparam Accessor Typ akcesora kt�ry b�dziemy przechowywa� bezpiecznie
	//! \tparam Impl Implementacja kt�ra bazuje na damnym akcesorze
	template<typename Accessor, typename Impl>
	//! Klasa pomocnicza realizuj�ca bezpieczny wrapper - trzyma dane na kt�rych chcemy operaowa�
	class SafeAccessorWrapper : public Impl, private utils::ValueCarrier<Accessor>
	{
	public:
		//! \tparam U Typ akcesora
		//! \tparam Args Typy argument�w przekazywane do akcesora
		template<typename U, class... Args>
		//! \param accessor Akcesor kt�rego wrapujemy
		//! \param arguments Argumenty przenoszone do implementacji wrappera
		SafeAccessorWrapper(const U & accessor, Args&&... arguments)
			: utils::ValueCarrier<Accessor>(accessor),
			Impl(utils::ValueCarrier<Accessor>::ref(), std::move(arguments)...)
		{
		}

		//! \tparam U Typ akcesora
		//! \tparam Args Typy argument�w przekazywane do akcesora
		template<typename U, class... Args>
		//! \param accessor Akcesor kt�rego wrapujemy
		//! \param arguments Argumenty kopiowane do implementacji wrappera
		SafeAccessorWrapper(const U & accessor, Args... arguments)
			: utils::ValueCarrier<Accessor>(accessor),
			Impl(utils::ValueCarrier<Accessor>::ref(), std::forward<Args>(arguments)...)
		{
		}

		//! Destruktor wirtualny
		virtual ~SafeAccessorWrapper() {}
	};
}

#endif  // __HEADER_GUARD_DATAACCESSOR__SAFEACCESSORWRAPPER_H__