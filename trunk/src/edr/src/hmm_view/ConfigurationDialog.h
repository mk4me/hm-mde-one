/********************************************************************
	created:	2011/10/18
	created:	18:10:2011   8:55
	filename: 	ConfigurationDialog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__CONFIGURATIONDIALOG_H__
#define HEADER_GUARD_HMM__CONFIGURATIONDIALOG_H__

#include <QtGui/QDialog>
#include <QtCore/QFileInfo>
#include "ConfigurationPainter.h"
#include "ui_configurationDialog.h"
#include "ui_FilterConfiguratorWidget.h"

class ConfigurationWidget : public QWidget, public Ui::FilterConfiguratorWidget
{
    Q_OBJECT;
public:
    ConfigurationWidget(QWidget* parent);

public:
    void setBackground(ConfigurationPainter& painter, const QString& name, QPixmapConstPtr pixmap);
    void setText(const QString& text);
    void loadConfigurations(const QString& frontXml, const QString& backXml, const  std::map<QString, std::pair<QString, QString>>& names);
    void setVisibles(const std::map<QString, bool>& visibles);

signals:
    void itemSelected(const QString&, bool);
    void elementHovered(const QString& name, bool selected);

public slots:
    void onElementHovered(const QString& name, bool selected);

private slots:
    void onItemSelected(const QString& name, bool selected)
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

    void onSwitchButton()
    {
        if (isFront) {
            showBack();
        } else {
            showFront();
        }
    }

private:   
    void loadPictures(ConfigurationPainter& painter, const QStringList &files );

    void loadPicture(ConfigurationPainter& painter, const QString& name, int x, int y, bool alwaysVisible = false);

    void loadXml(ConfigurationPainter& painter, const QString& filename);

    void createMarker(ConfigurationPainter &painter, const QString& name, int x, int y);
    
    void showFront()
    {
        UTILS_ASSERT(!isFront);
        this->painterFront.show();
        this->painterBack.hide();
        this->currentPainter = &painterFront;
        //this->switchButton->setText(tr("Back"));
        isFront = true;

    }

    void showBack()
    {
        UTILS_ASSERT(isFront);
        this->painterFront.hide();
        this->painterBack.show();
        this->currentPainter = &painterBack;
        //this->switchButton->setText(tr("Front"));
        isFront = false;
    }

private:
    ConfigurationPainter painterFront;
    ConfigurationPainter painterBack;
    ConfigurationPainter* currentPainter;
    QHBoxLayout scrollLayout;
    bool isFront;
};

#endif
