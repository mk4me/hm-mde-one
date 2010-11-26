#include "CorePCH.h"
#include <osgWidget/ViewerEventHandlers>
#include <core/MultiView.h>
#include <core/Placeable.h>
#include <core/AspectRatioKeeper.h>
#include <core/LabeledView.h>

typedef osgUI::Buttonized< osgUI::Borderized<osgWidget::Widget>, osgUI::BorderStylePolicy<osgUI::DefaultStylePolicy> > Button;

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

bool callbackWindowResize(osgWidget::Event& ev)
{
    if(!ev.getWindow() || !ev.getWindowManager()->isMiddleMouseButtonDown()) return false;
    ev.getWindow()->resizeAdd(ev.x, ev.y);
    ev.getWindow()->update();
    return true;
}

MultiView::MultiView( osgViewer::View * view, float width, float height, unsigned int mask, unsigned int flags )
: osgWidget::WindowManager(view, width, height, mask, flags)
{
    thumbnails = new osgUI::Grid("thumbnails", 0, 0);
    thumbnails->getBackground()->setColor(0, 0, 0, 0);

    thumbnails->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
    thumbnails->attachMoveCallback();
    thumbnails->addCallback( new osgWidget::Callback(&callbackWindowResize, osgWidget::EVENT_MOUSE_DRAG));

    thumbnails->setStrata( osgWidget::Window::STRATA_FOREGROUND );

    //thumbnails->setDimensions(1, 2);
    //thumbnails->addWidget( new Button("0, 0", 10, 64), 0, 0);
    //thumbnails->getByRowCol(0, 0)->setCanFill(true);
    //static_cast<Button*>(thumbnails->getByRowCol(0, 0))->setBorderAllColors(osgWidget::Color(0,0,0,0));
   // static_cast<Button*>(thumbnails->getByRowCol(0, 0))->setAllColors(osgWidget::Color(0,0,0,0));
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
    if ( getIterator(item) == items.end() ) {
        // stworzenie widoku
        
        typedef osgUI::LabeledView< Button > LabeledView;
        LabeledView * view = new LabeledView(item->getName());
        view->getBackground()->setColor(0,0,0,0);
        view->setEventMask( 0 );
        view->getOverlay()->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
        view->getOverlay()->setHooverColor(osgWidget::Color(1,0,0,0.5));
        view->getLabel()->setLabel( item->getName() );
        view->getLabel()->setColor(0.5,0.5,0.5,0.7);

        view->getOverlay()->addCallback( new osgWidget::Callback( &MultiView::onItemClicked, this, osgWidget::EVENT_MOUSE_PUSH ) );
        view->getOverlay()->setToggleEnabled(false);
        

        // dodanie widoku do grida
        osgWidget::Window::EmbeddedWindow* widget = view->embed();
        
        widget->setColor(0,0,0,0);
        widget->setCanFill(false);

        osgUI::AspectRatioKeeper* keeper = new osgUI::AspectRatioKeeper(widget, item->getAspectRatio());

        thumbnails->setDimensions( 1, thumbnails->getNumColumns() + 1 );
        thumbnails->addWidget( keeper, 0, thumbnails->getNumColumns() - 1 );
        thumbnails->resetFillable();

        // dodanie do listy
        Entry entry = { item, preview, view->getOverlay() };
        items.push_back( entry );

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
    // grid na górze na œrodku
    thumbnails->resize( getWidth(), 80 );
    thumbnails->setOrigin( (getWidth() - thumbnails->getWidth())/2, getHeight() - thumbnails->getHeight());
    thumbnails->update();
    thumbnails->resize();

    for ( Items::iterator it = items.begin(); it != items.end(); ++it ) {
        // ustawienie pozycji itema
        Entry& entry= *it;
        UTILS_ASSERT(entry.item && entry.widget);
        osgWidget::XYCoord position = entry.widget->getOrigin() + entry.widget->getParent()->getAbsoluteOrigin();
        entry.item->setLocation( position, entry.widget->getSize() );

        // aktualizacja wyœwietlanego itema
        if ( selectedItem == entry.item && entry.preview ) {
            entry.preview->setLocation(0, 0, getWidth(), getHeight() - thumbnails->getHeight());
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
    if (Button* button = dynamic_cast<Button*>(ev.getWidget())) {

        // wyszukujemy poprzednio zaznaczonego
        for ( Items::iterator it = items.begin(); it != items.end(); ++it ) {
            // ustawienie pozycji itema
            Button* prevButton = dynamic_cast<Button*>(it->widget.get());
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
                refreshLayout();
            }
        }

    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////