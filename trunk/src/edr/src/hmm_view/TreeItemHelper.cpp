#include "hmmPCH.h"
#include "TreeItemHelper.h"
#include "VisualizerManager.h"
#include <plugins/newChart/NewChartVisualizer.h>
osg::ref_ptr<osgText::Text> TreeItemHelper::chartTextPrototype;

//TreeItemHelper::TreeItemHelper( const core::ObjectWrapperPtr& wrapper ) :
//    wrapper(wrapper)
//{
//
//}

VisualizerPtr TreeWrappedItemHelper::createVisualizer()
{
    UTILS_ASSERT(wrapper, "Item should be initialized");
    return VisualizerManager::getInstance()->createVisualizer(wrapper->getTypeInfo());
}

void TreeWrappedItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    UTILS_ASSERT(wrapper, "Item should be initialized");
    auto serie = visualizer->createSerie(wrapper, path.toStdString());
    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serie));
}

void TreeItemHelper::setUpChart( ChartVisualizer* chart, const std::string& title )
{
    if (!chartTextPrototype) {
        // prototyp textu dla Charta

        chartTextPrototype = new osgText::Text();

        chartTextPrototype->setColor(osg::Vec4(0, 0, 0, 1));
        chartTextPrototype->setFontResolution(50,50);
        chartTextPrototype->setFont(osgText::readFontFile("fonts/arial.ttf"));    
        chartTextPrototype->setCharacterSize(12);
        chartTextPrototype->setCharacterSizeMode(osgText::TextBase::OBJECT_COORDS);
        chartTextPrototype->setAxisAlignment(osgText::Text::USER_DEFINED_ROTATION );
        chartTextPrototype->setAutoRotateToScreen(false);
        chartTextPrototype->setAlignment(osgText::Text::CENTER_CENTER);
        chartTextPrototype->setText("");
        chartTextPrototype->setLayout(osgText::Text::LEFT_TO_RIGHT);
    }
    chart->setAutoRefresh(false);

    chart->setLabelPrototype(chartTextPrototype);

    chart->setBackgroundColor(osg::Vec4(1,1,1,1));
    chart->setCursorColor(osg::Vec4(0,0,0,1));
    chart->setAxisesColor(osg::Vec4(0,0,0,1));
    chart->setFrameColor(osg::Vec4(0,0,0,1));
    chart->setShowFrame(true);

    chart->setGridColor(osg::Vec4(0.2,0.2,0.2,1));

    chart->setShowGridX(true);
    chart->setShowGridY(true);

    chart->setMargin(5);

    chart->setAxisXInside(false);
    chart->setAxisYInside(false);

    chart->setShowAxisX(true);
    chart->setShowAxisY(true);

    chart->setShowingXUnits(true);
    chart->setShowingYUnits(false);

    chart->setShowingXUnitsSeparately(true);
    chart->setShowingYUnitsSeparately(true);

    chart->setShowingTimeInCursor(true);
    chart->setShowingUnitsInCursor(true);

    chart->setShowCursorBackground(true);
    chart->setCursorBackgroundColor(osg::Vec4(1,1,1,0.95));

    chart->setShowTitle(true);

    
    chart->setTitleText(title);
    chart->setTitleTextSize(chartTextPrototype->getCharacterHeight());
    chart->setTitlePosition(Chart::TOP_GRID_CENTER);

    chart->setAutoRefresh(true);
}


VisualizerPtr ChartItemHelper::createVisualizer()
{
    VisualizerPtr visualizer = VisualizerManager::getInstance()->createVisualizer(ChartVisualizer::getClassID());
    visualizer->getOrCreateWidget();
    ChartVisualizer* chart = dynamic_cast<ChartVisualizer*>(visualizer->getImplementation());
    if (!chart) {
        UTILS_ASSERT(false);
        LOG_ERROR("Wrong visualizer type!");
    } else {
        std::string title;
        ScalarChannelReaderInterfacePtr scalar = wrapper->get();
        title += scalar->getName();
        title += " [";
        title += scalar->getValueBaseUnit();
        title += "]";
        setUpChart(chart, title);
    }

    return visualizer;
}


void ChartItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    auto serie = visualizer->createSerie(wrapper, path.toStdString());
    ScalarChannelReaderInterfaceConstPtr item = wrapper->get();
    QString text = item->getName().c_str();
    ChartVisualizer::ChartVisualizerSerie* chartSerie = dynamic_cast<ChartVisualizer::ChartVisualizerSerie*>(serie.get());
    if (chartSerie) {
        if(text.at(0) == 'L' || text.at(0) == 'l' || text.at(1) == 'X' || text.at(1) == 'x'){
            chartSerie->setColor(osg::Vec4(1,0,0,1));
        }else if(text.at(0) == 'R' || text.at(0) == 'r' || text.at(1) == 'Y' || text.at(1) == 'y'){
            chartSerie->setColor(osg::Vec4(0,1,0,1));
        }else{
            chartSerie->setColor(osg::Vec4(0.5,0,1,1));
        }
    } else {
        UTILS_ASSERT(false);
        LOG_ERROR("Wrong chart serie");
    }
    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serie));
}

VisualizerPtr Vector3ItemHelper::createVisualizer()
{
    VisualizerPtr visualizer = VisualizerManager::getInstance()->createVisualizer(ChartVisualizer::getClassID());
    visualizer->getOrCreateWidget();
    ChartVisualizer* chart = dynamic_cast<ChartVisualizer*>(visualizer->getImplementation());
    if (!chart) {
        UTILS_ASSERT(false);
        LOG_ERROR("Wrong visualizer type!");
    } else {
        VectorChannelPtr vector = wrapper->get();
        std::string title;
        title += vector->getName();
        title += " [";
        title += vector->getValueBaseUnit();
        title += "]";
        setUpChart(chart, title);
    }
    return visualizer;
}

void Vector3ItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    VectorChannelPtr vectorChannel = wrapper->get();

    ScalarChannelReaderInterfacePtr x(new VectorToScalarAdaptor(vectorChannel, 0));
    ScalarChannelReaderInterfacePtr y(new VectorToScalarAdaptor(vectorChannel, 1));
    ScalarChannelReaderInterfacePtr z(new VectorToScalarAdaptor(vectorChannel, 2));
    core::ObjectWrapperPtr wrapperX = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
    core::ObjectWrapperPtr wrapperY = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
    core::ObjectWrapperPtr wrapperZ = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
    wrapperX->set(x);
    wrapperY->set(y);
    wrapperZ->set(z);

    static int number = 0;
    // hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
    std::string suffix = boost::lexical_cast<std::string>(number++);
    wrapperX->setName  ("X_" + suffix);
    wrapperX->setSource("X_" + suffix);
    wrapperY->setName  ("Y_" + suffix);
    wrapperY->setSource("Y_" + suffix);
    wrapperZ->setName  ("Z_" + suffix);
    wrapperZ->setSource("Z_" + suffix);
    visualizer->getOrCreateWidget();

    auto serieX = visualizer->createSerie(wrapperX, wrapperX->getSource());
    auto serieY = visualizer->createSerie(wrapperY, wrapperY->getSource());
    auto serieZ = visualizer->createSerie(wrapperZ, wrapperZ->getSource());

    ChartVisualizer::ChartVisualizerSerie* chartSerieX = dynamic_cast<ChartVisualizer::ChartVisualizerSerie*>(serieX.get());
    ChartVisualizer::ChartVisualizerSerie* chartSerieY = dynamic_cast<ChartVisualizer::ChartVisualizerSerie*>(serieY.get());
    ChartVisualizer::ChartVisualizerSerie* chartSerieZ = dynamic_cast<ChartVisualizer::ChartVisualizerSerie*>(serieZ.get());

    chartSerieX->setColor(osg::Vec4(1,0,0,1));
    chartSerieY->setColor(osg::Vec4(0,1,0,1));
    chartSerieZ->setColor(osg::Vec4(0,0,1,1));

    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieX));
    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieY));
    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieZ));
}

void MultiserieHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    int count = wrappers.size();
    int count2 = count / 2;
    for (int i = 0; i < count; i++) {
        core::ObjectWrapperConstPtr wrapper = wrappers[i];
        auto serieX = visualizer->createSerie(wrapper, wrapper->getSource());
        ChartVisualizer::ChartVisualizerSerie* chartSerieX = dynamic_cast<ChartVisualizer::ChartVisualizerSerie*>(serieX.get());
        float r = 0;
        float g = 0;
        float b = 0;

        switch (colorPolicy) 
        {
        case Random:
            r = static_cast<float>(rand()) / RAND_MAX;
            g = static_cast<float>(rand()) / RAND_MAX;
            b = static_cast<float>(rand()) / RAND_MAX;
            break;

        case GreenRandom:
            r = (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
            g = 255;                                 
            b = (static_cast<float>(rand()) / RAND_MAX) * 0.99f;
            break;                                    
                                                      
        case RedRandom:                               
            r = 255;                                  
            g = (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
            b = (static_cast<float>(rand()) / RAND_MAX) * 0.99f;
            break;

        case Red:
            r = 255;                                  
            g = 0;
            b = 0;
            break;
        case Green:
            r = 0;
            g = 255;
            b = 0;
            break;
        case HalfRedHalfGreen:
            r = i >= count2 ? 255 : 0;
            g = i >= count2 ? 0 : 255;
            b = i >= count2 ? 0 : 0;
            break;
        }

        chartSerieX->setColor(osg::Vec4(r, g, b, 1.0f));
        series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieX));
    }
}


VisualizerPtr MultiserieHelper::createVisualizer()
{
    VisualizerPtr visualizer = VisualizerManager::getInstance()->createVisualizer(ChartVisualizer::getClassID());
    visualizer->getOrCreateWidget();
    ChartVisualizer* chart = dynamic_cast<ChartVisualizer*>(visualizer->getImplementation());
    if (!chart) {
        UTILS_ASSERT(false);
        LOG_ERROR("Wrong visualizer type!");
    } else {
        std::string title = "Forces - X [N]";
        setUpChart(chart, title);
    }

    return visualizer;
}
//
//Multiserie3D::Multiserie3D(MotionPtr motion) :
//    motion(motion)
//{
//    UTILS_ASSERT(motion);
//}

void Multiserie3D::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    core::ObjectWrapperConstPtr m = motion->getWrapperOfType(typeid(MarkerCollection));
    core::ObjectWrapperConstPtr j = motion->getWrapperOfType(typeid(kinematic::JointAnglesCollection));
    core::ObjectWrapperConstPtr g = motion->getWrapperOfType(typeid(GRFCollection));

    if (!m->isNull()) {
        series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(visualizer->createSerie(m ,path.toStdString())));
    }                                            
    if (!j->isNull()) {                          
        series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(visualizer->createSerie(j ,path.toStdString())));
    }                                            
    if (!g->isNull()) {                          
        series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(visualizer->createSerie(g ,path.toStdString())));
    }
}

VisualizerPtr Multiserie3D::createVisualizer()
{
    return VisualizerManager::getInstance()->createVisualizer(typeid(kinematic::JointAnglesCollection));                           
}

VisualizerPtr JointsItemHelper::createVisualizer()
{
    return VisualizerManager::getInstance()->createVisualizer(typeid(kinematic::JointAnglesCollection));
}

void JointsItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    core::ObjectWrapperConstPtr joints = motion->getWrapperOfType(typeid(kinematic::JointAnglesCollection));
    if (joints && !joints->isNull()) {
        series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(visualizer->createSerie(joints, path.toStdString())));
    } else {
        LOG_ERROR("Empty object - joints");
        UTILS_ASSERT(false);
    }
}

VisualizerPtr NewChartItemHelper::createVisualizer()
{
    VisualizerPtr visualizer = VisualizerManager::getInstance()->createVisualizer(NewChartVisualizer::getClassID());
    visualizer->getOrCreateWidget();
    NewChartVisualizer* chart = dynamic_cast<NewChartVisualizer*>(visualizer->getImplementation());
    if (!chart) {
        UTILS_ASSERT(false);
        LOG_ERROR("Wrong visualizer type!");
    } else {
        std::string title;
        ScalarChannelReaderInterfacePtr scalar = wrapper->get();
        title += scalar->getName();
        title += " [";
        title += scalar->getValueBaseUnit();
        title += "]";
        chart->setTitle(QString(title.c_str()));
    }

    return visualizer;
}


void NewChartItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    auto serie = visualizer->createSerie(wrapper, path.toStdString());
    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serie));
}

VisualizerPtr NewVector3ItemHelper::createVisualizer()
{
    VisualizerPtr visualizer = VisualizerManager::getInstance()->createVisualizer(NewChartVisualizer::getClassID());
    visualizer->getOrCreateWidget();
    NewChartVisualizer* chart = dynamic_cast<NewChartVisualizer*>(visualizer->getImplementation());
    if (!chart) {
        UTILS_ASSERT(false);
        LOG_ERROR("Wrong visualizer type!");
    } else {
        std::string title;
        VectorChannelPtr vectorChannel = wrapper->get();
        title += vectorChannel->getName();
        title += " [";
        title += vectorChannel->getValueBaseUnit();
        title += "]";
        chart->setTitle(QString(title.c_str()));
    }
    return visualizer;
}

void NewVector3ItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    VectorChannelConstPtr vectorChannel = wrapper->get();

    ScalarChannelReaderInterfacePtr x(new VectorToScalarAdaptor(vectorChannel, 0));
    ScalarChannelReaderInterfacePtr y(new VectorToScalarAdaptor(vectorChannel, 1));
    ScalarChannelReaderInterfacePtr z(new VectorToScalarAdaptor(vectorChannel, 2));
    core::ObjectWrapperPtr wrapperX = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
    core::ObjectWrapperPtr wrapperY = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
    core::ObjectWrapperPtr wrapperZ = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
    wrapperX->set(x);
    wrapperY->set(y);
    wrapperZ->set(z);

    static int number = 0;
    // hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
    std::string suffix = boost::lexical_cast<std::string>(number++);
    wrapperX->setName  ("X_" + suffix);
    wrapperX->setSource("X_" + suffix);
    wrapperY->setName  ("Y_" + suffix);
    wrapperY->setSource("Y_" + suffix);
    wrapperZ->setName  ("Z_" + suffix);
    wrapperZ->setSource("Z_" + suffix);
    visualizer->getOrCreateWidget();

    auto serieX = visualizer->createSerie(wrapperX, wrapperX->getSource());
    auto serieY = visualizer->createSerie(wrapperY, wrapperY->getSource());
    auto serieZ = visualizer->createSerie(wrapperZ, wrapperZ->getSource());

    NewChartVisualizer::NewChartSerie* chartSerieX = dynamic_cast<NewChartVisualizer::NewChartSerie*>(serieX.get());
    NewChartVisualizer::NewChartSerie* chartSerieY = dynamic_cast<NewChartVisualizer::NewChartSerie*>(serieY.get());
    NewChartVisualizer::NewChartSerie* chartSerieZ = dynamic_cast<NewChartVisualizer::NewChartSerie*>(serieZ.get());

    chartSerieX->setColor(255, 0, 0);
    chartSerieY->setColor(0, 255, 0);
    chartSerieZ->setColor(0, 0, 255);

    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieX));
    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieY));
    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieZ));
}



void NewMultiserieHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    int count = wrappers.size();
    int count2 = count / 2;
    for (int i = 0; i < count; i++) {
        core::ObjectWrapperConstPtr wrapper = wrappers[i];
        auto serieX = visualizer->createSerie(wrapper, wrapper->getSource());
        NewChartVisualizer::NewChartSerie* chartSerieX = dynamic_cast<NewChartVisualizer::NewChartSerie*>(serieX.get());
        int r = 0;
        int g = 0;
        int b = 0;

        switch (colorPolicy) 
        {
        case Random:
            r = rand() % 256;
            g = rand() % 256;
            b = rand() % 256;
            break;

        case GreenRandom:
            r = rand() % 200;
            g = 255;                                 
            b = rand() % 245;
            break;                                    

        case RedRandom:                               
            r = 255;                                  
            g = rand() % 200;
            b = rand() % 245;
            break;

        case Red:
            r = 255;                                  
            g = 0;
            b = 0;
            break;
        case Green:
            r = 0;
            g = 255;
            b = 0;
            break;
        case HalfRedHalfGreen:
            r = i >= count2 ? 255 : 0;
            g = i >= count2 ? 0 : 255;
            b = i >= count2 ? 0 : 0;
            break;
        }

        chartSerieX->setColor(r, g, b);
        series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieX));
    }
}


VisualizerPtr NewMultiserieHelper::createVisualizer()
{
    VisualizerPtr visualizer = VisualizerManager::getInstance()->createVisualizer(NewChartVisualizer::getClassID());
    visualizer->getOrCreateWidget();
    NewChartVisualizer* chart = dynamic_cast<NewChartVisualizer*>(visualizer->getImplementation());
    if (!chart) {
        UTILS_ASSERT(false);
        LOG_ERROR("Wrong visualizer type!");
    } else {
        chart->setTitle(title);
        chart->setShowLegend(false);
    }
    return visualizer;
}