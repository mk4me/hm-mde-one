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

namespace Ui { class LayeredImageVisualizer; }

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

private slots:
    void refresh();

private:
    //! ui z designera
    Ui::LayeredImageVisualizer* ui;
    //! model wizualizatora
    LayeredImageVisualizer* model;
};


}
#endif
