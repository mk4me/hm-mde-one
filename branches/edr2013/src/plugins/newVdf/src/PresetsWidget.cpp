#include "NewVdfPCH.h"
#include "PresetsWidget.h"
#include <QtWidgets/QListWidget>
#include <QtWidgets/QLayout>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <QtWidgets/QPushButton>
#include "QtWidgets/QFileDialog"

vdf::PresetsWidget::PresetsWidget( SceneModelPtr scene, TypesWindow* tw ) :
    model(scene), 
    tw(tw)
{
    setLayout(new QVBoxLayout());
    QWidget* down = new QWidget();
    down->setLayout(new QHBoxLayout());
    
    loadButton = new QPushButton(tr("Load"));
    saveButton = new QPushButton(tr("Save"));
    
    down->layout()->addWidget(loadButton);
    down->layout()->addWidget(saveButton);
    
    connect(loadButton, SIGNAL(clicked()), this, SLOT(load()));    
    connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
    
    listWidget = new QListWidget();
    //layout()->addWidget(listWidget);
    layout()->addWidget(down);
}

void vdf::PresetsWidget::load()
{
    SceneModelPtr scene = model.lock();
    if (!scene) {
        return;
    }
    QString filename = QFileDialog::getOpenFileName();
    std::ifstream ifs(filename.toStdString().c_str());
    if (ifs.good()) {
        boost::archive::xml_iarchive xmlIn(ifs);
        SceneModel::Serializer s(scene, tw);
        xmlIn >> BOOST_SERIALIZATION_NVP(s);
    }
}

void vdf::PresetsWidget::save()
{
    SceneModelPtr scene = model.lock();
    if (!scene) {
        return;
    }
    QString filename = QFileDialog::getSaveFileName();
    std::ofstream ofs(filename.toStdString().c_str());
    if(ofs.good()) {
        boost::archive::xml_oarchive oa(ofs);
        SceneModel::Serializer serializer(scene, tw);
        oa << BOOST_SERIALIZATION_NVP(serializer);
        ofs.close();
    }
}
