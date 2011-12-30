#include "CorePCH.h"
#include "FlexiTabSectionWidget.h"

FlexiTabSectionWidget::FlexiTabSectionWidget(const QString & sectionName, const QPicture * picture,
    QAction * configAction, QWidget* parent, Qt::WindowFlags f) : QFrame(parent, f), currentWidget(nullptr)
{
    setupUi(this);
    //userArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    setSectionName(sectionName);
    setPicture(picture);

    if(configAction != nullptr){
        this->configActionButton->setDefaultAction(configAction);
        this->configActionButton->show();
    }else{
        this->configActionButton->hide();
    }
}

FlexiTabSectionWidget::~FlexiTabSectionWidget()
{
    if(currentWidget != nullptr){
        auto item = userArea->layout()->takeAt(0);
        item->widget()->setParent(nullptr);
        item->widget()->setVisible(false);
        delete item;
    }
}

void FlexiTabSectionWidget::setSectionName(const QString & sectionName)
{
    this->sectionNameLabel->setText(sectionName);
}

void FlexiTabSectionWidget::setCfgButtonAction(QAction * configAction)
{
    this->configActionButton->setDefaultAction(configAction);

    if(configAction != nullptr){
        this->configActionButton->show();
    }else{
        this->configActionButton->hide();
    }
}

void FlexiTabSectionWidget::setPicture(const QPicture * picture)
{

}

void FlexiTabSectionWidget::setWidget(QWidget * widget)
{
    if(currentWidget != nullptr){
        auto item = userArea->layout()->takeAt(0);
        delete item;
    }

    currentWidget = widget;
    if(currentWidget != nullptr){
        userArea->layout()->addWidget(currentWidget);
    }
}