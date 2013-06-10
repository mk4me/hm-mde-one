#include "HmmLibPCH.h"
#include "HmmService.h"
#include <corelib/FilterBundle.h>
#include <plugins/subject/SubjectDataFilters.h>
#include <plugins/c3d/C3DCollections.h>
#include <plugins/kinematic/Wrappers.h>
#include <plugins/video/Wrappers.h>
#include <plugins/newVdf/TreeBuilder.h>
#include "SubjectFilterCommand.h" 
#include "ConfiguredFilterCommand.h"
#include <QtCore/QObject>
#include "MarkersFilter.h"
#include "EMGConfigFilter.h"
#include "KineticFilter.h"

core::IFilterProvider::FilterBundles HmmService::getFilterBundles() const
{
    QIcon iconAnalog(QString::fromUtf8(":/mde/icons/analogBig.png"));
    QIcon iconKinetic(QString::fromUtf8(":/mde/icons/kineticBig.png"));
    QIcon iconKinematic(QString::fromUtf8(":/mde/icons/kinematicBig.png"));
    QIcon iconVideo(QString::fromUtf8(":/mde/icons/videoBig.png"));

    QIcon iconEmgSmall(QString::fromUtf8(":/mde/icons/emg1Small.png"));
    QIcon iconForceSmall(QString::fromUtf8(":/mde/icons/forcesSmall.png"));
    QIcon iconPowerSmall(QString::fromUtf8(":/mde/icons/powerSmall.png"));
    QIcon iconGRFSmall(QString::fromUtf8(":/mde/icons/grfSmall.png"));
    QIcon iconMomentSmall(QString::fromUtf8(":/mde/icons/momentSmall.png"));
    QIcon iconJointSmall(QString::fromUtf8(":/mde/icons/jointSmall.png"));
    QIcon iconMarkerSmall(QString::fromUtf8(":/mde/icons/markerSmall.png"));
    QIcon iconVideoSmall(QString::fromUtf8(":/mde/icons/videoSmall.png"));

    QIcon iconIllness(QString::fromUtf8(":/mde/icons/jed.chorobowe.png"));
    QIcon iconEndo(QString::fromUtf8(":/mde/icons/po_endoplastyce.png"));
    QIcon iconStroke(QString::fromUtf8(":/mde/icons/po_udarze.png"));
    QIcon iconSpine(QString::fromUtf8(":/mde/icons/zwyrodnienia.png"));

    core::IFilterProvider::FilterBundles bundles;

    core::IFilterBundlePtr analog = utils::make_shared<core::FilterBundle>(QObject::tr("ANALOG"), iconAnalog);
    addSubjectFilterToBundle(analog, typeid(GRFCollection), QObject::tr("GRF"), iconGRFSmall);
    analog->addFilterCommand(utils::make_shared<EMGConfigFilter>());
    //addSubjectFilterToBundle(analog, typeid(EMGChannel), QObject::tr("EMG"), iconEmgSmall);
    bundles.push_back(analog);

    core::IFilterBundlePtr kinetic = utils::make_shared<core::FilterBundle>(QObject::tr("KINETIC"), iconKinetic);
    kinetic->addFilterCommand(utils::make_shared<ForcesFilter>());
    kinetic->addFilterCommand(utils::make_shared<MomentsFilter>());
    kinetic->addFilterCommand(utils::make_shared<PowersFilter>());
    bundles.push_back(kinetic);

    core::IFilterBundlePtr kinematic = utils::make_shared<core::FilterBundle>(QObject::tr("KINEMATIC"), iconKinematic);
    kinematic->addFilterCommand(utils::make_shared<MarkersFilter>());
    addSubjectFilterToBundle(kinematic, typeid(kinematic::JointAnglesCollection), QObject::tr("Joints"), iconJointSmall);
    bundles.push_back(kinematic);


    core::IFilterBundlePtr video = utils::make_shared<core::FilterBundle>(QObject::tr("VIDEO"), iconVideo);
    addSubjectFilterToBundle(video, typeid(VideoChannel), QObject::tr("Videos"), iconVideoSmall);
    bundles.push_back(video);


    core::IFilterBundlePtr illness = utils::make_shared<core::FilterBundle>(QObject::tr("ILLNESS"), iconIllness);
    addSubjectFilterToBundle(illness, typeid(GRFCollection), QObject::tr("Endo"), iconEndo);
    addSubjectFilterToBundle(illness, typeid(EMGCollection), QObject::tr("Stroke"), iconStroke);
    addSubjectFilterToBundle(illness, typeid(EMGCollection), QObject::tr("Spin"), iconSpine);
    bundles.push_back(illness);
        
    return bundles;
}

void HmmService::addSubjectFilterToBundle( core::IFilterBundlePtr fb, const core::TypeInfo& typeInfo, const QString& name, const QIcon& icon) const
{
    //SubjectHierarchyFilterPtr filter = utils::make_shared<SubjectHierarchyTypeFilter>(typeInfo);
    core::IFilterCommandPtr command = core::IFilterCommandPtr(new SimpleFilterCommand(typeInfo, name, icon));
    fb->addFilterCommand(command);
}
