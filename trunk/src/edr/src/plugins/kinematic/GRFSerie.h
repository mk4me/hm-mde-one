/********************************************************************
	created:	2011/08/29
	created:	29:8:2011   9:25
	filename: 	GRFDrawer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__GRFSERIE_H__
#define HEADER_GUARD_KINEMATIC__GRFSERIE_H__

#include <core/IVisualizer.h>
#include <osg/Geode>
#include <timelinelib/IChannel.h>
#include "KinematicVisualizer.h"

const float grfScale = -0.0008f;

class GRFSerie : public core::IVisualizer::SerieBase, public timeline::IChannel
{
public:
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
	typedef osg::ref_ptr<osg::Group> GroupPtr;

public:
	GRFSerie(KinematicVisualizer * visualizer) : 
	  visualizer(visualizer)
	  {

	  }

protected:
	virtual void setSerieName(const std::string & name){}			

	virtual void setSerieData(const core::ObjectWrapperConstPtr & data)
	{
		UTILS_ASSERT(data->getTypeInfo() == typeid(GRFCollection));
		grfCollection = data->get();
		if (grfCollection->getPlatforms().size() == 2) {
			f1[0] = grfCollection->getChannel("Fx1");
			f1[1] = grfCollection->getChannel("Fy1");
			f1[2] = grfCollection->getChannel("Fz1");
			f2[0] = grfCollection->getChannel("Fx2");
			f2[1] = grfCollection->getChannel("Fy2");
			f2[2] = grfCollection->getChannel("Fz2");
			visualizer->transformNode->addChild(createPlatformsGroup(grfCollection->getPlatforms()));
			visualizer->transformNode->addChild(createButterfly(grfCollection));
			visualizer->transformNode->addChild(createArrow());
		} else {
			grfCollection.reset();
		}
	}

	//! \return Sklonowane dane w kanale
	virtual timeline::IChannelPtr clone() const
	{
		//! NIE WSPIERAMY TUTAJ KLONOWANIA!!
		return timeline::IChannelPtr();
	}

	//! \return Dlugosc kanalu w sekundach
	virtual double getLength() const
	{
		UTILS_ASSERT(grfCollection);
		return grfCollection->getLength();
	}

	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kanalu w sekundach
	virtual void setTime(double time)
	{
		const float treshold = 0.01f;
		float t = static_cast<float>(time);
		osg::Vec3 v1((*f1[0])[t], (*f1[1])[t], (*f1[2])[t]);
		osg::Vec3 v2((*f2[0])[t], (*f2[1])[t], (*f2[2])[t]);
		v1 *= grfScale;
		v2 *= grfScale;
		osg::Vec3 origin1 = grfCollection->getPlatforms()[0]->getCenter();
		osg::Vec3 origin2 = grfCollection->getPlatforms()[1]->getCenter();
		(*arrow1)[0] = v1 + origin1;
		(*arrow1)[1] = origin1;
		(*arrow2)[0] = v2 + origin2;
		(*arrow2)[1] = origin2;

		if (v1.length2() > treshold) {	
			arrowLines1->setVertexArray(arrow1);
			platform1->setColor(osg::Vec4(0.4f, 0.4f, 0.0f, 1.0f));
		} else {
			platform1->setColor(osg::Vec4(0.5f, 0.5f, 0.3f, 1.0f));
		}
		if (v2.length2() > treshold) {
			arrowLines2->setVertexArray(arrow2);
			platform2->setColor(osg::Vec4(0.4f, 0.4f, 0.0f, 1.0f));
		} else {
			platform2->setColor(osg::Vec4(0.5f, 0.5f, 0.3f, 1.0f));
		}
	}

private:
	GroupPtr createPlatformsGroup(const c3dlib::ForcePlatformCollection& platforms);
	GroupPtr createButterfly(GRFCollectionConstPtr grf) const;
	GroupPtr createArrow();

private:
	KinematicVisualizer * visualizer;
	GRFCollectionPtr grfCollection;
	osg::ref_ptr<osg::Vec3Array> arrow1;
	osg::ref_ptr<osg::Vec3Array> arrow2;
	osg::ref_ptr<osg::Geometry> arrowLines1;
	osg::ref_ptr<osg::Geometry> arrowLines2;
	osg::ref_ptr<osg::ShapeDrawable> platform1;
	osg::ref_ptr<osg::ShapeDrawable> platform2;
	GRFChannelConstPtr f1[3];
	GRFChannelConstPtr f2[3];
};




#endif
