/********************************************************************
    created:  2011/07/15
    created:  15:7:2011   12:35
    filename: KinematicVisualizer.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___KINEMATICVISUALIZER_H__
#define HEADER_GUARD___KINEMATICVISUALIZER_H__

#include <QtCore/QObject>

#include <core/IVisualizer.h>
#include <osg/Geode>
#include <osgManipulator/Dragger>
#include <osgManipulator/Command>
#include <osgui/QOsgWidgets.h>
#include <QtGui/QIcon>
#include <QtGui/QMenu>
#include <QtGui/QWidget>
#include <QtGui/QDoubleSpinBox>
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include "ISchemeDrawer.h"
#include "OsgSchemeDrawer.h"
#include "LineSchemeDrawer.h"
#include "PointSchemeDrawer.h"
#include "GlPointSchemeDrawer.h"
#include "GlLineSchemeDrawer.h"
#include "SchemeDrawerContainer.h"
#include "Manipulators.h"
#include "TrajectoriesDialog.h"
#include "SchemeDialog.h"
#include "KinematicSerie.h"

/*! Klasa dostarcza scene 3d i zarzadza obiektami na niej */
class KinematicVisualizer :  public QObject, public core::IVisualizer
{
	// wszystkie mozliwe serie przeniesione do osobnego pliku
	//friend class GRFSerie;
	friend class MarkerSerie;
	friend class SkeletonSerie;
    friend class KinematicDraggerCallback;
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    typedef std::pair<QWidget*, QDoubleSpinBox*> SpinPair;
    Q_OBJECT;
    UNIQUE_ID("{E8B5DEB2-5C57-4323-937D-1FFD288B65B9}", "Kinematic visualizer");

public:
    KinematicVisualizer();

public:
    //! Update wywolywany przez EDR, sprawdza, czy zmienil sie czas serii, jesli tak, to konieczne jest odswiezenie
    //! \param deltaTime czas od ostatniego update'u
    virtual void update( double deltaTime );
    //! Setup wywolany przez system, resetuje scene
    //! \param source zrodlo danych
    virtual void setUp( core::IObjectSource* source );
	//! wizualizator nie ma ograniczen w liczbie serii
	virtual int getMaxDataSeries() const;
    //! Fabryka serii, na podstawie typu danych we wrapperze tworzona jest konkretna seria
    //! \param data Obiekt z danymi
    //! \param name Nazwa danych
    virtual core::IVisualizer::TimeSerieBase *createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name = std::string());
    //! Stworzenie serii na podstawie juz istniejacej, nie zaimplementowane
    //! \param serie Kopiowana seria
    virtual core::IVisualizer::TimeSerieBase *createSerie(const core::IVisualizer::SerieBase* serie);
    //! Usuwa serie z wizualizatora
    //! \param serie seria do usuniecia
    virtual void removeSerie(core::IVisualizer::SerieBase *serie);
	//! Tworzy pusty wizualizator
	virtual core::IVisualizer* createClone() const;
    //! Zwraca informacje o obslugiwanych danych
    //! \param info kolecja z typami danych
    virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info );
    //! Tworzy widget w ktorym znajduje sie 
    //! \param manager Manager Flexi Bara
    virtual QWidget* createWidget(core::IActionsGroupManager * manager);
    //! Tworzy ikone
    virtual QIcon* createIcon();
    //! Nazwa wizualizatora
    virtual const std::string& getName() const;
    //! resetuje ustawienia sceny 3d
    virtual void reset();
    //! Tworzy pixmape z aktualnym widokiem wizualizatora
    virtual QPixmap print() const;
	//! resetuje ustawienia sceny 3d
	void resetScene();

private:
    //! \return wezel z siatka reprezentujaca podloge
    osg::ref_ptr<osg::Group> createFloor();
    //! \return aktywna seria, o ile taka zostala wybrana
    KinematicSerie* tryGetCurrentSerie();
    //! odswieza dane w spinboxach zawierajacych informcacje o transformacjach manipulatorow
    void refreshSpinboxes();
    //! Metoda ustala, do ktorej serii nalezy dana geoda
    //! \param geode obiekt sceny, ktory chcemy sprawdzic
    //! \return seria, do ktorej nalezy obiekt lub nullptr jesli nie znaleziono
    KinematicSerie* getParentSerie(GeodePtr geode);
    //! Metoda pomocnicza, do konwersji zapisu rotacji
    //! \param q kwaternion z liczona rotacja
    //! \return wyliczone katy Eulera
    osg::Vec3 getEulerFromQuat(const osg::Quat& q);
    //! Metoda pomocnicza, do konwersji zapisu rotacji
    //! \param heading 
    //! \param attitude 
    //! \param bank 
    //! \return wyliczony kwaternion
    osg::Quat getQuatFromEuler(double heading, double attitude, double bank);
    //! Metoda pomocnicza, do konwersji zapisu rotacji
    //! \param euler katy eulera w postaci wektora
    //! \return wyliczony kwaternion
    osg::Quat getQuatFromEuler(const osg::Vec3& euler);
    //! Metoda ulatwiajaca tworzenie spinwidgetow z opisem. Tworzony jest widget rodzica zawierajacy spinbox i label z opisem
    //! \param parent rodzic, do ktorego zostanie towrzony widget
    //! \param name opis spinboxa
    //! \param step krok dla strzalek
    //! \param visible czy ma byc widoczny po utworzeniu
    //! \return para (widget, spinbox) widget jest rodzicem spinboxa
    SpinPair createSpinWidget( QWidget* parent, QString name, double step = 0.1, bool visible = true );
    //! Wymusza zmiane wezla podlegajacego wplywom manipulatorow
    //! \param serie seria, dla ktorej ma byc zmianiony wezel
    //! \param m macierz z transformacja, ktora ma zostac ustawiona w wezle
    void setTransformMatrix(KinematicSerie* serie, const osg::Matrix& m);
    //! Wymusza zmiane translacji wezla podlegajacego wplywom manipulatorow
    //! \param serie seria, dla ktorej ma byc zmianiony wezel
    //! \param index Indeks wspolrzednej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana wartosc translacji
    void setTranslation(KinematicSerie* serie, int index, double d );
    //! Wymusza zmiane obrotu wezla podlegajacego wplywom manipulatorow
    //! \param serie seria, dla ktorej ma byc zmianiony wezel
    //! \param index Indeks wspolrzednej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana wartosc katu Eulera
    void setRotation( KinematicSerie* serie, int index, double d );
    //! Wymusza zmiane skali wezla podlegajacego wplywom manipulatorow
    //! \param serie seria, dla ktorej ma byc zmianiony wezel
    //! \param index Indeks wspolrzednej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana wartosc skali
    void setScale(KinematicSerie* serie, int index, double d );
    //! Wymusza zmiane translacji wezla podlegajacego wplywom manipulatorow
    //! \param serie seria, dla ktorej ma byc zmianiony wezel
    //! \param t nowa translacja
    void setTranslation(KinematicSerie* serie, const osg::Vec3& t  );
    //! Wymusza zmiane obrotu wezla podlegajacego wplywom manipulatorow
    //! \param serie seria, dla ktorej ma byc zmianiony wezel
    //! \param r nowy obrot w postaci katow Eulera
    void setRotation( KinematicSerie* serie, const osg::Vec3& r );
    //! Wymusza zmiane obrotu wezla podlegajacego wplywom manipulatorow
    //! \param serie seria, dla ktorej ma byc zmianiony wezel
    //! \param q nowy obrot
    void setRotation( KinematicSerie* serie, const osg::Quat& q);
    //! Wymusza zmiane skali wezla podlegajacego wplywom manipulatorow
    //! \param serie seria, dla ktorej ma byc zmianiony wezel
    //! \param s nowa skala
    void setScale(KinematicSerie* serie, const osg::Vec3& s );
    //! tworzy wezel ze wskaznikiem aktywnej serii danych
    //! \return utworzony wezel
    TransformPtr createIndicator() const;
    //! Odwieza spinboxy translacji, jesli ulegly one zmianie
    void refreshTranslateSpinboxes();
    //! Odwieza spinboxy rotacji, jesli ulegly one zmianie
    void refreshRotateSpinboxes();
    //! Odwieza spinboxy skali, jesli ulegly one zmianie
    void refreshScaleSpinboxes();

private slots:
    //! Dialog z ustawieniami trajektorii stanie sie widoczny
    void showTrajectoriesDialog();
    //! Dialog z ustawieniami widocznosci wezlow stanie sie widoczny
    void showSchemeDialog();
    //! ustawienie aktywnej serii
    //! \param idx indeks serii, ktora ma stac sie aktywna. Musi byc z zakresu <0, liczbaSerii)
    void setActiveSerie(int idx);
    //! ustawienie aktywnej serii
    //! \param serie seria, ktora ma stac sie aktywna
    void setActiveSerie(KinematicSerie* serie);
    //! Pobiera aktywna serie i zminia jej wezel przeznaczony dla manipulatorow
    //! \param d wartosc X nowej translacji
    void shiftX(double d);
    //! Pobiera aktywna serie i zminia jej wezel przeznaczony dla manipulatorow
    //! \param d wartosc Y nowej translacji
    void shiftY(double d);
    //! Pobiera aktywna serie i zminia jej wezel przeznaczony dla manipulatorow
    //! \param d wartosc Z nowej translacji
    void shiftZ(double d);
    //! Pobiera aktywna serie i zminia jej wezel przeznaczony dla manipulatorow
    //! \param d wartosc X nowej rotacji
    void rotateX(double d);
    //! Pobiera aktywna serie i zminia jej wezel przeznaczony dla manipulatorow
    //! \param d wartosc Y nowej rotacji
    void rotateY(double d);
    //! Pobiera aktywna serie i zminia jej wezel przeznaczony dla manipulatorow
    //! \param d wartosc Z nowej rotacji
    void rotateZ(double d);
    //! Pobiera aktywna serie i zminia jej wezel przeznaczony dla manipulatorow
    //! \param d wartosc X nowej skali
    void scaleX(double d);
    //! Pobiera aktywna serie i zminia jej wezel przeznaczony dla manipulatorow
    //! \param d wartosc Y nowej skali
    void scaleY(double d);
    //! Pobiera aktywna serie i zminia jej wezel przeznaczony dla manipulatorow
    //! \param d wartosc Z nowej skali
    void scaleZ(double d);
    //! usuwa dzialanie manipulatorow
    void resetTransform();
	//! Ustawia widok sceny - strona lewa
	void setLeft();
	//! Ustawia widok sceny - strona prawa
	void setRight();
	//! Ustawia widok sceny - od przodu 
	void setFront();
	//! Ustawia widok sceny - z tylu 
	void setBehind();
	//! Ustawia widok sceny - z gory 
	void setTop();
	//! Ustawia widok sceny - od dolu 
	void setBottom();
    //! Zmienil sie stan manipulatora
    void draggerTriggered();

private:
    //! glowny wezel sceny 3d
    osg::ref_ptr<osg::Group> rootNode;
    //! wezel, pod ktory podpinane sa wezly serii
    TransformPtr transformNode;
    //! manipulator kamery (myszka + klawiatura)
	osg::ref_ptr<osgGA::OrbitManipulator> cameraManipulator;
    //! wezel ze wskaznikiem aktywnej serii
    TransformPtr indicatorNode;
    //! zwracany widget wizualizatora
    osg::ref_ptr<osgui::QOsgDefaultWidget> widget;
    //! pokazuje dialog z trajektoriami
    QAction* actionTrajectories;
    //! pokazuje dialog umozliwiajacy chowanie wezlow z geometria
    QAction* actionScheme;
    //! pokazuje ghosta dla markerow
    QAction* actionGhost;
    //! zamienia osie jesli jest taka potrzeba
    QAction* actionSwitchAxes;
    //! wywoluje stan 'pickera' - mozna wybrac aktywna serie klikajac na nia myszka
    QAction* pickerAction;
    //! wywoluje manipulator translacji
    QAction* translateAction;
    //! wywoluje manipulator rotacji
    QAction* rotateAction;
    //! wywoluje manipulator skali
    QAction* scaleAction;
    //! niweluje dzialanie manipulatorow
    QAction* resetAction;
    //! Dialog zarzadzajacy trajektoriami
    TrajectoriesDialog* trajectoriesDialog;
    //! Dialog zarzadzajacy widocznoscia wezlow na scenie
    SchemeDialog* schemeDialog;
    //! combo z seriami danych
    QComboBox* activeSerieCombo;
    //! nr aktywnej serii, -1 jesli takiej nie ma
    int currentSerie;
    //! kolecja z seriami danych podpietymi pod wizualizator
    std::vector<KinematicSerie*> series;
    //! spinbox pokazujacy translacje X
    SpinPair translateSpinWidgetX;
    //! spinbox pokazujacy translacje Y
    SpinPair translateSpinWidgetY;
    //! spinbox pokazujacy translacje Z
    SpinPair translateSpinWidgetZ;
    //! spinbox pokazujacy rotacje X
    SpinPair rotateSpinWidgetX;
    //! spinbox pokazujacy rotacje Y
    SpinPair rotateSpinWidgetY;
    //! spinbox pokazujacy rotacje Z
    SpinPair rotateSpinWidgetZ;
    //! spinbox pokazujacy skale X
    SpinPair scaleSpinWidgetX;
    //! spinbox pokazujacy skale Y
    SpinPair scaleSpinWidgetY;
    //! spinbox pokazujacy skale Z
    SpinPair scaleSpinWidgetZ;
    //! manipulator translacji
    DraggerContainerPtr translateDragger;
    //! manipulator rotacji
    DraggerContainerPtr rotationDragger;
    //! manipulator skali
    DraggerContainerPtr scaleDragger;
    //! obecnie wybrany manipulator lub nullptr jesli takiego nie ma
    DraggerContainerPtr currentDragger;
    //! ostatnio ustawiony czas, pomocne przy odswiezaniu wizualizatora
    double lastTime;
};

//! Klasa pomocnicza do obslugi komunikatow pochodzacych od manipulatorow
class KinematicDraggerCallback : public osgManipulator::DraggerCallback
{
public:
    //! Obiekt jest zaprzyjazniony z KinematicVisualizer
    //! \param kv obslugiwany wizualizator
    KinematicDraggerCallback(KinematicVisualizer* kv) : visualizer(kv) {}

public:
    //! Wywolywane, gdy stan manipulatora sie zmienil
    //! \param mc informacja, jakiego typu zaszla zmiana
    virtual bool receive( const osgManipulator::MotionCommand& mc);
private:
    //! Obiekt, na ktorego scenie dzialaja manipulatory
    KinematicVisualizer* visualizer;
};


#endif  //  HEADER_GUARD___KINEMATICVISUALIZER_H__