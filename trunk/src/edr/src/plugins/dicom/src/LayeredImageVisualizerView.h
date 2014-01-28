/********************************************************************
	created:	2013/10/17
	created:	17:10:2013   10:12
	filename: 	LayeredImageVisualizerView.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYEREDIMAGEVISUALIZERVIEW_H__
#define HEADER_GUARD_DICOM__LAYEREDIMAGEVISUALIZERVIEW_H__

#include <QtGui/QWidget>
#include <QtGui/QItemSelection>
#include <coreui/GraphicSceneWithStateMachine.h>
#include "LayeredModelView.h"
#include "LayeredStateMachine.h"

class QGraphicsScene;

namespace Ui { class LayeredImageVisualizer; }

class QGraphicsPixmapItem;

namespace dicom {

class LayeredImageVisualizer;



//! Widok wizualizatora obs³uguj¹cego obrazy z warstwami
class LayeredImageVisualizerView : public QWidget
{
    Q_OBJECT
public:
    //! Standardowy konstruktor 
    LayeredImageVisualizerView(LayeredImageVisualizer* model, QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~LayeredImageVisualizerView() {}
    
public Q_SLOTS:
    //! odœwie¿enie ca³ego wizualizatora, np. gdy zmieni³a siê aktywna seria
    void refresh();
    
private Q_SLOTS:
    void undo();
    void redo();
    void crop();
    void normalState();
    void moveState();

    void boneState();
    void skinState();
    void tendonState();
    void jointState();
    void inflamatoryState();
    void noiseState();

    void editSelectedSerie();
    void selectionChanged(const QModelIndex &);
private:
    //! ui z designera
    Ui::LayeredImageVisualizer* ui;
    //! model wizualizatora
    LayeredImageVisualizer* model;
    QGraphicsView* lastView;
};


}
#endif
