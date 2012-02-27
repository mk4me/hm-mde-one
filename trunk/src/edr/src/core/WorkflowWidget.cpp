#include "CorePCH.h"
#include "WorkflowWidget.h"
#include "WorkflowVDFModel.h"
#include "EDRDataFlow.h"
#include <QtGui/QHBoxLayout>

#include <iomanip>
#include <core/PluginCommon.h>
#include "DataProcessorManager.h"
#include "DataSourceManager.h"
#include "VisualizerManager.h"
#include <boost/bind.hpp>
#include <QtGui/QMessageBox>
#include "EDRDFNode.h"
#include "EDRDFSourceNode.h"
#include "VisualizerWidget.h"
#include "EDRTitleBar.h"

#ifdef _DEBUG

#include <core/MultiViewWidgetItem.h>

#endif

WorkflowWidget::WorkflowWidget(WorkflowService* service)
    :   service(service)
{
    setupUi(this);

    workflowVDFWidget = new WorkflowCustomQOSGWidget(this);
    workflowVDFWidget->setMouseTracking(true);
    workflowVDFWidget->setFocusPolicy(Qt::StrongFocus);

    //dla lapania pozycji kursora na menu kontekstowe poniewa¿ z poziomu OSG tracê czêœciowo t¹ informacjê
    workflowVDFWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    const osg::GraphicsContext::Traits* traits = workflowVDFWidget->getCamera()->getGraphicsContext()->getTraits();

    workflowVDFModel = new WorkflowVDFModel(
        workflowVDFWidget,
        traits->width,
        traits->height,
        0xF0000000
#ifdef _DEBUG
        , osgWidget::WindowManager::WM_PICK_DEBUG
#endif
        );

    workflowVDFModel->setModel(dflm::MPtr(new EDRDataFlow()));

    connect(workflowVDFWidget, SIGNAL(customContextMenuRequested(QPoint)), dynamic_cast<QObject*>(workflowVDFModel.get()), SLOT(contextMenuRequestPosition(QPoint)));

    // stworzenie kamery
    osg::Camera* multiViewCamera = workflowVDFModel->createParentOrthoCamera();
    multiViewCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
    multiViewCamera->setRenderOrder(osg::Camera::POST_RENDER, 1);
    multiViewCamera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->addChild(multiViewCamera);
    workflowVDFWidget->setSceneData(root);

    //workflowVDFWidget->addEventHandler( new core::OsgSceneDumpFileHandler("dump_vdf.txt") );
    workflowVDFWidget->addEventHandler( new osgWidget::MouseHandler(workflowVDFModel) );
    workflowVDFWidget->addEventHandler( new osgWidget::ResizeHandler(workflowVDFModel, multiViewCamera) );

    workflowVDFWidget->addEventHandler( new osgGA::StateSetManipulator( workflowVDFWidget->getCamera()->getOrCreateStateSet() ) );

    // dodanie do widgeta
    verticalLayout->addWidget( workflowVDFWidget );

    StylesSet styleSet = generateRequiredStyles();

    for(StylesSet::iterator it = styleSet.begin(); it != styleSet.end(); it++){
        workflowVDFModel->getStyleManager()->addStyle(*it);
    }

    workflowVDFModel->showDefaultToolbar(false);

    //inicjuj Toolbar w Qt wraz z Drag`n`Drop
    DataProcessorManager* dp = DataProcessorManager::getInstance();
    for(int i = 0; i < dp->getNumPrototypes(); i++){
        //opakuj prototyp w fabrykê wêz³a dla EDRDataFlow
        


        //dodaj odpowiadaj¹cy element do toolbara
    }

#ifndef _DEBUG

    //dodajemy dodatkowy zwyk³y widget
    QWidget * retWidget = new QWidget();
    retWidget->setLayout(new QVBoxLayout());
    retWidget->layout()->addWidget(workflowVDFWidget);

#else

    //widget proxy przekierowujacy focusa do okienka osg
    QWidget * retWidget = new core::QOsgEncapsulatorWidget(workflowVDFWidget);
    //dodajemy tez event handler ze statystykami
    workflowVDFWidget->addEventHandler( new osgViewer::StatsHandler() );
    workflowVDFWidget->setTimerActive(true);

#endif

    retWidget->layout()->setContentsMargins(2,0,2,2);
    retWidget->setFocusPolicy(Qt::StrongFocus);
    workflowVDFWidget->setFocusPolicy(Qt::StrongFocus);
}

WorkflowWidget::~WorkflowWidget()
{

}

void WorkflowCustomQOSGWidget::tryAddNode()
{
    try{
        WorkflowItemPtr item(workflowWidget->actionsItemBuilders[workflowWidget->currentAction]());

        if(item == nullptr){
            return;
        }

        dflm::DFNPtr node;
        //obuduj wezel
        if(dynamic_cast<InputDescription*>(item.get()) == nullptr){
            //sprawdz czy wezel poprawnie zbudowany!!
            node.reset((EDRDFNode*)(new EDRDFSourceNode(item, item->getName())));
        }else{
            node.reset(new EDRDFNode(item, item->getName()));
        }              

        workflowWidget->workflowVDFModel->addNode(node, osg::ref_ptr<osg::Image>(), node->getName(), osgWidget::XYCoord(pos.x(), pos.y()));

        workflowWidget->currentAction->blockSignals(true);
        workflowWidget->currentAction->setChecked(false);
        workflowWidget->currentAction->blockSignals(false);
        workflowWidget->currentAction = nullptr;

    }catch(std::runtime_error e){
        LOG_ERROR("Error during node addition to workflow: " << e.what());
        std::string info("Could not add requested node to workflow: ");
        info += e.what();
        QMessageBox msgBox;
        msgBox.setText(info.c_str());
        msgBox.exec();
    }catch(std::invalid_argument e){
        LOG_ERROR("Error during node addition to workflow: " << e.what());
        std::string info("Could not add requested node to workflow: ");
        info += e.what();
        QMessageBox msgBox;
        msgBox.setText(info.c_str());
        msgBox.exec();
    }catch(...){
        LOG_ERROR("UNKNOWN error during node addition to workflow");
        QMessageBox msgBox;
        msgBox.setText("Could not add requested node to workflow. Unknown error");
        msgBox.exec();
    }
}

void WorkflowCustomQOSGWidget::mousePressEvent(QMouseEvent * event)
{
    if(event->button() == Qt::LeftButton){
        //probuj dodac element
        if(workflowWidget->currentAction != nullptr){

            pos = event->pos();
            //pos = mapToGlobal(event->pos());
            pos.setY(height() - pos.y());

            QMetaObject::invokeMethod(this, "tryAddNode");

            return;
        }
    }else{
        //prawy klawisz wiec moze chce usunac polaczenia
        emit customContextMenuRequested(event->pos());
    }

    osgui::QOsgDefaultWidget::mousePressEvent(event);
}

std::string WorkflowWidget::colorToStr(const osgWidget::Color & color)
{
    std::stringstream str;
    str << std::fixed << std::setprecision(5) << color.r() << " " << color.g() << " " <<color.b() << " " << color.a();
    return str.str();
}

WorkflowWidget::StylesSet WorkflowWidget::generateRequiredStyles()
{
    StylesSet styleSet;

    std::string path = core::getResourceString("icons\\vdf\\");
    std::string graphExt = "png";

    //PIN STYLES
    //czcionka pinow
    styleSet.insert(new osgui::StyleEx("vdf.node.pin.text", "font_size 10\nfont_type \"font\\segoeui.ttf\""));

    //piny wyjsciowe
    styleSet.insert(new osgui::StyleEx("vdf.pin.out.ok", "rect_image_cache \"" + path + "out_OK." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.out.active", "rect_image_cache \"" +  path + "out_active." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.out.connected", "rect_image_cache \"" +  path + "out_connected." + graphExt + "\""));
    //styleSet.insert(new StyleEx("vdf.pin.out.incomplete", "rect_image_cache \"" +  path + "out_incomplete." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.out.incomplete", "rect_image_cache \"" +  path + "out_required." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.out.incompatible", "rect_image_cache \"" +  path + "out_incompatible." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.out.required", "rect_image_cache \"" +  path + "out_required." + graphExt + "\""));

    //piny wejsciowe
    styleSet.insert(new osgui::StyleEx("vdf.pin.in.ok", "rect_image_cache \"" +  path + "in_OK." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.in.active", "rect_image_cache \"" +  path + "in_active." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.in.connected", "rect_image_cache \"" +  path + "in_connected." + graphExt + "\""));
    //styleSet.insert(new StyleEx("vdf.pin.in.incomplete", "rect_image_cache \"" +  path + "in_incomplete." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.in.incomplete", "rect_image_cache \"" +  path + "in_required." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.in.incompatible", "rect_image_cache \"" +  path + "in_incompatible." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.in.required", "rect_image_cache \"" +  path + "in_required." + graphExt + "\""));

    //NODE STYLES
    //obwódka, cieñ
    //normal
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TL.normal", "rect_image_cache \"" +  path + "normal_board_TL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TM.normal", "rect_image_cache \"" +  path + "normal_board_TM." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TR.normal", "rect_image_cache \"" +  path + "normal_board_TR." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.ML.normal", "rect_image_cache \"" +  path + "normal_board_ML." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.MR.normal", "rect_image_cache \"" +  path + "normal_board_MR." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BL.normal", "rect_image_cache \"" +  path + "normal_board_BL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BM.normal", "rect_image_cache \"" +  path + "normal_board_BM." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BR.normal", "rect_image_cache \"" +  path + "normal_board_BR." + graphExt + "\""));

    //kolizja
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TL.collision", "rect_image_cache \"" +  path + "collision_board_TL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TM.collision", "rect_image_cache \"" +  path + "collision_board_TM." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TR.collision", "rect_image_cache \"" +  path + "collision_board_TR." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.ML.collision", "rect_image_cache \"" +  path + "collision_board_ML." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.MR.collision", "rect_image_cache \"" +  path + "collision_board_MR." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BL.collision", "rect_image_cache \"" +  path + "collision_board_BL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BM.collision", "rect_image_cache \"" +  path + "collision_board_BM." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BR.collision", "rect_image_cache \"" +  path + "collision_board_BR." + graphExt + "\""));

    //select
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TL.active", "rect_image_cache \"" +  path + "selected_board_TL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TM.active", "rect_image_cache \"" +  path + "selected_board_TM." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TR.active", "rect_image_cache \"" +  path + "selected_board_TR." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.ML.active", "rect_image_cache \"" +  path + "selected_board_ML." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.MR.active", "rect_image_cache \"" +  path + "selected_board_MR." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BL.active", "rect_image_cache \"" +  path + "selected_board_BL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BM.active", "rect_image_cache \"" +  path + "selected_board_BM." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BR.active", "rect_image_cache \"" +  path + "selected_board_BR." + graphExt + "\""));


    //cia³o wêz³a
    //normal
    //top
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TL.normal", "rect_image_cache \"" +  path + "normal_body_TL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TR.normal", "rect_image_cache \"" +  path + "normal_body_TR." + graphExt + "\""));

    //get color
    osg::Image * imgA = osgDB::readImageFile(path + "normal_body_TL." + graphExt);
    osg::Image * imgB = osgDB::readImageFile(path + "normal_body_TR." + graphExt);
    osgWidget::Color col(1,1,1,1);
    if(imgA != 0 && imgB != 0){
        col = (imgA->getColor(imgA->s()-1,0,imgA->r()-1) + imgB->getColor(0,0,imgB->r()-1)) / 2.0;
    }

    styleSet.insert(new osgui::StyleEx("vdf.node.body.TNameLabel.normal", "color " + colorToStr(col) + "\nfont_size 13\nfont_type \"font\\segoeui.ttf\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TConfigBCG.normal", "color " + colorToStr(col)));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TDeleteBCG.normal", "color " + colorToStr(col)));

    styleSet.insert(new osgui::StyleEx("vdf.node.body.config.normal", "rect_image_cache \"" +  path + "normal_body_CFG." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.delete.normal", "rect_image_cache \"" +  path + "normal_body_DEL." + graphExt + "\""));

    //bottom
    styleSet.insert(new osgui::StyleEx("vdf.node.body.BL.normal", "rect_image_cache \"" +  path + "normal_body_BL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.BR.normal", "rect_image_cache \"" +  path + "normal_body_BR." + graphExt + "\""));

    imgA = osgDB::readImageFile(path + "normal_body_BL." + graphExt);
    imgB = osgDB::readImageFile(path + "normal_body_BR." + graphExt);
    col = osgWidget::Color(1,1,1,1);
    if(imgA != 0 && imgB != 0){
        col = (imgA->getColor(imgA->s()-1,imgA->t()-1,imgA->r()-1) + imgB->getColor(0,imgB->t()-1,imgB->r()-1)) / 2.0;
    }

    styleSet.insert(new osgui::StyleEx("vdf.node.body.BM.normal", "color " + colorToStr(col)));

    //center
    styleSet.insert(new osgui::StyleEx("vdf.node.body.Center.normal", "color " + colorToStr(col)));

    //selection
    //top
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TL.active", "rect_image_cache \"" +  path + "selected_body_TL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TR.active", "rect_image_cache \"" +  path + "selected_body_TR." + graphExt + "\""));

    //bottom
    styleSet.insert(new osgui::StyleEx("vdf.node.body.BL.active", "rect_image_cache \"" +  path + "selected_body_BL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.BR.active", "rect_image_cache \"" +  path + "selected_body_BR." + graphExt + "\""));

    //collision
    //top
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TL.collision", "rect_image_cache \"" +  path + "collision_body_TL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TR.collision", "rect_image_cache \"" +  path + "collision_body_TR." + graphExt + "\""));

    //bottom
    styleSet.insert(new osgui::StyleEx("vdf.node.body.BL.collision", "rect_image_cache \"" +  path + "collision_body_BL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.BR.collision", "rect_image_cache \"" +  path + "collision_body_BR." + graphExt + "\""));


    //TOOLBAR STYLES
    styleSet.insert(new osgui::StyleEx("osgui.toolbar.tab.normal", "color " + colorToStr(osgWidget::Color(0.357f,0.357f,0.357f,1.0f)) + "\npadding 0\nvalign center\nhalign center\nfont_size 13\nfont_type \"font\\segoeui.ttf\"\nfont_align center_center"));//border_color " + colorToStr(osgWidget::Color(0.0f,1.0f,1.0f,1.0f))));
    styleSet.insert(new osgui::StyleEx("osgui.toolbar.tab.active", "color " + colorToStr(osgWidget::Color(0.537f,0.537f,0.537f,1.0f)) + "\npadding 0\nvalign center\nhalign center\nfont_size 13\nfont_type \"font\\segoeui.ttf\"\nfont_align center_center"));//border_color " + colorToStr(osgWidget::Color(1.0f,0.0f,1.0f,1.0f))));
    styleSet.insert(new osgui::StyleEx("osgui.toolbar.tab.disable", "color " + colorToStr(osgWidget::Color(0.0f,0.0f,0.0f,1.0f)) + "\npadding 0\nvalign center\nhalign center\nfont_size 13\nfont_type \"font\\segoeui.ttf\"\nfont_align center_center"));//border_color " + colorToStr(osgWidget::Color(1.0f,1.0f,1.0f,1.0f))));

    styleSet.insert(new osgui::StyleEx("osgui.toolbar.upperHalf", "color " + colorToStr(osgWidget::Color(0.0f,0.0f,0.0f,0.0f))));
    styleSet.insert(new osgui::StyleEx("osgui.toolbar.lowerHalf", "color " + colorToStr(osgWidget::Color(0.537f,0.537f,0.537f,1.0f))));


    //CONTEXT MENU STYLES
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.menu", "color " + colorToStr(osgWidget::Color(0.357f,0.357f,0.357f,1.0f))));
    //styleSet.insert(new StyleEx("osg.contextmenu.submenuitem", "color " + colorToStr(osgWidget::Color(0.537f,0.537f,0.537f,1.0f)) + "\nfont_type \"font\\segoeui.ttf\""));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.submenuitem.normal", "fill true\ncolor " + colorToStr(osgWidget::Color(0,0,0,1.0)) + "\nfont_size 13\nfont_color " + colorToStr(osgWidget::Color(1,1,1,1.0f)) + "\nfont_type \"font\\segoeui.ttf\"\nfont_fit all\nfont_align center_center"));// + "\nfont_type \"font\\segoeui.ttf\""));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.submenuitem.hovered", "fill true\ncolor " + colorToStr(osgWidget::Color(1,1,1,1.0)) + "\nfont_size 13\nfont_color " + colorToStr(osgWidget::Color(0,0,0,1.0)) + "\nfont_type \"font\\segoeui.ttf\"\nfont_fit all\nfont_align center_center"));// + "\nfont_type \"font\\segoeui.ttf\""));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.item.normal", "fill true\ncolor " + colorToStr(osgWidget::Color(0,0,0,1.0)) + "\nfont_size 13\nfont_color " + colorToStr(osgWidget::Color(1,1,1,1.0)) + "\nfont_type \"font\\segoeui.ttf\"\nfont_fit all\nfont_align center_center"));// + "\nfont_type \"font\\segoeui.ttf\""));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.item.hovered", "fill true\ncolor " + colorToStr(osgWidget::Color(1,1,1,1.0)) + "\nfont_size 13\nfont_color " + colorToStr(osgWidget::Color(0,0,0,1.0)) + "\nfont_type \"font\\segoeui.ttf\"\nfont_fit all\nfont_align center_center"));// + "\nfont_type \"font\\segoeui.ttf\""));

    styleSet.insert(new osgui::StyleEx("osg.contextmenu.checked.normal", "fill true\ncolor " + colorToStr(osgWidget::Color(1.0f,1.0f,0.0f,1.0f))));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.checked.hovered", "fill true\ncolor " + colorToStr(osgWidget::Color(0.0f,0.0f,0.0f,1.0f))));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.unchecked.normal", "fill true\ncolor " + colorToStr(osgWidget::Color(0,0,0,1.0f))));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.unchecked.hovered", "fill true\ncolor " + colorToStr(osgWidget::Color(1,1,1,1.0f))));

    //TOOLTIP STYLES
    styleSet.insert(new osgui::StyleEx("osgui.tooltip.base", "color " + colorToStr(osgWidget::Color(0.0f,0.0f,0.0f,0.0f))));
    styleSet.insert(new osgui::StyleEx("osgui.tooltip.text", "color " + colorToStr(osgWidget::Color(0.94,0.94,0.96,1.0f)) + "\nfont_color " + colorToStr(osgWidget::Color(0,0,0,1.0f)) + "\nfont_size 13\nfont_type \"font\\segoeui.ttf\"\nborder_color " + colorToStr(osgWidget::Color(0,0,0,1.0f))));


    //TOOLBAR BUTTONS
    styleSet.insert(new osgui::StyleEx("osgVDF.toolbar.button.normal", "border_color " + colorToStr(osgWidget::Color(0.0f,0.0f,0.0f,0.0f))));
    styleSet.insert(new osgui::StyleEx("osgVDF.toolbar.button.hover", "border_color " + colorToStr(osgWidget::Color(1.0f,1.0f,1.0f,1.0f))));
    styleSet.insert(new osgui::StyleEx("osgVDF.toolbar.button.toggle", "border_color " + colorToStr(osgWidget::Color(0.0f,1.0f,0.0f,1.0f))));
    styleSet.insert(new osgui::StyleEx("osgVDF.toolbar.button.pushed", "border_color " + colorToStr(osgWidget::Color(1.0f,0.0f,0.0f,1.0f))));

    return styleSet;
}

EDRWorkflowWidget::EDRWorkflowWidget() : currentAction(nullptr), model(new EDRDataFlow())
{
    //inicjalizacja toolbara
    toolbar = new QTabWidget();
    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);


    //Zródla danych
    QWidget* tmpWidget = new QWidget(toolbar);
    QHBoxLayout* tmpLayout = new QHBoxLayout(tmpWidget);
    tmpLayout->setAlignment(Qt::AlignLeft);
    tmpWidget->setLayout(tmpLayout);

    //inicjuj Toolbar w Qt wraz z Drag`n`Drop
    DataSourceManager* dsm = DataSourceManager::getInstance();
    for(int i = 0; i < dsm->getNumPrototypes(); i++){
        //opakuj prototyp w fabrykê wêz³a dla EDRDataFlow
        core::IDataSourceConstPtr proto(dsm->getPrototype(i));
        QToolButton* button = new QToolButton();
        QAction* action = new QAction(button);
        action->setText(QString::fromUtf8(proto->getName().c_str()));
        action->setCheckable(true);

        button->setDefaultAction(action);

        tmpWidget->layout()->addWidget(button);

        actionsItemBuilders[action] = boost::bind(static_cast<DataSourcePtr(DataSourceManager::*)( UniqueID )>(&DataSourceManager::createDataSource), dsm, proto->getID());

        connect(action, SIGNAL(toggled(bool)), this, SLOT(tollbarButoonChanged(bool)));

        //dodaj odpowiadaj¹cy element do toolbara
    }

    toolbar->addTab(tmpWidget, "Sources");

    //Elementy przetwarzajce
    tmpWidget = new QWidget(toolbar);
    tmpLayout = new QHBoxLayout(tmpWidget);
    tmpLayout->setAlignment(Qt::AlignLeft);
    tmpWidget->setLayout(tmpLayout);

    //inicjuj Toolbar w Qt wraz z Drag`n`Drop
    DataProcessorManager* dpm = DataProcessorManager::getInstance();
    for(int i = 0; i < dpm->getNumPrototypes(); i++){
        //opakuj prototyp w fabrykê wêz³a dla EDRDataFlow
        core::IDataProcessorConstPtr proto(dpm->getPrototype(i));
        QToolButton* button = new QToolButton();
        QAction* action = new QAction(button);
        action->setText(QString::fromUtf8(proto->getName().c_str()));
        action->setCheckable(true);

        button->setDefaultAction(action);

        actionsItemBuilders[action] = boost::bind(static_cast<DataProcessorPtr(DataProcessorManager::*)( UniqueID )>(&DataProcessorManager::createDataProcessor), dpm, proto->getID());

        tmpWidget->layout()->addWidget(button);

        connect(action, SIGNAL(toggled(bool)), this, SLOT(tollbarButoonChanged(bool)));

        //dodaj odpowiadaj¹cy element do toolbara
    }

    toolbar->addTab(tmpWidget, "Processors");

    //Wizualizatory
    tmpWidget = new QWidget(toolbar);
    tmpLayout = new QHBoxLayout(tmpWidget);
    tmpLayout->setAlignment(Qt::AlignLeft);
    tmpWidget->setLayout(tmpLayout);

    VisualizerManager* vm = VisualizerManager::getInstance();
    for(int i = 0; i < vm->getNumPrototypes(); i++){
        //opakuj prototyp w fabrykê wêz³a dla EDRDataFlow
        core::IVisualizerConstPtr proto(vm->getPrototype(i));
        QToolButton* button = new QToolButton();
        QAction* action = new QAction(button);
        action->setText(QString::fromUtf8(proto->getName().c_str()));
        action->setCheckable(true);

        button->setDefaultAction(action);

        actionsItemBuilders[action] = boost::bind(&EDRWorkflowWidget::buildAndInitializeVisualizer, this, proto->getID());

        tmpWidget->layout()->addWidget(button);

        connect(action, SIGNAL(toggled(bool)), this, SLOT(tollbarButoonChanged(bool)));
    }

    toolbar->addTab(tmpWidget, "Visualizers");

    //Elementy zapisujace
    tmpWidget = new QWidget(toolbar);
    tmpLayout = new QHBoxLayout(tmpWidget);
    tmpLayout->setAlignment(Qt::AlignLeft);
    tmpWidget->setLayout(tmpLayout);
    toolbar->addTab(tmpWidget, "Sinks");

    QWidget * mainWidget = new QWidget();
    auto layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addWidget(toolbar);
    mainWidget->setLayout(layout);

    setWidget(mainWidget);

    //inicjalizaca VDF
    workflowVDFWidget = new WorkflowCustomQOSGWidget(this);
    workflowVDFWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    workflowVDFWidget->workflowWidget = this;

    workflowVDFWidget->setTimerActive(true);

    workflowVDFWidget->setMouseTracking(true);

    //dla lapania pozycji kursora na menu kontekstowe poniewa¿ z poziomu OSG tracê czêœciowo t¹ informacjê
    workflowVDFWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    const osg::GraphicsContext::Traits* traits = workflowVDFWidget->getCamera()->getGraphicsContext()->getTraits();

    workflowVDFModel = new WorkflowVDFModel(
        workflowVDFWidget,
        traits->width,
        traits->height,
        0xF0000000
#ifdef _DEBUG
        , osgWidget::WindowManager::WM_PICK_DEBUG
#endif
        );

    workflowVDFModel->setNodeDeleteAction(boost::bind(&EDRWorkflowWidget::onNodeDelete, this, _1));
    workflowVDFModel->setNodeDoubleClickAction(boost::bind(&EDRWorkflowWidget::onNodeDoubleClick, this, _1, _2));

    //widget obserwuje model
    model->attach(this);
    workflowVDFModel->setModel(dflm::MPtr(model));  

    connect(workflowVDFWidget, SIGNAL(customContextMenuRequested(QPoint)), dynamic_cast<QObject*>(workflowVDFModel.get()), SLOT(contextMenuRequestPosition(QPoint)));

    // konfiguracja kamery
    osg::Camera* camera = workflowVDFWidget->getCamera();
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF);
    camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, double(traits->width), 0.0f, double(traits->height)));
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix(osg::Matrix::identity());
    workflowVDFWidget->setSceneData(workflowVDFModel);

    workflowVDFWidget->addEventHandler( new osgWidget::MouseHandler(workflowVDFModel) );
    workflowVDFWidget->addEventHandler( new osgWidget::ResizeHandler(workflowVDFModel, camera) );

    StylesSet styleSet = generateRequiredStyles();

    for(StylesSet::iterator it = styleSet.begin(); it != styleSet.end(); it++){
        workflowVDFModel->getStyleManager()->addStyle(*it);
    }

    workflowVDFModel->showDefaultToolbar(false);

    EDRTitleBar * titleBar = supplyWithEDRTitleBar(this);
    
    actionStart = new QAction(titleBar);
    actionStart->setText(tr("Start"));
    actionStart->setObjectName(QString::fromUtf8("actionStart"));
    actionStart->setEnabled(false);

    actionStop = new QAction(titleBar);
    actionStop->setEnabled(false);
    actionStop->setText(tr("Stop"));
    actionStop->setObjectName(QString::fromUtf8("actionStop"));

    connect(actionStart, SIGNAL(triggered()), this, SLOT(start()));
    connect(actionStop, SIGNAL(triggered()), this, SLOT(stop()));

    titleBar->addObject(actionStart, IEDRTitleBar::Left);
    titleBar->addObject(actionStop, IEDRTitleBar::Left);


#ifndef _DEBUG

    //dodajemy dodatkowy zwyk³y widget
    QWidget * retWidget = new QWidget();
    retWidget->setLayout(new QVBoxLayout());
    retWidget->layout()->addWidget(workflowVDFWidget);

#else

    //widget proxy przekierowujacy focusa do okienka osg
    QWidget * retWidget = new core::QOsgEncapsulatorWidget(workflowVDFWidget);
    //dodajemy tez event handler ze statystykami
    workflowVDFWidget->addEventHandler( new osgViewer::StatsHandler() );
    workflowVDFWidget->setTimerActive(true);

#endif

    retWidget->setFocusPolicy(Qt::StrongFocus);
    workflowVDFWidget->setFocusPolicy(Qt::StrongFocus);
    retWidget->layout()->setContentsMargins(2,0,2,2);

    layout->addWidget(retWidget);
}

EDRWorkflowWidget::~EDRWorkflowWidget()
{

}

void EDRWorkflowWidget::update(const dflm::Model* model)
{
    if(this->model->isFinished() == true){
        QMessageBox msgBox;
        msgBox.setText("Workflow has finished");
        msgBox.exec();

        //obsluga zakonczenia przetwarzania
        actionStop->setEnabled(true);
        actionStart->setEnabled(false);

    }else{
        if(model->isModelValid() == true && actionStart->isEnabled() == false){
            actionStart->setEnabled(true);
        }else if(model->isModelValid() == false && actionStart->isEnabled() == true){
            actionStart->setEnabled(false);
        }
    }
}

void EDRWorkflowWidget::tollbarButoonChanged(bool change)
{
    QAction* action = qobject_cast<QAction*>(sender());

    if(action == nullptr){
        return;
    }

    if(action != currentAction){
        if(currentAction != nullptr){
            currentAction->blockSignals(true);
            currentAction->setChecked(false);
            currentAction->blockSignals(false);
        }

        currentAction = action;
    }else{
        currentAction = nullptr;
    }
}

void EDRWorkflowWidget::start()
{
    try{
        workflowVDFWidget->setEnabled(false);

        actionStart->setEnabled(false);
        actionStop->setEnabled(true);

        dynamic_cast<EDRDataFlow*>(workflowVDFModel->getModel().get())->run();
        
    }catch(std::runtime_error e){
        LOG_WARNING(e.what());
        actionStart->setEnabled(true);
        actionStop->setEnabled(false);
        workflowVDFWidget->setEnabled(true);
    }
}

void EDRWorkflowWidget::start(bool s)
{
    if(s == true){
        //startujemy workflow
        start();
    }
}

void EDRWorkflowWidget::stop()
{
    try{
        dynamic_cast<EDRDataFlow*>(workflowVDFModel->getModel().get())->reset();
        actionStop->setEnabled(false);
        actionStart->setEnabled(true);
        workflowVDFWidget->setEnabled(true);
    }catch(std::runtime_error e){
        LOG_WARNING(e.what());
    }
}

WorkflowItemPtr EDRWorkflowWidget::buildAndInitializeVisualizer(UniqueID id)
{
    QMainWindow* window = qobject_cast<QMainWindow*>(parentWidget());
    if(window == nullptr){
        throw std::runtime_error("This should be main window application!");
    }

    VisualizerWidget* visWidget = new VisualizerWidget(id);
    visWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    ////blokujemy zmiane wizualizatora
    visWidget->setVisualizerSwitchEnable(false);
    ////nie niszcz wizualizatora przy jego zamykaniu !! dopiero usuniecie wêz³a powinno to robiæ
    visWidget->setPermanent(true);
    //dodajemy wizualizator do glownego okna
    window->addDockWidget(Qt::RightDockWidgetArea,visWidget);

    //aktualizuj mapowanie logiki do UI
    logicToUI[visWidget->getCurrentVisualizer().get()] = visWidget;

    return visWidget->getCurrentVisualizer();
}


void EDRWorkflowWidget::onNodeDelete(const osgVDF::osgVDFBaseNode * node)
{
    EDRDFNode* edrNode = dynamic_cast<EDRDFNode*>(node->getModelNode().get());

    auto it = logicToUI.find(edrNode->getWorkflowItem().get());

    if( it != logicToUI.end()){
        QMainWindow* window = qobject_cast<QMainWindow*>(it->second->parentWidget());
        window->removeDockWidget(it->second);
        delete it->second;
        logicToUI.erase(it);
    }
}
void EDRWorkflowWidget::onNodeDoubleClick(const osgVDF::osgVDFBaseNode * node, const osgWidget::XYCoord & pos)
{
    EDRDFNode* edrNode = dynamic_cast<EDRDFNode*>(node->getModelNode().get());

    auto it = logicToUI.find(edrNode->getWorkflowItem().get());

    if( it != logicToUI.end()){
        if(it->second->isVisible() == true){
            it->second->setVisible(false);
        }else{
            it->second->setVisible(true);
        }
    }
}

std::string EDRWorkflowWidget::colorToStr(const osgWidget::Color & color)
{
    std::stringstream str;
    str << std::fixed << std::setprecision(5) << color.r() << " " << color.g() << " " <<color.b() << " " << color.a();
    return str.str();
}

EDRWorkflowWidget::StylesSet EDRWorkflowWidget::generateRequiredStyles()
{
    StylesSet styleSet;

    std::string path = core::getResourceString("icons\\vdf\\");
    std::string graphExt = "png";

    //PIN STYLES
    //czcionka pinow
    styleSet.insert(new osgui::StyleEx("vdf.node.pin.text", "font_size 10\nfont_type \"font\\segoeui.ttf\""));

    //piny wyjsciowe
    styleSet.insert(new osgui::StyleEx("vdf.pin.out.ok", "rect_image_cache \"" + path + "out_OK." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.out.active", "rect_image_cache \"" +  path + "out_active." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.out.connected", "rect_image_cache \"" +  path + "out_connected." + graphExt + "\""));
    //styleSet.insert(new StyleEx("vdf.pin.out.incomplete", "rect_image_cache \"" +  path + "out_incomplete." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.out.incomplete", "rect_image_cache \"" +  path + "out_required." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.out.incompatible", "rect_image_cache \"" +  path + "out_incompatible." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.out.required", "rect_image_cache \"" +  path + "out_required." + graphExt + "\""));

    //piny wejsciowe
    styleSet.insert(new osgui::StyleEx("vdf.pin.in.ok", "rect_image_cache \"" +  path + "in_OK." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.in.active", "rect_image_cache \"" +  path + "in_active." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.in.connected", "rect_image_cache \"" +  path + "in_connected." + graphExt + "\""));
    //styleSet.insert(new StyleEx("vdf.pin.in.incomplete", "rect_image_cache \"" +  path + "in_incomplete." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.in.incomplete", "rect_image_cache \"" +  path + "in_required." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.in.incompatible", "rect_image_cache \"" +  path + "in_incompatible." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.pin.in.required", "rect_image_cache \"" +  path + "in_required." + graphExt + "\""));

    //NODE STYLES
    //obwódka, cieñ
    //normal
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TL.normal", "rect_image_cache \"" +  path + "normal_board_TL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TM.normal", "rect_image_cache \"" +  path + "normal_board_TM." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TR.normal", "rect_image_cache \"" +  path + "normal_board_TR." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.ML.normal", "rect_image_cache \"" +  path + "normal_board_ML." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.MR.normal", "rect_image_cache \"" +  path + "normal_board_MR." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BL.normal", "rect_image_cache \"" +  path + "normal_board_BL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BM.normal", "rect_image_cache \"" +  path + "normal_board_BM." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BR.normal", "rect_image_cache \"" +  path + "normal_board_BR." + graphExt + "\""));

    //kolizja
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TL.collision", "rect_image_cache \"" +  path + "collision_board_TL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TM.collision", "rect_image_cache \"" +  path + "collision_board_TM." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TR.collision", "rect_image_cache \"" +  path + "collision_board_TR." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.ML.collision", "rect_image_cache \"" +  path + "collision_board_ML." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.MR.collision", "rect_image_cache \"" +  path + "collision_board_MR." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BL.collision", "rect_image_cache \"" +  path + "collision_board_BL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BM.collision", "rect_image_cache \"" +  path + "collision_board_BM." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BR.collision", "rect_image_cache \"" +  path + "collision_board_BR." + graphExt + "\""));

    //select
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TL.active", "rect_image_cache \"" +  path + "selected_board_TL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TM.active", "rect_image_cache \"" +  path + "selected_board_TM." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.TR.active", "rect_image_cache \"" +  path + "selected_board_TR." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.ML.active", "rect_image_cache \"" +  path + "selected_board_ML." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.MR.active", "rect_image_cache \"" +  path + "selected_board_MR." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BL.active", "rect_image_cache \"" +  path + "selected_board_BL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BM.active", "rect_image_cache \"" +  path + "selected_board_BM." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.border.BR.active", "rect_image_cache \"" +  path + "selected_board_BR." + graphExt + "\""));


    //cia³o wêz³a
    //normal
    //top
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TL.normal", "rect_image_cache \"" +  path + "normal_body_TL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TR.normal", "rect_image_cache \"" +  path + "normal_body_TR." + graphExt + "\""));

    //get color
    osg::Image * imgA = osgDB::readImageFile(path + "normal_body_TL." + graphExt);
    osg::Image * imgB = osgDB::readImageFile(path + "normal_body_TR." + graphExt);
    osgWidget::Color col(1,1,1,1);
    if(imgA != 0 && imgB != 0){
        col = (imgA->getColor(imgA->s()-1,0,imgA->r()-1) + imgB->getColor(0,0,imgB->r()-1)) / 2.0;
    }

    styleSet.insert(new osgui::StyleEx("vdf.node.body.TNameLabel.normal", "color " + colorToStr(col) + "\nfont_size 13\nfont_type \"font\\segoeui.ttf\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TConfigBCG.normal", "color " + colorToStr(col)));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TDeleteBCG.normal", "color " + colorToStr(col)));

    styleSet.insert(new osgui::StyleEx("vdf.node.body.config.normal", "rect_image_cache \"" +  path + "normal_body_CFG." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.delete.normal", "rect_image_cache \"" +  path + "normal_body_DEL." + graphExt + "\""));

    //bottom
    styleSet.insert(new osgui::StyleEx("vdf.node.body.BL.normal", "rect_image_cache \"" +  path + "normal_body_BL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.BR.normal", "rect_image_cache \"" +  path + "normal_body_BR." + graphExt + "\""));

    imgA = osgDB::readImageFile(path + "normal_body_BL." + graphExt);
    imgB = osgDB::readImageFile(path + "normal_body_BR." + graphExt);
    col = osgWidget::Color(1,1,1,1);
    if(imgA != 0 && imgB != 0){
        col = (imgA->getColor(imgA->s()-1,imgA->t()-1,imgA->r()-1) + imgB->getColor(0,imgB->t()-1,imgB->r()-1)) / 2.0;
    }

    styleSet.insert(new osgui::StyleEx("vdf.node.body.BM.normal", "color " + colorToStr(col)));

    //center
    styleSet.insert(new osgui::StyleEx("vdf.node.body.Center.normal", "color " + colorToStr(col)));

    //selection
    //top
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TL.active", "rect_image_cache \"" +  path + "selected_body_TL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TR.active", "rect_image_cache \"" +  path + "selected_body_TR." + graphExt + "\""));

    //bottom
    styleSet.insert(new osgui::StyleEx("vdf.node.body.BL.active", "rect_image_cache \"" +  path + "selected_body_BL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.BR.active", "rect_image_cache \"" +  path + "selected_body_BR." + graphExt + "\""));

    //collision
    //top
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TL.collision", "rect_image_cache \"" +  path + "collision_body_TL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.TR.collision", "rect_image_cache \"" +  path + "collision_body_TR." + graphExt + "\""));

    //bottom
    styleSet.insert(new osgui::StyleEx("vdf.node.body.BL.collision", "rect_image_cache \"" +  path + "collision_body_BL." + graphExt + "\""));
    styleSet.insert(new osgui::StyleEx("vdf.node.body.BR.collision", "rect_image_cache \"" +  path + "collision_body_BR." + graphExt + "\""));


    //TOOLBAR STYLES
    styleSet.insert(new osgui::StyleEx("osgui.toolbar.tab.normal", "color " + colorToStr(osgWidget::Color(0.357f,0.357f,0.357f,1.0f)) + "\npadding 0\nvalign center\nhalign center\nfont_size 13\nfont_type \"font\\segoeui.ttf\"\nfont_align center_center"));//border_color " + colorToStr(osgWidget::Color(0.0f,1.0f,1.0f,1.0f))));
    styleSet.insert(new osgui::StyleEx("osgui.toolbar.tab.active", "color " + colorToStr(osgWidget::Color(0.537f,0.537f,0.537f,1.0f)) + "\npadding 0\nvalign center\nhalign center\nfont_size 13\nfont_type \"font\\segoeui.ttf\"\nfont_align center_center"));//border_color " + colorToStr(osgWidget::Color(1.0f,0.0f,1.0f,1.0f))));
    styleSet.insert(new osgui::StyleEx("osgui.toolbar.tab.disable", "color " + colorToStr(osgWidget::Color(0.0f,0.0f,0.0f,1.0f)) + "\npadding 0\nvalign center\nhalign center\nfont_size 13\nfont_type \"font\\segoeui.ttf\"\nfont_align center_center"));//border_color " + colorToStr(osgWidget::Color(1.0f,1.0f,1.0f,1.0f))));

    styleSet.insert(new osgui::StyleEx("osgui.toolbar.upperHalf", "color " + colorToStr(osgWidget::Color(0.0f,0.0f,0.0f,0.0f))));
    styleSet.insert(new osgui::StyleEx("osgui.toolbar.lowerHalf", "color " + colorToStr(osgWidget::Color(0.537f,0.537f,0.537f,1.0f))));


    //CONTEXT MENU STYLES
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.menu", "color " + colorToStr(osgWidget::Color(0.357f,0.357f,0.357f,1.0f))));
    //styleSet.insert(new StyleEx("osg.contextmenu.submenuitem", "color " + colorToStr(osgWidget::Color(0.537f,0.537f,0.537f,1.0f)) + "\nfont_type \"font\\segoeui.ttf\""));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.submenuitem.normal", "fill true\ncolor " + colorToStr(osgWidget::Color(0,0,0,1.0)) + "\nfont_size 13\nfont_color " + colorToStr(osgWidget::Color(1,1,1,1.0f)) + "\nfont_type \"font\\segoeui.ttf\"\nfont_fit all\nfont_align center_center"));// + "\nfont_type \"font\\segoeui.ttf\""));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.submenuitem.hovered", "fill true\ncolor " + colorToStr(osgWidget::Color(1,1,1,1.0)) + "\nfont_size 13\nfont_color " + colorToStr(osgWidget::Color(0,0,0,1.0)) + "\nfont_type \"font\\segoeui.ttf\"\nfont_fit all\nfont_align center_center"));// + "\nfont_type \"font\\segoeui.ttf\""));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.item.normal", "fill true\ncolor " + colorToStr(osgWidget::Color(0,0,0,1.0)) + "\nfont_size 13\nfont_color " + colorToStr(osgWidget::Color(1,1,1,1.0)) + "\nfont_type \"font\\segoeui.ttf\"\nfont_fit all\nfont_align center_center"));// + "\nfont_type \"font\\segoeui.ttf\""));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.item.hovered", "fill true\ncolor " + colorToStr(osgWidget::Color(1,1,1,1.0)) + "\nfont_size 13\nfont_color " + colorToStr(osgWidget::Color(0,0,0,1.0)) + "\nfont_type \"font\\segoeui.ttf\"\nfont_fit all\nfont_align center_center"));// + "\nfont_type \"font\\segoeui.ttf\""));

    styleSet.insert(new osgui::StyleEx("osg.contextmenu.checked.normal", "fill true\ncolor " + colorToStr(osgWidget::Color(1.0f,1.0f,0.0f,1.0f))));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.checked.hovered", "fill true\ncolor " + colorToStr(osgWidget::Color(0.0f,0.0f,0.0f,1.0f))));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.unchecked.normal", "fill true\ncolor " + colorToStr(osgWidget::Color(0,0,0,1.0f))));
    styleSet.insert(new osgui::StyleEx("osg.contextmenu.unchecked.hovered", "fill true\ncolor " + colorToStr(osgWidget::Color(1,1,1,1.0f))));

    //TOOLTIP STYLES
    styleSet.insert(new osgui::StyleEx("osgui.tooltip.base", "color " + colorToStr(osgWidget::Color(0.0f,0.0f,0.0f,0.0f))));
    styleSet.insert(new osgui::StyleEx("osgui.tooltip.text", "color " + colorToStr(osgWidget::Color(0.94,0.94,0.96,1.0f)) + "\nfont_color " + colorToStr(osgWidget::Color(0,0,0,1.0f)) + "\nfont_size 13\nfont_type \"font\\segoeui.ttf\"\nborder_color " + colorToStr(osgWidget::Color(0,0,0,1.0f))));


    //TOOLBAR BUTTONS
    styleSet.insert(new osgui::StyleEx("osgVDF.toolbar.button.normal", "border_color " + colorToStr(osgWidget::Color(0.0f,0.0f,0.0f,0.0f))));
    styleSet.insert(new osgui::StyleEx("osgVDF.toolbar.button.hover", "border_color " + colorToStr(osgWidget::Color(1.0f,1.0f,1.0f,1.0f))));
    styleSet.insert(new osgui::StyleEx("osgVDF.toolbar.button.toggle", "border_color " + colorToStr(osgWidget::Color(0.0f,1.0f,0.0f,1.0f))));
    styleSet.insert(new osgui::StyleEx("osgVDF.toolbar.button.pushed", "border_color " + colorToStr(osgWidget::Color(1.0f,0.0f,0.0f,1.0f))));

    return styleSet;
}