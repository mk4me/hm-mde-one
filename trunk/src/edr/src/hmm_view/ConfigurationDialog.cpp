#include "ConfigurationDialog.h"
#include <QtCore/QFileInfo>
#include <QtGui/QListWidgetItem>

const float SCALE = 0.6f;

ConfigurationWidget::ConfigurationWidget(QWidget* parent) :
    QWidget(parent),
    painterFront(this),
    painterBack(this),
    isFront(true)
{
    painterFront.setScale(SCALE);
    painterBack.setScale(SCALE);
    setupUi(this);
    scrollLayout.setMargin(0);
    scrollLayout.setContentsMargins(0, 0, 0, 0);
    scrollAreaWidget->setLayout(&scrollLayout);
    scrollLayout.addWidget(&painterFront);
    scrollLayout.addWidget(&painterBack);
    painterBack.hide();
    setWindowFlags(Qt::Tool);
    connect(&painterFront, SIGNAL(elementSelected(const QString&, bool)), this, SLOT(onItemSelected(const QString&, bool)));
    connect(&painterBack, SIGNAL(elementSelected(const QString&, bool)), this, SLOT(onItemSelected(const QString&, bool)));
    connect(&painterFront, SIGNAL(elementHovered(const QString&, bool)), this, SLOT(onElementHovered(const QString&, bool)));
    connect(&painterBack, SIGNAL(elementHovered(const QString&, bool)), this, SLOT(onElementHovered(const QString&, bool)));
    //connect(switchButton, SIGNAL(clicked()), this, SLOT(onSwitchButton()));
}

void ConfigurationWidget::setBackground(ConfigurationPainter& painter, const QString& name, QPixmapConstPtr pixmap )
{ 
    int w = static_cast<int>(SCALE * pixmap->width());
    int h = static_cast<int>(SCALE * pixmap->height());
    scrollArea->setMaximumSize(w, h);
    scrollArea->setMinimumSize(w, h);
    scrollAreaWidget->setGeometry(QRect(0, 0, w, h));
    painter.setBackground(name, pixmap);
}

void ConfigurationWidget::loadPicture(ConfigurationPainter& painter, const QString& name, int x, int y, bool alwaysVisible)
{
    SinglePicturePtr data(new SinglePicture(name, x, y, alwaysVisible));
    data->setActive(true);
    painter.addArea(data);
}

void ConfigurationWidget::loadPictures(ConfigurationPainter& painter, const QStringList &files )
{
    int count = files.size();
    for (int i = 0; i < count; i++) {
        const QString& name = files.at(i);
        loadPicture(painter, name, 0, 0);
    }

    painter.repaint(painter.rect());
}

void ConfigurationWidget::createMarker( ConfigurationPainter &painter, const QString& name, int x, int y)
{
    SingleMarkerPtr marker(new SingleMarker(name, x, y));
    marker->setActive(true);
    painter.addArea(marker);
}


void ConfigurationWidget::loadConfigurations(const QString& frontXml, const QString& backXml, const  std::map<QString, std::pair<QString, QString>>& names)
{
    loadXml(painterFront, frontXml);
    loadXml(painterBack, backXml);

    painterFront.intersectNames(names);
    painterBack.intersectNames(names);

    QString path1 = core::getResourceString("images/przod.png");
    QString path2 = core::getResourceString("images/tyl.png");
    loadPicture(painterFront, path1, 0, 0, true);
    loadPicture(painterBack, path2, 0, 0, true);
}



void ConfigurationWidget::loadXml(ConfigurationPainter& painter, const QString &filename )
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
    QPixmapPtr pixmap(new QImage(background));
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
            createMarker(painter, name, x, y);

        }
        element = element->NextSiblingElement();
    }
}

void ConfigurationWidget::onElementHovered( const QString& name, bool selected )
{
    emit elementHovered(name, selected);
    
}

void ConfigurationWidget::setText( const QString& text )
{
    this->label->setText(text);
}

void ConfigurationWidget::setVisibles( const std::map<QString, bool>& visibles )
{
    for (auto it = painterFront.begin(); it != painterFront.end(); it++) {
        auto found = visibles.find((*it)->getName());
        if (found == visibles.end()) {
            (*it)->setActive(true);
        } else {
            (*it)->setActive(found->second);
        }
    }
    for (auto it = painterBack.begin(); it != painterBack.end(); it++) {
        auto found = visibles.find((*it)->getName());
        if (found == visibles.end()) {
            (*it)->setActive(true);
        } else {
            (*it)->setActive(found->second);
        }
    }
}

void ConfigurationWidget::onItemSelected( const QString& name, bool selected )
{
    QFileInfo info(name);
    QString filename = info.baseName();
    if (filename == "przod") {
        showBack();
    } else if (filename == "tyl") {
        showFront();
    } else {
        emit itemSelected(name, selected);
        this->painterFront.trySetActive(name, selected);
        this->painterBack.trySetActive(name, selected);
    }
}

void ConfigurationWidget::onSwitchButton()
{
    if (isFront) {
        showBack();
    } else {
        showFront();
    }
}

void ConfigurationWidget::showFront()
{
    UTILS_ASSERT(!isFront);
    this->painterFront.show();
    this->painterBack.hide();
    this->currentPainter = &painterFront;
    //this->switchButton->setText(tr("Back"));
    isFront = true;
}

void ConfigurationWidget::showBack()
{
    UTILS_ASSERT(isFront);
    this->painterFront.hide();
    this->painterBack.show();
    this->currentPainter = &painterBack;
    //this->switchButton->setText(tr("Front"));
    isFront = false;
}

