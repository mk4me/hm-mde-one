/********************************************************************
	created:	2013/11/26
	created:	26:11:2013   22:01
	filename: 	AdnotationsDelegate.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__ADNOTATIONSDELEGATE_H__
#define HEADER_GUARD_DICOM__ADNOTATIONSDELEGATE_H__

#include <QtGui/QItemDelegate>

class QWidget;

namespace dicom {
    class AdnotationsDelegate : public QItemDelegate
    {
        Q_OBJECT

    public:
        AdnotationsDelegate(QObject *parent = 0);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;

        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model,
            const QModelIndex &index) const;

        void updateEditorGeometry(QWidget *editor,
            const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };
}

#endif
