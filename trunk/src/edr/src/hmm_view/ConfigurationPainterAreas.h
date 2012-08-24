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
    //! \return pozycja Y rysowanego obszaru
    virtual int getY() const = 0;
    //! \param val ustawia pozycj� Y rysowanego obszaru
    virtual void setY(int val) = 0;
    //! \return pozycja X rysowanego obszaru
    virtual int getX() const = 0;
    //! \param val ustawia pozycj� X rysowanego obszaru
    virtual void setX(int val) = 0;
    //! \return szeroko�� obszaru
    virtual int getWidth() const = 0;
    //! \return wysoko�� obszaru
    virtual int getHeight() const = 0;
    //! \return nazwa przypisana do obszaru
    virtual const QString& getName() const = 0;
    //! pozwala odrysowa� si� obiektowi
    //! \param painter obiekt paintera
    //! \param selected czy obiekt jest zaznaczony
    virtual void draw(QPainter& painter, bool selected) = 0;
    //! czy podany punkt znajduje si� wewn�trz obszaru
    //! \param x wsp�rz�dna x
    //! \param y wsp�rz�dna y
    virtual bool isInside(int x, int y) const;
    //! czy obszar jest aktywny (normalnie - czy klikni�to w niego myszka)
    bool isActive() const { return active; }
    //! ustawia aktywno�� obszaru (norm. po kliknieciu myszka)
    //! \param val
    void setActive(bool val) { active = val; }
    //! \return skala obszaru
    float getScale() const { return scale; }
    //! ustawia skal� rysowanego obszaru, nie mo�e by� r�wna 0
    //! \param val nowa skala
    void setScale(float val) { scale = val; }

private:
    //! czy obszar jest aktywny (normalnie - czy klikni�to w niego myszka)
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
    //! \param r promie� rysowanego kola
    SingleMarker(const QString& name, int posX, int posY, int r);

public:
    //! \return pozycja Y rysowanego markera (�rodek - promie�)
    virtual int getY() const { return getScale() * ( posY - r); }
    //! ustawia pozycj� Y rysowanego markera
    //! \param val nowa pozycja
    virtual void setY(int val) { posY = val + r; }
    //! \return pozycja Y rysowanego markera
    virtual int getX() const { return getScale() * (posX - r); }
    //! ustawia pozycj� X rysowanego markera  (�rodek - promie�)
    //! \param val nowa pozycja
    virtual void setX(int val) { posX = val + r; }
    //! \return szeroko�� rysowanego markera (2 x promie�)
    virtual int getWidth() const { return getScale() * (2 * r); }
    //! \return wysoko�� rysowanego markera (2 x promie�)
    virtual int getHeight() const { return getScale() * (2 * r); }
    //! \return nazwa markera
    virtual const QString& getName() const { return name; }
    //! pozwala odrysowa� si� obiektowi
    //! \param painter obiekt paintera
    //! \param selected czy marker jest zaznaczony przez myszk�
    virtual void draw(QPainter& painter, bool selected);

private:
    //! Inicjalizacja markera
    //! \param name nazwa markera
    //! \param posX wsp�rz�dna X �rodka markera
    //! \param posY wsp�rz�dna Y �rodka markera
    //! \param r promie� markera
    void init (const QString& name, int posX, int posY, int r);

private:
    //! pozycja x
    int posX;
    //! pozycja y
    int posY;
    //! promie� odrysowywanego kola
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
    //! \param alwaysVisible czy obrazek ma by� zawsze widoczny
    SinglePicture(const QString& name, int x = 0, int y = 0, bool alwaysVisible = false);
    //! Konstruktor
    //! \param name nazwa obszaru
    //! \param pixmap wy�wietlany obrazek
    //! \param x pozycja x
    //! \param y pozycja y
    //! \param alwaysVisible czy obrazek ma by� zawsze widoczny
    SinglePicture(const QString& name, const QPixmapPtr& pixmap, int x = 0, int y = 0, bool alwaysVisible = false);

public:
    //! \return wsp�rz�dna Y rysowanego obrazka
    virtual int getY() const { return getScale() * (y); }
    //! ustawia wsp�rz�dn� Y rysowanego obrazka
    //! \param val nowa wsp�rz�dna
    virtual void setY(int val) { y = val; }
    //! \return wsp�rz�dna X rysowanego obrazka
    virtual int getX() const { return getScale() * (x); }
    //! ustawia wsp�rz�dn� Y rysowanego obrazka
    //! \param val nowa wsp�rz�dna
    virtual void setX(int val) { x = val; }
    //! \return szeroko�� obrazka
    virtual int getWidth() const { return getScale() * (pixmap->width()); }
    //! \return wysoko�� obrazka
    virtual int getHeight() const { return getScale() * (pixmap->height()); }
    //! \return nazwa wy�wietlana wraz z obrazkiem
    virtual const QString& getName() const { return name; }
    //! pozwala odrysowa� si� obiektowi
    //! \param painter obiekt paintera
    //! \param selected czy marker jest zaznaczony przez myszk�
    virtual void draw(QPainter& painter, bool selected);
    //! czy podany punkt znajduje si� wewn�trz obszaru
    //! \param x wsp�rz�dna x
    //! \param y wsp�rz�dna y
    virtual bool isInside(int x, int y) const;

public:
    //! \return wy�wietlany obrazek
    QPixmapConstPtr getPixmap() const { return pixmap; }

public:
    //! ustawia nowa warto�� przezroczysto�ci obrazka
    //! \param pixmap obrazek, kt�rego alpha si� zmienia
    //! \param alpha nowa warto�� przezroczysto�ci
    static void setPixmapAlpha(QImage& pixmap, unsigned int alpha);
    
private:
    //! nazwa obszaru
    QString name;
    //! wy�wietlany obrazek
    QPixmapPtr pixmap;
    //! kopia obrazka ze zmieniona alpha (wy�wietlane, gdy zaznacza si� obszar)
    QPixmapPtr alphaPixmap;
    //! pozycja x
    int x;
    //! pozycja y
    int y;
    //! czy obrazek ma by� zawsze widoczny
    bool alwaysVisible;
};
typedef boost::shared_ptr<SinglePicture> SinglePicturePtr;
typedef boost::shared_ptr<const SinglePicture> SinglePictureConstPtr;

#endif
