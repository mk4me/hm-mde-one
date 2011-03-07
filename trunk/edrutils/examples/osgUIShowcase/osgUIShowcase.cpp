#include <iostream>

#include <boost/foreach.hpp>

#include <QtGui/QApplication>

#include <osgViewer/ViewerEventHandlers>
#include <osgWidget/WindowManager>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/Label>
#include <osgGA/StateSetManipulator>

#include <osgUI/QOsgWidgets.h>
#include <osgUI/QOsgWidgetsDeprecated.h>
#include <osgUI/Grid.h>
#include <osgUI/Borderized.h>
#include <osgUI/Buttonized.h>
#include <osgUI/AspectRatioKeeper.h>
#include <osgUI/Grid.h>
#include <osgUI/EmbeddedWindow.h>

using namespace osgUI;

bool callbackWindowResize(osgWidget::Event& ev) 
{
    if(!ev.getWindow() || !ev.getWindowManager()->isRightMouseButtonDown()) {
        return false;
    }
    ev.getWindow()->resizeAdd(ev.x, ev.y);
    ev.getWindow()->update();
    return true;
}

bool callbackWindowMove(osgWidget::Event& ev) 
{
    if(!ev.getWindow() || !ev.getWindowManager()->isLeftMouseButtonDown()) {
        return false;
    }
    ev.getWindow()->addOrigin(ev.x, ev.y);
    ev.getWindow()->update();
    ev.getWindow()->resizeAdd();
    return true;
}


void embeddSample(osgWidget::Widget* widget, osgWidget::WindowManager* wm, int x, int y)
{
    using namespace osgWidget;
    Box* box = new Box(std::string(widget->getName()) + "Box", Box::VERTICAL);

    box->getBackground()->setColor(0.8,0.8,0.8,0.5);
    box->setOrigin(x, y);
    //box->attachMoveCallback();
    box->addCallback(new Callback(&::callbackWindowResize, EVENT_MOUSE_DRAG));
    box->addCallback(new Callback(&::callbackWindowMove, EVENT_MOUSE_DRAG));

    Label* title = new Label(std::string(widget->getName()) + "Title");
    title->setFontSize(14);
    title->setCanFill(false);
    
    //title->setColor(0.8,0.8,0.8,1);
    title->setFontColor(0,0,0,1);
    title->setFont("fonts/arial.ttf");
    title->setLabel(widget->getName());

    widget->setCanFill(true);
    //widget->setPadding(2);

    box->addWidget(widget);
    box->addWidget(title);

    wm->addChild(box);
}

void createExample(osgViewer::View& view, int w, int h)
{
    using namespace osgWidget;

    // dodanie korzenia sceny
    WindowManager* wm = new WindowManager(&view, static_cast<point_type>(w), static_cast<point_type>(h), 0xF0000000, WindowManager::WM_PICK_DEBUG);
    view.addEventHandler( new osgViewer::StatsHandler());
    view.addEventHandler( new osgWidget::MouseHandler(wm) );
    view.addEventHandler( new osgWidget::KeyboardHandler(wm) );
    view.addEventHandler( new osgWidget::ResizeHandler(wm, view.getCamera()) );
    view.addEventHandler( new osgGA::StateSetManipulator( view.getCamera()->getOrCreateStateSet() ) );
    view.setSceneData(wm);

    {
        std::string lines[] = {
            "lmb + drag - move window",
            "rmb + drag - resize window"
        };
        Box* infoBox = new Box("infoBox", Box::VERTICAL);
        infoBox->getBackground()->setColor(0, 0, 0, 0);

        wm->addChild( infoBox );
        osg::ref_ptr<Label> linePrototype = new Label("label", "label");
        linePrototype->setFont("fonts/arial.ttf");
        linePrototype->setFontSize(14);
        linePrototype->setAlignHorizontal( Widget::HA_LEFT );
        linePrototype->setPadLeft( 1 );
        linePrototype->setPadRight( 1 );
        linePrototype->setCanFill(true);
        linePrototype->setLayer( Widget::LAYER_TOP );
        linePrototype->setColor(0,0,0,0);
        BOOST_REVERSE_FOREACH(std::string& line, lines) {
            osgWidget::Label* label = osg::clone(linePrototype.get(), line, osg::CopyOp::DEEP_COPY_ALL);
            label->setFont("fonts/arial.ttf");
            label->setFontSize(14);
            label->setLabel(line);
            infoBox->addWidget(label);
        }
        EmbeddedWindow* widget = EmbeddedWindow::embed(infoBox, "Usage");
        
        embeddSample(widget, wm, 400, 400);
    }


    {
        // borderized
        Borderized<Widget>* borderized = new Borderized<Widget>("Borderized<Widget>", 100, 100);
        borderized->setBorderWidth(5);
        borderized->setColor(1, 0, 0, 1);
        borderized->setBorderColor(1, 1, 1, 1);
        embeddSample(borderized, wm, 0, 0);
    }

    {
        // borderized
        auto buttonized = new Buttonized<Borderized<Label>, BorderStylePolicy<LabelStylePolicy> >("Buttonized<Borderized<Label>>");
        buttonized->setFontSize(14);
        buttonized->setFont("fonts/arial.ttf");
        buttonized->setLabel(buttonized->getName());
        buttonized->setSize(100, 100);
        buttonized->setNormalColor(Color(0, 0, 0, 1));
        buttonized->setPushedColor(Color(0, 1, 0, 1));
        buttonized->setHoverColor(Color(0, 0, 1, 1));
        buttonized->setToggleColor(Color(0, 1, 1, 1));
        buttonized->setLabelNormalColor(Color(1, 0, 0, 1));
        buttonized->setLabelPushedColor(Color(1, 1, 0, 1));
        buttonized->setLabelHoverColor(Color(1, 0, 1, 1));
        buttonized->setLabelToggleColor(Color(1, 1, 1, 1));
        buttonized->setBorderWidth(5);
        buttonized->setBorderNormalColor(Color(0, 1, 0, 1));
        buttonized->setBorderPushedColor(Color(0, 1, 1, 1));
        buttonized->setBorderHoverColor(Color(1, 1, 0, 1));
        buttonized->setBorderToggleColor(Color(1, 1, 1, 1));
        buttonized->setToggleEnabled(true);
        buttonized->setEventMask( EVENT_MASK_MOUSE_DRAG );
        embeddSample(buttonized, wm, 100, 100);
    }

    {
        // aspectratiokeeper
        Widget* widget = new Widget("AspectRatioKeeper, ratio=2", 100, 50);
        widget->setColor(0, 1, 0, 1);
        widget->setCanFill(false);
        AspectRatioKeeper* keeper = new AspectRatioKeeper(widget, 2);
        keeper->setSize(100, 100);
        embeddSample(keeper, wm, 200, 200);
    }
    
    {
        Grid* grid = new Grid("Grid 3x3");
        grid->setNumRows(3);
        grid->setNumColumns(3);

        grid->addWidget(new Widget("0, 0", 33.0f, 33.0f), 0, 0);
        grid->addWidget(new Widget("0, 1", 33.0f, 33.0f), 0, 1);
        grid->addWidget(new Widget("0, 2", 33.0f, 33.0f), 0, 2);

        grid->addWidget(new Widget("1, 0", 33.0f, 33.0f), 1, 0);
        grid->addWidget(new Widget("1, 1", 33.0f, 33.0f), 1, 1);
        grid->addWidget(new Widget("1, 2", 33.0f, 33.0f), 1, 2);

        grid->addWidget(new Widget("2, 0", 33.0f, 33.0f), 2, 0);
        grid->addWidget(new Widget("2, 1", 33.0f, 33.0f), 2, 1);
        grid->addWidget(new Widget("2, 2", 33.0f, 33.0f), 2, 2);

        // Use a hackish method of setting the spacing for all Widgets.
        color_type delta = 1.0f / (grid->getNumColumns() * grid->getNumRows());
        color_type r = 0;
        for(Table::Iterator i = grid->begin(); i != grid->end(); i++) {
            i->get()->setPadding(1.0f);
            i->get()->setCanFill(true);
            i->get()->setColor( r, 1-r, 0, 1 );
            i->get()->setMinimumSize(33.0f, 33.0f);
            r+=delta;
        }

        grid->setRowWeight(0, 0);
        grid->setRowWeight(1, 1);
        grid->setRowWeight(2, 4);

        grid->setColumnWeight(0, 0);
        grid->setColumnWeight(1, 1);
        grid->setColumnWeight(2, 4);

        embeddSample( EmbeddedWindow::embed(grid, "Grid 3x3") , wm, 300, 300);
    }

}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    // use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc,argv);

    // set up the usage document, in case we need to print out how to use this program.
    arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
    arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" example demonstrates the use of ImageStream for rendering movies as textures.");
    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options]");
    arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");
    arguments.getApplicationUsage()->addCommandLineOption("--nostandalone", "dont create standalone viewer");
    arguments.getApplicationUsage()->addCommandLineOption("--deprecated", "create deprecated viewer");
    //arguments.getApplicationUsage()->addCommandLineOption("--composite", "create composite viewer");
   
    // if user request help write it out to cout.
    if (arguments.read("-h") || arguments.read("--help")) {
        arguments.getApplicationUsage()->write(std::cout);
        return 1;
    }

    if ( arguments.read("--deprecated") ) {
        QOsgDeprecatedViewer* widget = new QOsgDeprecatedViewer();
        widget->setWindowTitle("QOsgDeprecatedViewer (standalone)");
        createExample(*widget, widget->width(), widget->height());
        widget->show();
    }
    
    if ( !arguments.read("--nostandalone") ) {
        QOsgViewer* widget = new QOsgViewer();
        widget->setWindowTitle("QOsgViewer (standalone)");
        createExample(*widget, widget->width(), widget->height());
        widget->show();
    }
    
    return application.exec();
}