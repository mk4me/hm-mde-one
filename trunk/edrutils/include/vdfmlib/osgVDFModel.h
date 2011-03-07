#ifndef __HEADER_GUARD__VDFMODEL_H__
#define __HEADER_GUARD__VDFMODEL_H__

#include <vdfmlib/osgVDFBaseModel.h>
#include <vdfmlib/osgVDFBasePin.h>
#include <vdfmlib/osgVDFBaseNode.h>

#include <osg/LineWidth>
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <osgDB/ReadFile>
#include <osgAnimation/Interpolator>
#include <boost/type_traits.hpp>
#include <algorithm>
#include <cmath>

#include <dfmlib/DFModel.h>
#include <vdfmlib/osgUI2DUtils.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

class LinearSelectionFactory{
public:
	static void refreshSelection(osg::Geode * selection){
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
		refreshSelection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}


	static osg::LineWidth * getSelectionBorderWidth(osg::Geode * selection){
		return dynamic_cast<osg::LineWidth*>(selection->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINEWIDTH));
	}

	static void setSelectionBorderWidth(osg::Geode * selection, float width){
		getSelectionBorderWidth(selection)->setWidth(width);
	}

	static void setSelectionBorderWidthAndRefresh(osg::Geode * selection, float width){
		setSelectionBorderWidth(selection,width);
		refreshSelection(selection);
	}

	static osgWidget::Color getSelectionBorderColor(osg::Geode * selection){
		return *(dynamic_cast<osg::Vec4Array*>(dynamic_cast<osg::Geometry*>(selection->getDrawable(0))->getColorArray())->begin());
	}

	static osgWidget::Color getSelectionBodyColor(osg::Geode * selection){
		//TODO
		//implement
		return osgWidget::Color();
	}

	static void setSelectionBodyColor(osg::Geode * selection, const osgWidget::Color & color){
		//TODO
		//implement
	}

	static void setSelectionBodyColorAndRefresh(osg::Geode * selection, const osgWidget::Color & color){
		setSelectionBodyColor(selection, color);
		refreshSelection(selection);
	}

	static void setSelectionBorderColor(osg::Geode * selection, const osgWidget::Color & color){
		*(dynamic_cast<osg::Vec4Array*>(dynamic_cast<osg::Geometry*>(selection->getDrawable(0))->getColorArray())->begin()) = color;
	}

	static void setSelectionBorderColorAndRefresh(osg::Geode * selection, const osgWidget::Color & color){
		setSelectionBorderColor(selection, color);
		refreshSelection(selection);
	}

	static osg::Geode * createSelection(const osgWidget::Color & color = osgWidget::Color(0.0,0.58,0.58,1), float width = selectionWidth){

		osg::Geode * geode = new osg::Geode();
		geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(width), osg::StateAttribute::ON);

		osg::Geometry* geom = new osg::Geometry();
		geom->setUseDisplayList(false);
		osg::DrawArrays* drawArrayLines = new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP); 
		geom->addPrimitiveSet(drawArrayLines); 
		osg::Vec3Array* vertexData = new osg::Vec3Array(4);
		geom->setVertexArray(vertexData); 

		osg::Vec4Array* c = new osg::Vec4Array();
		c->push_back( color );
		geom->setColorArray( c );
		geom->setColorBinding( osg::Geometry::BIND_OVERALL );

		geode->addDrawable( geom );
		return geode;
	}

	static void setSelectionStart(osg::Geode * selection, const osgWidget::XYCoord & startPos){
		setSelectionControlPoint(selection, 0, startPos);
	}

	static void setSelectionStartAndRefresh(osg::Geode * selection, const osgWidget::XYCoord & startPos){
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[0], startPos);
		regenerateOtherPoints(selection);
		refreshSelection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

	static osgWidget::XYCoord getSelectionStart(osg::Geode * selection){
		return getSelectionControlPoint(selection,0);
	}

	static osgWidget::XYCoord getSelectionEnd(osg::Geode * selection){
		return getSelectionControlPoint(selection,2);
	}

	static std::pair<osgWidget::XYCoord, osgWidget::XYCoord> getSelectionEndings(osg::Geode * selection){
		std::pair<osgWidget::XYCoord, osgWidget::XYCoord> ret;

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		ret.first.x() = (*vertexData)[0].x();
		ret.first.y() = (*vertexData)[0].y();

		ret.second.x() = (*vertexData)[2].x();
		ret.second.y() = (*vertexData)[2].y();

		return ret;
	}

	static void setSelectionEnd(osg::Geode * selection, const osgWidget::XYCoord & endPos){
		setSelectionControlPoint(selection, 1, endPos);
	}

	static void setSelectionEndAndRefresh(osg::Geode * selection, const osgWidget::XYCoord & endPos){
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[2], endPos);
		regenerateOtherPoints(selection);
		refreshSelection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

	static void setSelectionEndings(osg::Geode * selection, const osgWidget::XYCoord & startPos,
		const osgWidget::XYCoord & endPos){

			osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
			osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

			setControlPoint((*vertexData)[0], startPos);
			setControlPoint((*vertexData)[2], endPos);
	}

	static void setSelectionEndingsAndRefresh(osg::Geode * selection, const osgWidget::XYCoord & startPos,
		const osgWidget::XYCoord & endPos){

			osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
			osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

			setControlPoint((*vertexData)[0], startPos);
			setControlPoint((*vertexData)[2], endPos);
			regenerateOtherPoints(selection);

			refreshSelection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

	static void setSelectionEndingsAndRefresh(osg::Geode * selection, const osgWidget::XYCoord & startPos,
		const osgWidget::XYCoord & endPos, osgWidget::point_type z){

			osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
			osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

			setControlPoint((*vertexData)[0], startPos);
			setControlPoint((*vertexData)[2], endPos);

			regenerateOtherPoints(selection);

			for(unsigned int i = 0; i < vertexData->size(); i++){
				(*vertexData)[0][2] = z;
			}

			refreshSelection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

protected:

	static void regenerateOtherPoints(osg::Geode * selection){
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		float xDiff = (*vertexData)[2][0] - (*vertexData)[0][0];
		float yDiff = (*vertexData)[2][1] - (*vertexData)[0][1];

		(*vertexData)[1][0] = (*vertexData)[0][0] + xDiff;
		(*vertexData)[1][1] = (*vertexData)[2][1] - yDiff;

		(*vertexData)[3][0] = (*vertexData)[2][0] - xDiff;
		(*vertexData)[3][1] = (*vertexData)[0][1] + yDiff;
	}

	static void setSelectionControlPoint(osg::Geode * selection, unsigned int controlPointIdx,
		const osgWidget::XYCoord & controlPointPos){

			osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
			osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

			setControlPoint((*vertexData)[controlPointIdx], controlPointPos);
	}

	static osgWidget::XYCoord getSelectionControlPoint(osg::Geode * selection, unsigned int controlPointIdx){

		osgWidget::XYCoord ret;

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(selection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		ret.x() = (*vertexData)[controlPointIdx].x();
		ret.y() = (*vertexData)[controlPointIdx].y();

		return ret;
	}

	static void setControlPoint(osg::Vec3f & cp, const osgWidget::XYCoord & pos){
		cp[0] = pos.x();
		cp[1] = pos.y();
	}

	static void refreshSelection(osg::DrawArrays * drArrys){
		drArrys->setFirst(0); 
		drArrys->setCount(4);
		drArrys->dirty();
	}

protected:

	static float selectionWidth;
};

class LinearConnectionFactory{
public:
	static void refreshConnection(osg::Geode * connection){
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

	/*static bool pointOnConnection(osg::Geode * connection, const osgWidget::XYCoord & pos){
		bool ret = false;

		std::pair<osgWidget::XYCoord,osgWidget::XYCoord> endings = getConnectionEndings(connection);
		osg::LineWidth * lineWidth = getConnectionWidth(connection);
		float width = lineWidth->getWidth();
		float halfWidth = width / 2.0f;

		if(endings.first.x() >= endings.second.x()){
			std::swap(endings.first, endings.second);
		}

		if(pos.x() >= endings.first.x()-halfWidth && pos.x() <= endings.second.x() + halfWidth){
			
		}

		return ret;
	}*/

	static void finalizeConnection(osg::Geode * connection){
		setConnectionWidthAndRefresh(connection, connectedWidth);
	}

	static osg::LineWidth * getConnectionWidth(osg::Geode * connection){
		return dynamic_cast<osg::LineWidth*>(connection->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINEWIDTH));
	}

	static void setConnectionWidth(osg::Geode * connection, float width){
		getConnectionWidth(connection)->setWidth(width);
	}

	static void setConnectionWidthAndRefresh(osg::Geode * connection, float width){
		getConnectionWidth(connection)->setWidth(width);
		refreshConnection(connection);
	}

	static osgWidget::Color getConnectionColor(osg::Geode * connection){
		return *(dynamic_cast<osg::Vec4Array*>(dynamic_cast<osg::Geometry*>(connection->getDrawable(0))->getColorArray())->begin());
	}

	static void setConnectionColor(osg::Geode * connection, const osgWidget::Color & color){
		*(dynamic_cast<osg::Vec4Array*>(dynamic_cast<osg::Geometry*>(connection->getDrawable(0))->getColorArray())->begin()) = color;
	}

	static void setConnectionColorAndRefresh(osg::Geode * connection, const osgWidget::Color & color){
		setConnectionColor(connection, color);
		refreshConnection(connection);
	}

	static osg::Geode * createVisualConnection(const osgWidget::Color & color = osgWidget::Color(0.38,0.76,0.82,1), float width = connectingWidth, float z = 0){
			
		osg::Geode * geode = new osg::Geode();
		geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(width), osg::StateAttribute::ON);
				
		osg::Geometry* geom = new osg::Geometry();
		geom->setUseDisplayList(false);
		osg::DrawArrays* drawArrayLines = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP); 
		geom->addPrimitiveSet(drawArrayLines); 
		osg::Vec3Array* vertexData = new osg::Vec3Array(2, &osg::Vec3(0,0,z));
		geom->setVertexArray(vertexData); 

		osg::Vec4Array* c = new osg::Vec4Array();
		c->push_back( color );
		geom->setColorArray( c );
		geom->setColorBinding( osg::Geometry::BIND_OVERALL );

		geode->addDrawable( geom );
		return geode;
	}

	static void setConnectionStart(osg::Geode * connection, const osgWidget::XYCoord & startPos){
		setConnectionControlPoint(connection, 0, startPos);
	}

	static void setConnectionStartAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & startPos){
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[0], startPos);

		refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

	static osgWidget::XYCoord getConnectionStart(osg::Geode * connection){
		return getConnectionControlPoint(connection,0);
	}

	static osgWidget::XYCoord getConnectionEnd(osg::Geode * connection){
		return getConnectionControlPoint(connection,0);
	}

	static std::pair<osgWidget::XYCoord, osgWidget::XYCoord> getConnectionEndings(osg::Geode * connection){
		std::pair<osgWidget::XYCoord, osgWidget::XYCoord> ret;

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		ret.first.x() = (*vertexData)[0].x();
		ret.first.y() = (*vertexData)[0].y();

		ret.second.x() = (*vertexData)[1].x();
		ret.second.y() = (*vertexData)[1].y();

		return ret;
	}

	static void setConnectionEnd(osg::Geode * connection, const osgWidget::XYCoord & endPos){
		setConnectionControlPoint(connection, 1, endPos);
	}

	static void setConnectionEndAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & endPos){
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[1], endPos);

		refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

	static void setConnectionEndings(osg::Geode * connection, const osgWidget::XYCoord & startPos,
		const osgWidget::XYCoord & endPos){

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[0], startPos);
		setControlPoint((*vertexData)[1], endPos);
	}

	static void setConnectionEndingsAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & startPos,
		const osgWidget::XYCoord & endPos){

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[0], startPos);
		setControlPoint((*vertexData)[1], endPos);

		refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

	static void setConnectionEndingsAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & startPos,
		const osgWidget::XYCoord & endPos, osgWidget::point_type z){

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[0], startPos);
		setControlPoint((*vertexData)[1], endPos);

		for(unsigned int i = 0; i < vertexData->size(); i++){
			(*vertexData)[0][3] = z;
		}

		refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

protected:

	static void setConnectionControlPoint(osg::Geode * connection, unsigned int controlPointIdx,
		const osgWidget::XYCoord & controlPointPos){

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[controlPointIdx], controlPointPos);
	}

	static osgWidget::XYCoord getConnectionControlPoint(osg::Geode * connection, unsigned int controlPointIdx){

		osgWidget::XYCoord ret;

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		ret.x() = (*vertexData)[controlPointIdx].x();
		ret.y() = (*vertexData)[controlPointIdx].y();

		return ret;
	}

	static void setControlPoint(osg::Vec3f & cp, const osgWidget::XYCoord & pos){
		cp[0] = pos.x();
		cp[1] = pos.y();
	}

	static void refreshConnection(osg::DrawArrays * drArrys){
		drArrys->setFirst(0); 
		drArrys->setCount(2);
		drArrys->dirty();
	}

protected:

	static float connectingWidth;
	static float connectedWidth;
};


class BezierConnectionFactory{
public:
	
	static void refreshConnection(osg::Geode * connection){
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));

		//recalculate points
		regenerateInterPoints(connection);
		refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

	static void setConnectionZ(osg::Geode * connection, float z){
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		for(unsigned int i = 0; i < vertexData->size(); i++){
			(*vertexData)[i][2] = z;
		}
	}

	static void setConnectionZAndRefresh(osg::Geode * connection, float z){
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		for(unsigned int i = 0; i < vertexData->size(); i++){
			(*vertexData)[i][2] = z;
		}
	}

	static void finalizeConnection(osg::Geode * connection, float z){
		
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		for(unsigned int i = 0; i < vertexData->size(); i++){
			(*vertexData)[i][2] = z;
		}

		setConnectionWidthAndRefresh(connection, connectedWidth);
	}

	static void setOutDir(const osgWidget::XYCoord & outDir){
		BezierConnectionFactory::dirOut = outDir;
	}

	static const osgWidget::XYCoord & getOutDir(){
		return BezierConnectionFactory::dirOut;
	}

	static void setOnDir(const osgWidget::XYCoord & inDir){
		BezierConnectionFactory::dirIn = inDir;
	}

	static const osgWidget::XYCoord & getInDir(){
		return BezierConnectionFactory::dirIn;
	}

	static void setInterpSteps(unsigned int interpSteps){
		BezierConnectionFactory::interpCount = interpSteps;
	}

	static unsigned int getInterpSteps(){
		return BezierConnectionFactory::interpCount;
	}

	static void setCoeffMul(float coeffMul){
		BezierConnectionFactory::coeffMul = coeffMul;
	}

	static float getCoeffMul(){
		return BezierConnectionFactory::coeffMul;
	}

	static osg::LineWidth * getConnectionWidth(osg::Geode * connection){
		return dynamic_cast<osg::LineWidth*>(connection->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINEWIDTH));
	}

	static void setConnectionWidth(osg::Geode * connection, float width){
		getConnectionWidth(connection)->setWidth(width);
	}

	static void setConnectionWidthAndRefresh(osg::Geode * connection, float width){
		setConnectionWidth(connection,width);
		refreshConnection(connection);
	}

	static osgWidget::Color getConnectionColor(osg::Geode * connection){
		return *(dynamic_cast<osg::Vec4Array*>(dynamic_cast<osg::Geometry*>(connection->getDrawable(0))->getColorArray())->begin());
	}

	static void setConnectionColor(osg::Geode * connection, const osgWidget::Color & color){
		*(dynamic_cast<osg::Vec4Array*>(dynamic_cast<osg::Geometry*>(connection->getDrawable(0))->getColorArray())->begin()) = color;
	}

	static void setConnectionColorAndRefresh(osg::Geode * connection, const osgWidget::Color & color){
		setConnectionColor(connection, color);
		refreshConnection(connection);
	}

	static osg::Geode * createVisualConnection(const osgWidget::Color & color = osgWidget::Color(0.1,0.1,0.1,1)/*osgWidget::Color(0.38,0.76,0.82,1)*/, float width = connectingWidth, float z = 0){
		static osg::Vec3 defCoords(0,0,0);

		osg::Geode * geode = new osg::Geode();
		geode->getOrCreateStateSet()->setAttribute(new osg::LineWidth(width), osg::StateAttribute::ON);
				
		osg::Geometry* geom = new osg::Geometry();
		geom->setUseDisplayList(false);
		osg::DrawArrays* drawArrayLines = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP); 
		geom->addPrimitiveSet(drawArrayLines);
		defCoords.z() = z;
		osg::Vec3Array* vertexData = new osg::Vec3Array();
		vertexData->resize((unsigned int)interpCount, defCoords);
		geom->setVertexArray(vertexData); 

		osg::Vec4Array* c = new osg::Vec4Array();
		c->push_back( color );
		geom->setColorArray( c );
		geom->setColorBinding( osg::Geometry::BIND_OVERALL );

		geode->addDrawable( geom );
		return geode;
	}

	static void setConnectionStart(osg::Geode * connection, const osgWidget::XYCoord & startPos){
		setConnectionControlPoint(connection, 0, startPos);
	}

	static void setConnectionStartAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & startPos){
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		
		setConnectionControlPoint(connection, 0, startPos);

		refreshConnection(connection);
	}

	static osgWidget::XYCoord getConnectionStart(osg::Geode * connection){
		return getConnectionControlPoint(connection,0);
	}

	static osgWidget::XYCoord getConnectionEnd(osg::Geode * connection){
		return getConnectionControlPoint(connection,interpCount-1);
	}

	static std::pair<osgWidget::XYCoord, osgWidget::XYCoord> getConnectionEndings(osg::Geode * connection){
		std::pair<osgWidget::XYCoord, osgWidget::XYCoord> ret;

		ret.first = getConnectionStart(connection);
		ret.second = getConnectionEnd(connection);

		return ret;
	}

	static void setConnectionEnd(osg::Geode * connection, const osgWidget::XYCoord & endPos){
		setConnectionControlPoint(connection, interpCount-1, endPos);
	}

	static void setConnectionEndAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & endPos){
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		
		setConnectionControlPoint(connection, interpCount-1, endPos);

		refreshConnection(connection);
	}

	static void setConnectionEndings(osg::Geode * connection, const osgWidget::XYCoord & startPos,
		const osgWidget::XYCoord & endPos){

			setConnectionStart(connection, startPos);
			setConnectionEnd(connection, endPos);
	}

	static void setConnectionEndingsAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & startPos,
		const osgWidget::XYCoord & endPos){

		setConnectionEndings(connection, startPos, endPos);

		refreshConnection(connection);
	}

	static void setConnectionEndingsAndRefresh(osg::Geode * connection, const osgWidget::XYCoord & startPos,
		const osgWidget::XYCoord & endPos, osgWidget::point_type z){

		setConnectionEndings(connection, startPos, endPos);

		regenerateInterPoints(connection);

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		for(unsigned int i = 0; i < vertexData->size(); i++){
			(*vertexData)[i][2] = z;
		}
		
		refreshConnection(dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0)));
	}

protected:

	static void regenerateInterPoints(osg::Geode * connection){
		//initialize interpolator
		osgAnimation::Vec2CubicBezierInterpolator interp;

		osgWidget::XYCoord start = getConnectionStart(connection);
		osgWidget::XYCoord end = getConnectionEnd(connection);

		float dist = std::sqrt(std::pow(start.x() - end.x(), 2) + std::pow(start.y() - end.y(), 2));
		float coeff = coeffMul;

		if(dist < coeffMul){
			coeff /= 2.0;
		}

		osgWidget::XYCoord out = dirOut * coeff + start;
		osgWidget::XYCoord in = dirIn * coeff + end;

		osgAnimation::TemplateKeyframeContainer<osgAnimation::Vec2CubicBezier> keyFrames;
		keyFrames.push_back(osgAnimation::Vec2CubicBezierKeyframe(0, osgAnimation::Vec2CubicBezier(start, out, in)));
		keyFrames.push_back(osgAnimation::Vec2CubicBezierKeyframe(1, osgAnimation::Vec2CubicBezier(end)));

		//initialize control points
		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		osg::Vec2 res;

		for(unsigned int i = 1; i < interpCount-1; i++){
			interp.getValue(keyFrames, (float)i / (float)interpCount, res);
			(*vertexData)[i].x() = res.x();
			(*vertexData)[i].y() = res.y();
		}
	}

	static void setConnectionControlPoint(osg::Geode * connection, unsigned int controlPointIdx,
		const osgWidget::XYCoord & controlPointPos){

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		setControlPoint((*vertexData)[controlPointIdx], controlPointPos);
	}

	static osgWidget::XYCoord getConnectionControlPoint(osg::Geode * connection, unsigned int controlPointIdx){

		osgWidget::XYCoord ret;

		osg::Geometry * geom = dynamic_cast<osg::Geometry*>(connection->getDrawable(0));
		osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());

		ret.x() = (*vertexData)[controlPointIdx].x();
		ret.y() = (*vertexData)[controlPointIdx].y();

		return ret;
	}

	static void setControlPoint(osg::Vec3f & cp, const osgWidget::XYCoord & pos){
		cp[0] = pos.x();
		cp[1] = pos.y();
	}

	static void refreshConnection(osg::DrawArrays * drArrys){
		drArrys->setFirst(0); 
		drArrys->setCount(interpCount);
		drArrys->dirty();
	}

	static osgWidget::XYCoord dirOut;
	static osgWidget::XYCoord dirIn;
	static unsigned int interpCount;
	static float coeffMul;

	static float connectingWidth;
	static float connectedWidth;
};

template<class VNode, class VPin, class ConnectionFactory = BezierConnectionFactory,
	class SelectionFactory = LinearSelectionFactory>
class osgVDFModel :	public osgVDFBaseModel
{
	BOOST_STATIC_ASSERT((boost::is_base_of<osgVDFBaseNode, VNode>::value));
	BOOST_STATIC_ASSERT((boost::is_base_of<osgVDFBasePin, VPin>::value));

public:
	osgVDFModel(osgViewer::View* view = 0, osgWidget::point_type width = 0.0f, osgWidget::point_type height = 0.0f,
		unsigned int nodeMask = 0, unsigned int flags = 0);

	~osgVDFModel(void);


protected:

	virtual osg::ref_ptr<osgVDFBaseNode> createVisualNode(dflm::NPtr node, osg::ref_ptr<osg::Image> image = osg::ref_ptr<osg::Image>(),
		const std::string &  name = std::string());

	//user interaction - connection managment
	virtual osg::ref_ptr<osg::Geode> createVisualConnection(osgVDFBasePin * pinA, osgVDFBasePin * pinB);
	virtual osg::ref_ptr<osg::Geode> startVisualConnection(osgVDFBasePin * startPin);
	virtual void setVisualConnectionEnd(osg::Geode * connection, const osgWidget::XYCoord & endPos);
	virtual void setVisualConnectionStart(osg::Geode * connection, const osgWidget::XYCoord & startPos);
	virtual void setVisualConnectionZ(osg::Geode * connection, osgWidget::point_type z);
	virtual void setVisualConnectionColor(osg::Geode * connection, const osgWidget::Color & color);

	//user interaction - selection managment
	virtual osg::ref_ptr<osg::Geode> createVisualSelection(const osgWidget::XYCoord & startPos);
	virtual void continueVisulaSelection(osg::Geode * selection, const osgWidget::XYCoord & continuePos);		
};

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::osgVDFModel(osgViewer::View* view, osgWidget::point_type width,
	osgWidget::point_type height, unsigned int nodeMask, unsigned int flags)
	: osgVDFBaseModel(view, width, height, nodeMask, flags){

		BOOST_ASSERT((view != 0));
		view->getCamera()->setClearColor(osg::Vec4(0.3,0.3,0.3,1));
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::~osgVDFModel(void){

}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
osg::ref_ptr<osgVDFBaseNode> osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::createVisualNode(dflm::NPtr node, osg::ref_ptr<osg::Image> image,
	const std::string & name){

		osg::ref_ptr<osgVDFBaseNode> ret(new VNode(name, node, this, image));

		for(dflm::Node::PINS_SET::const_iterator it = node->getInPins().begin(); it != node->getInPins().end(); it++){
			osgVDFBasePin * pin = new VPin((*it)->getPinName(), *it, this);
			ret->addInPin(pin, (*it)->getPinName());
		}

		for(dflm::Node::PINS_SET::const_iterator it = node->getOutPins().begin(); it != node->getOutPins().end(); it++){
			osgVDFBasePin * pin = new VPin((*it)->getPinName(), *it, this);
			ret->addOutPin(pin, (*it)->getPinName());
		}

		return ret;
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
osg::ref_ptr<osg::Geode> osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::createVisualConnection(osgVDFBasePin * pinA, osgVDFBasePin * pinB){
	osg::ref_ptr<osg::Geode> ret(ConnectionFactory::createVisualConnection());
	ConnectionFactory::setConnectionEndingsAndRefresh(ret, pinA->getCenterPosition(), pinB->getCenterPosition());
	return ret;
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
osg::ref_ptr<osg::Geode> osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::startVisualConnection(osgVDFBasePin * startPin){
	osg::ref_ptr<osg::Geode> ret(ConnectionFactory::createVisualConnection());
	ConnectionFactory::setConnectionStart(ret, startPin->getCenterPosition());
	return ret;
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
void osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::setVisualConnectionEnd(osg::Geode * connection, const osgWidget::XYCoord & endPos){
	ConnectionFactory::setConnectionEndAndRefresh(connection, endPos);
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
void osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::setVisualConnectionStart(osg::Geode * connection, const osgWidget::XYCoord & startPos){
	ConnectionFactory::setConnectionStartAndRefresh(connection, startPos);
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
void osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::setVisualConnectionZ(osg::Geode * connection, osgWidget::point_type z){
	ConnectionFactory::setConnectionZAndRefresh(connection, z);
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
void osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::setVisualConnectionColor(osg::Geode * connection, const osgWidget::Color & color){
	ConnectionFactory::setConnectionColorAndRefresh(connection, color);
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
osg::ref_ptr<osg::Geode> osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::createVisualSelection(const osgWidget::XYCoord & startPos){
	osg::ref_ptr<osg::Geode> ret(SelectionFactory::createSelection());
	SelectionFactory::setSelectionStart(ret, startPos);
	return ret;
}

template<class VNode, class VPin, class ConnectionFactory, class SelectionFactory>
void osgVDFModel<VNode, VPin, ConnectionFactory, SelectionFactory>::continueVisulaSelection(osg::Geode * selection, const osgWidget::XYCoord & continuePos){
	SelectionFactory::setSelectionEndAndRefresh(selection, continuePos);
}

}

#endif
