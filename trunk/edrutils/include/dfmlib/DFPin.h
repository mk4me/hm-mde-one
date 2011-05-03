#ifndef HEADER_GUARD__DFPIN_H__
#define HEADER_GUARD__DFPIN_H__

#include <dfmlib/Pin.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa pin�w �wiadomych istnienia danych i wymiany tych danych. Dzia�aj� z w�z�ami �wiadomymi danych - DFNode
class DFPin : public Pin {
public:

    //! \param pinName nazwa pinu
    //! \param required Czy pin jest wymagany? (dotyczy tylko pin�w wej�ciowych)
    //! \param requiredPins Zbi�r pin�w od kt�rych jest uzale�nione wyst�powanie informacji na danym pinie (dotyczy pin�w wyj�ciowych)
    DFPin(const std::string & pinName = std::string(), bool required = false,
        const REQ_PINS_SET & requiredPins = REQ_PINS_SET());

    //! Wirtualny destruktor
	virtual ~DFPin(void);

    //! \return Czy pin zosta� zaktualizowany ( za�adowano mu dane )
	bool isUpdated() const;

    //! Resetuje stan update pinu po przekazaniu danych (w�ze� wyj�ciowy - do innego pina / w�ze� wej�ciowy - do w�z�a przetwarzaj�cego)
	void reset();

    //! \return Czy dane w pinie s� gotowe do odczytu
	void update();

    //! \param pin Pin do sprawdzenia czy jest typu DFPin
    //! \return Prawda jesli pin jest typu DFPin, inaczej false
	static bool isDFPin(CPinPtr pin);

    //! \param pin Pin do konwertowania do typu DFPin
    //! \return Wska�nik do pinu typu DFPin lub nullptr (pusty wska�nik)
	static DFPinPtr getDFPin(PinPtr pin);

    //! \param pin Pin do konwertowania do typu DFPin
    //! \return Wska�nik do pinu typu const DFPin lub nullptr (pusty wska�nik)
    static CDFPinPtr getDFPin(CPinPtr pin);

protected:

    //! Metoda odpalana przy ka�dym wywo�aniu update()
	virtual void onUpdate();

    //! Metoda pozwalaj�ca kopiowa� dane pomi�dzy w�z�ami - kopiowanie inicjuje w�ze� wyjsciowy i wywo�uje t� metod� na w�le wej�ciowym
    //! Mechanizm ten przenosi odpowiedzialno�� kopiowania danych na w�ze� wej�ciowy kt�ry mo�e miesz szersz� funkcjonalno�� (kompatybilno�� typ�w) ni� w�ze� wyj�ciowy
    //! \param pin Pin z kt�rego w�ze� wejsciowy powinien skopiowa� dane
	virtual void copyDataFromPin(DFPinPtr pin);

private:

    //! Czy w�ze� zosta� zaktualizowany - za�adowano nowe dane
	bool updated;
};

}

#endif