#include "CorePCH.h"
#include <osgWidget/ViewerEventHandlers>
#include <core/MultiView.h>
#include <core/Placeable.h>
#include <core/AspectRatioKeeper.h>
#include <core/LabeledView.h>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/cast.hpp>
#include <core/MultiViewFunctorWidgetAdapter.h>
#include <core/EmbeddedWindow.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Typ powiadamiaj�cy MultiView o tym, �e nast�pi� resize (nie da si� prze�adowa�,
 *  bo nie ma wirtualnych metod :()
 */
class ResizeNotifier : public osg::NodeCallback
{
private:
    //! Poprzedni rozmiar.
    osgWidget::XYCoord prevSize;

public:
    //! \param w Pocz�tkowa szeroko��.
    //! \param h Pocz�tkowa wysoko��.
    ResizeNotifier(osgWidget::XYCoord::value_type w = 0, osgWidget::XYCoord::value_type h = 0) :
    prevSize(w, h)
    {}

    //! \param node
    //! \param nv
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        if ( MultiView * view = dynamic_cast<MultiView*>(node) ) {
            osgWidget::XYCoord size(view->getWidth(), view->getHeight());
            if ( size != prevSize ) {
                prevSize = size;
                view->refreshLayout();
            }
        }
        traverse(node, nv);
    }
};

////////////////////////////////////////////////////////////////////////////////

MultiView::MultiView( osgViewer::View * view, float width, float height, unsigned int mask, unsigned int flags )
: osgWidget::WindowManager(view, width, height, mask, flags), thumbnailsPaneWidth(100.0f)
{
    thumbnails = new osgUI::Grid("thumbnails", 0, 0);
    thumbnails->getBackground()->setColor(0, 0, 0, 0);
    thumbnails->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);    
    thumbnails->setStrata( osgWidget::Window::STRATA_FOREGROUND );

    // dodanie obs�ugi zdarze�
    setUpdateCallback( new ResizeNotifier(width, height) );

    // dodanie dziecka
    addChild(thumbnails);

    setThumbnailBuitinTemplate(TemplatesLabelBased);
}

bool MultiView::addItem( Item* item, PreviewItem* preview /*= NULL*/ )
{
    UTILS_ASSERT(thumbnails->getNumObjects() >= items.size());
    if ( getIterator(item) == items.end() ) {

        // stworzenie nowej miniaturki na podstawie prototypu
        osgWidget::Widget* widget = osg::clone(thumbnailTemplate.get(), item->getName(), osg::CopyOp::DEEP_COPY_ALL);
        // nadanie stanu
        WidgetAdapter* adapter = adapterTemplate->clone();
        adapter->setLabel(widget, item->getName());
        adapter->setToggle(widget, false);
        osgWidget::EventInterface* ei = adapter->getEventSource(widget); 
        UTILS_ASSERT(ei, "Musi byc zrodlo eventow!");
        ei->addCallback( new osgWidget::Callback( &MultiView::onItemClicked, this, osgWidget::EVENT_MOUSE_PUSH ) );

        // opakowanie miniaturki tak, aby wsp�czynnik proporcji by� zachowany
        osgUI::AspectRatioKeeper* keeper = new osgUI::AspectRatioKeeper(widget, item->getAspectRatio());
        keeper->getWindow()->hide();

        // dodanie itema do listy
        Entry entry = { item, preview, widget, adapter, false, keeper };
        items.push_back( entry );

        // dodanie widgeta kontroluj�cego rozmiar do miniaturek
        thumbnails->setDirtyMode(true);
        thumbnails->flattenHorizontally();
        thumbnails->setNumColumns(items.size());
        thumbnails->addWidget( keeper, 0, items.size()-1 );
        thumbnails->resetFillables();

        // upewnienie si�, �e preview jest wy��czony
        if ( preview ) {
            preview->setSelected(false);
        }

        refreshLayout();
        return true;
    } else {
        // item zosta� ju� dodany
        return false;
    }
}

void MultiView::removeItem( Item* item )
{
    throw std::runtime_error("not implemented");
}



void MultiView::refreshLayout()
{
    // pobranie wsp�czynnik�w proporcji item�w
    std::vector<float> aspectRatios;
    aspectRatios.reserve(items.size());
    BOOST_FOREACH(Entry& entry, items) {
        aspectRatios.push_back( entry.item->getAspectRatio() );
        entry.keeper->setAspectRatio( aspectRatios.back() );
    } 

    // przed automatycznym dostosowaniem rozmiaru
    thumbnails->setDirtyMode(true);
    // usuwamy niepotrzebne elementy
    thumbnails->flattenHorizontally();
    thumbnails->setNumColumns(items.size());

    // dostosowujemy rozmiar oraz dodajemy widgety, kt�re zape�ni� woln� przestrze�
    thumbnails->adjustDimensions(getWidth(), thumbnailsPaneWidth, aspectRatios);
    thumbnails->fillEmpty();
    // faktyczny resize
    thumbnails->resize( getWidth(), thumbnailsPaneWidth );
    // aktualizacja po�o�enia
    thumbnails->setOrigin( (getWidth() - thumbnails->getWidth())/2, getHeight() - thumbnails->getHeight());
    thumbnails->update();
    // wy��czenie "brudnego" trybu
    thumbnails->setDirtyMode(false);


    BOOST_FOREACH(Entry& entry, items) {
        UTILS_ASSERT(entry.item && entry.widget);

        // nowe wsp�rz�dne i rozmiar item�w
        osgWidget::XYCoord position = entry.widget->getOrigin() + entry.widget->getParent()->getAbsoluteOrigin();
        osgWidget::XYCoord size = entry.widget->getSize();
        osgWidget::point_type margin = entry.adapter->getMargin(entry.widget);
        entry.item->setLocation( position.x() + margin, position.y() + margin, size.x() - 2*margin, size.y() - 2*margin );

        // aktualizacja wy�wietlanego itema
        if ( selectedItem == entry.item && entry.preview ) {
            //osgWidget::XYCoord position = previewOverlay->getOrigin() + previewOverlay->getParent()->getAbsoluteOrigin();
            //osgWidget::XYCoord size = previewOverlay->getSize();
            //osgWidget::point_type margin = previewOverlay->getBorderWidth();
            entry.preview->setLocation(0, 0, getWidth(), getHeight() - thumbnails->getHeight() );
        }
    }
}

void MultiView::setThumbnailsPaneWidth( osgWidget::point_type thumbnailsPaneWidth )
{
    this->thumbnailsPaneWidth = thumbnailsPaneWidth;
    refreshLayout();
}

void MultiView::setThumbnailBuitinTemplate(Templates templ)
{
    if ( templ == TemplatesLabelBased ) {
        //! Typ overlay'a nad miniaturk�.
        typedef osgUI::Buttonized< osgUI::Borderized<osgWidget::Label>, osgUI::BorderStylePolicy<osgUI::LabelStylePolicy> > OverlayButton;
        OverlayButton* button = new OverlayButton("TEMPLATE", "TEMPLATE");
        button->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
        button->setBorderWidth(2);
        button->setBorderNormalColor(osgWidget::Color(1.0f,1.0f,1.0f,1.0f));
        button->setBorderPushedColor(osgWidget::Color(1.0f,0.0f,0.0f,1.0f));
        button->setBorderToggleColor(osgWidget::Color(1.0f,0.0f,0.0f,1.0f));
        button->setBorderHooverColor(osgWidget::Color(1.0f,1.0f,1.0f,1.0f));
        button->setLabelNormalColor(osgWidget::Color(0.0f,0.0f,0.0f,0.0f));
        button->setLabelPushedColor(osgWidget::Color(1.0f,0.0f,0.0f,1.0f));
        button->setLabelToggleColor(osgWidget::Color(0.0f,0.0f,0.0f,0.0f));
        button->setLabelHooverColor(osgWidget::Color(1.0f,1.0f,1.0f,1.0f));
        button->setHooverColor(osgWidget::Color(1.0f,0.0f,0.0f,0.5f));
        button->setToggleEnabled(false);
        button->setCanFill(false);
        button->setPadding(2);

        MultiViewFunctorWidgetAdapter* adapter = new MultiViewFunctorWidgetAdapter();
        // najpierw po prostu rzutujemy na typ OverlayButton
        boost::function< OverlayButton* (osgWidget::Widget*) > getOverlayFunc = boost::bind(&boost::polymorphic_cast<OverlayButton*, osgWidget::Widget>, _1);
        // na tej podstawie dalej ju� �atwo
        adapter->getEventSourceFunc= boost::bind(getOverlayFunc, _1);
        // poniewa� osgWidget::Label::setLabel jest prze�adowane, trzeba jawnie wskaza� okre�lon� wersj�
        adapter->setLabelFunc = boost::bind( static_cast<void (osgWidget::Label::*)(const std::string&)>(&osgWidget::Label::setLabel), boost::bind(getOverlayFunc, _1), _2);
        adapter->setToggleFunc = boost::bind( &OverlayButton::setToggled, boost::bind(getOverlayFunc, _1), _2 );
        adapter->getMarginFunc = boost::bind( &OverlayButton::getBorderWidth, boost::bind(getOverlayFunc, _1) );

        thumbnailTemplate = button;
        adapterTemplate = adapter;
    } else if ( templ == TemplatesLabeledViewBased ) {
        typedef osgUI::Buttonized< osgUI::Borderized<osgWidget::Widget>, osgUI::BorderStylePolicy<osgUI::DefaultStylePolicy> > OverlayButton;
        typedef osgUI::LabeledView< OverlayButton > OverlayWindow;
        typedef osgUI::Embedded< OverlayWindow > Overlay;

        OverlayWindow* window = new OverlayWindow("OverlayWindow");
        window->getBackground()->setColor(0,0,0,0);
        window->setEventMask( 0 );
        window->getLabel()->setLabel("TEMPLATE");
        window->getLabel()->setColor(0.5,0.5,0.5,0.7);
        window->getOverlay()->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
        window->getOverlay()->setBorderWidth(2);
        window->getOverlay()->setBorderNormalColor(osgWidget::Color(1.0f,1.0f,1.0f,1.0f));
        window->getOverlay()->setBorderPushedColor(osgWidget::Color(1.0f,0.0f,0.0f,1.0f));
        window->getOverlay()->setBorderToggleColor(osgWidget::Color(1.0f,0.0f,0.0f,1.0f));
        window->getOverlay()->setBorderHooverColor(osgWidget::Color(1.0f,1.0f,1.0f,1.0f));
        window->getOverlay()->setHooverColor(osgWidget::Color(1.0f,0.0f,0.0f,0.5f));
        window->getOverlay()->setToggleEnabled(false);

        Overlay* embedded = new Overlay("TEMPLATE", window);
        embedded->setCanFill(false);
        embedded->setPadding(2);
        embedded->setEventMask(0);

        MultiViewFunctorWidgetAdapter* adapter = new MultiViewFunctorWidgetAdapter();
        // najpierw po prostu rzutujemy na typ OverlayButton
        boost::function< OverlayButton* (osgWidget::Widget*) > getOverlayFunc = boost::bind(
            // wywo�ujemy getOverlay...
            &OverlayWindow::getOverlay, boost::bind(
                // ... z Overlaya ...
                &Overlay::getWindow,
                boost::bind(
                    // kt�ry jest rzutowany z widgeta
                    &boost::polymorphic_cast<Overlay*, osgWidget::Widget>,
                    _1
                )
            )
        );

        // na tej podstawie dalej ju� �atwo
        adapter->getEventSourceFunc= boost::bind(getOverlayFunc, _1);
        adapter->setToggleFunc = boost::bind( &OverlayButton::setToggled, boost::bind(getOverlayFunc, _1), _2 );
        adapter->getMarginFunc = boost::bind( &OverlayButton::getBorderWidth, boost::bind(getOverlayFunc, _1) );
        // poniewa� osgWidget::Label::setLabel jest prze�adowane, trzeba jawnie wskaza� okre�lon� wersj�
        adapter->setLabelFunc = boost::bind( static_cast<void (osgWidget::Label::*)(const std::string&)>(&osgWidget::Label::setLabel), 
            boost::bind(
                // pobieramy labelk� ...
                &OverlayWindow::getLabel, boost::bind(
                    // ... z Overlaya ...
                    &Overlay::getWindow,
                    boost::bind(
                        // ... kt�ry jest rzutowany z widgeta
                        &boost::polymorphic_cast<Overlay*, osgWidget::Widget>,
                        _1
                    )
                )
            ), _2
        );

        thumbnailTemplate = embedded;
        adapterTemplate = adapter;
            
    } else {
        UTILS_ASSERT(false, "Nieznany domy�lny styl.");
    }
}

bool MultiView::onItemClicked( osgWidget::Event& ev )
{
    // wy��czenie pozosta�ych item�w
    Entry* selected = NULL;
    BOOST_FOREACH(Entry& entry, items) {
        UTILS_ASSERT(entry.adapter->getEventSource(entry.widget));
        if ( entry.adapter->getEventSource(entry.widget) != ev.getWidget() ) {
            if ( entry.toggled ) {
                entry.toggled = false;
                entry.adapter->setToggle(entry.widget, false);
                if ( entry.preview ) {
                    entry.preview->setSelected(false);
                }
            }
        } else {
            selected = &entry;
        }
    }
    // w��czenie wybranego
    if ( selected ) {
        Entry& entry = *selected;
        if ( !entry.toggled ) {
            entry.adapter->setToggle(entry.widget, true);
            entry.toggled = true;
            selectedItem = entry.item;
            if ( entry.preview ) {
                entry.preview->setSelected(true);
                entry.preview->setLocation(0, 0, getWidth(), getHeight() - thumbnails->getHeight());
                //refreshLayout();
            }
        }
    }
    return true;
}

MultiView::Items::iterator MultiView::getIterator( const Item* item )
{
    for ( Items::iterator it = items.begin(); it != items.end(); ++it ) {
        if ( it->item == item ) {
            return it;
        }
    }
    return items.end();
}

MultiView::Items::iterator MultiView::getIterator( const osgWidget::Widget* widget )
{
    for ( Items::iterator it = items.begin(); it != items.end(); ++it ) {
        if ( it->widget == widget ) {
            return it;
        }
    }
    return items.end();
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////