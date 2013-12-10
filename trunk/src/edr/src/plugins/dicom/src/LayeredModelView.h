/********************************************************************
	created:	2013/11/04
	created:	4:11:2013   20:46
	filename: 	LayeredModelView.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYEREDMODELVIEW_H__
#define HEADER_GUARD_DICOM__LAYEREDMODELVIEW_H__

#include <QtCore/QAbstractTableModel>
#include <QtCore/QString>
#include <plugins/dicom/ILayeredImage.h>

namespace dicom {

class LayeredModelView : public QAbstractTableModel
{
    Q_OBJECT
public:
    LayeredModelView(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    
    Qt::ItemFlags flags(const QModelIndex & index) const ;
    virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

    ILayeredImageConstPtr getImage() const;
    void setImage(ILayeredImagePtr val);

    void refresh() { reset(); }

protected:
    virtual bool removeRows( int row, int count, const QModelIndex &parent = QModelIndex( ) );

private:
    ILayeredImagePtr image;
    
Q_SIGNALS:
    void editCompleted(const QString &);


};

}

#endif
