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
#include <plugins/kinematic/SkeletalVisualizationScheme.h>
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

/*! Klasa dostarcza scen� 3d i zarz�dza obiektami na niej */
class KinematicVisualizer :  public QObject, public core::IVisualizer
{
	// wszystkie mo�liwe serie przeniesione do osobnego pliku
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
    //! Update wywo�ywany przez EDR, sprawdza, czy zmieni� si� czas serii, je�li tak, to konieczne jest od�wie�enie
    //! \param deltaTime czas od ostatniego update'u
    virtual void update( double deltaTime );
    //! Setup wywo�any przez system, resetuje scen�
    //! \param source �r�d�o danych
    virtual void setUp( core::IObjectSource* source );
	//! wizualizator nie ma ogranicze� w liczbie serii
	virtual int getMaxDataSeries() const;
    //! Fabryka serii, na podstawie typu danych we wrapperze tworzona jest konkretna seria
    //! \param data Obiekt z danymi
    //! \param name Nazwa danych
    virtual core::IVisualizer::TimeSerieBase *createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name = std::string());
    //! Tworzenie serii na podstawie ju� istniej�cej, nie zaimplementowane
    //! \param serie Kopiowana seria
    virtual core::IVisualizer::TimeSerieBase *createSerie(const core::IVisualizer::SerieBase* serie);
    //! Usuwa serie z wizualizatora
    //! \param serie seria do usuniecia
    virtual void removeSerie(core::IVisualizer::SerieBase *serie);
	//! Tworzy pusty wizualizator
	virtual core::IVisualizer* createClone() const;
    //! Zwraca informacje o obs�ugiwanych danych
    //! \param info kolecja z typami danych
    virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info );
    //! Tworzy widget w kt�rym znajduje si�
    //! \param manager Manager Flexi Bara
    virtual QWidget* createWidget(core::IActionsGroupManager * manager);
    //! Tworzy ikon�
    virtual QIcon* createIcon();
    //! Nazwa wizualizatora
    virtual const std::string& getName() const;
    //! resetuje ustawienia sceny 3d
    virtual void reset();
    //! Tworzy pixmap� z aktualnym widokiem wizualizatora
    virtual QPixmap print() const;
	//! resetuje ustawienia sceny 3d
	void resetScene();

private:
    //! \return w�ze� z siatk� reprezentuj�c� podlog�
    osg::ref_ptr<osg::Group> createFloor();
    //! \return aktywna seria, o ile taka zosta�a wybrana
    KinematicSerie* tryGetCurrentSerie();
    //! od�wie�a dane w spinboxach zawieraj�cych informcacje o transformacjach manipulator�w
    void refreshSpinboxes();
    //! Metoda ustala, do kt�rej serii nale�y dana geoda
    //! \param geode obiekt sceny, kt�ry chcemy sprawdzic
    //! \return seria, do kt�rej nale�y obiekt lub nullptr je�li nie znaleziono
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
    //! Metoda u�atwiaj�ca tworzenie spinwidget�w z opisem. Tworzony jest widget rodzica zawieraj�cy spinbox i label z opisem
    //! \param parent rodzic, do kt�rego zostanie towrzony widget
    //! \param name opis spinboxa
    //! \param step krok dla strzalek
    //! \param visible czy ma by� widoczny po utworzeniu
    //! \return para (widget, spinbox) widget jest rodzicem spinboxa
    SpinPair createSpinWidget( QWidget* parent, QString name, double step = 0.1, bool visible = true );
    //! Wymusza zmiane w�z�a podlegaj�cego wplywom manipulator�w
    //! \param serie seria, dla kt�rej ma by� zmianiony w�ze�
    //! \param m macierz z transformacja, kt�ra ma zostac ustawiona w wezle
    void setTransformMatrix(KinematicSerie* serie, const osg::Matrix& m);
    //! Wymusza zmiane translacji w�z�a podlegaj�cego wplywom manipulator�w
    //! \param serie seria, dla kt�rej ma by� zmianiony w�ze�
    //! \param index Indeks wsp�rz�dnej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana warto�� translacji
    void setTranslation(KinematicSerie* serie, int index, double d );
    //! Wymusza zmiane obrotu w�z�a podlegaj�cego wplywom manipulator�w
    //! \param serie seria, dla kt�rej ma by� zmianiony w�ze�
    //! \param index Indeks wsp�rz�dnej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana warto�� katu Eulera
    void setRotation( KinematicSerie* serie, int index, double d );
    //! Wymusza zmiane skali w�z�a podlegaj�cego wplywom manipulator�w
    //! \param serie seria, dla kt�rej ma by� zmianiony w�ze�
    //! \param index Indeks wsp�rz�dnej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana warto�� skali
    void setScale(KinematicSerie* serie, int index, double d );
    //! Wymusza zmiane translacji w�z�a podlegaj�cego wplywom manipulator�w
    //! \param serie seria, dla kt�rej ma by� zmianiony w�ze�
    //! \param t nowa translacja
    void setTranslation(KinematicSerie* serie, const osg::Vec3& t  );
    //! Wymusza zmiane obrotu w�z�a podlegaj�cego wplywom manipulator�w
    //! \param serie seria, dla kt�rej ma by� zmianiony w�ze�
    //! \param r nowy obrot w postaci k�t�w Eulera
    void setRotation( KinematicSerie* serie, const osg::Vec3& r );
    //! Wymusza zmiane obrotu w�z�a podlegaj�cego wplywom manipulator�w
    //! \param serie seria, dla kt�rej ma by� zmianiony w�ze�
    //! \param q nowy obrot
    void setRotation( KinematicSerie* serie, const osg::Quat& q);
    //! Wymusza zmiane skali w�z�a podlegaj�cego wplywom manipulator�w
    //! \param serie seria, dla kt�rej ma by� zmianiony w�ze�
    //! \param s nowa skala
    void setScale(KinematicSerie* serie, const osg::Vec3& s );
    //! tworzy w�ze� ze wska�nikiem aktywnej serii danych
    //! \return utworzony w�ze�
    TransformPtr createIndicator() const;
    //! Odwieza spinboxy translacji, je�li ulegly one zmianie
    void refreshTranslateSpinboxes();
    //! Odwieza spinboxy rotacji, je�li ulegly one zmianie
    void refreshRotateSpinboxes();
    //! Odwieza spinboxy skali, je�li ulegly one zmianie
    void refreshScaleSpinboxes();

private slots:
    //! Dialog z ustawieniami trajektorii stanie si� widoczny
    void showTrajectoriesDialog();
    //! Dialog z ustawieniami widoczno�ci w�z��w stanie si� widoczny
    void showSchemeDialog();
    //! ustawienie aktywnej serii
    //! \param idx indeks serii, kt�ra ma stac si� aktywna. Musi by� z zakresu <0, liczbaSerii)
    void setActiveSerie(int idx);
    //! ustawienie aktywnej serii
    //! \param serie seria, kt�ra ma stac si� aktywna
    void setActiveSerie(KinematicSerie* serie);
    //! Pobiera aktywna serie i zminia jej w�ze� przeznaczony dla manipulator�w
    //! \param d warto�� X nowej translacji
    void shiftX(double d);
    //! Pobiera aktywna serie i zminia jej w�ze� przeznaczony dla manipulator�w
    //! \param d warto�� Y nowej translacji
    void shiftY(double d);
    //! Pobiera aktywna serie i zminia jej w�ze� przeznaczony dla manipulator�w
    //! \param d warto�� Z nowej translacji
    void shiftZ(double d);
    //! Pobiera aktywna serie i zminia jej w�ze� przeznaczony dla manipulator�w
    //! \param d warto�� X nowej rotacji
    void rotateX(double d);
    //! Pobiera aktywna serie i zminia jej w�ze� przeznaczony dla manipulator�w
    //! \param d warto�� Y nowej rotacji
    void rotateY(double d);
    //! Pobiera aktywna serie i zminia jej w�ze� przeznaczony dla manipulator�w
    //! \param d warto�� Z nowej rotacji
    void rotateZ(double d);
    //! Pobiera aktywna serie i zminia jej w�ze� przeznaczony dla manipulator�w
    //! \param d warto�� X nowej skali
    void scaleX(double d);
    //! Pobiera aktywna serie i zminia jej w�ze� przeznaczony dla manipulator�w
    //! \param d warto�� Y nowej skali
    void scaleY(double d);
    //! Pobiera aktywna serie i zminia jej w�ze� przeznaczony dla manipulator�w
    //! \param d warto�� Z nowej skali
    void scaleZ(double d);
    //! usuwa dzia�anie manipulator�w
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
    //! zmieni� si� stan manipulatora
    void draggerTriggered();

private:
    //! g��wny w�ze� sceny 3d
    osg::ref_ptr<osg::Group> rootNode;
    //! w�ze�, pod kt�ry podpinane s� w�z�y serii
    TransformPtr transformNode;
    //! manipulator kamery (myszka + klawiatura)
	osg::ref_ptr<osgGA::OrbitManipulator> cameraManipulator;
    //! w�ze� ze wska�nikiem aktywnej serii
    TransformPtr indicatorNode;
    //! zwracany widget wizualizatora
    osg::ref_ptr<osgui::QOsgDefaultWidget> widget;
    //! pokazuje dialog z trajektoriami
    QAction* actionTrajectories;
    //! pokazuje dialog umo�liwiaj�cy chowanie w�z��w z geometria
    QAction* actionScheme;
    //! pokazuje ghosta dla marker�w
    QAction* actionGhost;
    //! zamienia osie je�li jest taka potrzeba
    QAction* actionSwitchAxes;
    //! wywoluje stan 'pickera' - mo�na wybra� aktywna serie klikaj�c na nia myszka
    QAction* pickerAction;
    //! wywoluje manipulator translacji
    QAction* translateAction;
    //! wywoluje manipulator rotacji
    QAction* rotateAction;
    //! wywoluje manipulator skali
    QAction* scaleAction;
    //! niweluje dzia�anie manipulator�w
    QAction* resetAction;
    //! Dialog zarz�dzaj�cy trajektoriami
    TrajectoriesDialog* trajectoriesDialog;
    //! Dialog zarz�dzaj�cy widoczno�ci� w�z��w na scenie
    SchemeDialog* schemeDialog;
    //! combo z seriami danych
    QComboBox* activeSerieCombo;
    //! nr aktywnej serii, -1 je�li takiej nie ma
    int currentSerie;
    //! kolecja z seriami danych podpi�tymi pod wizualizator
    std::vector<KinematicSerie*> series;
    //! spinbox pokazuj�cy translacje X
    SpinPair translateSpinWidgetX;
    //! spinbox pokazuj�cy translacje Y
    SpinPair translateSpinWidgetY;
    //! spinbox pokazuj�cy translacje Z
    SpinPair translateSpinWidgetZ;
    //! spinbox pokazuj�cy rotacje X
    SpinPair rotateSpinWidgetX;
    //! spinbox pokazuj�cy rotacje Y
    SpinPair rotateSpinWidgetY;
    //! spinbox pokazuj�cy rotacje Z
    SpinPair rotateSpinWidgetZ;
    //! spinbox pokazuj�cy skale X
    SpinPair scaleSpinWidgetX;
    //! spinbox pokazuj�cy skale Y
    SpinPair scaleSpinWidgetY;
    //! spinbox pokazuj�cy skale Z
    SpinPair scaleSpinWidgetZ;
    //! manipulator translacji
    DraggerContainerPtr translateDragger;
    //! manipulator rotacji
    DraggerContainerPtr rotationDragger;
    //! manipulator skali
    DraggerContainerPtr scaleDragger;
    //! obecnie wybrany manipulator lub nullptr je�li takiego nie ma
    DraggerContainerPtr currentDragger;
    //! ostatnio ustawiony czas, pomocne przy od�wie�aniu wizualizatora
    double lastTime;
};

//! Klasa pomocnicza do obs�ugi komunikat�w pochodz�cych od manipulator�w
class KinematicDraggerCallback : public osgManipulator::DraggerCallback
{
public:
    //! Obiekt jest zaprzyja�niony z KinematicVisualizer
    //! \param kv obs�ugiwany wizualizator
    KinematicDraggerCallback(KinematicVisualizer* kv) : visualizer(kv) {}

public:
    //! wywo�ywane, gdy stan manipulatora si� zmieni�
    //! \param mc informacja, jakiego typu zasz�a zmiana
    virtual bool receive( const osgManipulator::MotionCommand& mc);
private:
    //! Obiekt, na kt�rego scenie dzia�aj� manipulatory
    KinematicVisualizer* visualizer;
};


#endif  //  HEADER_GUARD___KINEMATICVISUALIZER_H__
