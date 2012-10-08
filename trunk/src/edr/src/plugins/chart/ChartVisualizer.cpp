#include "ChartPCH.h"
#include "ChartVisualizer.h"

ChartVisualizer::ChartVisualizer() : 
    name("Chart"), prevActiveSerie(-1),
    prevTime(0),
	actionNormalized(nullptr),
	activeSerieCombo(nullptr)
{

}

ChartVisualizer::~ChartVisualizer()
{
}

const std::string& ChartVisualizer::getName() const
{
    return name;
}

core::IVisualizer* ChartVisualizer::createClone() const
{
    return new ChartVisualizer();
}

void ChartVisualizer::getInputInfo(std::vector<core::IInputDescription::InputInfo>& info)
{
    core::IInputDescription::InputInfo input;

    input.type = typeid(ScalarChannelReaderInterface);
    input.name = "Scalar";
    input.modify = false;
    input.required = false;

    info.push_back(input);
}

void ChartVisualizer::update( double deltaTime )
{
    bool needsUpdate = false;
    if ( chart->getActiveSerieIndex() != prevActiveSerie ) {
        prevActiveSerie = chart->getActiveSerieIndex();
        chart->refreshAll();
        needsUpdate = true;   
    }
    if ( chart->getActiveSerie() && chart->getActiveSerie()->getTime() != prevTime ) {
        prevTime = chart->getActiveSerie()->getTime();
        chart->refreshCursor();
        needsUpdate = true;
    }
    
    if (needsUpdate ) {
        viewer->update();
    }
}

QWidget* ChartVisualizer::createWidget(std::vector<QObject*>& actions)
{
    viewer = new osgui::QOsgDefaultWidget();
    viewer->setTimerActive(false);
    viewer->addEventHandler( new osgViewer::StatsHandler() );

    // pobranie cech kontekstu graficznego
    const osg::GraphicsContext::Traits* traits = viewer->getCamera()->getGraphicsContext()->getTraits();

    // konfiguracja kamery
    osg::Camera* camera = viewer->getCamera();
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF);
    camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, double(traits->width), 0.0f, double(traits->height)));
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix(osg::Matrix::identity());
    //camera->setClearColor(osg::Vec4(0.73f, 0.73f, 0.73f, 1));
	camera->setClearColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1));
    //camera->setClearColor(osg::Vec4(1, 1, 1, 1));

    // dodanie callbacak dostosowującego rozmiar kamery do rozmiaru okna
    camera->setEventCallback( osgui::createEventCallback( 
        osgGA::GUIEventAdapter::RESIZE,
        [&](osg::Node* node, const osgGA::GUIEventAdapter* event) {
            //const osg::GraphicsContext::Traits* traits = viewer->getCamera()->getGraphicsContext()->getTraits();
            dynamic_cast<osg::Camera&>(*node).setProjectionMatrixAsOrtho2D(0, event->getWindowWidth(), 0, event->getWindowHeight()); 
            //dynamic_cast<osg::Camera&>(*node).setProjectionMatrixAsOrtho2D(0, traits->width, 0, traits->height); 
    } 
    ));

    chart = new Chart();
    chart->setAutoRefresh(false);
    chart->setLocation(0, 0, traits->width, traits->height);

    chart->setGridColor(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
    chart->setShowGridX(true);
    chart->setShowGridY(true);
    chart->setGridDensity(5);
    chart->setMargin(10);
    chart->setNormalized(false);

    chart->setZ(-0.5f);
    chart->setZRange(0.5f);

    chart->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    // dodanie callbacka dostosowującego rozmiar wykresu do rozmiaru okna
    chart->setEventCallback( osgui::createEventCallback( 
        osgGA::GUIEventAdapter::RESIZE,
        [](osg::Node* node, const osgGA::GUIEventAdapter* event) {
            dynamic_cast<Chart&>(*node).setLocation(0, 0, event->getWindowWidth(), event->getWindowHeight()); 
    }
    ));

    viewer->setSceneData(chart);
    chart->setAutoRefresh(true);
    

    // dodajemy akcje
    actionNormalized = new QAction("normalize", viewer);
    actionNormalized->setCheckable(true);
    actionNormalized->setChecked( chart->isNormalized() );
    connect(actionNormalized, SIGNAL(triggered(bool)), this, SLOT(setNormalized(bool)));
    actions.push_back(actionNormalized);

    activeSerieCombo = new QComboBox();
    activeSerieCombo->addItem(QString::fromUtf8("No active serie"));
    activeSerieCombo->setEnabled(false);
    connect(activeSerieCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setActiveSerie(int)));

    actions.push_back(activeSerieCombo);

    return viewer;
}

void ChartVisualizer::setUp( core::IObjectSource* source )
{

}

void ChartVisualizer::reset()
{

}

QIcon* ChartVisualizer::createIcon()
{
    return new QIcon( QString::fromUtf8(":/resources/icons/charts.png") );
}

int ChartVisualizer::getMaxDataSeries() const
{
    return -1;
}

ChartVisualizer::ChartVisualizerSerie* ChartVisualizer::createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name)
{
    //generuj kolor dla serii
    osg::Vec4 color;
    color.r() = (rand()%1000)/999.0f;
    color.g() = (rand()%1000)/999.0f;
    color.b() = (rand()%1000)/999.0f;
    color.a() = 1.0f; 

    bool noSerie = series.empty();

    LineChartSeriePtr lineChart = new LineChartSerie();
    ScalarChannelReaderInterfaceConstPtr d = data->get();

    lineChart->setData(d);
    lineChart->setColor(color);
    chart->addSerie(lineChart);

    ChartVisualizerSerie* serie = new ChartVisualizerSerie(lineChart, this);
    serie->setName(name);
    
    if(noSerie == true){
        activeSerieCombo->blockSignals(true);
        activeSerieCombo->clear();
        activeSerieCombo->addItem(name.c_str());
        activeSerieCombo->setCurrentIndex(0);
        activeSerieCombo->blockSignals(false);
    }else{
        activeSerieCombo->addItem(name.c_str());
    }

    series.push_back(serie);

    activeSerieCombo->setEnabled(true);
    return serie;
}

ChartVisualizer::ChartVisualizerSerie* ChartVisualizer::createSerie(const SerieBase * serie)
{
    return nullptr;
}

void ChartVisualizer::removeSerie(core::IVisualizer::SerieBase* serie)
{
    ChartVisualizerSerie* chSerie = dynamic_cast<ChartVisualizerSerie*>(serie);

    if(chSerie == nullptr){
        throw std::runtime_error("Serie type not generated by this Visualizer!");
    }

    auto it = std::find(series.begin(), series.end(), chSerie);
    if(it == series.end()){
        throw std::runtime_error("Given serie dos not belong to this visualizer!");
    }

    activeSerieCombo->blockSignals(true);

    activeSerieCombo->removeItem(chart->getSerieIndex(chSerie->getSerie()));

    chart->removeSerie(chSerie->getSerie());

    series.erase(it);

    if(series.empty() == true){
        activeSerieCombo->addItem(QString::fromUtf8("No active serie"));
        activeSerieCombo->setCurrentIndex(0);
        activeSerieCombo->setEnabled(false);
    }else{
        activeSerieCombo->setCurrentIndex(chart->getActiveSerieIndex());
    }

    activeSerieCombo->blockSignals(false);
}

void ChartVisualizer::setNormalized( bool normalized )
{
    if ( sender() != actionNormalized ) {
        actionNormalized->setChecked(normalized);
    } else {
        chart->setNormalized(normalized);
        viewer->update();
    }
}

void ChartVisualizer::setActiveSerie( int idx )
{
    chart->setActiveSerie(idx);
}

float ChartVisualizer::getMargin() const
{
    return chart->getMargin();
}

void ChartVisualizer::setMargin(float margin)
{
    chart->setMargin(margin);
}

bool ChartVisualizer::isShowingFrame() const
{
    return chart->isShowingFrame();
}

void ChartVisualizer::setShowFrame(bool showFrame)
{
    chart->setShowFrame(showFrame);
}

bool ChartVisualizer::isShowingGridX() const
{
    return chart->isShowingGridX();
}

void ChartVisualizer::setShowGridX(bool showGridX)
{
    chart->setShowGridX(showGridX);
}

bool ChartVisualizer::isShowingGridY() const
{
    return chart->isShowingGridY();
}

void ChartVisualizer::setShowGridY(bool showGridY)
{
    chart->setShowGridY(showGridY);
}

int ChartVisualizer::getGridDensity() const
{
    return chart->getGridDensity();
}

void ChartVisualizer::setGridDensity(int gridDensity)
{
    chart->setGridDensity(gridDensity);
}

osg::Vec4 ChartVisualizer::getFrameColor() const
{
    return chart->getFrameColor();
}

void ChartVisualizer::setFrameColor(osg::Vec4 frameColor)
{
    chart->setFrameColor(frameColor);
}

osg::Vec4 ChartVisualizer::getGridColor() const
{
    return chart->getGridColor();
}

void ChartVisualizer::setGridColor(osg::Vec4 gridColor)
{
    chart->setGridColor(gridColor);
}

float ChartVisualizer::getFrameWidth() const
{
    return chart->getFrameWidth();
}

void ChartVisualizer::setFrameWidth(float frameWidth)
{
    chart->setFrameWidth(frameWidth);
}

float ChartVisualizer::getGridDashWidth() const
{
    return chart->getGridDashWidth();
}

void ChartVisualizer::setGridDashWidth(float dashWidth)
{
    chart->setGridDashWidth(dashWidth);
}

bool ChartVisualizer::isShowingAxisX() const
{
    return chart->isShowingAxisX();
}

void ChartVisualizer::setShowAxisX(bool showAxisX)
{
    chart->setShowAxisX(showAxisX);
}

bool ChartVisualizer::isAxisXInside() const
{
    return chart->isAxisXInside();
}

void ChartVisualizer::setAxisXInside(bool axisXInside)
{
    chart->setAxisXInside(axisXInside);
}

bool ChartVisualizer::isShowingAxisY() const
{
    return chart->isShowingAxisY();
}

void ChartVisualizer::setShowAxisY(bool showAxisY)
{
    chart->setShowAxisY(showAxisY);
}

bool ChartVisualizer::isAxisYInside() const
{
    return chart->isAxisYInside();
}

void ChartVisualizer::setAxisYInside(bool axisYInside)
{
    chart->setAxisYInside(axisYInside);
}

osg::Vec4 ChartVisualizer::getAxisesColor() const
{
    return chart->getAxisesColor();
}

void ChartVisualizer::setAxisesColor(osg::Vec4 color)
{
    chart->setAxisesColor(color);
}

osg::Vec4 ChartVisualizer::getCursorColor() const
{
    return chart->getCursorColor();
}

void ChartVisualizer::setCursorColor(osg::Vec4 color)
{
    chart->setCursorColor(color);
}

const osgText::Text* ChartVisualizer::getLabelPrototype() const
{
    return chart->getLabelPrototype();
}

void ChartVisualizer::setLabelPrototype(osgText::Text* prototype)
{
    chart->setLabelPrototype(prototype);
}

void ChartVisualizer::setBackgroundColor(osg::Vec4 color)
{
    viewer->getCamera()->setClearColor(color);
}

osg::Vec4 ChartVisualizer::setBackgroundColor() const
{
    return viewer->getCamera()->getClearColor();
}

bool ChartVisualizer::isShowingTitle() const
{
    return chart->isShowingTitle();
}

void ChartVisualizer::setShowTitle(bool showTitle)
{
    chart->setShowTitle(showTitle);
}

std::string ChartVisualizer::getTitleText() const
{
    return chart->getTitleText();
}

void ChartVisualizer::setTitleText(const std::string & titleText)
{
    chart->setTitleText(titleText);
}

int ChartVisualizer::getTitleTextSize() const
{
    return chart->getTitleTextSize();
}

void ChartVisualizer::setTitleTextSize(int titleTextSize)
{
    chart->setTitleTextSize(titleTextSize);
}

Chart::TitlePosition ChartVisualizer::getTitlePosition() const
{
    return chart->getTitlePosition();
}

void ChartVisualizer::setTitlePosition(Chart::TitlePosition position)
{
    chart->setTitlePosition(position);
}

bool ChartVisualizer::isShowingXUnitsSeparately() const
{
    return chart->isShowingXUnitsSeparately();
}

void ChartVisualizer::setShowingXUnitsSeparately(bool unitsSeparate)
{
    chart->setShowingXUnitsSeparately(unitsSeparate);
}

bool ChartVisualizer::isShowingYUnitsSeparately() const
{
    return chart->isShowingYUnitsSeparately();
}

void ChartVisualizer::setShowingYUnitsSeparately(bool unitsSeparate)
{
    chart->setShowingYUnitsSeparately(unitsSeparate);
}

bool ChartVisualizer::getAutoRefresh() const
{
    return chart->getAutoRefresh();
}

void ChartVisualizer::setAutoRefresh(bool autoRefresh)
{
    chart->setAutoRefresh(autoRefresh);
}

bool ChartVisualizer::isShowingXUnits() const
{
    return chart->isShowingXUnits();
}

void ChartVisualizer::setShowingXUnits(bool showUnits)
{
    chart->setShowingXUnits(showUnits);
}

bool ChartVisualizer::isShowingYUnits() const
{
    return chart->isShowingYUnits();
}

void ChartVisualizer::setShowingYUnits(bool showUnits)
{
    chart->setShowingYUnits(showUnits);
}

bool ChartVisualizer::isShowingTimeInCursor() const
{
    return chart->isShowingTimeInCursor();
}

void ChartVisualizer::setShowingTimeInCursor(bool showTime)
{
    chart->setShowingTimeInCursor(showTime);
}

bool ChartVisualizer::isShowingUnitsInCursor() const
{
    return chart->isShowingUnitsInCursor();
}

void ChartVisualizer::setShowingUnitsInCursor(bool showUnits)
{
    chart->setShowingUnitsInCursor(showUnits);
}

bool ChartVisualizer::isShowingCursorBackground() const
{
    return chart->isShowingCursorBackground();
}

void ChartVisualizer::setShowCursorBackground(bool showBackground)
{
    chart->setShowCursorBackground(showBackground);
}

osg::Vec4 ChartVisualizer::getCursorBackgroundColor() const
{
    return chart->getCursorBackgroundColor();
}

void ChartVisualizer::setCursorBackgroundColor(osg::Vec4 color)
{
    chart->setCursorBackgroundColor(color);
}
