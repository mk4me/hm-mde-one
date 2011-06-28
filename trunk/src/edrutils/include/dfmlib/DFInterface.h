#ifndef HEADER_GUARD__DFINTERFACE_H__
#define HEADER_GUARD__DFINTERFACE_H__

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa wprowadzaj¹ca obs³uge przep³ywu danych w data flow.
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

    //! Metoda resetuj¹ca stan obiektu - wywo³ywana po ka¿dym pe³nym cyklu danych
	virtual void reset();

    //! Metoda informuj¹ca o zmianie otoczenia obiektu - obiekt sam musi zweryfikowaæ zmianê i na ni¹ zareagowaæ
	void notify();

protected:

    //! Metoda czysto wirtualna - tutaj obiekt implementuje przetwarzanie danych
	virtual void process() = 0;

private:

    //! Czy obiekt mo¿e przetwarzaæ dane
	bool objectEnable;
};

}

#endif
