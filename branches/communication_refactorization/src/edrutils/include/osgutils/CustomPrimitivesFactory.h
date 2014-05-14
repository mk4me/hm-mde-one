/********************************************************************
    created:  2013/12/13
    created:  13:12:2013   7:33
    filename: CustomPrimitivesFactory.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_OSG_UTILS__CUSTOMPRIMITIVESFACTORY_H__
#define HEADER_GUARD_OSG_UTILS__CUSTOMPRIMITIVESFACTORY_H__

#include <utils/Debug.h>
#include <boost/type_traits.hpp>
#include <osg/StateSet>
#include <osg/Geometry>

namespace osgutils
{

	struct GeometryBase
	{
		osg::ref_ptr<osg::Vec3Array> verticies;
		osg::ref_ptr<osg::Vec3Array> normals;
	};

	struct CustonGeometryDescriptionBase
	{		
		osg::ref_ptr<osg::Geometry> geom;
		osg::ref_ptr<osg::Vec4Array> colors;
		GeometryBase geomBase;
		unsigned int complex;
	};

	struct CustomCylinderDescription : public CustonGeometryDescriptionBase
	{
		double height;
		double radius;
	};

	struct CustomSphereDescription : public CustonGeometryDescriptionBase
	{
		double radius;
	};

	class CustomPrimitivesFactory
	{
	public:
		//! Metoda tworzy w�ze� z cylindrem
		//! \param complex z�o�ono�� cylindra (l. wierzcho�k�w podstawy)
		//! \param height d�ugo�� / wysoko�� cylindra
		//! \param radius promie� cylindra
		//! \param color kolor cylindra
		//! \return w�ze� z nowo utworzonym cylindrem
		static const CustomCylinderDescription createCylinder( const unsigned int complex,
			const double height,
			const double radius,
			const osg::Vec4 & color );

		//! Tworzy geometri� ze sfera
		//! \param complex z�o�ono�� sfery
		//! \param color kolor sfery
		//! \return wska�nik do stworzonej geometrii
		static const CustomSphereDescription createSphere( const unsigned int complex,
			const double radius,
			const osg::Vec4 & color );

	};


	//! Klasa z metodami pomocniczymy przy tworzeniu modyfikacji prymityw�w
	//! Staraj� si� maxymalizowa� wykorzystanie dost�pnych zasob�w przez wsp�dzielenie ich
	//! Klonowania odbywaj� si� tylko tam gdzie jest to niezb�dne
	class CustomPrimitivesHelper
	{
	public:

		static void scaleSphereRadius(osg::Vec3Array * vertices,
			const double radiusScaleFactor);

		static void scaleCylinderRadius(osg::Vec3Array * vertices,
			const double radiusScaleFactor);

		static void scaleCylinderHeight(osg::Vec3Array * vertices,
			const double radiusScaleFactor);

		static void scaleCylinderHeightRadius(osg::Vec3Array * vertices,
			const double radiusScaleFactor,
			const double heightScaleFactor);

		static const GeometryBase createSphereGeometryBase(const double radius,
			const unsigned int complex);

		static osg::ref_ptr<osg::Vec3Array> recreateVerticesForRadius(const CustomSphereDescription & sphereDesription,
			const double radius);

		static const GeometryBase createCylinderGeometryBase(const double height,
			const double radius, const unsigned int complex);

		static osg::ref_ptr<osg::Vec3Array> recreateVerticesForRadius(const CustomCylinderDescription & cylinderDesription,
			const double radius);

		static osg::ref_ptr<osg::Vec3Array> recreateVerticesForHeight(const CustomCylinderDescription & cylinderDesription,
			const double height);

		static osg::ref_ptr<osg::Vec3Array> recreateVerticesForHeightRadius(const CustomCylinderDescription & cylinderDesription,
			const double height, const double radius);	

		static void setColor(CustonGeometryDescriptionBase & primitiveDescription, osg::Vec4Array * color);

		static void updatePrimitiveShape(CustonGeometryDescriptionBase & primitiveDescription, osg::Vec3Array * vertices);

		//! \tparam T Obiekt deskryptora pochodz�cego od CustonGeometryDescriptionBase
		//! \param primitiveDesription Deskryptor klonowany na potrzeby zmiany koloru
		//! \param color Nowy kolor prymitywu
		//! \return Deskryptor prymitywu ze zmienionym kolorem
		template<typename T>
		static const T cloneForColorChange(const T & primitiveDesription, const osg::Vec4 & color)
		{
			UTILS_STATIC_ASSERT((boost::is_base_of<CustonGeometryDescriptionBase,T>::value), "Klonowany obiekt musi by� pochodn� CustonGeometryDescriptionBase");

			T ret = primitiveDesription;
			ret.geom = dynamic_cast<osg::Geometry*>(ret.geom->clone(osg::CopyOp::SHALLOW_COPY));
			ret.colors = new osg::Vec3Array;
			ret.colors->push_back(color);
			ret.geom->setColorArray(ret.colors);

			return ret;
		}

		//! \param sphereDesription Deskryptor sfery klonowany na potrzeby zmiany promienia
		//! \param radius Nowy promie�
		//! \return Deskryptor prymitywu ze zmienionym promieniem
		static const CustomSphereDescription cloneForRadiusChange(const CustomSphereDescription & sphereDesription,
			const double radius);

		//! \param cylinderDesription Deskryptor cylindra klonowany na potrzeby zmiany promienia
		//! \param radius Nowy promie�
		//! \return Deskryptor prymitywu ze zmienionym promieniem
		static const CustomCylinderDescription cloneForRadiusChange(const CustomCylinderDescription & cylinderDesription,
			const double radius);

		//! \param cylinderDesription Deskryptor cylindra klonowany na potrzeby zmiany wysoko�ci
		//! \param height Nowa wysoko��
		//! \return Deskryptor prymitywu ze zmienion� wysoko�ci�
		static const CustomCylinderDescription cloneForHeightChange(const CustomCylinderDescription & cylinderDesription,
			const double height);
	};
}

#endif	//	HEADER_GUARD_OSG_UTILS__CUSTOMPRIMITIVESFACTORY_H__
