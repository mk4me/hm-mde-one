#include "ConfigurationDialog.h"
#include <QtCore/QFileInfo>
#include <QtGui/QListWidgetItem>

ConfigurationDialog::ConfigurationDialog(QWidget* parent) :
    QDialog(parent),
    painterFront(this),
    painterBack(this)
{
    setupUi(this);
    scrollAreaWidget->setLayout(&scrollLayout);
    scrollLayout.addWidget(&painterFront);
    scrollLayout.addWidget(&painterBack);
    painterBack.hide();
    setWindowFlags(Qt::Tool);
    connect(&painterFront, SIGNAL(elementSelected(const QString&, bool)), this, SLOT(onItemSelected(const QString&, bool)));
    connect(&painterBack, SIGNAL(elementSelected(const QString&, bool)), this, SLOT(onItemSelected(const QString&, bool)));
}

void ConfigurationDialog::setBackground(ConfigurationPainter& painter, const QString& name, QPixmapConstPtr pixmap )
{ 
    scrollAreaWidget->setFixedSize(pixmap->width(), pixmap->height());
    painter.setBackground(name, pixmap);
}


void ConfigurationDialog::loadPictures(ConfigurationPainter& painter, const QStringList &files )
{
    int count = files.size();
    for (int i = 0; i < count; i++) {
        const QString& name = files.at(i);
        loadPicture(painter, name, 0, 0);
    }

    painter.repaint(painter.rect());
}

void ConfigurationDialog::loadConfigurations(const QString& frontXml, const QString& backXml)
{
    loadXml(painterFront, frontXml);
    loadXml(painterBack, backXml);
    QString path1 = core::getResourceString("images/przod.png");
    QString path2 = core::getResourceString("images/tyl.png");
    loadPicture(painterFront, path1, 10, 10, true);
    loadPicture(painterBack, path2, 10, 10, true);
}

void ConfigurationDialog::loadPicture(ConfigurationPainter& painter, const QString& name, int x, int y, bool alwaysVisible)
{
    SinglePicturePtr data(new SinglePicture(name, x, y, alwaysVisible));
    painter.addArea(data);
}

void ConfigurationDialog::loadXml(ConfigurationPainter& painter, const QString &filename )
{
    QFileInfo fileInfo(filename);
    QString path = fileInfo.absolutePath();
    TiXmlDocument document(filename.toStdString());
    if(!document.LoadFile()) {
        throw std::runtime_error("Unable to load file");
    }
    TiXmlHandle hDocument(&document);
    TiXmlElement* root = hDocument.FirstChildElement().Element();
    if (!root) {
        throw std::runtime_error("Unable to load file");
    }

    QString background(root->Attribute("BackgroundFilename"));
    background = path + "\\" + background;
    QPixmapPtr pixmap(new QPixmap(background));
    setBackground(painter, background, pixmap);

    TiXmlElement* element = root->FirstChildElement();
    while (element) {
        int x, y;
        element->Attribute("X", &x);
        element->Attribute("Y", &y);
        std::string val = element->Value();
        if (val == "Picture") {
            QString filename(element->Attribute("Filename"));
            filename = path + "\\" + filename;
            loadPicture(painter, filename, x, y);
        } else if (val == "Marker") {
            QString name(element->Attribute("Name"));
            SingleMarkerPtr marker(new SingleMarker(name, x, y));
            marker->setActive(true);
            painter.addArea(marker);
        }
        element = element->NextSiblingElement();
    }
}
