/********************************************************************
    created:  2013/01/30
    created:  30:1:2013   12:37
    filename: CoreCompoundVisualizerWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___CORECOMPOUNDVISUALIZERWIDGET_H__
#define HEADER_GUARD___CORECOMPOUNDVISUALIZERWIDGET_H__

#include <corelib/Visualizer.h>
#include <QtGui/QWidget>
#include <QtGui/QComboBox>

namespace coreUI{

	class CoreWidgetAction;
	class CoreVisualizerWidget;

	class CoreCompoundVisualizerWidget : public QWidget
	{
		Q_OBJECT

	private:
		QComboBox * visualizerSwitch;
		CoreWidgetAction * switchVisualizerAction;

	private:

		void innerShowCurrentIndex(int idx);

	public:
		CoreCompoundVisualizerWidget(CoreCompoundVisualizerWidget & visualizer);

		CoreCompoundVisualizerWidget();

		~CoreCompoundVisualizerWidget();

	public:
		void addVisualizer(core::VisualizerPtr visualizer, const QString & visualizerName);
		void addVisualizer(const QIcon & icon, core::VisualizerPtr visualizer, const QString & visualizerName);
		int count() const;
		int currentIndex() const;
		core::VisualizerPtr currentVisualizer() const;
		CoreVisualizerWidget * currentVisualizerWidget() const;
		bool nameDuplicatesEnabled() const;
		int findVisualizer(core::VisualizerPtr visualizer) const;
		int findVisualizer(const QString & visualizerName) const;
		void insertVisualizer(int index, core::VisualizerPtr visualizer, const QString & visualizerName);
		void insertVisualizer(int index, const QIcon & icon, core::VisualizerPtr visualizer, const QString & visualizerName);
		QComboBox::InsertPolicy insertPolicy() const;
		void insertSeparator(int index);
		QIcon visualizerIcon(int index) const;
		QString visualizerText(int index) const;
		int maxCount() const;
		int maxVisibleItems() const;
		int minimumContentsLength() const;
		void removeVisualizer(int index);
		void setNameDuplicatesEnabled(bool enable);
		void setIconSize(const QSize & size);
		void setInsertPolicy(QComboBox::InsertPolicy policy);
		void setVisualizerIcon(int index, const QIcon & icon);
		void setVisualizerText(int index, const QString & text);
		void setMaxCount(int max);
		void setMaxVisibleVisualizers(int maxItems);
		void setMinimumContentsLength(int characters);
		void setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy policy);
		QComboBox::SizeAdjustPolicy sizeAdjustPolicy() const;

	public slots:
		void clear();
		void setCurrentIndex(int index);

	signals:
		void currentIndexChanged(int index);
	};

}

#endif	//	HEADER_GUARD___CORECOMPOUNDVISUALIZERWIDGET_H__
