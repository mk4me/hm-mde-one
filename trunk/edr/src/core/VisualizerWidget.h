/********************************************************************
	created:  2011/03/10
	created:  10:3:2011   17:15
	filename: VisualizerWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__VISUALIZERWIDGET_H__
#define HEADER_GUARD_CORE__VISUALIZERWIDGET_H__

#include <QtGui/QDockWidget>
#include "Visualizer.h"
#include "VisualizerManager.h"
#include "VisualizerTitleBar.h"

//! Widget wizualizacyjny.
class VisualizerWidget : public QDockWidget
{
    Q_OBJECT
    //! Wewn�trzny widget.
    class InnerWidget;
private:
    //! Wewn�trzny widget.
    InnerWidget* innerWidget;
    //! Pasek tytu�owy.
    VisualizerTitleBar* titleBar;

public:
    //! Zeruj�cy konstruktor.
    //! \param parent
    //! \param flags
    VisualizerWidget(QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
    //!
    VisualizerWidget(UniqueID visualizerID, QWidget* parent = nullptr, Qt::WindowFlags flags = 0);
    //! Zapewnia mo�liwo�� kasowanie widgeta wizualizatora przez jego implementacj�.
    ~VisualizerWidget();

public:
    //! Inicjalizacja wizualizatorem.
    void initWithVisualizer(UniqueID id);

    //! \return Pasek tytu�owy.
    VisualizerTitleBar* getTitleBar()
    {
        return titleBar;
    }
    //! \return Pasek tytu�owy.
    const VisualizerTitleBar* getTitleBar() const
    {
        return titleBar;
    }

public slots:
    //! Zmienia stan dokowania.
    void toggleFloating();
    //! \param floating Czy widget ma by� oddokowany?
    void setFloating(bool floating);
    //! \param Czy zamieni� pasek tytu�owy na systemowy?
    void setReplaceTitleBar(bool replace);
    //! \param Czy zamieni� pasek tytu�owy na systemowy?
    void queueReplaceTitleBar(bool replace);

private slots:
    //! \param Nowy wizualizator.
    void setVisualizer(const VisualizerPtr& visualizer);

private:
    //! Inicjalizacja wizualizatora.
    void init();
};


#endif  // HEADER_GUARD_CORE__VISUALIZERWIDGET_H__