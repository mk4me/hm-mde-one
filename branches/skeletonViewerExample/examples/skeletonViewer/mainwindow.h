#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ref_ptr>
#include <KinematicModelLibrary/SkeletalParsers.h>
#include <QtGui/QMainWindow>
#include <QtGUi/QTreeWidgetItem>
#include "QOsgWidgets.h"

namespace Ui {
    class MainWindow;
}


typedef osg::ref_ptr<osg::Geode> GeodePtr;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();


    void on_pushButton_2_clicked();

    void on_plusButton_clicked();

    void on_minusButton_clicked();

private:
    void createTree(kinematic::JointPtr parentJoint, QTreeWidgetItem* parentItem);
    void createMarkersCrowd(kinematic::JointPtr joint, const osg::Vec3& position = osg::Vec3());
    void initViewer();
    GeodePtr createMarker(const osg::Vec4& color, float scale = 1.0f) const;
    void addPoint(const osg::Vec3& point);
    void addLine(const osg::Vec3& from, const osg::Vec3& to);

private:
    Ui::MainWindow *ui;
    osg::ref_ptr<osg::Group> root;
    core::QOsgDefaultWidget* viewer;

};

#endif // MAINWINDOW_H
