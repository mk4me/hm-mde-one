#ifndef HEADER_GUARD__DFSOURCENODE_H__
#define HEADER_GUARD__DFSOURCENODE_H__

#include <dfmlib/DFNode.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

//! Klasa rozszerzaj�ca w�ze� DFNode o w�z�y �r�d�owe - nie maj� one pin�w wej�ciowych , oferuj� informacj� o dostepno�ci kolejnych danych na wyj�ciach
class DFSourceNode :
	public virtual DFNode
{
public:

    //! \param nodeName Nazwa w�z�a
    //! \param processingAllowed Czy przetwarzanie danych jest dozwolone
    //! \param propagatingAllowed Czy propagowanie danych jest dozwolone
    DFSourceNode(const std::string & nodeName = std::string(), bool processingAllowed = true,
        bool propagatingAllowed = true);

    //! Wirtualny destruktor
	virtual ~DFSourceNode(void);

    //! Czysto wirtualna funkcja informuj�ca o dost�pno�ci kolejnych danych w w�le
    //! \return Czy w�ze� mo�e zaoferowa� kolejn� porcj� danych
	virtual bool hasMoreData() const = 0;

    //! Wywolanie tej metody zrzuca wyj�tek - nie jest dozwolone dodawanie pin�w wej�ciowych do �r�de�
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