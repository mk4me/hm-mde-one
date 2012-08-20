/********************************************************************
	created:	2012/06/05
	created:	5:6:2012   8:57
	filename: 	ConfigurationPainterAreas.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_HMM__CONFIGURATIONPAINTERAREAS_H__
#define HEADER_GUARD_HMM__CONFIGURATIONPAINTERAREAS_H__

#include <boost/shared_ptr.hpp>

//! okresla obszar na konfiguratorze graficznym
class IArea
{
public:
    IArea();
    virtual ~IArea() {}

public:
    //! \return pozycja Y
    virtual int getY() const = 0;
    //!
    //! \param val
    virtual void setY(int val) = 0;
    //! \return pozycja X
    virtual int getX() const = 0;
    //!
    //! \param val
    virtual void setX(int val) = 0;
    //! \return szerokosc obszaru
    virtual int getWidth() const = 0;
    //! \return wysokosc obszaru
    virtual int getHeight() const = 0;
    //! \return nazwa przypisana do obszaru
    virtual const QString& getName() const = 0;
    //! pozwala odrysowac sie obiektowi
    //! \param painter obiekt paintera
    //! \param selected czy obiekt jest zaznaczony
    virtual void draw(QPainter& painter, bool selected) = 0;
    //! czy podany punkt znajduje sie wewnatrz obszaru
    //! \param x wspolrzedna x
    //! \param y wspolrzedna y
    virtual bool isInside(int x, int y) const;
    //! czy obszar jest aktywny (normalnie - czy kliknieto w niego myszka)
    bool isActive() const { return active; }
    //! ustawia aktywnosc obszaru (norm. po kliknieciu myszka)
    //! \param val
    void setActive(bool val) { active = val; }
    //! \return skala obszaru
    float getScale() const { return scale; }
    //!
    //! \param val
    void setScale(float val) { scale = val; }

private:
    //! czy obszar jest aktywny (normalnie - czy kliknieto w niego myszka)
    bool active;
    //! skala obszaru
    float scale;
};
typedef boost::shared_ptr<IArea> IAreaPtr;
typedef boost::shared_ptr<const IArea> IAreaConstPtr;

//! zwykly punkt na konfiguratorze
//! rysowany jako polprzezroczyste kolo
class SingleMarker : public IArea
{
public:
    //! Konstruktor
    //! \param name nazwa punktu
    //! \param posX pozycja x
    //! \param posY pozycja y
    SingleMarker(const QString& name, int posX, int posY);
    //! Konstruktor
    //! \param name nazwa punktu
    //! \param posX pozycja x
    //! \param posY pozycja y
    //! \param r promien rysowanego kola
    SingleMarker(const QString& name, int posX, int posY, int r);

public:
    //!
    virtual int getY() const { return getScale() * ( posY - r); }
    //!
    //! \param val
    virtual void setY(int val) { posY = val + r; }
    //!
    virtual int getX() const { return getScale() * (posX - r); }
    //!
    //! \param val
    virtual void setX(int val) { posX = val + r; }
    //!
    virtual int getWidth() const { return getScale() * (2 * r); }
    //!
    virtual int getHeight() const { return getScale() * (2 * r); }
    //!
    virtual const QString& getName() const { return name; }
    //! pozwala odrysowac sie obiektowi
    //! \param painter obiekt paintera
    //! \param selected czy marker jest zaznaczony przez myszke
    virtual void draw(QPainter& painter, bool selected);

private:
    void init (const QString& name, int posX, int posY, int r);

private:
    //! pozycja x
    int posX;
    //! pozycja y
    int posY;
    //! promien odrysowywanego kola
    int r;
    //! nazwa obiektu
    QString name;
};
typedef boost::shared_ptr<SingleMarker> SingleMarkerPtr;
typedef boost::shared_ptr<const SingleMarker> SingleMarkerConstPtr;

typedef boost::shared_ptr<QImage> QPixmapPtr;
typedef boost::shared_ptr<const QImage> QPixmapConstPtr;

//! reprezentuje obrazek na konfiguratorze
class SinglePicture : public IArea
{
public:
    //! Konstruktor
    //! \param name nazwa obrazka do wczytania
    //! \param posX pozycja x
    //! \param posY pozycja y
    //! \param alwaysVisible czy obrazek ma byc zawsze widoczny
    SinglePicture(const QString& name, int x = 0, int y = 0, bool alwaysVisible = false);
    //! Konstruktor
    //! \param name nazwa obszaru
    //! \param pixmap wyswietlany obrazek
    //! \param x pozycja x
    //! \param y pozycja y
    //! \param alwaysVisible czy obrazek ma byc zawsze widoczny
    SinglePicture(const QString& name, const QPixmapPtr& pixmap, int x = 0, int y = 0, bool alwaysVisible = false);

public:
    //!
    virtual int getY() const { return getScale() * (y); }
    //!
    //! \param val
    virtual void setY(int val) { y = val; }
    //!
    virtual int getX() const { return getScale() * (x); }
    //!
    //! \param val
    virtual void setX(int val) { x = val; }
    //!
    virtual int getWidth() const { return getScale() * (pixmap->width()); }
    //!
    virtual int getHeight() const { return getScale() * (pixmap->height()); }
    //!
    virtual const QString& getName() const { return name; }
    //! pozwala odrysowac sie obiektowi
    //! \param painter obiekt paintera
    //! \param selected czy marker jest zaznaczony przez myszke
    virtual void draw(QPainter& painter, bool selected);
    //! czy podany punkt znajduje sie wewnatrz obszaru
    //! \param x wspolrzedna x
    //! \param y wspolrzedna y
    virtual bool isInside(int x, int y) const;

public:
    //! \return wyswietlany obrazek
    QPixmapConstPtr getPixmap() const { return pixmap; }

public:
    //! ustawia nowa wartosc przezroczystosci obrazka
    //! \param pixmap obrazek, ktorego alpha sie zmienia
    //! \param alpha nowa wartosc przezroczystosci
    static void setPixmapAlpha(QImage& pixmap, unsigned int alpha);


private:
    //! nazwa obszaru
    QString name;
    //! wyswietlany obrazek
    QPixmapPtr pixmap;
    //! kopia obrazka ze zmieniona alpha (wyswietlane, gdy zaznacza sie obszar)
    QPixmapPtr alphaPixmap;
    //! pozycja x
    int x;
    //! pozycja y
    int y;
    //! czy obrazek ma byc zawsze widoczny
    bool alwaysVisible;
};
typedef boost::shared_ptr<SinglePicture> SinglePicturePtr;
typedef boost::shared_ptr<const SinglePicture> SinglePictureConstPtr;

#endif
