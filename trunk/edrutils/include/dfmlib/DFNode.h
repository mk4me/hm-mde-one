#ifndef HEADER_GUARD__DFNODE_H__
#define HEADER_GUARD__DFNODE_H__

#include <dfmlib/Node.h>
#include <dfmlib/DFInterface.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa reprezentuj�ca w�ze� z mo�liwo�cia przetwarzania i przekazywania danych na bazie DFPin�w w DFModel
class DFNode :
	public Node, public DFInterface
{
public:

    //! Wirtualny destruktor
	virtual ~DFNode(void);

    //! \param newPin Pin wej�ciowy do dodania
	bool addInPin(PinPtr newPin);

    //! \param newPin Pin wyj�ciowy do dodania
	bool addOutPin(PinPtr newPin);

    //! \param allow Zezwala/wylancza przetwarzanie danych
	void allowProcessing(bool allow);

    //! \param allow Zezwala/wylancza propagowanie danych dalej
	void allowPropagating(bool allow);

    //! \return Czy przetwarzanie jest dozwolone
	bool isProcessingAllowed() const;

    //! \return Czy propagowanie danych dalej jest dozwolone
	bool isPropagatingAllowed() const;

    //! Funkcja pozwalaj�ca podpi�� metody konfiguracji w�z�a
	virtual void configureNode();

    //! \param node W�ze� podstawowy do "podniesienia" na DFNode
    //! \return DFNode dla danego w�z�a je�li jest jego pochodn� lub nullptr je�li konwersja niemo�liwa
	static DFNPtr getDFNode(NPtr node);

protected:

    //! Chroniony konstruktor
    //! \param nodeName Nazwa w�z�a
    //! \param processingAllowed Czy przetwarzanie danych aktywne
    //! \param propagatingAllowed Czy propagowanie danych aktywne
	DFNode(const std::string & nodeName = std::string(), bool processingAllowed = true,
		bool propagatingAllowed = true);

    //! ---------------- Interfejs DFInterface ---------------------------

    virtual void onEnableChange();

	void process();

    //! ---------------- DFNode -------------------------------------------

    //! \return Czy piny wej�ciowe maj� za�adowane dane poprawnie
	virtual bool checkInputPins() const;

    //! Aktualizuje status pin�w wyj�ciowych - notify()
	virtual void updateOutputPins();

    //! Ustawia dane pin�w wyj�ciowych po przetworzeniu danych
    virtual void setOutputData();

    //! Przetwarza dane w�a�ciwe
    virtual void processData();

private:


    //! Czy przetwarzanie danych aktywne
	bool processingAllowed;

    //! Czy Propagowanie danych aktywne
	bool propagatingAllowed;
};

}

#endif