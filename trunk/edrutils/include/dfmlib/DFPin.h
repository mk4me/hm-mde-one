#ifndef HEADER_GUARD__DFPIN_H__
#define HEADER_GUARD__DFPIN_H__

#include <dfmlib/Pin.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa pinów œwiadomych istnienia danych i wymiany tych danych. Dzia³aj¹ z wêz³ami œwiadomymi danych - DFNode
class DFPin : public Pin {
public:

    //! \param pinName nazwa pinu
    //! \param required Czy pin jest wymagany? (dotyczy tylko pinów wejœciowych)
    //! \param requiredPins Zbiór pinów od których jest uzale¿nione wystêpowanie informacji na danym pinie (dotyczy pinów wyjœciowych)
    DFPin(const std::string & pinName = std::string(), bool required = false,
        const REQ_PINS_SET & requiredPins = REQ_PINS_SET());

    //! Wirtualny destruktor
	virtual ~DFPin(void);

    //! \return Czy pin zosta³ zaktualizowany ( za³adowano mu dane )
	bool isUpdated() const;

    //! Resetuje stan update pinu po przekazaniu danych (wêze³ wyjœciowy - do innego pina / wêze³ wejœciowy - do wêz³a przetwarzaj¹cego)
	void reset();

    //! \return Czy dane w pinie s¹ gotowe do odczytu
	void update();

    //! \param pin Pin do sprawdzenia czy jest typu DFPin
    //! \return Prawda jesli pin jest typu DFPin, inaczej false
	static bool isDFPin(CPinPtr pin);

    //! \param pin Pin do konwertowania do typu DFPin
    //! \return WskaŸnik do pinu typu DFPin lub nullptr (pusty wskaŸnik)
	static DFPinPtr getDFPin(PinPtr pin);

    //! \param pin Pin do konwertowania do typu DFPin
    //! \return WskaŸnik do pinu typu const DFPin lub nullptr (pusty wskaŸnik)
    static CDFPinPtr getDFPin(CPinPtr pin);

protected:

    //! Metoda odpalana przy ka¿dym wywo³aniu update()
	virtual void onUpdate();

    //! Metoda pozwalaj¹ca kopiowaæ dane pomiêdzy wêz³ami - kopiowanie inicjuje wêze³ wyjsciowy i wywo³uje t¹ metodê na wêŸle wejœciowym
    //! Mechanizm ten przenosi odpowiedzialnoœæ kopiowania danych na wêze³ wejœciowy który mo¿e miesz szersz¹ funkcjonalnoœæ (kompatybilnoœæ typów) ni¿ wêze³ wyjœciowy
    //! \param pin Pin z którego wêze³ wejsciowy powinien skopiowaæ dane
	virtual void copyDataFromPin(DFPinPtr pin);

private:

    //! Czy wêze³ zosta³ zaktualizowany - za³adowano nowe dane
	bool updated;
};

}

#endif