#include "CorePCH.h"
#include <osgWidget/ViewerEventHandlers>
#include <core/MultiView.h>
#include <core/Placeable.h>
#include <core/AspectRatioKeeper.h>
#include <core/LabeledView.h>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/cast.hpp>
#include <boost/lambda/lambda.hpp>
#include <core/MultiViewFunctorWidgetAdapter.h>
#include <core/EmbeddedWindow.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Typ powiadamiaj¹cy MultiView o tym, ¿e nast¹pi³ resize (nie da siê prze³adowaæ,
 *  bo nie ma wirtualnych metod :()
 */
class ResizeNotifier : public osg::NodeCallback
{
private:
    //! Poprzedni rozmiar.
    osgWidget::XYCoord prevSize;

public:
    //! \param w Pocz¹tkowa szerokoœæ.
    //! \param h Pocz¹tkowa wysokoœæ.
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


////////////////////////////////////////////////////////////////////////////////

MultiView::MultiView( osgViewer::View * view, float width, float height, unsigned int mask, unsigned int flags )
: osgWidget::WindowManager(view, width, height, mask, flags), thumbnailsPaneWidth(100.0f)
{
    thumbnails = new osgUI::Grid("thumbnails", 0, 0);
    thumbnails->getBackground()->setColor(0, 0, 0, 0);
    thumbnails->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);    
    thumbnails->setStrata( osgWidget::Window::STRATA_FOREGROUND );
    setThumbnailBuitinTemplate(TemplatesLabelBased);

    // dodanie obs³ugi zdarzeñ
    setUpdateCallback( new ResizeNotifier(width, height) );
    // dodanie dziecka
    addChild(thumbnails);
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

        // opakowanie miniaturki tak, aby wspó³czynnik proporcji by³ zachowany
        osgUI::AspectRatioKeeper* keeper = new osgUI::AspectRatioKeeper(widget, item->getAspectRatio());
        keeper->getWindow()->hide();

        // dodanie itema do listy
        Entry entry = { item, preview, widget, adapter, false, keeper };
        items.push_back( entry );

        // dodanie widgeta kontroluj¹cego rozmiar do miniaturek
        thumbnails->setDirtyMode(true);
        thumbnails->flattenHorizontally();
        thumbnails->setNumColumns(items.size());
        thumbnails->addWidget( keeper, 0, items.size()-1 );
        thumbnails->resetFillables();

        // upewnienie siê, ¿e preview jest wy³¹czony
        if ( preview ) {
            preview->setSelected(false);
        }

        refreshLayout();
        return true;
    } else {
        // item zosta³ ju¿ dodany
        return false;
    }
}

void MultiView::removeItem( Item* item )
{
    throw std::runtime_error("not implemented");
}

void MultiView::removeAllItems()
{
    // wy³¹czenie wybranego elementu
    setSelected(NULL);

    // wyczyszczenie _lastEvent oraz flag przycisków
    pointerMove(-FLT_MAX, -FLT_MAX);
    // wyczyszczenie _lastPush
    mouseReleasedLeft(0, 0);

    // wyczysczenie
    thumbnails->setDimensions(0, 0);
    items.clear();
}

void MultiView::refreshLayout()
{
    // pobranie wspó³czynników proporcji itemów
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

    // dostosowujemy rozmiar oraz dodajemy widgety, które zape³ni¹ woln¹ przestrzeñ
    thumbnails->adjustDimensions(getWidth(), thumbnailsPaneWidth, aspectRatios);
    thumbnails->fillEmpty();
    // faktyczny resize
    thumbnails->resize( getWidth(), thumbnailsPaneWidth );
    // aktualizacja po³o¿enia
    thumbnails->setOrigin( (getWidth() - thumbnails->getWidth())/2, getHeight() - thumbnails->getHeight());
    thumbnails->update();
    // wy³¹czenie "brudnego" trybu
    thumbnails->setDirtyMode(false);


    BOOST_FOREACH(Entry& entry, items) {
        UTILS_ASSERT(entry.item && entry.widget);

        // nowe wspó³rzêdne i rozmiar itemów
        osgWidget::XYCoord position = entry.widget->getOrigin() + entry.widget->getParent()->getAbsoluteOrigin();
        osgWidget::XYCoord size = entry.widget->getSize();
        osgWidget::point_type margin = entry.adapter->getMargin(entry.widget);
        entry.item->setLocation( position.x() + margin, position.y() + margin, size.x() - 2*margin, size.y() - 2*margin );

        // aktualizacja wyœwietlanego itema
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
        //! Typ overlay'a nad miniaturk¹.
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
        // na tej podstawie dalej ju¿ ³atwo
        adapter->getEventSourceFunc= boost::bind(getOverlayFunc, _1);
        // poniewa¿ osgWidget::Label::setLabel jest prze³adowane, trzeba jawnie wskazaæ okreœlon¹ wersjê
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
            // wywo³ujemy getOverlay...
            &OverlayWindow::getOverlay, boost::bind(
                // ... z Overlaya ...
                &Overlay::getWindow,
                boost::bind(
                    // który jest rzutowany z widgeta
                    &boost::polymorphic_cast<Overlay*, osgWidget::Widget>,
                    _1
                )
            )
        );

        // na tej podstawie dalej ju¿ ³atwo
        adapter->getEventSourceFunc= boost::bind(getOverlayFunc, _1);
        adapter->setToggleFunc = boost::bind( &OverlayButton::setToggled, boost::bind(getOverlayFunc, _1), _2 );
        adapter->getMarginFunc = boost::bind( &OverlayButton::getBorderWidth, boost::bind(getOverlayFunc, _1) );
        // poniewa¿ osgWidget::Label::setLabel jest prze³adowane, trzeba jawnie wskazaæ okreœlon¹ wersjê
        adapter->setLabelFunc = boost::bind( static_cast<void (osgWidget::Label::*)(const std::string&)>(&osgWidget::Label::setLabel), 
            boost::bind(
                // pobieramy labelkê ...
                &OverlayWindow::getLabel, boost::bind(
                    // ... z Overlaya ...
                    &Overlay::getWindow,
                    boost::bind(
                        // ... który jest rzutowany z widgeta
                        &boost::polymorphic_cast<Overlay*, osgWidget::Widget>,
                        _1
                    )
                )
            ), _2
        );

        thumbnailTemplate = embedded;
        adapterTemplate = adapter;
            
    } else {
        UTILS_ASSERT(false, "Nieznany domyœlny styl.");
    }
}

bool MultiView::onItemClicked( osgWidget::Event& ev )
{
    // wy³¹czenie pozosta³ych itemów
    Entry* selected = NULL;
    BOOST_FOREACH(Entry& entry, items) {
        UTILS_ASSERT(entry.adapter->getEventSource(entry.widget));
        if ( entry.adapter->getEventSource(entry.widget) != ev.getWidget() ) {
            setEntrySelected(entry, false);
        } else {
            selected = &entry;
        }
    }
    // w³¹czenie wybranego
    if ( selected ) {
        setEntrySelected(*selected, true);
        selectedItem = selected->item;
    }
    return true;
}


void MultiView::setSelected( Item* item )
{
    if ( item != selectedItem ) {
        if ( selectedItem ) {
            setEntrySelected( *checked(getIterator(selectedItem)), false);
            selectedItem = NULL;
        }
        if ( item ) {
            setEntrySelected( *checked(getIterator(item)), true);
            selectedItem = item;
        }
    }
}

void MultiView::setEntrySelected( Entry &entry, bool selected )
{
    if ( !selected ) {
        if ( entry.toggled ) {
            entry.toggled = false;
            entry.adapter->setToggle(entry.widget, false);
            if ( entry.preview ) {
                entry.preview->setSelected(false);
            }
        }
    } else {
        if ( !entry.toggled ) {
            entry.adapter->setToggle(entry.widget, true);
            entry.toggled = true;
            if ( entry.preview ) {
                entry.preview->setSelected(true);
                // poniewa¿ dopiero co pokazaliœmy trzeba wypozycjonowaæ
                entry.preview->setLocation(0, 0, getWidth(), getHeight() - thumbnails->getHeight());
            }
        }
    }
}
std::pair<MultiView::Item*, MultiView::PreviewItem*> MultiView::getSelected()
{
    if ( selectedItem ) {
        Items::iterator found = checked(getIterator(selectedItem));
        return std::make_pair(found->item.get(), found->preview.get());
    } else {
        return std::make_pair<Item*, PreviewItem*>(NULL, NULL); 
    }
}

std::pair<const MultiView::Item*, const MultiView::PreviewItem*> MultiView::getSelected() const
{
    if ( selectedItem ) {
        Items::const_iterator found = checked(getIterator(selectedItem));
        return std::make_pair(found->item.get(), found->preview.get());
    } else {
        return std::make_pair<Item*, PreviewItem*>(NULL, NULL); 
    }
}

unsigned MultiView::getSelectedIndex() const
{
    if ( selectedItem ) {
        return std::distance(items.begin(), checked(getIterator(selectedItem)));
    } else {
        return items.size();
    }
}

void MultiView::setSelectedByIndex( unsigned idx )
{
    if ( idx >= items.size() ) {
        throw std::out_of_range("No such item.");
    }
    if ( selectedItem ) {
        setEntrySelected(*checked(getIterator(selectedItem)), false);
        selectedItem = NULL;
    }
    setEntrySelected( items[idx], true );
    selectedItem = items[idx].item;
}

MultiView::Items::iterator& MultiView::checked( Items::iterator& it )
{
    if ( it == items.end() ) {
        throw std::runtime_error("Not found.");
    }
    return it;
}

MultiView::Items::const_iterator& MultiView::checked( Items::const_iterator& it ) const
{
    if ( it == items.end() ) {
        throw std::runtime_error("Not found.");
    }
    return it;
}

unsigned MultiView::getNumItems() const
{
    return items.size();
}

void MultiView::restoreRequiredChildren()
{
    addChild(thumbnails);
}


MultiView::Items::iterator MultiView::getIterator( const Item* item )
{
    return std::find_if( items.begin(), items.end(), bind(&Entry::item, _1) == item );
}

MultiView::Items::iterator MultiView::getIterator( const osgWidget::Widget* widget )
{
    return std::find_if( items.begin(), items.end(), bind(&Entry::widget, _1) == widget );
}

MultiView::Items::const_iterator MultiView::getIterator( const Item* item ) const
{
    return std::find_if( items.begin(), items.end(), bind(&Entry::item, _1) == item );
}

MultiView::Items::const_iterator MultiView::getIterator( const osgWidget::Widget* widget ) const
{
    return std::find_if( items.begin(), items.end(), bind(&Entry::widget, _1) == widget );
    //     for ( Items::const_iterator it = items.begin(); it != items.end(); ++it ) {
    //         if ( it->widget == widget ) {
    //             return it;
    //         }
    //     }
    //     return items.end();
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////