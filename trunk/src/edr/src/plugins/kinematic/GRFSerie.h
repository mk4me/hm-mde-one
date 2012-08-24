/********************************************************************
	created:	2011/08/29
	created:	29:8:2011   9:25
	filename: 	GRFDrawer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__GRFSERIE_H__
#define HEADER_GUARD_KINEMATIC__GRFSERIE_H__

#include <list>
#include <core/IVisualizer.h>
#include <utils/DataChannel.h>
#include <plugins/c3d/IForcePlatform.h>
#include <osg/Geode>
#include "KinematicVisualizer.h"

//! empirycznie dobrany wsp�czynnik d�ugo�ci wizualizowanych si�
const float grfScale = 0.0008f;

//! Seria danych odpowiedzialna za wizualizacje p�yt naciskowych i si� na nie dzia�aj�cych
class GRFSerie : public KinematicSerie
{
public:
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
	typedef osg::ref_ptr<osg::Group> GroupPtr;
	typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
	typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
	typedef osg::ref_ptr<osg::ShapeDrawable> ShapeDrawablePtr;
	typedef utils::DataChannelTimeAccessor<osg::Vec3f, float> TimeAccessor;

public:
	//! Konstruktor
	//! \param visualizer wizualizator tworz�cy ta serie
	GRFSerie(KinematicVisualizer * visualizer);

public:
	//! Ustawia nazw� serii
	//! \param name ustawiana nazwa 
	virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string & getName() const;
	//! Ustawienie danych, inicjalizacja 
	//! \param data dane typu GRFCollection
	virtual void setData(const core::ObjectWrapperConstPtr & data);
    //! \return ustawione dane
    virtual const core::ObjectWrapperConstPtr & getData() const;
	//! \return d�ugo�� kana�u w sekundach
	virtual double getLength() const;
	//! Czas zawiera si� mi�dzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kana�u w sekundach
	virtual void setLocalTime(double time);

private:
    //! Struktura pomocna przy wizualizacji wektora si�y.
    //! Wektor sk�ada si� z prostopadloscianu i stozka
	struct Arrow 
	{
        //! g��wny w�ze� z wektorem
		TransformPtr mainPtr;
        //! w�ze� z prostopadloscianem
		TransformPtr boxPtr;
        //! w�ze� ze stozkiem
		TransformPtr conePtr;
        //! geometria prostopadloscianu
		ShapeDrawablePtr boxShape; 
        //! geometria stozka
		ShapeDrawablePtr coneShape;

		//! Ustawienie transformacji, w taki spos�b, aby wektor znalazl si� pomi�dzy dwoma punktami
		//! \param from punkt, od kt�rego prowadzony jest wektor
		//! \param to punkt, do kt�rego prowadzone jest wektor
		void setArrow(osg::Vec3 from, osg::Vec3 to);
		//! Ustawienie koloru wektora
		//! \param color kolor do ustawienia RGBA (0 - 1)
		void setColor(const osg::Vec4& color);
		//! \return aktualny kolor wektora
		const osg::Vec4& getColor() const;
	};
	typedef boost::shared_ptr<Arrow> ArrowPtr;
	typedef boost::shared_ptr<const Arrow> ArrowConstPtr;

    typedef std::map<IForcePlatformConstPtr, osg::ref_ptr<osg::ShapeDrawable> > Platform2Shape;

    //! Klasa u�atwia rysowanie "ducha" strza�ki. Wizualizowane s� poprzednie pozycje strza�ki. Im starcza pozycja tym mniejsza alfa.
	class GhostStack
	{
	public:
        //! Stan strza�ki, pozycje - od - do
		typedef std::pair<osg::Vec3, osg::Vec3> ArrowState;
		//! Konstruktor
		//! \param maxSize maksymalny rozmiar stosu strza�ek
		//! \param hookNode w�ze�, pod kt�ry beda podczepiane strza�ki
		//! \param color kolor strza�ek
		GhostStack(int maxSize, GroupPtr hookNode, const osg::Vec4& color );
        //! Dodanie strza�ki do stosu i usuwa najstarsza strza�ke, je�li jest taka potrzeba
        //! \param state 
        void addState(const ArrowState& state);
		//! Aktualizacja kolor�w strza�ek, zarz�dzanie widoczno�ci� w�z��w
		void update();

	private:
        //! Kolor strza�ek
		osg::Vec4 color;
        //! Stos ze strza�kami
		std::list<ArrowPtr> stackArrows;
        //! w�ze�, pod kt�ry podpi�te s� strza�ki
		GroupPtr hookNode;
        //! maksymalny rozmiar stosu
		int maxSize;
	};
	typedef boost::shared_ptr<GhostStack> GhostStackPtr;
	typedef boost::shared_ptr<const GhostStack> GhostStackConstPtr;

private:
	//! Tworzy w�ze� z geometria wszystkich p�yt GRF
	//! \param platforms kolecja platform pochodz�cych z C3D
    //! \return utworzony w�ze�
	GroupPtr createPlatformsGroup(const IForcePlatformCollection& platforms);
	//! Tworzy "skrzydla motyla" czyli graficzna reprezentacje zmiany si�y nacisku konkretnej p�yty w czasie
	//! \param grf dane p�yty GRF
	//! \param maxLength tutaj zwracana jest maksymalna d�ugo�� si�y, jest to pomocne przy kolorowaniu wektora
    //! \return utworzony w�ze�
	GroupPtr createButterfly(GRFCollectionConstPtr grf, float& maxLength) const;
    //! Tworzy pojedyncze "skrzydla motyla" czyli graficzna reprezentacje zmiany si�y nacisku konkretnej p�yty w czasie
    //! \param step wykryty wczesniej krok w obr�bie p�yty pomiarowej
    //! \param maxLength maksymalna d�ugo�� si�y, jest to pomocne przy kolorowaniu wektora
    //! \param platform plaforma, dla kt�rej tworzona jest geometria
    //! \return utworzony w�ze� 
    GeodePtr createStep( IForcePlatform::IStepConstPtr step, float &maxLength, IForcePlatformConstPtr platform) const;
    //! Tworzy w�ze� z geometria pojedynczej p�yty GRF
    //! \param texture tekstura, kt�ra ma by� na prostopadloscianie, mo�e by� nullptr
    //! \param origin punkt, w kt�rym znajduje si� lewy g�rny r�g p�yty
    //! \param width szeroko�� p�yty
    //! \param lenght d�ugo�� p�yty
    //! \param height wysoko�� p�yty
    //! \return utworzony w�ze�
    TransformPtr createPlatformTransform(osg::Texture2D* texture, const osg::Vec3& origin, float width, float lenght, float height) const;
	//! Statyczna metoda, tworzy strukture z geometria i opisem strza�ki
    //! \return utworzony w�ze�
	static ArrowPtr createArrow();
	//! Pobiera teksture z resource'�w
	//! \param number obecnie 1 lub 2
    //! \return zaladowana tekstura, wyj�tek w przypadku bledu
	static osg::ref_ptr<osg::Texture2D> getTexture(int number);
    //! pr�buje pobra� teksture z resource'�w
    //! \param ret tutaj trafi pobrana tekstura
    //! \param number obecnie 1 lub 2
    //! \return czy operacja si� udala
    static bool tryGetTexture(osg::ref_ptr<osg::Texture2D>& ret, int number);
	
private:
    //! Wizualizator, kt�ry utworzy� serie
	KinematicVisualizer * visualizer;
    //! Dane GRF dostarczone serii
	GRFCollectionPtr grfCollection;
    //! maksymalna d�ugo�� si�y, pomocne przy kolorowaniu wektora
	float maxLength;
    //! mapa (krok -> (strza�ka, jej ghost) pomocne przy rysowaniu i od�wie�aniu wizualizacji
    std::map<IForcePlatform::IStepConstPtr, std::pair<ArrowPtr, GhostStackPtr>> stepsData;
    //! pobrana z zasob�w tekstura dla pierwszej p�yty
	static osg::ref_ptr<osg::Texture2D> texture1;
    //! pobrana z zasob�w tekstura dla drugiej p�yty
    static osg::ref_ptr<osg::Texture2D> texture2;
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;
    //! nazwa serii
    std::string name;
    //! mapa, kt�ra u�atwia pobranie geometrii na postawie platformy z C3D
    Platform2Shape platform2Shape;
};




#endif
