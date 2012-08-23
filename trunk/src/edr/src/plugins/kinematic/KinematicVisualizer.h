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

/*! Klasa dostarcza scenê 3d i zarz¹dza obiektami na niej */
class KinematicVisualizer :  public QObject, public core::IVisualizer
{
	// wszystkie mo¿liwe serie przeniesione do osobnego pliku
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
    //! Update wywo³ywany przez EDR, sprawdza, czy zmieni³ siê czas serii, jeœli tak, to konieczne jest odœwie¿enie
    //! \param deltaTime czas od ostatniego update'u
    virtual void update( double deltaTime );
    //! Setup wywo³any przez system, resetuje scenê
    //! \param source Ÿród³o danych
    virtual void setUp( core::IObjectSource* source );
	//! wizualizator nie ma ograniczeñ w liczbie serii
	virtual int getMaxDataSeries() const;
    //! Fabryka serii, na podstawie typu danych we wrapperze tworzona jest konkretna seria
    //! \param data Obiekt z danymi
    //! \param name Nazwa danych
    virtual core::IVisualizer::TimeSerieBase *createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name = std::string());
    //! Tworzenie serii na podstawie ju¿ istniej¹cej, nie zaimplementowane
    //! \param serie Kopiowana seria
    virtual core::IVisualizer::TimeSerieBase *createSerie(const core::IVisualizer::SerieBase* serie);
    //! Usuwa serie z wizualizatora
    //! \param serie seria do usuniecia
    virtual void removeSerie(core::IVisualizer::SerieBase *serie);
	//! Tworzy pusty wizualizator
	virtual core::IVisualizer* createClone() const;
    //! Zwraca informacje o obs³ugiwanych danych
    //! \param info kolecja z typami danych
    virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info );
    //! Tworzy widget w którym znajduje siê
    //! \param manager Manager Flexi Bara
    virtual QWidget* createWidget(core::IActionsGroupManager * manager);
    //! Tworzy ikonê
    virtual QIcon* createIcon();
    //! Nazwa wizualizatora
    virtual const std::string& getName() const;
    //! resetuje ustawienia sceny 3d
    virtual void reset();
    //! Tworzy pixmapê z aktualnym widokiem wizualizatora
    virtual QPixmap print() const;
	//! resetuje ustawienia sceny 3d
	void resetScene();

private:
    //! \return wêze³ z siatk¹ reprezentuj¹c¹ podlogê
    osg::ref_ptr<osg::Group> createFloor();
    //! \return aktywna seria, o ile taka zosta³a wybrana
    KinematicSerie* tryGetCurrentSerie();
    //! odœwie¿a dane w spinboxach zawieraj¹cych informcacje o transformacjach manipulatorów
    void refreshSpinboxes();
    //! Metoda ustala, do której serii nale¿y dana geoda
    //! \param geode obiekt sceny, który chcemy sprawdzic
    //! \return seria, do której nale¿y obiekt lub nullptr jeœli nie znaleziono
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
    //! Metoda u³atwiaj¹ca tworzenie spinwidgetów z opisem. Tworzony jest widget rodzica zawieraj¹cy spinbox i label z opisem
    //! \param parent rodzic, do którego zostanie towrzony widget
    //! \param name opis spinboxa
    //! \param step krok dla strzalek
    //! \param visible czy ma byæ widoczny po utworzeniu
    //! \return para (widget, spinbox) widget jest rodzicem spinboxa
    SpinPair createSpinWidget( QWidget* parent, QString name, double step = 0.1, bool visible = true );
    //! Wymusza zmiane wêz³a podlegaj¹cego wplywom manipulatorów
    //! \param serie seria, dla której ma byæ zmianiony wêze³
    //! \param m macierz z transformacja, która ma zostac ustawiona w wezle
    void setTransformMatrix(KinematicSerie* serie, const osg::Matrix& m);
    //! Wymusza zmiane translacji wêz³a podlegaj¹cego wplywom manipulatorów
    //! \param serie seria, dla której ma byæ zmianiony wêze³
    //! \param index Indeks wspó³rzêdnej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana wartoœæ translacji
    void setTranslation(KinematicSerie* serie, int index, double d );
    //! Wymusza zmiane obrotu wêz³a podlegaj¹cego wplywom manipulatorów
    //! \param serie seria, dla której ma byæ zmianiony wêze³
    //! \param index Indeks wspó³rzêdnej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana wartoœæ katu Eulera
    void setRotation( KinematicSerie* serie, int index, double d );
    //! Wymusza zmiane skali wêz³a podlegaj¹cego wplywom manipulatorów
    //! \param serie seria, dla której ma byæ zmianiony wêze³
    //! \param index Indeks wspó³rzêdnej (0 - x, 1 - y, 2 - z)
    //! \param d ustawiana wartoœæ skali
    void setScale(KinematicSerie* serie, int index, double d );
    //! Wymusza zmiane translacji wêz³a podlegaj¹cego wplywom manipulatorów
    //! \param serie seria, dla której ma byæ zmianiony wêze³
    //! \param t nowa translacja
    void setTranslation(KinematicSerie* serie, const osg::Vec3& t  );
    //! Wymusza zmiane obrotu wêz³a podlegaj¹cego wplywom manipulatorów
    //! \param serie seria, dla której ma byæ zmianiony wêze³
    //! \param r nowy obrot w postaci k¹tów Eulera
    void setRotation( KinematicSerie* serie, const osg::Vec3& r );
    //! Wymusza zmiane obrotu wêz³a podlegaj¹cego wplywom manipulatorów
    //! \param serie seria, dla której ma byæ zmianiony wêze³
    //! \param q nowy obrot
    void setRotation( KinematicSerie* serie, const osg::Quat& q);
    //! Wymusza zmiane skali wêz³a podlegaj¹cego wplywom manipulatorów
    //! \param serie seria, dla której ma byæ zmianiony wêze³
    //! \param s nowa skala
    void setScale(KinematicSerie* serie, const osg::Vec3& s );
    //! tworzy wêze³ ze wskaŸnikiem aktywnej serii danych
    //! \return utworzony wêze³
    TransformPtr createIndicator() const;
    //! Odwieza spinboxy translacji, jeœli ulegly one zmianie
    void refreshTranslateSpinboxes();
    //! Odwieza spinboxy rotacji, jeœli ulegly one zmianie
    void refreshRotateSpinboxes();
    //! Odwieza spinboxy skali, jeœli ulegly one zmianie
    void refreshScaleSpinboxes();

private slots:
    //! Dialog z ustawieniami trajektorii stanie siê widoczny
    void showTrajectoriesDialog();
    //! Dialog z ustawieniami widocznoœci wêz³ów stanie siê widoczny
    void showSchemeDialog();
    //! ustawienie aktywnej serii
    //! \param idx indeks serii, która ma stac siê aktywna. Musi byæ z zakresu <0, liczbaSerii)
    void setActiveSerie(int idx);
    //! ustawienie aktywnej serii
    //! \param serie seria, która ma stac siê aktywna
    void setActiveSerie(KinematicSerie* serie);
    //! Pobiera aktywna serie i zminia jej wêze³ przeznaczony dla manipulatorów
    //! \param d wartoœæ X nowej translacji
    void shiftX(double d);
    //! Pobiera aktywna serie i zminia jej wêze³ przeznaczony dla manipulatorów
    //! \param d wartoœæ Y nowej translacji
    void shiftY(double d);
    //! Pobiera aktywna serie i zminia jej wêze³ przeznaczony dla manipulatorów
    //! \param d wartoœæ Z nowej translacji
    void shiftZ(double d);
    //! Pobiera aktywna serie i zminia jej wêze³ przeznaczony dla manipulatorów
    //! \param d wartoœæ X nowej rotacji
    void rotateX(double d);
    //! Pobiera aktywna serie i zminia jej wêze³ przeznaczony dla manipulatorów
    //! \param d wartoœæ Y nowej rotacji
    void rotateY(double d);
    //! Pobiera aktywna serie i zminia jej wêze³ przeznaczony dla manipulatorów
    //! \param d wartoœæ Z nowej rotacji
    void rotateZ(double d);
    //! Pobiera aktywna serie i zminia jej wêze³ przeznaczony dla manipulatorów
    //! \param d wartoœæ X nowej skali
    void scaleX(double d);
    //! Pobiera aktywna serie i zminia jej wêze³ przeznaczony dla manipulatorów
    //! \param d wartoœæ Y nowej skali
    void scaleY(double d);
    //! Pobiera aktywna serie i zminia jej wêze³ przeznaczony dla manipulatorów
    //! \param d wartoœæ Z nowej skali
    void scaleZ(double d);
    //! usuwa dzia³anie manipulatorów
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
    //! zmieni³ siê stan manipulatora
    void draggerTriggered();

private:
    //! g³ówny wêze³ sceny 3d
    osg::ref_ptr<osg::Group> rootNode;
    //! wêze³, pod który podpinane s¹ wêz³y serii
    TransformPtr transformNode;
    //! manipulator kamery (myszka + klawiatura)
	osg::ref_ptr<osgGA::OrbitManipulator> cameraManipulator;
    //! wêze³ ze wskaŸnikiem aktywnej serii
    TransformPtr indicatorNode;
    //! zwracany widget wizualizatora
    osg::ref_ptr<osgui::QOsgDefaultWidget> widget;
    //! pokazuje dialog z trajektoriami
    QAction* actionTrajectories;
    //! pokazuje dialog umo¿liwiaj¹cy chowanie wêz³ów z geometria
    QAction* actionScheme;
    //! pokazuje ghosta dla markerów
    QAction* actionGhost;
    //! zamienia osie jeœli jest taka potrzeba
    QAction* actionSwitchAxes;
    //! wywoluje stan 'pickera' - mo¿na wybraæ aktywna serie klikaj¹c na nia myszka
    QAction* pickerAction;
    //! wywoluje manipulator translacji
    QAction* translateAction;
    //! wywoluje manipulator rotacji
    QAction* rotateAction;
    //! wywoluje manipulator skali
    QAction* scaleAction;
    //! niweluje dzia³anie manipulatorów
    QAction* resetAction;
    //! Dialog zarz¹dzaj¹cy trajektoriami
    TrajectoriesDialog* trajectoriesDialog;
    //! Dialog zarz¹dzaj¹cy widocznoœci¹ wêz³ów na scenie
    SchemeDialog* schemeDialog;
    //! combo z seriami danych
    QComboBox* activeSerieCombo;
    //! nr aktywnej serii, -1 jeœli takiej nie ma
    int currentSerie;
    //! kolecja z seriami danych podpiêtymi pod wizualizator
    std::vector<KinematicSerie*> series;
    //! spinbox pokazuj¹cy translacje X
    SpinPair translateSpinWidgetX;
    //! spinbox pokazuj¹cy translacje Y
    SpinPair translateSpinWidgetY;
    //! spinbox pokazuj¹cy translacje Z
    SpinPair translateSpinWidgetZ;
    //! spinbox pokazuj¹cy rotacje X
    SpinPair rotateSpinWidgetX;
    //! spinbox pokazuj¹cy rotacje Y
    SpinPair rotateSpinWidgetY;
    //! spinbox pokazuj¹cy rotacje Z
    SpinPair rotateSpinWidgetZ;
    //! spinbox pokazuj¹cy skale X
    SpinPair scaleSpinWidgetX;
    //! spinbox pokazuj¹cy skale Y
    SpinPair scaleSpinWidgetY;
    //! spinbox pokazuj¹cy skale Z
    SpinPair scaleSpinWidgetZ;
    //! manipulator translacji
    DraggerContainerPtr translateDragger;
    //! manipulator rotacji
    DraggerContainerPtr rotationDragger;
    //! manipulator skali
    DraggerContainerPtr scaleDragger;
    //! obecnie wybrany manipulator lub nullptr jeœli takiego nie ma
    DraggerContainerPtr currentDragger;
    //! ostatnio ustawiony czas, pomocne przy odœwie¿aniu wizualizatora
    double lastTime;
};

//! Klasa pomocnicza do obs³ugi komunikatów pochodz¹cych od manipulatorów
class KinematicDraggerCallback : public osgManipulator::DraggerCallback
{
public:
    //! Obiekt jest zaprzyjaŸniony z KinematicVisualizer
    //! \param kv obs³ugiwany wizualizator
    KinematicDraggerCallback(KinematicVisualizer* kv) : visualizer(kv) {}

public:
    //! wywo³ywane, gdy stan manipulatora siê zmieni³
    //! \param mc informacja, jakiego typu zasz³a zmiana
    virtual bool receive( const osgManipulator::MotionCommand& mc);
private:
    //! Obiekt, na którego scenie dzia³aj¹ manipulatory
    KinematicVisualizer* visualizer;
};


#endif  //  HEADER_GUARD___KINEMATICVISUALIZER_H__
