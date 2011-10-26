#include "ConfigurationPainter.h"
#include <QtGui/QMouseEvent>

void ConfigurationPainter::paintEvent( QPaintEvent * )
{
    QPainter painter(this);
    if (background) {
        painter.drawPixmap(rect(), *background);
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
            //currentPicture->setGlobalAlpha(255);
            emit elementHovered(currentArea->getName(), false);
        }
        if (picture) {
           // picture->setGlobalAlpha(100);
            emit elementHovered(picture->getName(), true);
        }
        currentArea = picture;
        repaint();
    }
}

void ConfigurationPainter::mousePressEvent( QMouseEvent * event )
{
   /* if (event->button() == Qt::LeftButton) {
        QPointF p = event->posF();
        currentPicture = getSinglePicture(p.x(), p.y());
        pressedX = p.x();
        pressedY = p.y();
        repaint(rect());
    }*/
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
        int x1 = (*it)->getX();
        int x2 = x1 + (*it)->getWidth();
        int y1 = (*it)->getY();
        int y2 = y1 + (*it)->getHeight();
        if (x1 <= x && x2 >= x && y1 <= y && y2 >= y) {
            // TODO : obrazki moglyby byc sprawdzane za pomoca alphy
            int w2 = (*it)->getWidth() / 2;
            int h2 = (*it)->getHeight() / 2;
            int distX = x - (x1 + w2);
            int distY = y - (y1 + h2);
            clickedAreas.push_back(std::make_pair(*it, distX * distX + distY * distY));
        }
    }

    if (clickedAreas.size() > 0) {
        if (clickedAreas.size() > 1) {
            int dupa = 1;
            dupa += 10;
        }
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

void SinglePicture::setPixmapAlpha(QPixmap& pixmap, unsigned int alpha )
{
    QPixmap alphaChannel = pixmap;
    QPainter p(&alphaChannel);
    p.fillRect(alphaChannel.rect(), QColor(alpha, alpha, alpha));
    p.end();
    pixmap.setAlphaChannel(alphaChannel);
}
