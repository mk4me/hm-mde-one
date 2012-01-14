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
#include <list>
#include "KinematicVisualizer.h"
#include <utils/DataChannel.h>

const float grfScale = 0.0008f;

class GRFSerie : public KinematicSerie, private utils::GeneralDataChannelTimeAccessor<osg::Vec3f, float>
{
public:
	typedef osg::ref_ptr<osg::Geode> GeodePtr;
	typedef osg::ref_ptr<osg::Group> GroupPtr;
	typedef osg::ref_ptr<osg::Geometry> GeometryPtr;
	typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
	typedef osg::ref_ptr<osg::ShapeDrawable> ShapeDrawablePtr;

public:
	GRFSerie(KinematicVisualizer * visualizer) : 
	  visualizer(visualizer)
	  {

	  }

	virtual void setName(const std::string & name)
    {
        this->name = name;
    }

    virtual const std::string & getName() const
    {
        return name;
    }

	virtual void setData(const core::ObjectWrapperConstPtr & data);

    virtual const core::ObjectWrapperConstPtr & getData() const;

	//! \return Dlugosc kanalu w sekundach
	virtual float getLength() const
	{
		UTILS_ASSERT(grfCollection);
		return grfCollection->getLength();
	}

	//! Czas zawiera siê miêdzy 0 a getLength()
	//! \param time Aktualny, lokalny czas kanalu w sekundach
	virtual void setTime(float time);

private:
	struct Arrow 
	{
		TransformPtr mainPtr;
		TransformPtr boxPtr;
		TransformPtr conePtr;
		ShapeDrawablePtr boxShape; 
		ShapeDrawablePtr coneShape;
		void setArrow(osg::Vec3 from, osg::Vec3 to);
		void setColor(const osg::Vec4& color);
		const osg::Vec4& getColor() const;
	};
	typedef boost::shared_ptr<Arrow> ArrowPtr;
	typedef boost::shared_ptr<const Arrow> ArrowConstPtr;

	friend class GhostStack;
	class GhostStack
	{
	public:
		typedef std::pair<osg::Vec3, osg::Vec3> ArrowState;

		GhostStack(int maxSize, GroupPtr hookNode, const osg::Vec4& color ) :
		  maxSize(maxSize) ,
		  hookNode(hookNode),
		  color(color)
		  {
			  UTILS_ASSERT(maxSize > 0);
		  }

		 void addState(const ArrowState& state)
		 {
			 int no = takenArrows.size();
			 if (no < maxSize) {
				 ArrowPtr a = createArrow();
				 a->setArrow(state.first, state.second);
				 a->setColor(color);
				 hookNode->addChild(a->mainPtr);
				 takenArrows.push_back(a);
			 } else {
				 ArrowPtr a = *takenArrows.begin();
				 takenArrows.pop_front();
				 takenArrows.push_back(a);
				 a->setArrow(state.first, state.second);
				 a->setColor(color);
			 }
		 }

		 void update()
		 {
			 float delta = color[3] / static_cast<float>(maxSize);
			 
			 for (auto it = takenArrows.begin(); it != takenArrows.end(); it++) {
				 ArrowPtr a = *it;
				 const osg::Vec4& color = a->getColor();
				 float alpha = color[3] - delta;
				 if (alpha > 0) {
					a->mainPtr->setNodeMask(0xffff);
					a->setColor(osg::Vec4(color[0], color[1], color[2], alpha));
				 } else {
					a->mainPtr->setNodeMask(0);
				 }
			 }
		 }

	private:
		osg::Vec4 color;
		//std::list<ArrowPtr> freeArrows;
		std::list<ArrowPtr> takenArrows;
		GroupPtr hookNode;
		int maxSize;
	};
	typedef boost::shared_ptr<GhostStack> GhostStackPtr;
	typedef boost::shared_ptr<const GhostStack> GhostStackConstPtr;

private:
	GroupPtr createPlatformsGroup(const c3dlib::ForcePlatformCollection& platforms);
	GroupPtr createButterfly(GRFCollectionConstPtr grf, float& maxLength) const;
	TransformPtr createPlatformTransform(osg::Texture2D* texture, const osg::Vec3& origin, float width, float lenght, float height) const;
	static ArrowPtr createArrow();
	static osg::ref_ptr<osg::Texture2D> getTexture(int number);
    static bool tryGetTexture(osg::ref_ptr<osg::Texture2D>& ret, int number);
	
private:
	KinematicVisualizer * visualizer;
	GRFCollectionPtr grfCollection;
	osg::ref_ptr<osg::ShapeDrawable> platform1;
	osg::ref_ptr<osg::ShapeDrawable> platform2;
	GRFChannelConstPtr f1;
	GRFChannelConstPtr f2;
	float maxLength;
	ArrowPtr a1, a2;
	GhostStackPtr g1, g2;
	static osg::ref_ptr<osg::Texture2D> texture1, texture2;
    core::ObjectWrapperConstPtr data;
    std::string name;
};




#endif
