#include "CoreUiPCH.h"
#include <coreui/CoreVisualizerWidget.h>

#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QMenu>
#include <QtGui/QToolButton>
#include <iterator>
#include <corelib/StringTools.h>
#include <boost/foreach.hpp>

#include <corelib/DataAccessors.h>
#include <coreui/CoreMainWindow.h>
#include <corelib/DataAccessors.h>
#include <coreui/CoreTitleBar.h>
#include <coreUI/CoreWidgetAction.h>
#include <coreUI/CoreAction.h>

using namespace coreUI;
using namespace core;

struct FutureSerieData {
	std::string serieName;
	TypeInfo requestedType;
	ObjectWrapperConstPtr data;
	int localIdx;
};

Q_DECLARE_METATYPE(FutureSerieData);
Q_DECLARE_METATYPE(Visualizer::VisualizerSerie*);


//! Zerujący konstruktor.
//! \param parent
//! \param flags
CoreVisualizerWidget::CoreVisualizerWidget(core::VisualizerPtr visualizer, QWidget* parent, Qt::WindowFlags flags) : QWidget(parent, flags),
	visualizer_(visualizer), activeSerieSwitch(nullptr)
{
	auto visWidget = visualizer->getOrCreateWidget();

	if(visWidget == nullptr){
		throw std::runtime_error("nullptr instead of QWidget obtained from Visualizer");
	}

	visualizer_->getSupportedTypes(supportedDataTypes);

	auto layout = new QHBoxLayout;
	setLayout(layout);
	layout->addWidget(visWidget);

	//wybór danych
	QMenu * dataSelectioMenu = new QMenu(tr("Data"), visWidget);
	dataSelectioMenu->setIcon(QIcon(QString::fromUtf8(":/resources/icons/dane.wejsciowe.png")));
	connect(dataSelectioMenu, SIGNAL(aboutToShow()), this, SLOT(fillSourcesMenu()));
	connect(dataSelectioMenu, SIGNAL(aboutToHide()), this, SLOT(clearSourcesMenu()));
	CoreWidgetAction * dataSelectAction = new CoreWidgetAction(this, tr("Settings"), CoreTitleBar::Left);
	QToolButton * dataSelectionButton = new QToolButton(visWidget);
	dataSelectionButton->setText(tr("Data"));
	dataSelectionButton->setIcon(QIcon(QString::fromUtf8(":/resources/icons/dane.wejsciowe.png")));
	dataSelectionButton->setMenu(dataSelectioMenu);
	dataSelectAction->setDefaultWidget(dataSelectionButton);
	addAction(dataSelectAction);

	//ustawianie aktualnej serii danych
	if(visualizer_->getMaxSeries() != 1){
		activeSerieSwitch = new QComboBox(visWidget);
		activeSerieSwitch->addItem(tr("No active session"));
		connect(activeSerieSwitch, SIGNAL(currentIndexChanged(int)), this, SLOT(serieSelected(int)));
		CoreWidgetAction * activeDataSelectAction = new CoreWidgetAction(this, tr("Settings"), CoreTitleBar::Left);
		activeDataSelectAction->setDefaultWidget(activeSerieSwitch);
		addAction(activeDataSelectAction);
	}

	//screenshot
	CoreAction * screenshotAction = new CoreAction(tr("Utils"), tr("Screenshot"), this, CoreTitleBar::Right);
	connect(screenshotAction, SIGNAL(triggered()), visualizer.get(), SLOT(onScreenshotTrigger()));
	addAction(screenshotAction);

	//akcja czyszcząca wszystkie dane wizualizatora
	dataDeselectAll = new QAction(tr("Remove all"), this);
	connect(dataDeselectAll, SIGNAL(triggered()), this, SLOT(removeAllSeries()));

	//! Akcja odpowiedzialna za przełanczanie pomiędzy aktywnym obserwowaniem dataManagera w przyapdku zmian danych związanych z seriami danych
	CoreAction * liveObserveDataAction = new CoreAction(tr("Utils"), QIcon(), tr("Live data update"), this, CoreTitleBar::Right);
	liveObserveDataAction->setCheckable(true);
	liveObserveDataAction->setChecked(true);
	connect(liveObserveDataAction, SIGNAL(triggered(bool)), this, SLOT(onLiveObserveChange(bool)));

	addAction(liveObserveDataAction);
}

CoreVisualizerWidget::CoreVisualizerWidget(const CoreVisualizerWidget & visualizer)
{
	//TODO
}

//! Zapewnia możliwość kasowanie widgeta wizualizatora przez jego implementację.
CoreVisualizerWidget::~CoreVisualizerWidget()
{
	
}

core::VisualizerPtr CoreVisualizerWidget::getVisualizer()
{
	return visualizer_;
}

void CoreVisualizerWidget::fillSourcesMenu()
{
	const static QString dataActionName(tr("(Source: %1) Name: %2"));
	const static QString localDataName(tr("Local data name %1"));
	const static QString unknownDataSource(tr("Unknown"));

	QMenu * menu = qobject_cast<QMenu*>(sender());

	auto allDataSubmenu =  menu->addMenu(tr("All data"));
	auto activeDataSubmenu = menu->addMenu(tr("Active data"));
	menu->addSeparator();
	menu->addAction(dataDeselectAll);

	if(activeData.empty() == true){
		dataDeselectAll->setEnabled(false);
	}else{

		//TODO
		//oznaczyć serie które są w wizualizatorze ale nie ma ich już w samych danych wizualizatora!!

		//najpierw serie aktywne
		std::map<TypeInfo, QMenu*> typeMenus;
		for(auto typeIT = activeData.begin(); typeIT != activeData.end(); ++typeIT){
			auto typeMenu = activeDataSubmenu->addMenu(QString::fromUtf8(typeIT->first.name()));
			for(auto dataIT = typeIT->second.begin(); dataIT != typeIT->second.end(); ++dataIT){
				std::string dataSource = unknownDataSource.toStdString();
				getDataSource(dataIT->first, dataSource);
				
				auto action = typeMenu->addAction(dataActionName.arg(QString::fromStdString(dataSource)).arg(QString::fromStdString(dataIT->second->serie()->getName())));
				action->setCheckable(true);
				action->setChecked(true);
				QVariant data;
				data.setValue(dataIT->second);
				action->setData(data);
				connect(action, SIGNAL(triggered()), this, SLOT(removeSerie()));
			}
		}
	}

	//teraz wszystkie dane

	if(visualizer_->getNumSeries() == visualizer_->getMaxSeries()){
		allDataSubmenu->setEnabled(false);
	}

	//lokalny index danych jeśli nie mają nazwy
	int localDataIndex = 0;

	//TODO
	//oznaczyć serie które są w wizualizatorze ale nie ma ich już w samych danych wizualizatora!!

	//wszystkie dane wg typów jakie obsługuje wizualizator
	for(auto typeIT = supportedDataTypes.begin(); typeIT != supportedDataTypes.end(); ++typeIT){
		core::ConstObjectsList data;
		visualizer_->getData(*typeIT, data, false);
		if(data.empty() == false){
			auto activeTypeIT = activeData.find(*typeIT);
			auto subDataManu = allDataSubmenu->addMenu(QString::fromUtf8((*typeIT).name()));
			for(auto dataIT = data.begin(); dataIT != data.end(); ++dataIT){
				std::string dataSource = unknownDataSource.toStdString();
				getDataSource(*dataIT, dataSource);

				int serieLocIndex = -1;

				std::string dataName;
				if(getDataName(*dataIT, dataName) == false){
					serieLocIndex = localDataIndex = nextValidLocalSerieIdx(localDataIndex);
					dataName = localDataName.arg(localDataIndex++).toStdString();
				}

				auto action = subDataManu->addAction(dataActionName.arg(QString::fromStdString(dataSource)).arg(QString::fromStdString(dataName)));
				action->setCheckable(true);
				
				bool active = false;
				if(activeTypeIT != activeData.end()){
					auto activeDataIT = activeTypeIT->second.find(*dataIT);
					if(activeDataIT != activeTypeIT->second.end()){
						active = true;
						action->setChecked(true);
						QVariant data;
						data.setValue(activeDataIT->second);
						action->setData(data);
						connect(action, SIGNAL(triggered()), this, SLOT(removeSerie()));
					}
				}

				if(active == false){
					FutureSerieData serieData;
					serieData.data = *dataIT;
					serieData.requestedType = *typeIT;
					serieData.serieName = dataName;
					serieData.localIdx = serieLocIndex;
					action->setChecked(false);
					QVariant data;
					data.setValue(serieData);
					action->setData(data);
					connect(action, SIGNAL(triggered()), this, SLOT(addSerie()));
				}
			}
		}
	}
}

const int CoreVisualizerWidget::nextValidLocalSerieIdx(int startIdx) const
{
	if(usedLocalNameIndexes.empty() == true || startIdx < (*usedLocalNameIndexes.begin()) || startIdx > (*usedLocalNameIndexes.rbegin())){
		return startIdx;
	}

	while(usedLocalNameIndexes.find(startIdx) != usedLocalNameIndexes.end()) { ++startIdx; }

	return startIdx;
}

void CoreVisualizerWidget::clearSourcesMenu()
{
	QMenu * menu = qobject_cast<QMenu*>(sender());
	menu->blockSignals(true);
	menu->clear();
	menu->blockSignals(false);
}

void CoreVisualizerWidget::removeAllSeries()
{	
	visualizer_->setActiveSerie(nullptr);
	if(activeSerieSwitch != nullptr){
		activeSerieSwitch->blockSignals(true);
		activeSerieSwitch->clear();
		activeSerieSwitch->blockSignals(false);
	}
	std::set<int>().swap(usedLocalNameIndexes);
	std::map<core::TypeInfo, std::map<core::ObjectWrapperConstPtr, core::Visualizer::VisualizerSerie*>>().swap(activeData);
	std::map<core::Visualizer::VisualizerSerie*, int>().swap(serieLocalIdx);
	visualizer_->destroyAllSeries();
}

void CoreVisualizerWidget::serieSelected(int idx)
{
	if(idx == 0){
		visualizer_->setActiveSerie(nullptr);
	}else{
		visualizer_->setActiveSerie(activeSerieSwitch->itemData(idx).value<Visualizer::VisualizerSerie*>());
	}
}

void CoreVisualizerWidget::addSerie()
{
	QAction * action = qobject_cast<QAction*>(sender());
	FutureSerieData data = action->data().value<FutureSerieData>();
	auto serie = visualizer_->createSerie(data.requestedType, data.data);
	serie->serie()->setName(data.serieName);
	activeData[data.requestedType].insert(std::map<core::ObjectWrapperConstPtr, core::Visualizer::VisualizerSerie*>::value_type(data.data, serie));
	if(data.localIdx > -1){
		serieLocalIdx[serie] = data.localIdx;
		usedLocalNameIndexes.insert(data.localIdx);
	}

	if(activeSerieSwitch != nullptr){
		QVariant serieData;
		serieData.setValue(serie);
		activeSerieSwitch->addItem(QString::fromStdString(data.serieName), serieData);
		activeSerieSwitch->setEnabled(true);
		if(activeSerieSwitch->count() == 2){
			activeSerieSwitch->setCurrentIndex(1);
		}
	}else{
		visualizer_->setActiveSerie(serie);
	}
}

void CoreVisualizerWidget::removeSerie()
{
	QAction * action = qobject_cast<QAction*>(sender());
	Visualizer::VisualizerSerie * serie = action->data().value<Visualizer::VisualizerSerie *>();

	auto activeDataIT = activeData.find(serie->serie()->getRequestedDataType());

	activeDataIT->second.erase(serie->serie()->getData());

	if(activeDataIT->second.empty() == true){
		activeData.erase(activeDataIT);
	}

	auto it = serieLocalIdx.find(serie);
	if(it != serieLocalIdx.end())
	{
		usedLocalNameIndexes.erase(it->second);
		serieLocalIdx.erase(it);
	}

	if(activeSerieSwitch != nullptr){

		QVariant serieData;
		serieData.setValue(serie);

		int idx = activeSerieSwitch->findData(serieData);
		if(activeSerieSwitch->currentIndex() == idx){
			int newIdx = 0;
			if(activeData.empty() == false){
				if(idx == 1){
					newIdx = 2;
				}else{
					newIdx = idx -1;
				}
			}

			activeSerieSwitch->setCurrentIndex(newIdx);
			activeSerieSwitch->removeItem(idx);
			if(activeSerieSwitch->count() == 1){
				activeSerieSwitch->setEnabled(false);
			}
		}
	}else{
		visualizer_->setActiveSerie(nullptr);
	}

	visualizer_->destroySerie(serie);
	
}

const bool CoreVisualizerWidget::getDataName(core::ObjectWrapperConstPtr data, std::string & dataName)
{
	return (data->tryGetMeta("core/uiUserName", dataName) == false && data->tryGetMeta("core/uiName", dataName) == false
		&& data->tryGetMeta("core/name", dataName) == false);
}

const bool CoreVisualizerWidget::getDataSource(core::ObjectWrapperConstPtr data, std::string & dataSource)
{
	return data->tryGetMeta("core/source", dataSource);
}

void CoreVisualizerWidget::onLiveObserveChange(bool observe)
{
	visualizer_->setLiveObserveActive(observe);
}