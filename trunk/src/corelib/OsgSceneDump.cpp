#include "CorePCH.h"
#include <fstream>
#include <Qt/qtreewidget.h>
#include <core/OsgSceneDump.h>


////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

void OsgSceneDumpString::apply( osg::Group& group )
{
    log(group, group.getStateSet());
    increaseIdent();
    traverse(group);
    decreaseIdent();
}

void OsgSceneDumpString::apply( osg::Node& node )
{
    log(node, node.getStateSet());
    traverse(node);
}

void OsgSceneDumpString::apply( osg::Geode& geode )
{
    log(geode, geode.getStateSet());

    increaseIdent();
    for (unsigned i = 0; i < geode.getNumDrawables(); ++i) {
        apply( *geode.getDrawable(i) );
    }
    decreaseIdent();

    traverse(geode);
}

void OsgSceneDumpString::apply( osg::Drawable& drawable )
{
    log(drawable, drawable.getStateSet());
}

void OsgSceneDumpString::log( osg::Object& object, osg::StateSet* stateset )
{
    stream<<identation<<object.libraryName()<<"::"<<object.className()<<" ("<<object.getName()<<")"<<std::endl;
    if ( stateset ) {
        increaseIdent();
        log(*stateset, NULL);
        decreaseIdent();
    }
}

void OsgSceneDumpString::increaseIdent()
{
    identation.append("\t");
}

void OsgSceneDumpString::decreaseIdent()
{
    identation.erase( identation.size() - 1 );
}

std::string OsgSceneDumpString::getString()
{
    return stream.str();
}

std::ostringstream& OsgSceneDumpString::getStream()
{
    return stream;
}
//------------------------------------------------------------------------------

OsgSceneDumpQtTree::OsgSceneDumpQtTree( QTreeWidget* widget, QTreeWidgetItem* root /*= NULL*/ ) :
widget(widget), currentParent(root)
{
}

void OsgSceneDumpQtTree::apply( osg::Group& group )
{
    QTreeWidgetItem* prevParent = currentParent;
    currentParent = createItem(group, group.getStateSet());
    traverse(group);
    currentParent = prevParent;
}

void OsgSceneDumpQtTree::apply( osg::Node& node )
{
    createItem(node, node.getStateSet());
    traverse(node);
}

void OsgSceneDumpQtTree::apply( osg::Geode& geode )
{
    QTreeWidgetItem* prevParent = currentParent;
    currentParent = createItem(geode, geode.getStateSet());
    for (unsigned i = 0; i < geode.getNumDrawables(); ++i) {
        apply( *geode.getDrawable(i) );
    }
    currentParent = prevParent;
    traverse(geode);
}


void OsgSceneDumpQtTree::apply( osg::Drawable& drawable )
{
    createItem(drawable, drawable.getStateSet());
}

QTreeWidgetItem* OsgSceneDumpQtTree::createItem( osg::Object& object, osg::StateSet* stateset )
{
    QTreeWidgetItem* item = currentParent ? new QTreeWidgetItem(currentParent) : new QTreeWidgetItem(widget);
    item->setText(1, QString::fromStdString(std::string(object.libraryName()) + "::" + object.className()));
    item->setText(2, QString::fromStdString(object.getName()) );
    item->setText(0, QString::fromStdString( boost::lexical_cast<std::string>( &object ) ));
    if ( stateset ) {
        QTreeWidgetItem* prevParent = currentParent;
        currentParent = item;
        createItem(*stateset, NULL);
        currentParent = prevParent;
    }
    return item;
}

////////////////////////////////////////////////////////////////////////////////

bool OsgSceneDumpFileHandler::handle( const osgGA::GUIEventAdapter& gea, osgGA::GUIActionAdapter& gaa, osg::Object* obj, osg::NodeVisitor* nv )
{
    osgGA::GUIEventAdapter::EventType ev = gea.getEventType();

    if(ev == osgGA::GUIEventAdapter::KEYUP) {
        int key     = gea.getKey();
        int keyMask = gea.getModKeyMask();
        
        if(key == 'd' || key == 'D') {
            if ( osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&gaa) ) {
                std::ofstream output(filename.c_str());
                if ( output ) {
                    osg::Node* root = viewer->getSceneData();
                    if ( root ) {
                        OsgSceneDumpString populator;
                        populator.setTraversalMode( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN );
                        root->accept(populator);
                        output << populator.getString();
                    } else {
                        OSG_WARN<<"OsgSceneDumpFileHandler: scene empty."<<std::endl;
                    }
                } else {
                    OSG_FATAL<<"OsgSceneDumpFileHandler: file "<<filename<<" could not be created"<<std::endl;
                }
            } else {
                OSG_FATAL<<"OsgSceneDumpFileHandler: view not found."<<std::endl;
            }
            
            
        }
    }
    
    return false;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
