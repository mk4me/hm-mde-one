/********************************************************************
	created:	2011/09/06
	created:	6:9:2011   13:34
	filename: 	TrajectoriesDrawer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TRAJECTORIES_DRAWER__TRAJECTORIESDRAWER_H__
#define HEADER_GUARD_TRAJECTORIES_DRAWER__TRAJECTORIESDRAWER_H__

#include <osg/Geode>
#include <plugins/kinematic/skeletalVisualizationScheme.h>
#include "OsgSchemeDrawer.h"

class TrajectoryDrawer : public OsgSchemeDrawer
{
public:
	TrajectoryDrawer(const osg::Vec4& color, int density);

public:
	virtual osg::ref_ptr<osg::Node> getNode();
	virtual void init(SkeletalVisualizationSchemeConstPtr scheme);
	virtual void deinit();
	virtual void draw();
	virtual void update();

	void setVisible(const std::string& name, bool visible);

	osg::Vec4 getColor(const std::string& name);
	void setColor(const std::string& name, const osg::Vec4& color);

	void setLineWidth(const std::string& name, float width);


private:
	void createTrajectories(MarkerCollectionConstPtr markers);

private:
	 osg::ref_ptr<osg::Group> node;
	 int density;
	 osg::Vec4 color;
	 std::map<std::string, GeodePtr> trajectoriesMap;
};
typedef boost::shared_ptr<TrajectoryDrawer> TrajectoryDrawerPtr;
typedef boost::shared_ptr<const TrajectoryDrawer> TrajectoryDrawerConstPtr;

#endif