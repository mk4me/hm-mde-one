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

typedef boost::shared_ptr<QImage> QPixmapPtr;
typedef boost::shared_ptr<const QImage> QPixmapConstPtr;

#include <QtGui/QPen>
#include <QtGui/QPainter>

class IArea
{
public:
    IArea() : active(false),  scale(1.0f) {}
    virtual ~IArea() {}
public:
    virtual int getY() const = 0;
    virtual void setY(int val) = 0;
    virtual int getX() const = 0;
    virtual void setX(int val) = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual const QString& getName() const = 0;
    virtual void draw(QPainter& painter, bool selected) = 0;

    virtual bool isInside(int x, int y) const;

    bool isActive() const { return active; }
    void setActive(bool val) { active = val; }
    float getScale() const { return scale; }
    void setScale(float val) { scale = val; }


private:
    bool active;
    float scale;
    
};
typedef boost::shared_ptr<IArea> IAreaPtr;
typedef boost::shared_ptr<const IArea> IAreaConstPtr;

class SingleMarker : public IArea
{
public:
    SingleMarker(const QString& name, int posX, int posY)
    {
        init(name, posX, posY, 15);
    }

    SingleMarker(const QString& name, int posX, int posY, int r)
    {
        init(name, posX, posY, r);
    }

public:
    virtual int getY() const { return getScale() * ( posY - r); }
    virtual void setY(int val) { posY = val + r; }
    virtual int getX() const { return getScale() * (posX - r); }
    virtual void setX(int val) { posX = val + r; }
    virtual int getWidth() const { return getScale() * (2 * r); }
    virtual int getHeight() const { return getScale() * (2 * r); }
    virtual const QString& getName() const { return name; }
    virtual void draw(QPainter& painter, bool selected) 
    {
        QPen pen;
        QBrush brush;
        brush.setStyle(Qt::Dense5Pattern);
        pen.setWidth(3);
        if (isActive()) {
            if (selected) {
                pen.setColor(QColor(100,255,10));
                brush.setColor(QColor(100,255,10,50));
            } else {
                pen.setColor(QColor(100,200,10));
                brush.setColor(QColor(100,200,10,50));
            }
        } else {
            if (selected) {
                pen.setColor(QColor(100,100,100));
                brush.setColor(QColor(100,100,100,50));
            } else {
                pen.setColor(QColor(100,100,100,60));
                brush.setColor(QColor(100,100,100,10));
            }
        }

        painter.setPen(pen);
        painter.setBrush(brush);
        painter.drawEllipse(getX(), getY(), getWidth(), getHeight());
    }

private:
    void init (const QString& name, int posX, int posY, int r)
    {
        this->name = name;
        this->posX = posX;
        this->posY = posY;
        this->r = r;
    }

private:
    int posX, posY;
    int r;
    QString name;
};
typedef boost::shared_ptr<SingleMarker> SingleMarkerPtr;
typedef boost::shared_ptr<const SingleMarker> SingleMarkerConstPtr;

class SinglePicture : public IArea
{
public:
      SinglePicture(const QString& name, int x = 0, int y = 0, bool alwaysVisible = false) :
		  name(QFileInfo(name).baseName()),
          pixmap(new QImage(name)),
          alphaPixmap( new QImage(name)),
          x(x), y(y),
          alwaysVisible(alwaysVisible)
      {
          //QFileInfo info(name);
          //this->name = info.baseName();
          setPixmapAlpha(*alphaPixmap, 100);
      }

      SinglePicture(const QString& name, const QPixmapPtr& pixmap, int x = 0, int y = 0, bool alwaysVisible = false) :
		  name(name),
          pixmap(pixmap),
          alphaPixmap(new QImage(*pixmap)),
          x(x), y(y),
          alwaysVisible(alwaysVisible)
      {
          setPixmapAlpha(*alphaPixmap, 100);
      }

public:
    virtual int getY() const { return getScale() * (y); }
    virtual void setY(int val) { y = val; }
    virtual int getX() const { return getScale() * (x); }
    virtual void setX(int val) { x = val; }
    virtual int getWidth() const { return getScale() * (pixmap->width()); }
    virtual int getHeight() const { return getScale() * (pixmap->height()); }
    virtual const QString& getName() const { return name; }
    virtual void draw(QPainter& painter, bool selected);
    virtual bool isInside(int x, int y) const;

public:
    QPixmapConstPtr getPixmap() const { return pixmap; }

public:
    static void setPixmapAlpha(QImage& pixmap, unsigned int alpha);


private:
    QString name;
    QPixmapPtr pixmap;
    QPixmapPtr alphaPixmap;

    int x, y;
    bool alwaysVisible;
};
typedef boost::shared_ptr<SinglePicture> SinglePicturePtr;
typedef boost::shared_ptr<const SinglePicture> SinglePictureConstPtr;


class ConfigurationPainter : public QWidget
{
    Q_OBJECT;
public:
    ConfigurationPainter(QWidget* parent);

public:
    typedef std::list<IAreaPtr> AreasList;
    typedef std::map<QString, std::pair<QString, QString>> NamesDictionary;
public:
    QPixmapConstPtr getBackground() const { return background; }
    const QString& getName() const { return name; }
    void setBackground(const QString& name, const QPixmapConstPtr & val) { this->name = name; background = val; }
    void addArea(const IAreaPtr & data);
    void removeArea(const QString& name);
    AreasList::const_iterator begin() const { return areas.cbegin(); }
    AreasList::const_iterator end() const { return areas.cend(); }
    AreasList::iterator begin() { return areas.begin(); }
    AreasList::iterator end() { return areas.end(); }
    void trySetActive( const QString& name, bool selected );
    void intersectNames( const NamesDictionary& names );

    float getScale() const { return scale; }
    void setScale(float val);

signals:
    void elementHovered(const QString& name, bool selected);
    void elementSelected(const QString& name, bool selected);

protected:
    virtual void mouseMoveEvent ( QMouseEvent * event );
    virtual void mousePressEvent ( QMouseEvent * event );
    virtual void mouseReleaseEvent ( QMouseEvent * event );
    virtual void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *obj, QEvent *event);
    
private:
    IAreaPtr getArea(int x, int y);

private:
    QPixmapConstPtr background;
    QString name;
    AreasList areas;
    IAreaPtr currentArea;
    float scale;
};


#endif
