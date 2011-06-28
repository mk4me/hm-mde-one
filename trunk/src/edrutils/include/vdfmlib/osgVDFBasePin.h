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

    //! Typ reprezentuj�cy wizualny stan pinu - aktywny i statyczny stan
	enum VisualStatus{
        OK,             //! z pinem wszystko ok, ewentualnie mo�na ��czyc piny
		ACTIVE,         //! pin aktualnie aktywny, aktualnie wskazywany
		INCOMPATIBLE,   //! pin niekompatybilny podczas ��czenia
		REQUIRED,       //! pin wymagany
		CONNECTED,      //! pin pod��czony
		INCOMPLETE      //! pin, kt�rego zale�no�ci nie zostas�y w pe�ni spe�nione
    };

    //! Mapa ��cz�ca wizualne po��czenie z pinami wizualnymi po drugiej stornie tych po��cze�
	typedef std::map<osg::Geode*, osgVDFBasePin*> Connections;

public:
	//META_Object(osgWidget::Widget, osgVDFBasePin);

	osgVDFBasePin (const std::string& name = std::string());
    osgVDFBasePin (const osgVDFBasePin& pin, const osg::CopyOp& co);

	~osgVDFBasePin(void);

    //!
    virtual void positioned();

    //! \param wm WindowManager kt�ry nami zarz�dza
	virtual void managed(osgWidget::WindowManager* wm);

    //! \return Pin logiczny kt�ry reprezentujemy
	const dflm::PinPtr & getModelPin() const;

    //! \return Wizualny w�ze� b�d�cy naszym rodzicem
	osgVDFBaseNode * getVisualNode() const;

    //! \return Aktualny statyczny stantus
	VisualStatus getStaticVisualStatus() const;

    //! \param refPin Pin wzgl�dem kt�rego sprawdzxamy nasz dynamiczny status
    //! \param model Model logiczny wzgl�dem kt�rego sprawdzamy nasz stan
	VisualStatus getDynamicVisualStatus(const dflm::PinPtr & refPin, const dflm::MPtr & model) const;

    //! \param refPin Pin wzgl�dem kt�rego sprawdzxamy nasz dynamiczny status
    //! \param model Model logiczny wzgl�dem kt�rego sprawdzamy nasz stan
	VisualStatus getDynamicVisualStatus(osgVDFBasePin * refPin, const dflm::MPtr & model) const;

    //! \return Centralny punkt pinu z punktu widzenia przy�anczania po��cze�
	virtual const osgWidget::XYCoord & getCenterPosition();
    //! \return Centralny punkt pinu z punktu widzenia przy�anczania po��cze�
	virtual osgWidget::XYCoord getCenterPosition() const;
	
    //! \return Mapa po��cze� wizualnych pinu
	const Connections & getConnections() const;

    //! \return Zwraca aktualny wizualny status pinu
	VisualStatus getVisualStatus() const;

private:

    //! \param pinVisualStatus Wizualny status pinu oparty na podj�tych na nim akcjach b�d� jego statycznemu statusowi
    virtual void graphSetStatus(VisualStatus pinVisualStatus) = 0;

    //! \param pinVisualStatus Nowy wizualny status pinu,
    //! wi��e si� ze zmian� wygl�du pinu przez dostepn� metod� wirtualn� implementowan� przez konkretn� reprezentacj� wizualn� pinu
    void setVisualStatus(VisualStatus pinVisualStatus);

    //! \param connection Po��czenie wizualne kt�re dodajemy
    //! \param other Pin wizualny po drugiej stronie po��czenia
	void addConnection(osg::Geode * connection, osgVDFBasePin * other);

    //! \param connection Po��czenie wizualne, kt�re usuwamy
	void removeConnection(osg::Geode * connection);

private:

    //! modelPin logiczny, kt�rego ta wizualna reprezentacja dotyczy
	dflm::PinPtr modelPin;

    //! Mapa po��cze� i poin�w po drugiej stornie po��czenia
	Connections connections;

    //! Wizualny w�ze� w kt�rym wyst�puje ten wizulany modelPin
	osgVDFBaseNode * visualParentNode;

    //! Wizualny model w kontek�cie kt�rego wyst�puje ten wizualny modelPin
	osgVDFBaseModel * visualModel;

    //! Wiazualny status tego pinu
	VisualStatus visualStatus;

    //! Pozycja �roka pinu potrzebna przy realizacji wizualnych po��cze�
	osgWidget::XYCoord centerPosition;

    //! Czy pozycj� centraln� trzeba ponownie wyznaczy�, bo modelPin zmieni� pozycj�
	bool centerChanged;

    //! Funktor nadany przez model do aktualizacji po��cze� vizualnych
    VisualConnectionsUpdater connectionsUpdater;
};

}

#endif