/********************************************************************
	created:	2014/07/27
	created:	16:27
	filename: 	PythonDataChannel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__PYTHONDATACHANNEL_H__
#define HEADER_GUARD_PYTHON__PYTHONDATACHANNEL_H__

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <plugins/c3d/C3DChannels.h>

namespace python {
	class PythonDataChannel 
	{
	public:
		static PythonDataChannel convert(c3dlib::VectorChannelReaderInterfaceConstPtr channel);
		static c3dlib::VectorChannelReaderInterfacePtr convert(const PythonDataChannel& obj);

	public:
		PythonDataChannel();
		virtual ~PythonDataChannel();

	public:
		std::vector<osg::Vec3> getData() const;
		float getFrequency() const ;
		std::string getName() const;

		void setData(const std::vector<osg::Vec3>& data);
		void setFrequency(float frq);
		void setName(const std::string& name);

	private:
		std::vector<osg::Vec3> data;
		float frequency;
		std::string name;
	};
	DEFINE_SMART_POINTERS(PythonDataChannel);
}

#endif // HEADER_GUARD 
