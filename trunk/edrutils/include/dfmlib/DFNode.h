#ifndef HEADER_GUARD__DFNODE_H__
#define HEADER_GUARD__DFNODE_H__

#include <dfmlib/Node.h>
#include <dfmlib/DFInterface.h>
#include <boost/function.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

class DFModel;

//! Klasa reprezentuj¹ca wêze³ z mo¿liwoœcia przetwarzania i przekazywania danych na bazie DFPinów w DFModel
class DFNode :
	public Node, public DFInterface
{

    friend class DFModel;

private:

    typedef boost::function<void(void)> OnLeafProcessedCallback;

public:

    //! \param nodeName Nazwa wêz³a
    //! \param processingAllowed Czy przetwarzanie danych aktywne
    //! \param propagatingAllowed Czy propagowanie danych aktywne
    DFNode(const std::string & nodeName = std::string()/*, bool processingAllowed = true,
        bool propagatingAllowed = true*/);

    //! Wirtualny destruktor
	virtual ~DFNode(void);

    //! \param allow Zezwala/wylancza przetwarzanie danych
	void allowProcessing(bool allow);

    //! \param allow Zezwala/wylancza propagowanie danych dalej
	void allowPropagating(bool allow);

    //! \return Czy przetwarzanie jest dozwolone
	bool isProcessingAllowed() const;

    //! \return Czy propagowanie danych dalej jest dozwolone
	bool isPropagatingAllowed() const;

    //! \param node Wêze³ podstawowy do "podniesienia" na DFNode
    //! \return DFNode dla danego wêz³a jeœli jest jego pochodn¹ lub nullptr jeœli konwersja niemo¿liwa
	static DFNPtr getDFNode(const NPtr & node);

protected:

    //! ---------------- Interfejs DFInterface ---------------------------

	virtual void process();

    //! ---------------- DFNode -------------------------------------------

    //! \return Czy piny wejœciowe maj¹ za³adowane dane poprawnie
	virtual bool checkInputPins() const;

    //! Aktualizuje status pinów wyjœciowych - notify()
	virtual void updateOutputPins();

    //! Ustawia dane pinów wyjœciowych po przetworzeniu danych
    virtual void setOutputData();

    //! Przetwarza dane w³aœciwe
    virtual void processData();

private:

    //! Czy przetwarzanie danych aktywne
	bool processingAllowed;

    //! Czy Propagowanie danych aktywne
	bool propagatingAllowed;

    //! Akcja wywo³ywana na rzecz modelu przez wêze³ liœæ
    OnLeafProcessedCallback onLeafProcessedCallback;
};

}

#endif