#include "ConfigurationPainter.h"
#include <QtGui/QMouseEvent>

ConfigurationPainter::ConfigurationPainter( QWidget* parent ) :
QWidget(parent),
    scale(1.0f)
{
    setMouseTracking(true);
    installEventFilter(this);
}


void ConfigurationPainter::paintEvent( QPaintEvent * )
{
    QPainter painter(this);
    if (background) {
        painter.drawImage(QRect(0, 0, scale * background->width(), scale * background->height()), *background);
    }

    for (auto it = areas.begin(); it != areas.end(); it++) {
        (*it)->draw(painter, (*it) == currentArea);
    }
    painter.end();
}

void ConfigurationPainter::addArea( IAreaPtr data )
{
    for (auto it = areas.begin(); it != areas.end(); it++) {
        if ((*it)->getName() == data->getName()) {
            throw std::runtime_error("Picture already added!");
        }
    }
    data->setScale(scale);
    areas.push_back(data);
}

void ConfigurationPainter::removeArea( const QString& name )
{
    for (auto it = areas.begin(); it != areas.end(); it++) {
        if ((*it)->getName() == name) {
            areas.erase(it);
            return;
        }
    }

    throw std::runtime_error("Picture not found!");
}

void ConfigurationPainter::mouseMoveEvent( QMouseEvent * event )
{
    QPointF p = event->posF();
    auto picture = getArea(p.x(), p.y());
    if (picture != currentArea) {
        if (currentArea) {
            emit elementHovered(currentArea->getName(), false);
        }
        if (picture) {
            emit elementHovered(picture->getName(), true);
        }
        currentArea = picture;
        repaint();
    }
}

void ConfigurationPainter::mousePressEvent( QMouseEvent * event )
{
}

void ConfigurationPainter::mouseReleaseEvent( QMouseEvent * event )
{
    if (event->button() == Qt::LeftButton && currentArea) {
        bool active = !currentArea->isActive();
        currentArea->setActive(active);
        //currentPicture->setGlobalAlpha(255);
        emit elementSelected(currentArea->getName(), active);
        repaint();
    }
}

IAreaPtr ConfigurationPainter::getArea( int x, int y )
{
    typedef std::pair<IAreaPtr, int> pair;
    std::vector<pair> clickedAreas;
    for (auto it = areas.rbegin(); it != areas.rend(); it++) {
        if ((*it)->isInside(x, y)) {
            int w2 = (*it)->getWidth() / 2;
            int h2 = (*it)->getHeight() / 2;
            int distX = x - ((*it)->getX() + w2);
            int distY = y - ((*it)->getY() + h2);
            // obsluga przypadku, gdy kliknieto wiecej niz jeden obszar
            // wykorzystywana jest odleglosc kursora od srodka (metryka kwadratowa bo jest szybsza)
            clickedAreas.push_back(std::make_pair(*it, distX * distX + distY * distY));
        }
    }

    if (clickedAreas.size() > 0) {
        // jesli sa jakies obszary, na ktore kliknieto, to zwracany jest ten, 
        // ktorego srodek jest blizej kursora 
        auto lambda_sort = [&](const pair& left, const pair& right) 
        {
            return left.second < right.second;
        };
        std::sort(clickedAreas.begin(), clickedAreas.end(), lambda_sort);

        return clickedAreas[0].first;
    }

    return IAreaPtr();
}

bool ConfigurationPainter::eventFilter( QObject *obj, QEvent *event )
{
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        this->mouseMoveEvent(mouseEvent);
    }
    return false;
}

void ConfigurationPainter::trySetActive( const QString& name, bool selected )
{
    for (auto it = areas.begin(); it != areas.end(); it++) {
        if ((*it)->getName() == name) {
            (*it)->setActive(selected);
            return;
        }
    }
}

void ConfigurationPainter::intersectNames( const  NamesDictionary& names )
{
    std::list<AreasList::iterator> toErase;
    for (AreasList::iterator it = areas.begin(); it != areas.end(); it++) {
        const QString& areaName = (*it)->getName();
        bool found = false;
        for (auto it2 = names.begin(); it2 != names.end(); it2++) {
            QString name = it2->first;
            if (name == areaName) {
                found = true;
                break;
            }
        }
        if (!found) {
            toErase.push_back(it);
        }
    }

    for (auto it = toErase.begin(); it != toErase.end(); it++) {
        areas.erase(*it);
    }
}

void ConfigurationPainter::setScale( float val )
{
    UTILS_ASSERT(val > 0.0f);
    scale = val;
    for (AreasList::iterator it = areas.begin(); it != areas.end(); it++) {
        (*it)->setScale(val);
    }
}

void SinglePicture::setPixmapAlpha(QImage& pixmap, unsigned int alpha )
{
    QImage alphaChannel = pixmap;
    QPainter p(&alphaChannel);
    p.fillRect(alphaChannel.rect(), QColor(alpha, alpha, alpha));
    p.end();
    pixmap.setAlphaChannel(alphaChannel);
}

void SinglePicture::draw( QPainter& painter, bool selected )
{
    if (alwaysVisible || isActive()) {
        int shift = selected ? 2 : 0;
        painter.drawImage(QRect(getX() - shift, getY() - shift, getWidth() + shift, getHeight() + shift), *pixmap);
    } else {
        if (selected) {
            painter.drawImage(QRect(getX(), getY(), getWidth(), getHeight()), *alphaPixmap);
        }
    }

}

bool SinglePicture::isInside( int x, int y ) const
{
    if (IArea::isInside(x, y)) {
        int shiftX = (x - getX()) / getScale();
        int shiftY = (y - getY()) / getScale();
        const QRgb* data = reinterpret_cast<const QRgb*>(pixmap->constBits());

        int alpha = qAlpha(data[shiftX + shiftY * pixmap->width()]);

        if (alpha != 0) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool IArea::isInside( int x, int y ) const
{
    int x1 = getX();
    int x2 = x1 + getWidth();
    int y1 = getY();
    int y2 = y1 + getHeight();
    return (x1 <= x && x2 >= x && y1 <= y && y2 >= y);
}
