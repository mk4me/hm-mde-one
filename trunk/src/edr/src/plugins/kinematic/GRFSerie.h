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

//! empirycznie dobrany wspolczynnik dlugosci wizualizowanych sil
const float grfScale = 0.0008f;

//! Seria danych odpowiedzialna za wizualizacje plyt naciskowych i sil na nich dzialajacych
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
	//! \param visualizer wizualizator tworzacy ta serie
	GRFSerie(KinematicVisualizer * visualizer);

public:
	//! Ustawia nazwe serii
	//! \param name ustawiana nazwa 
	virtual void setName(const std::string & name);
    //! \return nazwa serii
    virtual const std::string & getName() const;
	//! Ustawienie danych, inicjalizacja 
	//! \param data dane typu GRFCollection
	virtual void setData(const core::ObjectWrapperConstPtr & data);
    //! \return ustawione dane
    virtual const core::ObjectWrapperConstPtr & getData() const;
	//! \return Dlugosc kanalu w sekundach
	virtual double getLength() const;
	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kanalu w sekundach
	virtual void setLocalTime(double time);

private:
    //! Struktura pomocna przy wizualizacji wektora sily.
    //! Wektor sklada sie z prostopadloscianu i stozka
	struct Arrow 
	{
        //! glowny wezel z wektorem
		TransformPtr mainPtr;
        //! wezel z prostopadloscianem
		TransformPtr boxPtr;
        //! wezel ze stozkiem
		TransformPtr conePtr;
        //! geometria prostopadloscianu
		ShapeDrawablePtr boxShape; 
        //! geometria stozka
		ShapeDrawablePtr coneShape;

		//! Ustawienie transformacji, w taki sposob, aby wektor znalazl sie pomiedzy dwoma punktami
		//! \param from punkt, od ktorego prowadzony jest wektor
		//! \param to punkt, do ktorego prowadzone jest wektor
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

    //! Klasa ulatwia rysowanie "ducha" strzalki. Wizualizowane sa poprzednie pozycje strzalki. Im starcza pozycja tym mniejsza alfa.
	class GhostStack
	{
	public:
        //! Stan strzalki, pozycje - od - do
		typedef std::pair<osg::Vec3, osg::Vec3> ArrowState;
		//! Konstruktor
		//! \param maxSize maksymalny rozmiar stosu strzalek
		//! \param hookNode wezel, pod ktory beda podczepiane strzalki
		//! \param color kolor strzalek
		GhostStack(int maxSize, GroupPtr hookNode, const osg::Vec4& color );
        //! Dodanie strzalki do stosu i usuwa najstarsza strzalke, jesli jest taka potrzeba
        //! \param state 
        void addState(const ArrowState& state);
		//! Aktualizacja kolorow strzalek, zarzadzanie widocznoscia wezlow
		void update();

	private:
        //! Kolor strzalek
		osg::Vec4 color;
        //! Stos ze strzalkami
		std::list<ArrowPtr> stackArrows;
        //! Wezel, pod ktory podpiete sa strzalki
		GroupPtr hookNode;
        //! maksymalny rozmiar stosu
		int maxSize;
	};
	typedef boost::shared_ptr<GhostStack> GhostStackPtr;
	typedef boost::shared_ptr<const GhostStack> GhostStackConstPtr;

private:
	//! Tworzy wezel z geometria wszystkich plyt GRF
	//! \param platforms kolecja platform pochodzacych z C3D
    //! \return utworzony wezel
	GroupPtr createPlatformsGroup(const IForcePlatformCollection& platforms);
	//! Tworzy "skrzydla motyla" czyli graficzna reprezentacje zmiany sily nacisku konkretnej plyty w czasie
	//! \param grf dane plyty GRF
	//! \param maxLength tutaj zwracana jest maksymalna dlugosc sily, jest to pomocne przy kolorowaniu wektora
    //! \return utworzony wezel
	GroupPtr createButterfly(GRFCollectionConstPtr grf, float& maxLength) const;
    //! Tworzy pojedyncze "skrzydla motyla" czyli graficzna reprezentacje zmiany sily nacisku konkretnej plyty w czasie
    //! \param step wykryty wczesniej krok w obrebie plyty pomiarowej
    //! \param maxLength maksymalna dlugosc sily, jest to pomocne przy kolorowaniu wektora
    //! \param platform plaforma, dla ktorej tworzona jest geometria
    //! \return utworzony wezel 
    GeodePtr createStep( IForcePlatform::IStepConstPtr step, float &maxLength, IForcePlatformConstPtr platform) const;
    //! Tworzy wezel z geometria pojedynczej plyty GRF
    //! \param texture tekstura, ktora ma byc na prostopadloscianie, moze byc nullptr
    //! \param origin punkt, w ktorym znajduje sie lewy gorny rog plyty
    //! \param width szerokosc plyty
    //! \param lenght dlugosc plyty
    //! \param height wysokosc plyty
    //! \return utworzony wezel
    TransformPtr createPlatformTransform(osg::Texture2D* texture, const osg::Vec3& origin, float width, float lenght, float height) const;
	//! Statyczna metoda, tworzy strukture z geometria i opisem strzalki
    //! \return utworzony wezel
	static ArrowPtr createArrow();
	//! Pobiera teksture z resource'ow
	//! \param number obecnie 1 lub 2
    //! \return zaladowana tekstura, wyjatek w przypadku bledu
	static osg::ref_ptr<osg::Texture2D> getTexture(int number);
    //! Probuje pobrac teksture z resource'ow
    //! \param ret tutaj trafi pobrana tekstura
    //! \param number obecnie 1 lub 2
    //! \return czy operacja sie udala
    static bool tryGetTexture(osg::ref_ptr<osg::Texture2D>& ret, int number);
	
private:
    //! Wizualizator, ktory utworzyl serie
	KinematicVisualizer * visualizer;
    //! Dane GRF dostarczone serii
	GRFCollectionPtr grfCollection;
    //! maksymalna dlugosc sily, pomocne przy kolorowaniu wektora
	float maxLength;
    //! mapa (krok -> (strzalka, jej ghost) pomocne przy rysowaniu i odswiezaniu wizualizacji
    std::map<IForcePlatform::IStepConstPtr, std::pair<ArrowPtr, GhostStackPtr>> stepsData;
    //! pobrana z resource'ow testura dla pierwszej plyty
	static osg::ref_ptr<osg::Texture2D> texture1;
    //! pobrana z resource'ow testura dla drugiej plyty
    static osg::ref_ptr<osg::Texture2D> texture2;
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;
    //! nazwa serii
    std::string name;
    //! mapa, ktora ulatwia pobranie geometrii na postawie platformy z C3D
    Platform2Shape platform2Shape;
};




#endif
