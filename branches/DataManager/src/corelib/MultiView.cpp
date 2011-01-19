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

MultiView::MultiView( osgViewer::View * view, float width, float height, unsigned int mask, unsigned int flags ) : 
osgWidget::WindowManager(view, width, height, mask, flags), 
thumbnailsPaneWidth(100.0f), 
previewVisible(true)
{
    osgWidget::Widget* button = new osgWidget::Widget("button", 32, 32);
    button->addCallback( new osgWidget::Callback( &MultiView::onShowHideButtonClicked, this, osgWidget::EVENT_MOUSE_PUSH ) );
    //button->addCallback( new osgWidget::Callback( &MultiView::onShowHideButtonClicked, this, osgWidget::EVENT_MOUSE_ENTER ) );
    button->setEventMask( osgWidget::EVENT_MASK_MOUSE_DRAG );

    buttons = new osgWidget::Box("buttons");
    buttons->getBackground()->setColor(0, 0, 0, 0);
    buttons->setStrata( osgWidget::Window::STRATA_FOREGROUND );
    buttons->addWidget( button );

    thumbnails = new osgUI::Grid("thumbnails", 0, 0);
    thumbnails->getBackground()->setColor(0, 0, 0, 0);
    //thumbnails->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);    
    thumbnails->setStrata( osgWidget::Window::STRATA_NONE );
    setThumbnailBuitinTemplate(TemplatesLabelBased);

    // dodanie obs³ugi zdarzeñ
    setUpdateCallback( new ResizeNotifier(width, height) );
    // dodanie dziecka
    addChild(thumbnails);
    addChild(buttons);
}

bool MultiView::addItem( Item* thumbnail, Item* preview /*= NULL*/ )
{
    UTILS_ASSERT(thumbnails->getNumObjects() >= items.size());
    if ( getIterator(thumbnail) == items.end() ) {

        // stworzenie nowej miniaturki na podstawie prototypu
        osgWidget::Widget* widget = osg::clone(thumbnailTemplate.get(), thumbnail->getName(), osg::CopyOp::DEEP_COPY_ALL);
        // nadanie stanu
        WidgetAdapter* adapter = adapterTemplate->clone();
        adapter->setLabel(widget, thumbnail->getName());
        adapter->setToggle(widget, false);
        osgWidget::EventInterface* ei = adapter->getEventSource(widget); 
        UTILS_ASSERT(ei, "Musi byc zrodlo eventow!");
        ei->addCallback( new osgWidget::Callback( &MultiView::onItemClicked, this, osgWidget::EVENT_MOUSE_PUSH ) );

        // opakowanie miniaturki tak, aby wspó³czynnik proporcji by³ zachowany
        osgUI::AspectRatioKeeper* keeper = new osgUI::AspectRatioKeeper(widget, thumbnail->getAspectRatio());
        keeper->getWindow()->hide();

        // dodanie itema do listy
        Entry entry = { thumbnail, preview, widget, adapter, false, keeper };
        items.push_back( entry );

        // dodanie widgeta kontroluj¹cego rozmiar do miniaturek
        thumbnails->setDirtyMode(true);
        thumbnails->flattenHorizontally();
        thumbnails->setNumColumns(items.size());
        thumbnails->addWidget( keeper, 0, items.size()-1 );
        thumbnails->resetFillables();

        // upewnienie siê, ¿e preview jest wy³¹czony
        thumbnail->setVisible(true);
        if ( preview ) {
            preview->setVisible(false);
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

void MultiView::setThumbnailsVisible( bool visible )
{
    if ( visible ) {
        thumbnails->show();
    } else {
        thumbnails->hide();
    }
    refreshLayout();
}

bool MultiView::isThumbnailsVisible() const
{
    return thumbnails->isVisible();
}

void MultiView::setPreviewVisible( bool visible )
{
    previewVisible = visible;
    refreshLayout();
}

bool MultiView::isPreviewVisible() const
{
    return previewVisible;
}


void MultiView::refreshLayout()
{
    // obs³uga grida z miniaturkami
    if ( isThumbnailsVisible() ) {
        // dostêpna wysokoœæ
        osgWidget::point_type height = isPreviewVisible() ? thumbnailsPaneWidth : getHeight();
        // pobranie wspó³czynników proporcji itemów
        std::vector<float> aspectRatios;
        aspectRatios.reserve(items.size());
        BOOST_FOREACH(Entry& entry, items) {
            aspectRatios.push_back( entry.thumbnail->getAspectRatio() );
            entry.keeper->setAspectRatio( aspectRatios.back() );
        } 
        // przed automatycznym dostosowaniem rozmiaru
        thumbnails->setDirtyMode(true);
        // usuwamy niepotrzebne elementy
        thumbnails->flattenHorizontally();
        thumbnails->setNumColumns(items.size());
        // dostosowujemy rozmiar oraz dodajemy widgety, które zape³ni¹ woln¹ przestrzeñ
        thumbnails->adjustDimensions(getWidth(), height, aspectRatios);
        thumbnails->fillEmpty();
        // faktyczny resize
        thumbnails->resize( getWidth(), height );
        // aktualizacja po³o¿enia
        thumbnails->setOrigin( (getWidth() - thumbnails->getWidth())/2, getHeight() - thumbnails->getHeight());
        thumbnails->update();
        // wy³¹czenie "brudnego" trybu
        thumbnails->setDirtyMode(false);
    }

    Item* selectedPreview = NULL;
    BOOST_FOREACH(Entry& entry, items) {
        UTILS_ASSERT(entry.thumbnail && entry.widget);
        // obs³uga miniaturek
        if ( isThumbnailsVisible() ) {
            // nowe wspó³rzêdne i rozmiar itemów
            osgWidget::XYCoord position = entry.widget->getOrigin() + entry.widget->getParent()->getAbsoluteOrigin();
            osgWidget::XYCoord size = entry.widget->getSize();
            osgWidget::point_type margin = entry.adapter->getMargin(entry.widget);
            entry.thumbnail->setVisible( true );
            entry.thumbnail->setLocation( position.x() + margin, position.y() + margin, size.x() - 2*margin, size.y() - 2*margin );
        } else {
            // schowanie itemów
            entry.thumbnail->setVisible( false );
        }
        // obs³uga podgl¹du
        if ( entry.preview ) {
            if ( selectedItem == entry.thumbnail ) {
                if ( isPreviewVisible() ) {
                    selectedPreview = entry.preview;
                } else {
                    entry.preview->setVisible(false);
                }
            } else {
                entry.preview->setVisible(false);
            }
        }
    }
    if ( selectedPreview ) {
        selectedPreview->setVisible(true);
        selectedPreview->setLocation(0, 0, getWidth(), getHeight() - thumbnails->getHeight());
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
    if(!ev.getWindow() || !ev.getWindowManager()->isLeftMouseButtonDown()) {
        return false;
    }

    // wybór itema bazuj¹cy na Ÿródle eventów
    BOOST_FOREACH(Entry& entry, items) {
        UTILS_ASSERT(entry.adapter->getEventSource(entry.widget));
        if ( entry.adapter->getEventSource(entry.widget) == ev.getWidget() ) {
            setSelectedByEntry( &entry );
            break;
        }
    }

    // po wyborze itema przywracamy panel z podgl¹dem
    if ( !isPreviewVisible() ) {
        setPreviewVisible(true);
    }


//     // wy³¹czenie pozosta³ych itemów
//     Entry* selected = NULL;
//     BOOST_FOREACH(Entry& entry, items) {
//         UTILS_ASSERT(entry.adapter->getEventSource(entry.widget));
//         if ( entry.adapter->getEventSource(entry.widget) != ev.getWidget() ) {
//             setEntrySelected(entry, false);
//         } else {
//             selected = &entry;
//         }
//     }
//     // w³¹czenie wybranego
//     if ( selected ) {
//         setEntrySelected(*selected, true);
//         selectedItem = selected->thumbnail;
//     }
//     if ( !isPreviewVisible() ) {
//         setPreviewVisible(true);
//     }
    
    return true;
}

bool MultiView::onShowHideButtonClicked( osgWidget::Event& ev )
{
    setPreviewVisible( !isPreviewVisible() );
//     thumbnailsPaneExtended = !thumbnailsPaneExtended;
//     if (thumbnailsPaneExtended) {
//         prevSelectedItem = selectedItem;
//         selectedItem = NULL;
//         BOOST_FOREACH(Entry& entry, items) {
//             if ( entry.preview ) {
//                 entry.preview->setVisible(false);
//             }
//         }
//     } else {
//         selectedItem = prevSelectedItem;
//         BOOST_FOREACH(Entry& entry, items) {
//             if ( entry.thumbnail == selectedItem && entry.preview ) {
//                 entry.preview->setVisible(true);
//             }
//         }
//     }
//     refreshLayout();
    return true;
}


void MultiView::setSelectedByEntry( Entry* selected )
{
    // wy³¹czenie pozosta³ych itemów
    BOOST_FOREACH(Entry& entry, items) {
        UTILS_ASSERT(entry.adapter->getEventSource(entry.widget));
        if ( !selected || entry.thumbnail != selected->thumbnail ) {
            entry.toggled = false;
            entry.adapter->setToggle( entry.widget, false );
        }
    }
    // w³¹czenie wybranego
    if ( selected ) {
        selected->toggled = true;
        selected->adapter->setToggle( selected->widget, true );
        selectedItem = selected->thumbnail;
    } else {
        selectedItem = NULL;
    }
    refreshLayout();
}

void MultiView::setSelected( Item* item )
{
    if ( item != selectedItem ) {
        if ( item ) {
            setSelectedByEntry( &*checked(getIterator(item)) );
        } else {
            setSelectedByEntry( NULL );
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
                entry.preview->setVisible(false);
            }
        }
    } else {
        if ( !entry.toggled ) {
            entry.adapter->setToggle(entry.widget, true);
            entry.toggled = true;
            if ( entry.preview ) {
                entry.preview->setVisible(true);
                // poniewa¿ dopiero co pokazaliœmy trzeba wypozycjonowaæ
                entry.preview->setLocation(0, 0, getWidth(), getHeight() - thumbnails->getHeight());
            }
        }
    }
}
std::pair<MultiViewItem*, MultiViewItem*> MultiView::getSelected()
{
    if ( selectedItem ) {
        Items::iterator found = checked(getIterator(selectedItem));
        return std::make_pair(found->thumbnail.get(), found->preview.get());
    } else {
        return std::make_pair<Item*, Item*>(NULL, NULL); 
    }
}

std::pair<const MultiViewItem*, const MultiViewItem*> MultiView::getSelected() const
{
    if ( selectedItem ) {
        Items::const_iterator found = checked(getIterator(selectedItem));
        return std::make_pair(found->thumbnail.get(), found->preview.get());
    } else {
        return std::make_pair<Item*, Item*>(NULL, NULL); 
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
        throw std::out_of_range("No such thumbnail.");
    }
    if ( getSelectedIndex() != idx ) {
        setSelectedByEntry( &items[idx] );
    }
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
    addChild(buttons);
}


MultiView::Items::iterator MultiView::getIterator( const Item* item )
{
    return std::find_if( items.begin(), items.end(), bind(&Entry::thumbnail, _1) == item );
}

MultiView::Items::iterator MultiView::getIterator( const osgWidget::Widget* widget )
{
    return std::find_if( items.begin(), items.end(), bind(&Entry::widget, _1) == widget );
}

MultiView::Items::const_iterator MultiView::getIterator( const Item* item ) const
{
    return std::find_if( items.begin(), items.end(), bind(&Entry::thumbnail, _1) == item );
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