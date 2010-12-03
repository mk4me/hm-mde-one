#include "CorePCH.h"
#include <osgWidget/ViewerEventHandlers>
#include <core/MultiView.h>
#include <core/Placeable.h>
#include <core/AspectRatioKeeper.h>
#include <core/LabeledView.h>
#include <boost/foreach.hpp>

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

MultiView::MultiView( osgViewer::View * view, float width, float height, unsigned int mask, unsigned int flags )
: osgWidget::WindowManager(view, width, height, mask, flags), thumbnailsPaneWidth(100.0f)
{
    // stworzenie szablonu dla miniaturek
    OverlayWindow* thumbnailWindow = new OverlayWindow("OverlayWindow");
    thumbnailWindow->getBackground()->setColor(0,0,0,0);
    thumbnailWindow->setEventMask( 0 );
    thumbnailWindow->getLabel()->setLabel("TEMPLATE");
    thumbnailWindow->getLabel()->setColor(0.5,0.5,0.5,0.7);
    thumbnailWindow->getOverlay()->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
    thumbnailWindow->getOverlay()->setBorderWidth(2);
    thumbnailWindow->getOverlay()->setBorderNormalColor(osgWidget::Color(1.0f,1.0f,1.0f,1.0f));
    thumbnailWindow->getOverlay()->setBorderPushedColor(osgWidget::Color(1.0f,0.0f,0.0f,1.0f));
    thumbnailWindow->getOverlay()->setBorderToggleColor(osgWidget::Color(1.0f,0.0f,0.0f,1.0f));
    thumbnailWindow->getOverlay()->setBorderHooverColor(osgWidget::Color(1.0f,1.0f,1.0f,1.0f));

    thumbnailWindow->getOverlay()->setLabelNormalColor(osgWidget::Color(0.0f,0.0f,0.0f,0.0f));
    thumbnailWindow->getOverlay()->setLabelPushedColor(osgWidget::Color(1.0f,0.0f,0.0f,1.0f));
    thumbnailWindow->getOverlay()->setLabelToggleColor(osgWidget::Color(0.0f,0.0f,0.0f,0.0f));
    thumbnailWindow->getOverlay()->setLabelHooverColor(osgWidget::Color(1.0f,1.0f,1.0f,1.0f));

    thumbnailWindow->getOverlay()->setHooverColor(osgWidget::Color(1.0f,0.0f,0.0f,0.5f));
    thumbnailWindow->getOverlay()->addCallback( new osgWidget::Callback( &MultiView::onItemClicked, this, osgWidget::EVENT_MOUSE_PUSH ) );
    thumbnailWindow->getOverlay()->setToggleEnabled(false);
    thumbnailTemplate = new Overlay("TEMPLATE", thumbnailWindow);
    thumbnailTemplate->setCanFill(false);
    thumbnailTemplate->setPadding(2);
    thumbnailTemplate->setEventMask(0);

    // stworzenie szablonu dla podgl¹du
    OverlayWindow* viewWindow = new OverlayWindow("ViewWindow");
    viewWindow->getBackground()->setColor(0,0,0,0);
    viewWindow->setEventMask( 0 );
    viewWindow->getLabel()->setLabel("TEMPLATE");
    viewWindow->getLabel()->setColor(0.5,0.5,0.5,0.7);
    viewWindow->getOverlay()->setEventMask(0);
    viewWindow->getOverlay()->setBorderWidth(2);
    viewWindow->getOverlay()->setBorderAllColors(osgWidget::Color(1,1,1,1));
    viewTemplate = new Overlay("TEMPLATE", viewWindow);
    viewTemplate->setCanFill(true);
    viewTemplate->setPadding(2);


    //thumbnailTemplate->resize(64, 64);

    thumbnails = new osgUI::Grid("thumbnails", 0, 0);
    thumbnails->getBackground()->setColor(0, 0, 0, 0);
    thumbnails->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);    
    thumbnails->setStrata( osgWidget::Window::STRATA_FOREGROUND );


    //thumbnails->setDimensions(1, 2);
    //thumbnails->addWidget( new OverlayButton("0, 0", 10, 64), 0, 0);
    //thumbnails->getByRowCol(0, 0)->setCanFill(true);
    //static_cast<OverlayButton*>(thumbnails->getByRowCol(0, 0))->setBorderAllColors(osgWidget::Color(0,0,0,0));
   // static_cast<OverlayButton*>(thumbnails->getByRowCol(0, 0))->setAllColors(osgWidget::Color(0,0,0,0));
    //thumbnails->addWidget( osg::clone(thumbnails->getByRowCol(0, 0), "0, 1", osg::CopyOp::DEEP_COPY_ALL), 0, 1);
    //thumbnails->getByRowCol(0, 0)->setCanFill(true);
    //thumbnails->getByRowCol(0, 1)->setCanFill(true);

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
        Overlay* widget = osg::clone(thumbnailTemplate.get(), item->getName(), osg::CopyOp::DEEP_COPY_ALL);
        widget->getWindow()->setName( item->getName() + "Window" );
        widget->getWindow()->getLabel()->setLabel( item->getName() );
        widget->getWindow()->getOverlay()->setLabel( item->getName() );

        // opakowanie miniaturki tak, aby wspó³czynnik proporcji by³ zachowany
        osgUI::AspectRatioKeeper* keeper = new osgUI::AspectRatioKeeper(widget, item->getAspectRatio());
        keeper->getWindow()->hide();

        // dodanie itema do listy
        Entry entry = { item, preview, widget->getWindow()->getOverlay() };
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



void MultiView::refreshLayout()
{
    // pobranie wspó³czynników proporcji itemów
    std::vector<float> aspectRatios;
    aspectRatios.reserve(items.size());
    BOOST_FOREACH(Entry& entry, items) {
        aspectRatios.push_back( entry.item->getAspectRatio() );
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
        osgWidget::point_type margin = entry.widget->getBorderWidth();
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

bool MultiView::onItemClicked( osgWidget::Event& ev )
{
    if (OverlayButton* button = dynamic_cast<OverlayButton*>(ev.getWidget())) {

        // wyszukujemy poprzednio zaznaczonego
        for ( Items::iterator it = items.begin(); it != items.end(); ++it ) {
            // ustawienie pozycji itema
            OverlayButton* prevButton = dynamic_cast<OverlayButton*>(it->widget.get());
            // wy³¹czenie poprzednio aktywnego
            if ( prevButton != button && prevButton->isToggled() ) {
                prevButton->setToggled(false);
                if ( it->preview ) {
                    it->preview->setSelected(false);
                }
            }
        }

        //
        if ( !button->isToggled() ) {
            button->setToggled(true);
            Items::iterator it = getIterator(button);;
            selectedItem = it->item;
            if ( it->preview ) {
                it->preview->setSelected(true);
                it->preview->setLocation(0, 0, getWidth(), getHeight() - thumbnails->getHeight());
                //refreshLayout();
            }
        }

    }
    return true;
}

void MultiView::setThumbnailsPaneWidth( osgWidget::point_type thumbnailsPaneWidth )
{
    this->thumbnailsPaneWidth = thumbnailsPaneWidth;
    refreshLayout();
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////