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

//! klasa odpowiada za wyœwietlenie graficznego konfiguratora
class ConfigurationPainter : public QWidget
{
    Q_OBJECT;
public:
    ConfigurationPainter(QWidget* parent);

public:
    //! lista rysowanych obszarów
    typedef std::list<IAreaPtr> AreasList;
    //! 
    typedef std::map<QString, std::pair<QString, QString>> NamesDictionary;
public:
    //! \return obrazek z t³em
    QPixmapConstPtr getBackground() const { return background; }
    //! \return nazwa paintera
    const QString& getName() const { return name; }
    //! ustawia obrazek z t³em
    //! \param name nazwa obrazka
    //! \param val pixmapa z obrazkiem
    void setBackground(const QString& name, const QPixmapConstPtr & val) { this->name = name; background = val; }
    //! dodaje nowy obszar do paintera (marker lub obrazek)
    //! \param data dodawany obszar
    void addArea(const IAreaPtr & data);
    //! usuwa obszar (marker lub obrazek) z paintera
    //! \param name nazwa obszaru
    void removeArea(const QString& name);
    //! \return niemodyfikowalny iterator wskazuj¹cy pocz¹tek kolekcji obszarów
    AreasList::const_iterator begin() const { return areas.cbegin(); }
    //! \return niemodyfikowalny iterator wskazuj¹cy koniec kolekcji obszarów
    AreasList::const_iterator end() const { return areas.cend(); }
    //! \return iterator wskazuj¹cy pocz¹tek kolekcji obszarów
    AreasList::iterator begin() { return areas.begin(); }
    //! \return iterator wskazuj¹cy koniec kolekcji obszarów
    AreasList::iterator end() { return areas.end(); }
    //! jeœli obszar o danej nazwie jest w painterze, to staje siê lub przestaje byæ aktywny
    //! \param name nazwa elementu
    //! \param selected czy ma byæ lub przestaæ byæ aktywny
    void trySetActive( const QString& name, bool selected );
    //! Iloczyn zbiorów nazw dostarczonych przez konfigurator i nazw obs³ugiwanych (nazw dostêpnych obszarów)
    //! \param names zbior do porownania (brany jest pod uwage tylko pierwszy element wpisu mapy)
    void intersectNames( const NamesDictionary& names );
    //! \return globalna skala dla paintera
    float getScale() const { return scale; }
    //! ustawia globalna skale dla paintera
    //! \param val nowa skala
    void setScale(float val);

signals:
    //! obszar zosta³ najechany myszk¹
    //! \param name nazwa obszaru
    //! \param selected czy zosta³ lub przesta³ byæ zaznaczony
    void elementHovered(const QString& name, bool selected);
    //! obszar zosta³ klikniêty myszk¹
    //! \param name nazwa obszaru
    //! \param selected czy sta³ siê lub przesta³ byæ aktywny
    void elementSelected(const QString& name, bool selected);

protected:
    //! obs³uga ruchu myszki
    //! \param event
    virtual void mouseMoveEvent ( QMouseEvent * event );
    //! obs³uga naciœniêcia przycisku myszki
    //! \param event
    virtual void mousePressEvent ( QMouseEvent * event );
    //! obs³uga zwolnienia przycisku myszki
    //! \param event
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    //! odrysowanie konfiguratora
    //! \param *
    virtual void paintEvent(QPaintEvent *);
    //! filtruje zdarzenia zwi¹zane z ruchem myszki
    //! \param obj
    //! \param event
    bool eventFilter(QObject *obj, QEvent *event);

private:
    //! pobiera arene na podstawie wspó³rzêdnych (w przypadku obrazków uwzglêdniana jest tez alpha)
    //! \param x wspó³rzêdna x
    //! \param y wspó³rzêdna y
    //! \return obszar wystepuj¹cy w punkcie lub null jeœli takiego nie ma. Jeœli wystepuje kilka takich obszarów to zwracany jest najbli¿szy
    IAreaPtr getArea(int x, int y);

private:
    //! t³o paintera
    QPixmapConstPtr background;
    //! nazwa paintera
    QString name;
    //! lista obszarów w painterze
    AreasList areas;
    //! element aktualnie najechany myszka
    IAreaPtr currentArea;
    //! globalna skala paintera
    float scale;
};


#endif
