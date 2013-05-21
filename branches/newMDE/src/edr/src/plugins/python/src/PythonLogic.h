/********************************************************************
	created:	2012/12/05
	created:	5:12:2012   10:20
	filename: 	PythonLogic.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__PYTHONLOGIC_H__
#define HEADER_GUARD_PYTHON__PYTHONLOGIC_H__

#include <boost/python.hpp>
#include <map>
class PythonLogic
{
public:
    PythonLogic();
	virtual ~PythonLogic();

public:
    void run( const std::string& script);
    boost::python::object& getDict();

private:
    boost::python::object mainModule;
    boost::python::object mainNamespace;
};
typedef core::shared_ptr<PythonLogic> PythonLogicPtr;
typedef core::shared_ptr<const PythonLogic> PythonLogicConstPtr;


#endif
