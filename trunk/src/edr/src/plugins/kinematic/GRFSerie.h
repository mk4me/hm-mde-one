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

//! empirycznie dobrany wspó³czynnik d³ugoœci wizualizowanych si³
const float grfScale = 0.0008f;

//! Seria danych odpowiedzialna za wizualizacje p³yt naciskowych i si³ na nie dzia³aj¹cych
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
	//! \param visualizer wizualizator tworz¹cy ta serie
	GRFSerie(KinematicVisualizer * visualizer);

public:
	//! Ustawia nazwê serii
	//! \param name ustawiana nazwa 
	virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string & getName() const;
	//! Ustawienie danych, inicjalizacja 
	//! \param data dane typu GRFCollection
	virtual void setData(const core::ObjectWrapperConstPtr & data);
    //! \return ustawione dane
    virtual const core::ObjectWrapperConstPtr & getData() const;
	//! \return d³ugoœæ kana³u w sekundach
	virtual double getLength() const;
	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kana³u w sekundach
	virtual void setLocalTime(double time);

private:
    //! Struktura pomocna przy wizualizacji wektora si³y.
    //! Wektor sk³ada siê z prostopadloscianu i stozka
	struct Arrow 
	{
        //! g³ówny wêze³ z wektorem
		TransformPtr mainPtr;
        //! wêze³ z prostopadloscianem
		TransformPtr boxPtr;
        //! wêze³ ze stozkiem
		TransformPtr conePtr;
        //! geometria prostopadloscianu
		ShapeDrawablePtr boxShape; 
        //! geometria stozka
		ShapeDrawablePtr coneShape;

		//! Ustawienie transformacji, w taki sposób, aby wektor znalazl siê pomiêdzy dwoma punktami
		//! \param from punkt, od którego prowadzony jest wektor
		//! \param to punkt, do którego prowadzone jest wektor
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

    //! Klasa u³atwia rysowanie "ducha" strza³ki. Wizualizowane s¹ poprzednie pozycje strza³ki. Im starcza pozycja tym mniejsza alfa.
	class GhostStack
	{
	public:
        //! Stan strza³ki, pozycje - od - do
		typedef std::pair<osg::Vec3, osg::Vec3> ArrowState;
		//! Konstruktor
		//! \param maxSize maksymalny rozmiar stosu strza³ek
		//! \param hookNode wêze³, pod który beda podczepiane strza³ki
		//! \param color kolor strza³ek
		GhostStack(int maxSize, GroupPtr hookNode, const osg::Vec4& color );
        //! Dodanie strza³ki do stosu i usuwa najstarsza strza³ke, jeœli jest taka potrzeba
        //! \param state 
        void addState(const ArrowState& state);
		//! Aktualizacja kolorów strza³ek, zarz¹dzanie widocznoœci¹ wêz³ów
		void update();

	private:
        //! Kolor strza³ek
		osg::Vec4 color;
        //! Stos ze strza³kami
		std::list<ArrowPtr> stackArrows;
        //! wêze³, pod który podpiête s¹ strza³ki
		GroupPtr hookNode;
        //! maksymalny rozmiar stosu
		int maxSize;
	};
	typedef boost::shared_ptr<GhostStack> GhostStackPtr;
	typedef boost::shared_ptr<const GhostStack> GhostStackConstPtr;

private:
	//! Tworzy wêze³ z geometria wszystkich p³yt GRF
	//! \param platforms kolecja platform pochodz¹cych z C3D
    //! \return utworzony wêze³
	GroupPtr createPlatformsGroup(const IForcePlatformCollection& platforms);
	//! Tworzy "skrzydla motyla" czyli graficzna reprezentacje zmiany si³y nacisku konkretnej p³yty w czasie
	//! \param grf dane p³yty GRF
	//! \param maxLength tutaj zwracana jest maksymalna d³ugoœæ si³y, jest to pomocne przy kolorowaniu wektora
    //! \return utworzony wêze³
	GroupPtr createButterfly(GRFCollectionConstPtr grf, float& maxLength) const;
    //! Tworzy pojedyncze "skrzydla motyla" czyli graficzna reprezentacje zmiany si³y nacisku konkretnej p³yty w czasie
    //! \param step wykryty wczesniej krok w obrêbie p³yty pomiarowej
    //! \param maxLength maksymalna d³ugoœæ si³y, jest to pomocne przy kolorowaniu wektora
    //! \param platform plaforma, dla której tworzona jest geometria
    //! \return utworzony wêze³ 
    GeodePtr createStep( IForcePlatform::IStepConstPtr step, float &maxLength, IForcePlatformConstPtr platform) const;
    //! Tworzy wêze³ z geometria pojedynczej p³yty GRF
    //! \param texture tekstura, która ma byæ na prostopadloscianie, mo¿e byæ nullptr
    //! \param origin punkt, w którym znajduje siê lewy górny róg p³yty
    //! \param width szerokoœæ p³yty
    //! \param lenght d³ugoœæ p³yty
    //! \param height wysokoœæ p³yty
    //! \return utworzony wêze³
    TransformPtr createPlatformTransform(osg::Texture2D* texture, const osg::Vec3& origin, float width, float lenght, float height) const;
	//! Statyczna metoda, tworzy strukture z geometria i opisem strza³ki
    //! \return utworzony wêze³
	static ArrowPtr createArrow();
	//! Pobiera teksture z resource'ów
	//! \param number obecnie 1 lub 2
    //! \return zaladowana tekstura, wyj¹tek w przypadku bledu
	static osg::ref_ptr<osg::Texture2D> getTexture(int number);
    //! próbuje pobraæ teksture z resource'ów
    //! \param ret tutaj trafi pobrana tekstura
    //! \param number obecnie 1 lub 2
    //! \return czy operacja siê udala
    static bool tryGetTexture(osg::ref_ptr<osg::Texture2D>& ret, int number);
	
private:
    //! Wizualizator, który utworzy³ serie
	KinematicVisualizer * visualizer;
    //! Dane GRF dostarczone serii
	GRFCollectionPtr grfCollection;
    //! maksymalna d³ugoœæ si³y, pomocne przy kolorowaniu wektora
	float maxLength;
    //! mapa (krok -> (strza³ka, jej ghost) pomocne przy rysowaniu i odœwie¿aniu wizualizacji
    std::map<IForcePlatform::IStepConstPtr, std::pair<ArrowPtr, GhostStackPtr>> stepsData;
    //! pobrana z zasobów tekstura dla pierwszej p³yty
	static osg::ref_ptr<osg::Texture2D> texture1;
    //! pobrana z zasobów tekstura dla drugiej p³yty
    static osg::ref_ptr<osg::Texture2D> texture2;
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;
    //! nazwa serii
    std::string name;
    //! mapa, która u³atwia pobranie geometrii na postawie platformy z C3D
    Platform2Shape platform2Shape;
};




#endif
