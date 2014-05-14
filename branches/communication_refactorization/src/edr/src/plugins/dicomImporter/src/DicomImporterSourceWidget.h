/********************************************************************
	created:	2013/11/18
	created:	18:11:2013   12:08
	filename: 	DicomImporterSourceWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOMIMPORTER__DICOMIMPORTERSOURCEWIDGET_H__
#define HEADER_GUARD_DICOMIMPORTER__DICOMIMPORTERSOURCEWIDGET_H__

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

namespace dicomImporter {

    class DicomImporterSource;

    class DicomImporterSourceWidget : public QWidget
    {
    private:
        Q_OBJECT;
    public:
        DicomImporterSourceWidget(DicomImporterSource *source);

    private slots:
        /*void onLoadFiles();
        void onLoadDirectory();
*/
        //void onOpenProject();
        void onImport();
        void onSelectImportDir();
        void onSelectSaveDir();
        void onUpdateMeta();

    private:
        DicomImporterSource* dicomSource;
        Ui::DicomSource* ui;
    };
}

#endif
