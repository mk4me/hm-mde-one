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
        if (picture && picture->isActive() == false) {
           // picture->setGlobalAlpha(100);
            emit elementHovered(picture->getName(), true);
        }
        currentArea = picture;
        repaint();
    }
      
    /*if (currentPicture) {
        QPointF p = event->posF();
        int gX = p.x();
        int gY = p.y();
        currentPicture->setX(currentPicture->getX() + (gX - pressedX));
        currentPicture->setY(currentPicture->getY() + (gY - pressedY));
        pressedX = gX;
        pressedY = gY;
        repaint(rect());
    }*/

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
    for (auto it = areas.rbegin(); it != areas.rend(); it++) {
        int x1 = (*it)->getX();
        int x2 = x1 + (*it)->getWidth();
        int y1 = (*it)->getY();
        int y2 = y1 + (*it)->getHeight();
        if (x1 <= x && x2 >= x && y1 <= y && y2 >= y) {
            return *it;
        }
    }

    return SinglePicturePtr();
}

bool ConfigurationPainter::eventFilter( QObject *obj, QEvent *event )
{
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        this->mouseMoveEvent(mouseEvent);
    }
    return false;
}

void SinglePicture::setPixmapAlpha(QPixmap& pixmap, unsigned int alpha )
{
    QPixmap alphaChannel = pixmap;
    QPainter p(&alphaChannel);
    p.fillRect(alphaChannel.rect(), QColor(alpha, alpha, alpha));
    p.end();
    pixmap.setAlphaChannel(alphaChannel);
}
