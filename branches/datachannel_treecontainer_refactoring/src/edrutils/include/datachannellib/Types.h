/********************************************************************
	created:  2015/07/28	14:58:20
	filename: Types.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATACHANNEL__TYPES_H__
#define __HEADER_GUARD_DATACHANNEL__TYPES_H__

namespace dataaccessor
{
	//! Typ cech kana�u
	enum FeatureType
	{
		Description = 0,	//! Opis zawarto�ci kana�u, argument�w, warto�ci i ich jednostek
		Statistics,			//! Statystyki kana�u
		Function,			//! Opis czy dane reprezentuj� funkcj�
		FunctionDescription,//! Opis funkcji - parzysto��, nieparzysto��,  monotoniczno��, ...
		ArgumentBounded,	//! Opis zakresu argument�w
		ValueBounded,		//! Opis zakresu warto�ci
		UniformArguments,	//! Opis argument�w r�wnooodalonych
		Periodic,			//! Opis okresu
		CustomBase = 100	//! Indeks dla w�asnych cech
	};

	//! Typ monotoniczno�ci
	enum MonotonyType
	{
		NonMonotonic,	//! Niemonotonoczna
		NonDecreasing,	//! Niemalej�ca
		Decreasing,		//! �ci�le malej�ca
		Growing,		//! �ci�le rosn�ca
		NonGrowing,		//! Nierosn�ca
		Constant		//! Sta�a
	};
}

#endif	// __HEADER_GUARD_DATACHANNEL__TYPES_H__