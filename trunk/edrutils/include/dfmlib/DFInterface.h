#ifndef HEADER_GUARD__DFINTERFACE_H__
#define HEADER_GUARD__DFINTERFACE_H__

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa wprowadzaj�ca obs�uge przep�ywu danych w data flow.
class DFInterface
{
public:

    //! Konstruktor domyslny
    //! \param enable Czy obiekt moze przetwarzac dane podczas notyfikacji
	DFInterface(bool enable = true);

    //! Wirtualny destruktor
	virtual ~DFInterface(void);

    //! \param enable Czy obiekt moze przetwarzac dane w momencie notyfikacji
    //! false blokuje przetwarzanie danych
    void setEnable(bool enable);

    //! \return Czy obiekt moze przetwarzac informacje
	bool isEnable() const;

    //! Metoda resetuj�ca stan obiektu - wywo�ywana po ka�dym pe�nym cyklu danych
	virtual void reset();

    //! Metoda informuj�ca o zmianie otoczenia obiektu - obiekt sam musi zweryfikowa� zmian� i na ni� zareagowa�
	void notify();

protected:

    //! Metoda czysto wirtualna - tutaj obiekt implementuje przetwarzanie danych
	virtual void process() = 0;

private:

    //! Czy obiekt mo�e przetwarza� dane
	bool objectEnable;
};

}

#endif
