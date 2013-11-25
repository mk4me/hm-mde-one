/********************************************************************
	created:	2013/10/06
	created:	6:10:2013   14:34
	filename: 	DicomSourceWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DICOMSOURCEWIDGET_H__
#define HEADER_GUARD_DICOM__DICOMSOURCEWIDGET_H__

#include <QtGui/QWidget>
#include <corelib/IFileDataManager.h>

class QPushButton;
class QLayout;

namespace core
{
    class IMemoryDataManager;
}

namespace Ui {
    class DicomSource;
}

namespace dicom {

    class DicomSource;

    class DicomSourceWidget : public QWidget
    {
    private:
        Q_OBJECT;
    public:
        DicomSourceWidget(DicomSource *source);

    private slots:
        void onLoadFiles();
        void onLoadDirectory();

        void onOpenProject();
        void onImport();
        void onSelectImportDir();
        void onSelectSaveDir();
    
    private:
        DicomSource* dicomSource;
        Ui::DicomSource* ui;
    };
}

#endif