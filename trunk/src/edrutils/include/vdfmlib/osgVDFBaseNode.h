#ifndef HEADER_GUARD__VDFBASENODE_H__
#define HEADER_GUARD__VDFBASENODE_H__

#include <vdfmlib/osgVDFBasePin.h>
#include <osgWidget/Window>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////


    //! Klasa bazowa dla wizualnych reprezentacji wêz³ów logicznych z Data Flow.
class osgVDFBaseModel;

class osgVDFBaseNode : public osgWidget::Window
{
    friend class osgVDFBaseModel;

private:

    typedef std::set<osgVDFBaseNode*> VNodes;
    typedef std::vector<osgVDFBaseNode*> VNodesDifference;

public:
    //! Typ opisuj¹cy stan wizualny wêz³a w wyniku operacji na nim (wizualnych), wynikaj¹cy z jego po³o¿enia
	enum VisualStatus {
        OK,         //! z wêz³em wszystko ok
        COLLISION,  //! wêze³ bierz eudzia³ w kolizji - nachodzi siê z innym wêzê³m/ami
        ACTIVE      //! wêze³ zaznaczony, nadrzêdny status wzglêdem wszystkich innych, kiedy wêze³ jest zaznaczony
    };


    //TODO - do zmian póxniej, musi to byæ bardziej elastyczne!!
    //! Typ opisuj¹cy charakter wêz³a, uzywany przy tworzeniu toolbara
	enum NodeType {
        SOURCE,    //! wêze³ Ÿród³o
        VISUALIZE, //! wêze³ wizualizuj¹cy
        PROCESS,   //! wêze³ przetwarzaj¹cy
        ANALYZE    //! wêze³ analizuj¹cy dane
    };

    //! Zbiór pinów wizualnych
	typedef std::set<osg::ref_ptr<osgVDFBasePin> > Pins;

public:

	//META_Window(osgWidget::Window, osgVDFBaseNode);

	osgVDFBaseNode(const std::string & nodeName = std::string());
	virtual ~osgVDFBaseNode(void);

    //! \return Logiczny wêze³ zwi¹zany z t¹ graficzna reprezentacj¹
	const dflm::NPtr & getModelNode() const;

    //! \return Wizualny model, z którym skojarzony jest ten wizualny wêze³
	osgVDFBaseModel * getVisualModel() const;

    //! \return Kolekcja wizualnych pinów wejsciowych
	const Pins & getInPins() const;

    //! \return Kolekcja wizualnych pinów wyjœciowych
	const Pins & getOutPins() const;

	//for debug purpose only!!
	virtual void loggZCoordinates() const;

    //! \return Wizualn¹ nazwê wêz³a
	virtual std::string getName() const = 0;

    //! \return Wizualny stan wêz³a
	VisualStatus getVisualStatus() const;

    //! \return Powierzchnia wêz³a
    osgWidget::point_type getArea() const;

    //! Usuwa wêze³ z modelu, wczeœniej wywo³uje zadan¹ funkcjê jeœli ustawiono
    void deleteNode();

private:

    //! \param inPin Wizualny pin wejsciowy do dodania
    //! \param pinName Nazwa dodawanego wizualnego pina wejsciowego
    void addInPin(osgVDFBasePin * inPin, const std::string & pinName = std::string());

    //! \param outPin Wizualny pin wyjsciowy do dodania
    //! \param pinName Nazwa dodawanego wizualnego pina wyjsciowego
    void addOutPin(osgVDFBasePin * outPin, const std::string & pinName = std::string());

    //! \param nodeVisualStatus Wizualny stan wêz³a ze wzglêdu na podjête na nim akcje u¿ytkownika lub jego statyczny stan w modelu logicznym
    void setVisualStatus(VisualStatus nodeVisualStatus);

    //! \param name Nowa wizualna nazwa wêz³a
    virtual void setName(const std::string & name) = 0;

    //! \param inPin Wizualny pin dodawany do wêz³a
    //! \param pinName Nazwa wizualnego pinu
	virtual void graphAddInPin(osgVDFBasePin * inPin, const std::string & pinName) = 0;

    //! \param outPin Wyjœciowy wizualny pin dodawany do wêz³a
    //! \param pinName Nazwa wizualnego pinu dodawanego do wêz³a
	virtual void graphAddOutPin(osgVDFBasePin * outPin, const std::string & pinName) = 0;

    //! \param nodeVisualStatus Nowy wizualny status wêz³a
	virtual void graphSetStatus(VisualStatus nodeVisualStatus) = 0;

    //! Chwilowo wy³ancza mo¿liwoœæ edycji i usuwania wêz³a
    virtual void lockForEditing(bool lock) = 0;

    //! \param area powierzchnia wêz³a
    void setArea(osgWidget::point_type area);

    void setCollisionNodes(const VNodes & vnodes);

    const VNodes & getCollisionNodes() const;

    protected:

    //! Aktualny status wizulany wêz³a
	VisualStatus visualStatus;

	//! Wêze³ logiczny odpowiadaj¹cy tej wizualnej reprezentacji
	dflm::NPtr modelNode;

	//! Piny wejœciowe w wersji wizualnej
	Pins inPins;

    //! Piny wyjœciowe w wersji wizualnej
	Pins outPins;

    //! Wizualny model wêz³a
	osgVDFBaseModel * visualModel;

    //! Obszar wêz³a
    osgWidget::point_type area;

    //! Wizualne wezly z ktorymi aktualnie jest w kolizji
    VNodes collisionNodes;
};

}

#endif
