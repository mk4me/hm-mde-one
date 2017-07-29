/********************************************************************
	created:  2016/03/15 10:02
	filename: KioskUtilities.h
	author:	  Wojciech Knie�
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___KIOSKUTILITIES_H__
#define HEADER_GUARD___KIOSKUTILITIES_H__

#include <functional>
#include <coreui/Export.h>

namespace coreUI {

//! Klasa zawiera w sobie metody u�atwiaj�ce uruchomienie trybu "Kiosk".
//! Aplikacja taka powinna zablokowa� mo�liwo�� wyj�cia z niej za pomoc� standardowych przycisk�w
//! oraz kombinacji klawiszy (ALT + TAB i podobnych)
class COREUI_EXPORT KioskUtilities
{
public:
	using additionalOperations = std::function < void(void) > ;
	//! Blokuje kombinacje klawiszy, kt�re mog� wyj�� z aplikacji
	static void blockSpecialKeyCombinations();
	//! Odblokowuje kombinacje klawiszy, kt�re mog� wyj�� z aplikacji
	static void unblockSpecialKeyCombinations();
	//! Uruchamia tryb kiosk
	//! \param f dodatkowe operacje, kt�re zostan� przeprowadzone przy uruchomieniu trybu
	static void startKioskMode(additionalOperations f = [](){});
	//! Uruchamia tryb kiosk je�li ustawiony jest parametr aplikacji --kiosk 
	//! \param f dodatkowe operacje, kt�re zostan� przeprowadzone przy uruchomieniu trybu
	static void startKioskModeIfParamIsSet(additionalOperations f = [](){});
	//! Ko�czy tryb kiosk
	//! \param f dodatkowe operacje, kt�re zostan� przeprowadzone przy zako�czeniu trybu
	static void stopKioskMode(additionalOperations f = [](){});
	//! Ko�czy tryb kiosk, nie rzuci wyj�tku przy b��dzie
	//! \param f dodatkowe operacje, kt�re zostan� przeprowadzone przy zako�czeniu trybu
	static void safeStopKioskMode(additionalOperations f = [](){});
};

class COREUI_EXPORT ScopedKiosk {
public:
	ScopedKiosk(KioskUtilities::additionalOperations startOp = [](){}, KioskUtilities::additionalOperations stopOp = [](){}, bool onlyIfParam = true);
	~ScopedKiosk();
private:
	KioskUtilities::additionalOperations stopOperation;
};
}
#endif