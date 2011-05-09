//#include "PCH.h"
//#include <kinematiclib/SkeletalParsers.h>
//#include <QtGUI/QTreeWidgetItem>
//#include <QtGUI/QFileDialog>
//#include <boost/filesystem.hpp>
//UTILS_PUSH_WARNINGS
//#include <osg/Point>
//#include <osg/Geometry>
//#include <osg/MatrixTransform>
//#include <osgDB/ReadFile>
//#include <osgViewer/Viewer>
//UTILS_POP_WARNINGS
//#include "uniqueCollection.h"
//#include <osgui/QOsgWidgets.h>
//#include "mainwindow.h"
//#include "ui_mainwindow.h"
//#include <plugins/kinematic/skeletalVisualizationScheme.h>
//#include "skeletonViewerLogic.h"
//
//using namespace std;
//using namespace boost;
//using namespace boost::filesystem;
//using namespace kinematic;
//using namespace osg;
//
//// dlaczego fileName.toStdString() nie dziala?
//
//std::string toStdString(const QString& qstring) {
//    const QByteArray asc = qstring.toAscii();
//    return std::string(asc.constData(), asc.length());
//}
//
//MainWindow::MainWindow(QWidget *parent) :
//    QMainWindow(parent),
//    ui(new Ui::MainWindow)
//{
//    ui->setupUi(this);
//    ui->timeLabel->setText("");
//    ui->frameLabel->setText("");
//    initViewer();
//    treeRootItem = new QTreeWidgetItem(ui->treeWidget);
//    timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(on_update()));
//}
//
//MainWindow::~MainWindow() {
//    delete ui;
//}
//
//
//void MainWindow::initViewer() {
//    viewer = new osgui::QOsgDefaultWidget(this);
//    ui->vbox->addWidget(viewer);
//    const osg::GraphicsContext::Traits* traits = viewer->getCamera()->getGraphicsContext()->getTraits();
//    // tworzenie kamery
//    viewer->getCamera()->setClearColor(osg::Vec4(0.705f, 0.72f, 0.705f, 1));
//    viewer->getCamera()->setNearFarRatio(0.000000001);
//    Vec3 pos( 0.0f, 0.0f, 9.0f);
//    Vec3 at;
//    Vec3 up(0, 1, 0);
//    viewer->getCamera()->setViewMatrixAsLookAt(pos, at, up);
//    rootNode = new osg::Group;
//    viewer->setSceneData(rootNode);
//}
//
//void MainWindow::setLogic( SkeletonViewerLogicPtr val )
//{
//     logic = val;
//     logic->setMainWindow(this);
//}
//
//void MainWindow::on_loadSkeletonButton_clicked()
//{
//    try {
//        QString fileName = QFileDialog::getOpenFileName(
//            this,tr("Open File"),
//            "D:\\MotionCapture\\tests\\kinematic\\acclaim\\cmu\\01",
//            tr("Animation Files (*.bvh *.amc)")
//            );
//
//
//        path p(toStdString(fileName));
//        if(extension(p) == ".amc") {
//            QString asfName = QFileDialog::getOpenFileName(
//                this,tr("Open File"),
//                "D:\\MotionCapture\\tests\\kinematic\\acclaim\\cmu\\01",
//                tr("Animation Files (*.asf)")
//                );
//            path pp(toStdString(asfName));
//            if (extension(pp) != ".asf") {
//                MessageBox("Wrong file", QMessageBox::Critical);
//                return;
//            }
//            logic->loadAsfAmc(toStdString(asfName), toStdString(fileName));
//            ui->loadSkeletonButton->setEnabled(false);
//        } else if (extension(p) == ".bvh")  {
//            logic->loadBVH(toStdString(fileName));
//            ui->loadSkeletonButton->setEnabled(false);
//        } else {
//           MessageBox("Wrong file", QMessageBox::Critical);
//        }
//    } catch (kinematic::Exception e) {
//        MessageBox(e.what(), QMessageBox::Critical);
//    }
//}
//
//void MainWindow::MessageBox(const QString& message, QMessageBox::Icon icon)
//{
//    QMessageBox msgBox;
//    msgBox.setText(message);
//    msgBox.setIcon(icon);
//    msgBox.exec();
//}
//
//void MainWindow::on_playButton_clicked()
//{
//    logic->start();
//}
//
//void MainWindow::on_pauseButton_clicked()
//{
//    logic->pause();
//}
//
//void MainWindow::on_stopButton_clicked()
//{
//    logic->stop();
//}
//
//void MainWindow::on_timeSlider_sliderMoved(int position)
//{
//    double pos = static_cast<double>(position);
//    double len = static_cast<double>(ui->timeSlider->maximum());
//    logic->setTime(pos/len);
//    logic->update(false);
//}
//
//void MainWindow::on_update()
//{
//    logic->update();
//}
//
//void MainWindow::refreshTime()
//{
//    std::string s = boost::lexical_cast<std::string>(logic->getTime());
//    ui->timeLabel->setText(s.c_str());
//}
//
//QLabel* MainWindow::getTimeLabel() const
//{
//    return ui->timeLabel;
//}
//
//QLabel* MainWindow::getFrameLabel() const
//{
//    return ui->frameLabel;
//}
//
//QSlider* MainWindow::getSlider() const
//{
//    return ui->timeSlider;
//}
