#include "VdfPCH.h"
#include "VdfWidget.h"
//#include <vdfmlib/osgVDFNode.h>
//#include <vdfmlib/osgVDFPin.h>
#include "VdfQtModel.h"
#include <testnodefactory/TestNodeFactory.h>
#include <iomanip>
#include <core/PluginCommon.h>
#include "VdfCustomQOsgWidget.h"

#ifdef _DEBUG
#define WM_FLAGS osgWidget::WindowManager::WM_PICK_DEBUG
#else
#define WM_FLAGS 0
#endif

class TestSrcNodeDescriptor : public osgVDF::osgVDFNodeTypeDescriptor{
public:
    TestSrcNodeDescriptor() {
        m_pImage = osgDB::readImageFile(core::getResourceString("icons\\vdf\\testSourceNode.gif"));
        m_pButtonImage = osgDB::readImageFile(core::getResourceString("icons\\vdf\\Source.png"));
        m_sText = "TestSourceNode";
        m_sName = "TestA";
    };

    osgVDF::osgVDFBaseNode::NODE_TYPE getNodeType() const{
        return osgVDF::osgVDFBaseNode::NODE_SOURCE;
    }

    osg::Image * getButtonImage() const{
        return m_pButtonImage;
    }

    const std::string & getButtonText() const{
        return m_sText;
    }

    osg::Image * getNodeDefaultImage() const{
        return m_pImage;
    }

    dflm::NPtr createNode() const{
        return TestNodeFactory::createSourceNode("testSourceNode",3);
    }

    const std::string & getName() const{
        return m_sName;
    }

protected:
    osg::ref_ptr<osg::Image> m_pImage;
    osg::ref_ptr<osg::Image> m_pButtonImage;
    std::string m_sText;
    std::string m_sName;
};

class TestProcNodeDescriptor : public osgVDF::osgVDFNodeTypeDescriptor{
public:
    TestProcNodeDescriptor() {
        m_pImage = osgDB::readImageFile(core::getResourceString("icons\\vdf\\testProcessingNode.gif"));
        m_pButtonImage = osgDB::readImageFile(core::getResourceString("icons\\vdf\\Processing.png"));
        m_sText = "TestProcessingNode";
        m_sName = "TestB";
    };

    osgVDF::osgVDFBaseNode::NODE_TYPE getNodeType() const{
        return osgVDF::osgVDFBaseNode::NODE_PROCESS;
    }

    osg::Image * getButtonImage() const{
        return m_pButtonImage;
    }

    const std::string & getButtonText() const{
        return m_sText;
    }

    osg::Image * getNodeDefaultImage() const{
        return m_pImage;
    }

    dflm::NPtr createNode() const{
        return TestNodeFactory::createProcessingNode("testSourceNode",3,3,1,true);
    }

    const std::string & getName() const{
        return m_sName;
    }

protected:
    osg::ref_ptr<osg::Image> m_pImage;
    osg::ref_ptr<osg::Image> m_pButtonImage;
    std::string m_sText;
    std::string m_sName;
};

VdfWidget::VdfWidget(VdfService* service)
:   service(service)
{
    setupUi(this);

    //vdfQWidget = new osgui::QOsgDefaultWidget(this);
    vdfQWidget = new VdfCustomQOsgWidget(this);
    vdfQWidget->setMouseTracking(true);

    //dla lapania pozycji kursora na menu kontekstowe poniewa¿ z poziomu OSG tracê czêœciowo t¹ informacjê
    vdfQWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

    const osg::GraphicsContext::Traits* traits = vdfQWidget->getCamera()->getGraphicsContext()->getTraits();

    //vdfModel = new osgVDF::osgVDFModel<osgVDF::osgVDFNode, osgVDF::osgVDFPin<osgVDF::BezierConnectionFactory> >(
    vdfModel = new VdfQtModel(
        vdfQWidget,
        traits->width,
        traits->height,
        0xF0000000,
        osgWidget::WindowManager::WM_PICK_DEBUG
        );

    connect(vdfQWidget, SIGNAL(customContextMenuRequested(QPoint)), dynamic_cast<QObject*>(vdfModel.get()), SLOT(contextMenuRequestPosition(QPoint)));

    // stworzenie kamery
    osg::Camera* multiViewCamera = vdfModel->createParentOrthoCamera();
    multiViewCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
    multiViewCamera->setRenderOrder(osg::Camera::POST_RENDER, 1);
    multiViewCamera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->addChild(multiViewCamera);
    vdfQWidget->setSceneData(root);

    //vdfQWidget->addEventHandler( new core::OsgSceneDumpFileHandler("dump_vdf.txt") );
    vdfQWidget->addEventHandler( new osgWidget::MouseHandler(vdfModel) );
    vdfQWidget->addEventHandler( new osgWidget::ResizeHandler(vdfModel, multiViewCamera) );
    vdfQWidget->addEventHandler( new osgViewer::StatsHandler );
    vdfQWidget->addEventHandler( new osgGA::StateSetManipulator( vdfQWidget->getCamera()->getOrCreateStateSet() ) );

    //vdfQWidget->getCamera()->setClearColor(osg::Vec4(0.73f, 0.73f, 0.73f, 1));

    // dodanie do widgeta
    verticalLayout->addWidget( vdfQWidget );

    StylesSet styleSet = generateRequiredStyles();

    for(StylesSet::iterator it = styleSet.begin(); it != styleSet.end(); it++){
		vdfModel->getStyleManager()->addStyle(*it);
	}

	vdfModel->registerNodeType(new TestProcNodeDescriptor());
	vdfModel->registerNodeType(new TestSrcNodeDescriptor());

    vdfModel->showDefaultToolbar(true);
}

VdfWidget::~VdfWidget()
{

}

std::string VdfWidget::colorToStr(const osgWidget::Color & color)
{
    std::stringstream str;
    str << std::fixed << std::setprecision(5) << color.r() << " " << color.g() << " " <<color.b() << " " << color.a();
    return str.str();
}

VdfWidget::StylesSet VdfWidget::generateRequiredStyles()
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
