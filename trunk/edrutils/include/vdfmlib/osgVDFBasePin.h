#ifndef HEADER_GUARD__VDFBASEPIN_H__
#define HEADER_GUARD__VDFBASEPIN_H__

#include <osgWidget/Widget>
#include <dfmlib/DFPin.h>
#include <map>
#include <osgui/Utils2D.h>
#include <boost/function.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

class osgVDFBaseModel;
class osgVDFBaseNode;

class osgVDFBasePin : public osgWidget::Widget
{
	friend class osgVDFBaseModel;
	friend class osgVDFBaseNode;

private:

    typedef boost::function<void(osg::Geode *, const osgWidget::XYCoord &)> VisualConnectionsUpdater;

public:

    //! Typ reprezentuj¹cy wizualny stan pinu - aktywny i statyczny stan
	enum VisualStatus{
        OK,             //! z pinem wszystko ok, ewentualnie mo¿na ³¹czyc piny
		ACTIVE,         //! pin aktualnie aktywny, aktualnie wskazywany
		INCOMPATIBLE,   //! pin niekompatybilny podczas ³¹czenia
		REQUIRED,       //! pin wymagany
		CONNECTED,      //! pin pod³¹czony
		INCOMPLETE      //! pin, którego zale¿noœci nie zostas³y w pe³ni spe³nione
    };

    //! Mapa ³¹cz¹ca wizualne po³¹czenie z pinami wizualnymi po drugiej stornie tych po³¹czeñ
	typedef std::map<osg::Geode*, osgVDFBasePin*> Connections;

public:
	//META_Object(osgWidget::Widget, osgVDFBasePin);

	osgVDFBasePin (const std::string& name = std::string());
    osgVDFBasePin (const osgVDFBasePin& pin, const osg::CopyOp& co);

	~osgVDFBasePin(void);

    //!
    virtual void positioned();

    //! \param wm WindowManager który nami zarz¹dza
	virtual void managed(osgWidget::WindowManager* wm);

    //! \return Pin logiczny który reprezentujemy
	const dflm::PinPtr & getModelPin() const;

    //! \return Wizualny wêze³ bêd¹cy naszym rodzicem
	osgVDFBaseNode * getVisualNode() const;

    //! \return Aktualny statyczny stantus
	VisualStatus getStaticVisualStatus() const;

    //! \param refPin Pin wzglêdem którego sprawdzxamy nasz dynamiczny status
    //! \param model Model logiczny wzglêdem którego sprawdzamy nasz stan
	VisualStatus getDynamicVisualStatus(const dflm::PinPtr & refPin, const dflm::MPtr & model) const;

    //! \param refPin Pin wzglêdem którego sprawdzxamy nasz dynamiczny status
    //! \param model Model logiczny wzglêdem którego sprawdzamy nasz stan
	VisualStatus getDynamicVisualStatus(osgVDFBasePin * refPin, const dflm::MPtr & model) const;

    //! \return Centralny punkt pinu z punktu widzenia przy³anczania po³¹czeñ
	virtual const osgWidget::XYCoord & getCenterPosition();
    //! \return Centralny punkt pinu z punktu widzenia przy³anczania po³¹czeñ
	virtual osgWidget::XYCoord getCenterPosition() const;
	
    //! \return Mapa po³¹czeñ wizualnych pinu
	const Connections & getConnections() const;

    //! \return Zwraca aktualny wizualny status pinu
	VisualStatus getVisualStatus() const;

private:

    //! \param pinVisualStatus Wizualny status pinu oparty na podjêtych na nim akcjach b¹dŸ jego statycznemu statusowi
    virtual void graphSetStatus(VisualStatus pinVisualStatus) = 0;

    //! \param pinVisualStatus Nowy wizualny status pinu,
    //! wi¹¿e siê ze zmian¹ wygl¹du pinu przez dostepn¹ metodê wirtualn¹ implementowan¹ przez konkretn¹ reprezentacjê wizualn¹ pinu
    void setVisualStatus(VisualStatus pinVisualStatus);

    //! \param connection Po³¹czenie wizualne które dodajemy
    //! \param other Pin wizualny po drugiej stronie po³¹czenia
	void addConnection(osg::Geode * connection, osgVDFBasePin * other);

    //! \param connection Po³¹czenie wizualne, które usuwamy
	void removeConnection(osg::Geode * connection);

private:

    //! modelPin logiczny, którego ta wizualna reprezentacja dotyczy
	dflm::PinPtr modelPin;

    //! Mapa po³¹czeñ i poinów po drugiej stornie po³¹czenia
	Connections connections;

    //! Wizualny wêze³ w którym wystêpuje ten wizulany modelPin
	osgVDFBaseNode * visualParentNode;

    //! Wizualny model w kontekœcie którego wystêpuje ten wizualny modelPin
	osgVDFBaseModel * visualModel;

    //! Wiazualny status tego pinu
	VisualStatus visualStatus;

    //! Pozycja œroka pinu potrzebna przy realizacji wizualnych po³¹czeñ
	osgWidget::XYCoord centerPosition;

    //! Czy pozycjê centraln¹ trzeba ponownie wyznaczyæ, bo modelPin zmieni³ pozycjê
	bool centerChanged;

    //! Funktor nadany przez model do aktualizacji po³¹czeñ vizualnych
    VisualConnectionsUpdater connectionsUpdater;
};

}

#endif