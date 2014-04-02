/********************************************************************
	created:	2013/11/26
	created:	26:11:2013   22:01
	filename: 	AdnotationsDelegate.h
	author:		Wojciech Kniec
	
	purpose:	Delegat obs³uguj¹cy edycjê adnotacji w drzewie
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__ADNOTATIONSDELEGATE_H__
#define HEADER_GUARD_DICOM__ADNOTATIONSDELEGATE_H__

#include <QtGui/QItemDelegate>
#include <plugins/dicom/ILayeredImage.h>

class QWidget;

namespace dicom {
    class AdnotationsDelegate : public QItemDelegate
    {
        Q_OBJECT

    public:
        AdnotationsDelegate(const int column, QObject *parent = 0);
		//! \param val Obraz adnotacji dla którego bêdzie tworzony edytor
		void setImage(ILayeredImageConstPtr val);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;

        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model,
            const QModelIndex &index) const;

        void updateEditorGeometry(QWidget *editor,
            const QStyleOptionViewItem &option, const QModelIndex &index) const;

	private:
		//! Obraz adnotacji
		ILayeredImageConstPtr val;
		//! Kolumna dla której mamy delegata
		const int column;
    };
}

#endif
