#include <KinematicModelLibrary/SkeletalParsers.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGUI/QTreeWidgetItem>
#include <QtGUI/QFileDialog>
#include <boost/filesystem.hpp>
#include <osg/Point>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>

#include <osgViewer/Viewer>

#include "QOsgWidgets.h"

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace kinematic;
using namespace osg;

// dlaczego fileName.toStdString() nie dziala?

std::string toStdString(const QString& qstring) {
    const QByteArray asc = qstring.toAscii();
    return std::string(asc.constData(), asc.length());
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initViewer();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    SkeletalModel::Ptr skeletalModel(new SkeletalModel);
    QString fileName = QFileDialog::getOpenFileName(
        this,tr("Open File"),
        "D:\\MotionCapture\\tests\\",
        tr("Animation Files (*.bvh *.amc)")
        );
    
    
    path p(toStdString(fileName));
    if (extension(p) == ".amc") {
        fileName = QFileDialog::getOpenFileName(
            this,tr("Open File"),
            "D:\\MotionCapture\\tests\\",
            tr("Animation Files (*.asf)")
            );
        AsfParser asf;
        asf.parse(skeletalModel, toStdString(fileName));
    } else {
        BvhParser bvh;
        bvh.parse(skeletalModel, toStdString(fileName));
    }

    JointPtr root = skeletalModel->getSkeleton().getRoot();
    QTreeWidgetItem* rootItem = new QTreeWidgetItem(ui->treeWidget);
    QString rootName(skeletalModel->getSkeleton().getRootName().c_str());
    rootItem->setText(0, rootName);
    createTree(root, rootItem);
    createMarkersCrowd(root);
}

void MainWindow::createTree( JointPtr parentJoint, QTreeWidgetItem* parentItem ) {
    int count = parentJoint->children.size();
    for (int i = 0; i < count; i++) {
        QTreeWidgetItem* item = new QTreeWidgetItem(parentItem);
        JointPtr child = parentJoint->children[i];
        QString name(child->name.c_str());
        item->setText(0, name);
        createTree(child, item);
    }
}

void MainWindow::initViewer() {
    viewer = new core::QOsgDefaultWidget(this);
    ui->vbox->addWidget(viewer);
    const osg::GraphicsContext::Traits* traits = viewer->getCamera()->getGraphicsContext()->getTraits();
    // tworzenie kamery
    viewer->getCamera()->setClearColor(osg::Vec4(0.705f, 0.72f, 0.705f, 1));
    viewer->getCamera()->setNearFarRatio(0.000000001);
    Vec3 pos( 0.0f, 0.0f, 90.0f);
    Vec3 at;
    Vec3 up(0, 1, 0);
    viewer->getCamera()->setViewMatrixAsLookAt(pos, at, up);
    root = new osg::Group;
    viewer->setSceneData(root);
}

GeodePtr MainWindow::createMarker( const osg::Vec4& color , float scale) const
{
    GeodePtr geode = new osg::Geode();
    ref_ptr<Geometry> geometry = new osg::Geometry();

    
    geode->addDrawable(geometry);
    
    ref_ptr<Vec3Array> pyramidVertices = new osg::Vec3Array;

    pyramidVertices->push_back( osg::Vec3( 0.0f, 0.0f, 0.0f) * scale );
    pyramidVertices->push_back( osg::Vec3( 1.0f, 0.0f, 0.0f) * scale ); 
    pyramidVertices->push_back( osg::Vec3( 1.0f, 1.0f, 0.0f) * scale ); 
    pyramidVertices->push_back( osg::Vec3( 0.0f, 1.0f, 0.0f) * scale ); 
    pyramidVertices->push_back( osg::Vec3( 0.5f, 0.5f, 1.0f) * scale ); 

    geometry->setVertexArray( pyramidVertices );

    osg::DrawElementsUInt* geometryBase =
        new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
    geometryBase->push_back(3);
    geometryBase->push_back(2);
    geometryBase->push_back(1);
    geometryBase->push_back(0);
    geometry->addPrimitiveSet(geometryBase);
        
    osg::DrawElementsUInt* pyramidFaceOne =
        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    pyramidFaceOne->push_back(0);
    pyramidFaceOne->push_back(1);
    pyramidFaceOne->push_back(4);
    geometry->addPrimitiveSet(pyramidFaceOne);

    osg::DrawElementsUInt* pyramidFaceTwo =
        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    pyramidFaceTwo->push_back(1);
    pyramidFaceTwo->push_back(2);
    pyramidFaceTwo->push_back(4);
    geometry->addPrimitiveSet(pyramidFaceTwo);

    osg::DrawElementsUInt* pyramidFaceThree =
        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    pyramidFaceThree->push_back(2);
    pyramidFaceThree->push_back(3);
    pyramidFaceThree->push_back(4);
    geometry->addPrimitiveSet(pyramidFaceThree);

    osg::DrawElementsUInt* pyramidFaceFour =
        new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    pyramidFaceFour->push_back(3);
    pyramidFaceFour->push_back(0);
    pyramidFaceFour->push_back(4);
    geometry->addPrimitiveSet(pyramidFaceFour);

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(color);
    colors->push_back(color);
    colors->push_back(color);
    colors->push_back(color);
    colors->push_back(color);
    
    geometry->setColorArray(colors);
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    return geode;
}

void MainWindow::on_pushButton_2_clicked()
{ 
    addLine(Vec3(), Vec3());
    Vec4 color(1,1,1,1);
    float rand = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    GeodePtr marker = createMarker(color * rand, rand);
    
    ref_ptr<MatrixTransform> transform = new MatrixTransform();

    root->addChild(transform);
    transform->addChild(marker);

    float x = static_cast<float>(std::rand() % 4 - 2);
    float y = static_cast<float>(std::rand() % 4 - 2);
    float z = static_cast<float>(std::rand() % 4 - 2);

    osg::Vec3 pyramidTwoPosition(x, y, z);
    osg::Matrix mat;
    mat.identity();
    mat.setTrans(pyramidTwoPosition);
    transform->setMatrix(mat);
    root->addChild(transform.get());
    
}

void MainWindow::addPoint( const osg::Vec3& point )
{
    
    float rand = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    Vec4 color(rand,1,rand,1);
    GeodePtr marker = createMarker(color, 1.5f);

    ref_ptr<MatrixTransform> transform = new MatrixTransform();

    root->addChild(transform);
    transform->addChild(marker);

    osg::Matrix mat;
    mat.identity();
    mat.setTrans(point);
    transform->setMatrix(mat);
    root->addChild(transform.get());
}

void MainWindow::createMarkersCrowd( kinematic::JointPtr joint, const Vec3& position)
{
    cout << joint->name << " adds point at : " << position[0] << ", " << position[1] << ", " << position[2] << endl;
    addPoint(position);
    Vec3 dir = joint->direction * joint->length;
    Vec3 position2 = position + dir;
    addLine(position, position2);
    int count = joint->children.size();
    if (count > 0) {
        for (int i = 0; i < count; ++i) {
            createMarkersCrowd(joint->children[i], position2);
        }
    } else {
        addPoint(position2);
        cout << joint->name << " adds point at : " << position2[0] << ", " << position2[1] << ", " << position2[2] << endl;
    }
}

void MainWindow::on_plusButton_clicked()
{
    
}

void MainWindow::on_minusButton_clicked()
{

}

void MainWindow::addLine( const osg::Vec3& from, const osg::Vec3& to )
{
    GeodePtr geode = new Geode();
    osg::ref_ptr<Geometry> linesGeom = new osg::Geometry();

    ref_ptr<Vec3Array> vertices = new osg::Vec3Array(2);
    (*vertices)[0] = from;
    (*vertices)[1] = to;
    
    linesGeom->setVertexArray(vertices);

    ref_ptr<Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
    linesGeom->setColorArray(colors);
    linesGeom->setColorBinding(osg::Geometry::BIND_OVERALL);


    osg::Vec3Array* normals = new osg::Vec3Array;
    normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
    linesGeom->setNormalArray(normals);
    linesGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

    linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));
    
    geode->addDrawable(linesGeom);
    root->addChild(geode);
}
