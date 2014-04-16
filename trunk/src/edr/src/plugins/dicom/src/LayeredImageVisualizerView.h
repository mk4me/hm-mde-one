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
#include <coreui/CoreAction.h>
#include "AdnotationsDelegate.h"
#include "IDicomService.h"

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

public:
    void setAnnotationStatus(webservices::xmlWsdl::AnnotationStatus::Type status);
    
public Q_SLOTS:
    //! odœwie¿enie ca³ego wizualizatora, np. gdy zmieni³a siê aktywna seria
    void refresh();
	//! \param enable Czy button do usuwania ma byc aktywny - dezaktywuje dla poziomu krwii i poziomu schorzenia
	void setDeletionButtonEnabled(const bool enable);
    
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

	void acceptAnnotation();
	void rejectAnnotation();
	void requestAnnotationVerification();

private:

	static const QString getComment(const QString & title, const QString & content = QString());

	void setActionsEnabled(const bool enable);

	void refreshChat(const IDicomService::AnnotationStatus & as);

	const bool verifySerie();

private:
    //! ui z designera
    Ui::LayeredImageVisualizer* ui;
    //! model wizualizatora
    LayeredImageVisualizer* model;
    QGraphicsView* lastView;
	AdnotationsDelegate * adnotationDelegate0;
	AdnotationsDelegate * adnotationDelegate1;
};


}
#endif
