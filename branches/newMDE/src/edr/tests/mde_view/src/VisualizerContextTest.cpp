#include "VisualizerContextTest.h"
#include "..\src\mde_view\src\ContextEventFilter.h"
#include <qtest.h>
#include <qtestmouse.h>
#include "../src/mde_view/src/MdeMainWindow.h"
#include "../src/mde_view/src/MdeContexts.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( VisualizerContextTest );

void VisualizerContextTest::testEventFilter()
{
    MdeMainWindow mmw([](){});

    QWidget w;
    IAppUsageContextPtr context = utils::make_shared<MdeEmptyContext>();
    mmw.addContext(context);
    mmw.addWidgetToContext(context, &w);
    ContextEventFilterPtr cef = utils::make_shared<ContextEventFilter>(&mmw);
    cef->registerClosableContextWidget(&w);
    VisualizerEventFilter vef(cef);
    ConnectionTester ct;
    QObject::connect(&vef, SIGNAL(focusOn(QWidget*)), &ct, SLOT(onFocus(QWidget*)));
    w.installEventFilter(&vef);
    QTest::mouseClick ( &w, Qt::MouseButton::LeftButton);

    CPPUNIT_ASSERT(ct.getWidget() == &w);
    CPPUNIT_ASSERT(mmw.isCurrentContextWidget(&w));
    CPPUNIT_ASSERT(mmw.getCurrentContextWidget() == &w);
}

void VisualizerContextTest::setUp()
{
    char** argv = nullptr;
    int arc = 0;
    app = new QApplication(arc, argv);
}

void VisualizerContextTest::tearDown()
{
    delete app;
}


