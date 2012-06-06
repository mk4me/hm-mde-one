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
#include <QtGui/QPen>
#include <QtGui/QPainter>
#include "ConfigurationPainterAreas.h"

//! klasa odpowiada za wyswietlenie graficznego konfiguratora
class ConfigurationPainter : public QWidget
{
    Q_OBJECT;
public:
    ConfigurationPainter(QWidget* parent);

public:
    typedef std::list<IAreaPtr> AreasList;
    typedef std::map<QString, std::pair<QString, QString>> NamesDictionary;
public:
    //! \return obrazek z tlem
    QPixmapConstPtr getBackground() const { return background; }
    //! \return nazwa paintera
    const QString& getName() const { return name; }
    //! ustawia obrazek z tlem
    //! \param name nazwa obrazka
    //! \param val pixmapa z obrazkiem
    void setBackground(const QString& name, const QPixmapConstPtr & val) { this->name = name; background = val; }
    //! dodaje nowy obszar do paintera (marker lub obrazek)
    //! \param data dodawany obszar
    void addArea(const IAreaPtr & data);
    //! usuwa obszar (marker lub obrazek) z paintera
    //! \param name nazwa obszaru
    void removeArea(const QString& name);
    //! 
    AreasList::const_iterator begin() const { return areas.cbegin(); }
    //! 
    AreasList::const_iterator end() const { return areas.cend(); }
    //! 
    AreasList::iterator begin() { return areas.begin(); }
    //! 
    AreasList::iterator end() { return areas.end(); }
    //! Jesli obszar o danej nazwie jest w painterze, to staje sie lub przestaje byc aktywny
    //! \param name nazwa elementu
    //! \param selected czy ma byc lub przestac byc aktywny
    void trySetActive( const QString& name, bool selected );
    //! Iloczyn zbiorow nazw dostarczonych przez konfigurator i nazw obslugiwanych (nazw dostepnych obszarow)
    //! \param names zbior do porownania (brany jest pod uwage tylko pierwszy element wpisu mapy)
    void intersectNames( const NamesDictionary& names );
    //! \return globalna skala dla paintera
    float getScale() const { return scale; }
    //! ustawia globalna skale dla paintera
    //! \param val 
    void setScale(float val);

signals:
    //! obszar zostal najechany myszka
    //! \param name nazwa obszaru
    //! \param selected czy zostal lub przestal byc zaznaczony
    void elementHovered(const QString& name, bool selected);
    //! obszar zostal klikniety myszka
    //! \param name nazwa obszaru
    //! \param selected czy stal sie lub przestal byc aktywny
    void elementSelected(const QString& name, bool selected);

protected:
    //! 
    //! \param event 
    virtual void mouseMoveEvent ( QMouseEvent * event );
    //! 
    //! \param event 
    virtual void mousePressEvent ( QMouseEvent * event );
    //! 
    //! \param event 
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    //! 
    //! \param * 
    virtual void paintEvent(QPaintEvent *);
    //! 
    //! \param obj 
    //! \param event 
    bool eventFilter(QObject *obj, QEvent *event);
    
private:
    //! pobiera arene na podstawie wspolrzednych (w przypadku obrazkow uwzgledniana jest tez alpha)
    //! \param x wspolrzedna x
    //! \param y wspolrzedna y
    //! \return obszar wystepujacy w punkcie lub null jesli takiego nie ma. Jesli wystepuje kilka takich obszarow to zwracany jest najblizszy
    IAreaPtr getArea(int x, int y);

private:
    //! tlo paintera
    QPixmapConstPtr background;
    //! nazwa paintera
    QString name;
    //! lista obszarow w painterze
    AreasList areas;
    //! element aktualnie najechany myszka
    IAreaPtr currentArea;
    //! globalna skala paintera
    float scale;
};


#endif
