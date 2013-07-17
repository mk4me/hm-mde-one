/********************************************************************
    created:  2013/06/18
    created:  18:6:2013   10:36
    filename: IProgress.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IPROGRESS_H__
#define HEADER_GUARD___IPROGRESS_H__

namespace utils {

	//! Interfejs pozwalaj�cy okre�li� procentowy stan pracy w�tku
	//! Post�p podawany jest w procentach od 0% do 100%;
	class IPercentageProgress
	{
	public:
		//! Destruktor wirtualny
		virtual ~IPercentageProgress() {}
		//! \return Post�p realizacji przetwarzania od 0% do 100%
		virtual const float progress() const = 0;
	};

	//! Interfejs pozwalaj�cy okre�li� szacowany czas do ko�ca zadania oraz czas jaki up�yn�� od momentu jego wystartowania
	//! Czas podawany jest w sekundach
	class ITimeProgress : public IPercentageProgress
	{
	public:
		//! Destruktor wirtualny
		virtual ~ITimeProgress() {}		
		//! \return Szacowany czas [s] do zako�czenia przetwarzania
		virtual const float remaining() const = 0;
	};

}

#endif	//	HEADER_GUARD___IPROGRESS_H__
