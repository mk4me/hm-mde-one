#ifndef HEADER_GUARD__VDFBASENODE_H__
#define HEADER_GUARD__VDFBASENODE_H__

#include <vdfmlib/osgVDFBasePin.h>
#include <osgWidget/Window>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////


    //! Klasa bazowa dla wizualnych reprezentacji w�z��w logicznych z Data Flow.
class osgVDFBaseModel;

class osgVDFBaseNode : public osgWidget::Window
{
    friend class osgVDFBaseModel;

private:

    typedef std::set<osgVDFBaseNode*> VNodes;
    typedef std::vector<osgVDFBaseNode*> VNodesDifference;

public:
    //! Typ opisuj�cy stan wizualny w�z�a w wyniku operacji na nim (wizualnych), wynikaj�cy z jego po�o�enia
	enum VisualStatus {
        OK,         //! z w�z�em wszystko ok
        COLLISION,  //! w�ze� bierz eudzia� w kolizji - nachodzi si� z innym w�z�m/ami
        ACTIVE      //! w�ze� zaznaczony, nadrz�dny status wzgl�dem wszystkich innych, kiedy w�ze� jest zaznaczony
    };


    //TODO - do zmian p�xniej, musi to by� bardziej elastyczne!!
    //! Typ opisuj�cy charakter w�z�a, uzywany przy tworzeniu toolbara
	enum NodeType {
        SOURCE,    //! w�ze� �r�d�o
        VISUALIZE, //! w�ze� wizualizuj�cy
        PROCESS,   //! w�ze� przetwarzaj�cy
        ANALYZE    //! w�ze� analizuj�cy dane
    };

    //! Zbi�r pin�w wizualnych
	typedef std::set<osg::ref_ptr<osgVDFBasePin> > Pins;

public:

	//META_Window(osgWidget::Window, osgVDFBaseNode);

	osgVDFBaseNode(const std::string & nodeName = std::string());
	virtual ~osgVDFBaseNode(void);

    //! \return Logiczny w�ze� zwi�zany z t� graficzna reprezentacj�
	const dflm::NPtr & getModelNode() const;

    //! \return Wizualny model, z kt�rym skojarzony jest ten wizualny w�ze�
	osgVDFBaseModel * getVisualModel() const;

    //! \return Kolekcja wizualnych pin�w wejsciowych
	const Pins & getInPins() const;

    //! \return Kolekcja wizualnych pin�w wyj�ciowych
	const Pins & getOutPins() const;

	//for debug purpose only!!
	virtual void loggZCoordinates() const;

    //! \return Wizualn� nazw� w�z�a
	virtual std::string getName() const = 0;

    //! \return Wizualny stan w�z�a
	VisualStatus getVisualStatus() const;

    //! \return Powierzchnia w�z�a
    osgWidget::point_type getArea() const;

    //! Usuwa w�ze� z modelu, wcze�niej wywo�uje zadan� funkcj� je�li ustawiono
    void deleteNode();

private:

    //! \param inPin Wizualny pin wejsciowy do dodania
    //! \param pinName Nazwa dodawanego wizualnego pina wejsciowego
    void addInPin(osgVDFBasePin * inPin, const std::string & pinName = std::string());

    //! \param outPin Wizualny pin wyjsciowy do dodania
    //! \param pinName Nazwa dodawanego wizualnego pina wyjsciowego
    void addOutPin(osgVDFBasePin * outPin, const std::string & pinName = std::string());

    //! \param nodeVisualStatus Wizualny stan w�z�a ze wzgl�du na podj�te na nim akcje u�ytkownika lub jego statyczny stan w modelu logicznym
    void setVisualStatus(VisualStatus nodeVisualStatus);

    //! \param name Nowa wizualna nazwa w�z�a
    virtual void setName(const std::string & name) = 0;

    //! \param inPin Wizualny pin dodawany do w�z�a
    //! \param pinName Nazwa wizualnego pinu
	virtual void graphAddInPin(osgVDFBasePin * inPin, const std::string & pinName) = 0;

    //! \param outPin Wyj�ciowy wizualny pin dodawany do w�z�a
    //! \param pinName Nazwa wizualnego pinu dodawanego do w�z�a
	virtual void graphAddOutPin(osgVDFBasePin * outPin, const std::string & pinName) = 0;

    //! \param nodeVisualStatus Nowy wizualny status w�z�a
	virtual void graphSetStatus(VisualStatus nodeVisualStatus) = 0;

    //! Chwilowo wy�ancza mo�liwo�� edycji i usuwania w�z�a
    virtual void lockForEditing(bool lock) = 0;

    //! \param area powierzchnia w�z�a
    void setArea(osgWidget::point_type area);

    void setCollisionNodes(const VNodes & vnodes);

    const VNodes & getCollisionNodes() const;

    protected:

    //! Aktualny status wizulany w�z�a
	VisualStatus visualStatus;

	//! W�ze� logiczny odpowiadaj�cy tej wizualnej reprezentacji
	dflm::NPtr modelNode;

	//! Piny wej�ciowe w wersji wizualnej
	Pins inPins;

    //! Piny wyj�ciowe w wersji wizualnej
	Pins outPins;

    //! Wizualny model w�z�a
	osgVDFBaseModel * visualModel;

    //! Obszar w�z�a
    osgWidget::point_type area;

    //! Wizualne wezly z ktorymi aktualnie jest w kolizji
    VNodes collisionNodes;
};

}

#endif
