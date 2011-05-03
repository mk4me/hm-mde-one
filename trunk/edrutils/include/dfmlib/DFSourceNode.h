#ifndef HEADER_GUARD__DFSOURCENODE_H__
#define HEADER_GUARD__DFSOURCENODE_H__

#include <dfmlib/DFNode.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa rozszerzaj¹ca wêze³ DFNode o wêz³y Ÿród³owe - nie maj¹ one pinów wejœciowych , oferuj¹ informacjê o dostepnoœci kolejnych danych na wyjœciach
class DFSourceNode :
	public virtual DFNode
{
public:

    //! \param nodeName Nazwa wêz³a
    //! \param processingAllowed Czy przetwarzanie danych jest dozwolone
    //! \param propagatingAllowed Czy propagowanie danych jest dozwolone
    DFSourceNode(const std::string & nodeName = std::string(), bool processingAllowed = true,
        bool propagatingAllowed = true);

    //! Wirtualny destruktor
	virtual ~DFSourceNode(void);

    //! Czysto wirtualna funkcja informuj¹ca o dostêpnoœci kolejnych danych w wêŸle
    //! \return Czy wêze³ mo¿e zaoferowaæ kolejn¹ porcjê danych
	virtual bool hasMoreData() const = 0;

    //! Wywolanie tej metody zrzuca wyj¹tek - nie jest dozwolone dodawanie pinów wejœciowych do Ÿróde³
    //! \param newPin Pin wejsciowy do dodania
	virtual void addInPin(PinPtr newPin);

protected:

    //! \return Zawsze true
	virtual bool checkInputPins() const;

    //! Nie robi nic
	virtual void resetInputPins();
};

}

#endif