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
#include <osgGA/OrbitManipulator>
#include <osgManipulator/Dragger>
#include <osgManipulator/Command>
#include <osgui/QOsgWidgets.h>
#include <QtGui/QIcon>
#include <QtGui/QMenu>
#include <QtGui/QWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QComboBox>
#include "SkeletalVisualizationScheme.h"
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

/*! Klasa dostarcza scenę 3d i zarządza obiektami na niej */
class KinematicVisualizer :  public QObject, public core::IVisualizer
{
	// wszystkie możliwe serie przeniesione do osobnego pliku
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
    //! Update wywoływany przez EDR, sprawdza, czy zmienił się czas serii, jeśli tak, to konieczne jest odświeżenie
    //! \param deltaTime czas od ostatniego update'u
    virtual void update( double deltaTime );
    //! Setup wywołany przez system, resetuje scenę
    //! \param source źródło danych
    virtual void setUp( core::IObjectSource* source );
	//! wizualizator nie ma ograniczeń w liczbie serii
	virtual int getMaxDataSeries() const;
    //! Fabryka serii, na podstawie typu danych we wrapperze tworzona jest konkretna seria
    //! \param data Obiekt z danymi
    //! \param name Nazwa danych
    virtual core::IVisualizer::TimeSerieBase *createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name = std::string());
    //! Tworzenie serii na podstawie już istniejącej, nie zaimplementowane
    //! \param serie Kopiowana seria
    virtual core::IVisualizer::TimeSerieBase *createSerie(const core::IVisualizer::SerieBase* serie);
    //! Usuwa serie z wizualizatora
    //! \param serie seria do usunięcia
    virtual void removeSerie(core::IVisualizer::SerieBase *serie);
	//! Tworzy pusty wizualizator
	virtual core::IVisualizer* createClone() const;
    //! Zwraca informacje o obsługiwanych danych
    //! \param info kolecja z typami danych
    virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info );
    //! Tworzy widget w którym znajduje się
    //! \param manager Manager Flexi Bara
    virtual QWidget* createWidget(core::IActionsGroupManager * manager);
    //! Tworzy ikonę
    virtual QIcon* createIcon();
    //! Nazwa wizualizatora
    virtual const std::string& getName() const;
    //! resetuje ustawienia sceny 3d
    virtual void reset();
    //! Tworzy pixmapę z aktualnym widokiem wizualizatora
    virtual QPixmap print() const;
	//! resetuje ustawienia sceny 3d
	void resetScene();

private:
    //! \return węzeł z siatką reprezentującą podlogę
    osg::ref_ptr<osg::Group> createFloor();
    //! \return aktywna seria, o ile taka została wybrana
    KinematicSerie* tryGetCurrentSerie();
    //! odświeża dane w spinboxach zawierających informcacje o transformacjach manipulatorów
    void refreshSpinboxes();
    //! Metoda ustala, do której serii należy dana geoda
    //! \param geode obiekt sceny, który chcemy sprawdzic
    //! \return seria, do której należy obiekt lub nullptr jeśli nie znaleziono
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
    //! Metoda ułatwiająca tworzenie spinwidgetów z opisem. Tworzony jest widget rodzica zawierający spinbox i label z opisem
    //! \param parent rodzic, do którego zostanie towrzony widget
    //! \param name opis spinboxa
    //! \param step krok dla strzałek
    //! \param visible czy ma być widoczny po utworzeniu
    //! \return para (widget, spinbox) widget jest rodzicem spinboxa
    SpinPair createSpinWidget( QWidget* parent, QString name, double step = 0.1, bool visible = true );
    //! Wymusza zmiane węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param m macierz z transformacja, która ma zostac ustawiona w wezle
    void setTransformMatrix(KinematicSerie* serie, const osg::Matrix& m);
    //! Wymusza zmiane translacji węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param index Indeks współrzędnej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana wartość translacji
    void setTranslation(KinematicSerie* serie, int index, double d );
    //! Wymusza zmiane obrotu węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param index Indeks współrzędnej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana wartość katu Eulera
    void setRotation( KinematicSerie* serie, int index, double d );
    //! Wymusza zmiane skali węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param index Indeks współrzędnej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana wartość skali
    void setScale(KinematicSerie* serie, int index, double d );
    //! Wymusza zmiane translacji węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param t nowa translacja
    void setTranslation(KinematicSerie* serie, const osg::Vec3& t  );
    //! Wymusza zmiane obrotu węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param r nowy obrot w postaci kątów Eulera
    void setRotation( KinematicSerie* serie, const osg::Vec3& r );
    //! Wymusza zmiane obrotu węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param q nowy obrot
    void setRotation( KinematicSerie* serie, const osg::Quat& q);
    //! Wymusza zmiane skali węzła podlegającego wplywom manipulatorów
    //! \param serie seria, dla której ma być zmianiony węzeł
    //! \param s nowa skala
    void setScale(KinematicSerie* serie, const osg::Vec3& s );
    //! tworzy węzeł ze wskaźnikiem aktywnej serii danych
    //! \return utworzony węzeł
    TransformPtr createIndicator() const;
    //! Odwieza spinboxy translacji, jeśli ulegly one zmianie
    void refreshTranslateSpinboxes();
    //! Odwieza spinboxy rotacji, jeśli ulegly one zmianie
    void refreshRotateSpinboxes();
    //! Odwieza spinboxy skali, jeśli ulegly one zmianie
    void refreshScaleSpinboxes();

private slots:
    //! Dialog z ustawieniami trajektorii stanie się widoczny
    void showTrajectoriesDialog();
    //! Dialog z ustawieniami widoczności węzłów stanie się widoczny
    void showSchemeDialog();
    //! ustawienie aktywnej serii
    //! \param idx indeks serii, która ma stac się aktywna. Musi być z zakresu <0, liczbaSerii)
    void setActiveSerie(int idx);
    //! ustawienie aktywnej serii
    //! \param serie seria, która ma stac się aktywna
    void setActiveSerie(KinematicSerie* serie);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość X nowej translacji
    void shiftX(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość Y nowej translacji
    void shiftY(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość Z nowej translacji
    void shiftZ(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość X nowej rotacji
    void rotateX(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość Y nowej rotacji
    void rotateY(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość Z nowej rotacji
    void rotateZ(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość X nowej skali
    void scaleX(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość Y nowej skali
    void scaleY(double d);
    //! Pobiera aktywna serie i zminia jej węzeł przeznaczony dla manipulatorów
    //! \param d wartość Z nowej skali
    void scaleZ(double d);
    //! usuwa działanie manipulatorów
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
    //! zmienił się stan manipulatora
    void draggerTriggered();

private:
    //! główny węzeł sceny 3d
    osg::ref_ptr<osg::Group> rootNode;
    //! węzeł, pod który podpinane są węzły serii
    TransformPtr transformNode;
    //! manipulator kamery (myszka + klawiatura)
	osg::ref_ptr<osgGA::OrbitManipulator> cameraManipulator;
    //! węzeł ze wskaźnikiem aktywnej serii
    TransformPtr indicatorNode;
    //! zwracany widget wizualizatora
    osg::ref_ptr<osgui::QOsgDefaultWidget> widget;
    //! pokazuje dialog z trajektoriami
    QAction* actionTrajectories;
    //! pokazuje dialog umożliwiający chowanie węzłów z geometria
    QAction* actionScheme;
    //! pokazuje ghosta dla markerów
    QAction* actionGhost;
    //! zamienia osie jeśli jest taka potrzeba
    QAction* actionSwitchAxes;
    //! wywołuje stan 'pickera' - można wybrać aktywna serie klikając na nia myszka
    QAction* pickerAction;
    //! wywołuje manipulator translacji
    QAction* translateAction;
    //! wywołuje manipulator rotacji
    QAction* rotateAction;
    //! wywołuje manipulator skali
    QAction* scaleAction;
    //! niweluje działanie manipulatorów
    QAction* resetAction;
    //! Dialog zarządzający trajektoriami
    TrajectoriesDialog* trajectoriesDialog;
    //! Dialog zarządzający widocznością węzłów na scenie
    SchemeDialog* schemeDialog;
    //! combo z seriami danych
    QComboBox* activeSerieCombo;
    //! nr aktywnej serii, -1 jeśli takiej nie ma
    int currentSerie;
    //! kolecja z seriami danych podpiętymi pod wizualizator
    std::vector<KinematicSerie*> series;
    //! spinbox pokazujący translacje X
    SpinPair translateSpinWidgetX;
    //! spinbox pokazujący translacje Y
    SpinPair translateSpinWidgetY;
    //! spinbox pokazujący translacje Z
    SpinPair translateSpinWidgetZ;
    //! spinbox pokazujący rotacje X
    SpinPair rotateSpinWidgetX;
    //! spinbox pokazujący rotacje Y
    SpinPair rotateSpinWidgetY;
    //! spinbox pokazujący rotacje Z
    SpinPair rotateSpinWidgetZ;
    //! spinbox pokazujący skale X
    SpinPair scaleSpinWidgetX;
    //! spinbox pokazujący skale Y
    SpinPair scaleSpinWidgetY;
    //! spinbox pokazujący skale Z
    SpinPair scaleSpinWidgetZ;
    //! manipulator translacji
    DraggerContainerPtr translateDragger;
    //! manipulator rotacji
    DraggerContainerPtr rotationDragger;
    //! manipulator skali
    DraggerContainerPtr scaleDragger;
    //! obecnie wybrany manipulator lub nullptr jeśli takiego nie ma
    DraggerContainerPtr currentDragger;
    //! ostatnio ustawiony czas, pomocne przy odświeżaniu wizualizatora
    double lastTime;
};

//! Klasa pomocnicza do obsługi komunikatów pochodzących od manipulatorów
class KinematicDraggerCallback : public osgManipulator::DraggerCallback
{
public:
    //! Obiekt jest zaprzyjaźniony z KinematicVisualizer
    //! \param kv obsługiwany wizualizator
    KinematicDraggerCallback(KinematicVisualizer* kv) : visualizer(kv) {}

public:
    //! wywoływane, gdy stan manipulatora się zmienił
    //! \param mc informacja, jakiego typu zaszła zmiana
    virtual bool receive( const osgManipulator::MotionCommand& mc);
private:
    //! Obiekt, na którego scenie działają manipulatory
    KinematicVisualizer* visualizer;
};


#endif  //  HEADER_GUARD___KINEMATICVISUALIZER_H__
