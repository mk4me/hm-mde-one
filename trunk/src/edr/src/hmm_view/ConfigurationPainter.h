/********************************************************************
	created:	2011/10/18
	created:	18:10:2011   8:58
	filename: 	ConfigurationPainter.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMM__CONFIGURATIONPAINTER_H__
#define HEADER_GUARD_HMM__CONFIGURATIONPAINTER_H__

#include <QtCore/QFileInfo>
#include <map>
#include <list>
#include <QtGui/QWidget>
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include "ConfigurationPainterAreas.h"

//! klasa odpowiada za wy�wietlenie graficznego konfiguratora
class ConfigurationPainter : public QWidget
{
    Q_OBJECT;
public:
    ConfigurationPainter(QWidget* parent);

public:
    //! lista rysowanych obszar�w
    typedef std::list<IAreaPtr> AreasList;
    //! 
    typedef std::map<QString, std::pair<QString, QString>> NamesDictionary;
public:
    //! \return obrazek z t�em
    QPixmapConstPtr getBackground() const { return background; }
    //! \return nazwa paintera
    const QString& getName() const { return name; }
    //! ustawia obrazek z t�em
    //! \param name nazwa obrazka
    //! \param val pixmapa z obrazkiem
    void setBackground(const QString& name, const QPixmapConstPtr & val) { this->name = name; background = val; }
    //! dodaje nowy obszar do paintera (marker lub obrazek)
    //! \param data dodawany obszar
    void addArea(const IAreaPtr & data);
    //! usuwa obszar (marker lub obrazek) z paintera
    //! \param name nazwa obszaru
    void removeArea(const QString& name);
    //! \return niemodyfikowalny iterator wskazuj�cy pocz�tek kolekcji obszar�w
    AreasList::const_iterator begin() const { return areas.cbegin(); }
    //! \return niemodyfikowalny iterator wskazuj�cy koniec kolekcji obszar�w
    AreasList::const_iterator end() const { return areas.cend(); }
    //! \return iterator wskazuj�cy pocz�tek kolekcji obszar�w
    AreasList::iterator begin() { return areas.begin(); }
    //! \return iterator wskazuj�cy koniec kolekcji obszar�w
    AreasList::iterator end() { return areas.end(); }
    //! je�li obszar o danej nazwie jest w painterze, to staje si� lub przestaje by� aktywny
    //! \param name nazwa elementu
    //! \param selected czy ma by� lub przesta� by� aktywny
    void trySetActive( const QString& name, bool selected );
    //! Iloczyn zbior�w nazw dostarczonych przez konfigurator i nazw obs�ugiwanych (nazw dost�pnych obszar�w)
    //! \param names zbior do porownania (brany jest pod uwage tylko pierwszy element wpisu mapy)
    void intersectNames( const NamesDictionary& names );
    //! \return globalna skala dla paintera
    float getScale() const { return scale; }
    //! ustawia globalna skale dla paintera
    //! \param val nowa skala
    void setScale(float val);

signals:
    //! obszar zosta� najechany myszk�
    //! \param name nazwa obszaru
    //! \param selected czy zosta� lub przesta� by� zaznaczony
    void elementHovered(const QString& name, bool selected);
    //! obszar zosta� klikni�ty myszk�
    //! \param name nazwa obszaru
    //! \param selected czy sta� si� lub przesta� by� aktywny
    void elementSelected(const QString& name, bool selected);

protected:
    //! obs�uga ruchu myszki
    //! \param event
    virtual void mouseMoveEvent ( QMouseEvent * event );
    //! obs�uga naci�ni�cia przycisku myszki
    //! \param event
    virtual void mousePressEvent ( QMouseEvent * event );
    //! obs�uga zwolnienia przycisku myszki
    //! \param event
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    //! odrysowanie konfiguratora
    //! \param *
    virtual void paintEvent(QPaintEvent *);
    //! filtruje zdarzenia zwi�zane z ruchem myszki
    //! \param obj
    //! \param event
    bool eventFilter(QObject *obj, QEvent *event);

private:
    //! pobiera arene na podstawie wsp�rz�dnych (w przypadku obrazk�w uwzgl�dniana jest tez alpha)
    //! \param x wsp�rz�dna x
    //! \param y wsp�rz�dna y
    //! \return obszar wystepuj�cy w punkcie lub null je�li takiego nie ma. Je�li wystepuje kilka takich obszar�w to zwracany jest najbli�szy
    IAreaPtr getArea(int x, int y);

private:
    //! t�o paintera
    QPixmapConstPtr background;
    //! nazwa paintera
    QString name;
    //! lista obszar�w w painterze
    AreasList areas;
    //! element aktualnie najechany myszka
    IAreaPtr currentArea;
    //! globalna skala paintera
    float scale;
};


#endif
