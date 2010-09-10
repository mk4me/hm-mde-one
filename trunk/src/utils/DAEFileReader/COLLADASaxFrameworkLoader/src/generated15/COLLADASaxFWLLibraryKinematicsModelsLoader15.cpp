/*
    Copyright (c) 2008-2009 NetAllied Systems GmbH

    This file is part of COLLADASaxFrameworkLoader.

    Licensed under the MIT Open Source License, 
    for details please see LICENSE file or the website
    http://www.opensource.org/licenses/mit-license.php
*/


#include "COLLADASaxFWLStableHeaders.h"
#include "COLLADASaxFWLLibraryKinematicsModelsLoader15.h"


namespace COLLADASaxFWL
{


bool LibraryKinematicsModelsLoader15::end__library_kinematics_models()
{
SaxVirtualFunctionTest15(end__library_kinematics_models());
return mLoader->end__library_kinematics_models();
}


bool LibraryKinematicsModelsLoader15::begin__kinematics_model( const COLLADASaxFWL15::kinematics_model__AttributeData& attributeData )
{
SaxVirtualFunctionTest15(begin__kinematics_model(attributeData));
COLLADASaxFWL::kinematics_model__AttributeData attrData;
attrData.id = attributeData.id;
attrData.name = attributeData.name;
return mLoader->begin__kinematics_model(attrData);
}


bool LibraryKinematicsModelsLoader15::end__kinematics_model()
{
SaxVirtualFunctionTest15(end__kinematics_model());
return mLoader->end__kinematics_model();
}


bool LibraryKinematicsModelsLoader15::begin__technique_common____kinematics_model_technique_type()
{
SaxVirtualFunctionTest15(begin__technique_common____kinematics_model_technique_type());
return mLoader->begin__technique_common____kinematics_model_technique_type();
}


bool LibraryKinematicsModelsLoader15::end__technique_common____kinematics_model_technique_type()
{
SaxVirtualFunctionTest15(end__technique_common____kinematics_model_technique_type());
return mLoader->end__technique_common____kinematics_model_technique_type();
}


bool LibraryKinematicsModelsLoader15::begin__newparam____kinematics_newparam_type( const COLLADASaxFWL15::newparam____kinematics_newparam_type__AttributeData& attributeData )
{
SaxVirtualFunctionTest15(begin__newparam____kinematics_newparam_type(attributeData));
COLLADASaxFWL::newparam____kinematics_newparam_type__AttributeData attrData;
attrData.sid = attributeData.sid;
return mLoader->begin__newparam____kinematics_newparam_type(attrData);
}


bool LibraryKinematicsModelsLoader15::end__newparam____kinematics_newparam_type()
{
SaxVirtualFunctionTest15(end__newparam____kinematics_newparam_type());
return mLoader->end__newparam____kinematics_newparam_type();
}


bool LibraryKinematicsModelsLoader15::begin__instance_joint( const COLLADASaxFWL15::instance_joint__AttributeData& attributeData )
{
SaxVirtualFunctionTest15(begin__instance_joint(attributeData));
COLLADASaxFWL::instance_joint__AttributeData attrData;
attrData.present_attributes = attributeData.present_attributes;
attrData.url = attributeData.url;
attrData.sid = attributeData.sid;
attrData.name = attributeData.name;
return mLoader->begin__instance_joint(attrData);
}


bool LibraryKinematicsModelsLoader15::end__instance_joint()
{
SaxVirtualFunctionTest15(end__instance_joint());
return mLoader->end__instance_joint();
}


bool LibraryKinematicsModelsLoader15::begin__link( const COLLADASaxFWL15::link__AttributeData& attributeData )
{
SaxVirtualFunctionTest15(begin__link(attributeData));
COLLADASaxFWL::link__AttributeData attrData;
attrData.sid = attributeData.sid;
attrData.name = attributeData.name;
return mLoader->begin__link(attrData);
}


bool LibraryKinematicsModelsLoader15::end__link()
{
SaxVirtualFunctionTest15(end__link());
return mLoader->end__link();
}


bool LibraryKinematicsModelsLoader15::begin__attachment_full( const COLLADASaxFWL15::attachment_full__AttributeData& attributeData )
{
SaxVirtualFunctionTest15(begin__attachment_full(attributeData));
COLLADASaxFWL::attachment_AttributeData attrData;
attrData.joint = attributeData.joint;
return mLoader->begin__attachment_full(attrData);
}


bool LibraryKinematicsModelsLoader15::end__attachment_full()
{
SaxVirtualFunctionTest15(end__attachment_full());
return mLoader->end__attachment_full();
}


bool LibraryKinematicsModelsLoader15::begin__attachment_start( const COLLADASaxFWL15::attachment_start__AttributeData& attributeData )
{
SaxVirtualFunctionTest15(begin__attachment_start(attributeData));
COLLADASaxFWL::attachment_AttributeData attrData;
attrData.joint = attributeData.joint;
return mLoader->begin__attachment_start(attrData);
}


bool LibraryKinematicsModelsLoader15::end__attachment_start()
{
SaxVirtualFunctionTest15(end__attachment_start());
return mLoader->end__attachment_start();
}


bool LibraryKinematicsModelsLoader15::begin__attachment_end( const COLLADASaxFWL15::attachment_end__AttributeData& attributeData )
{
SaxVirtualFunctionTest15(begin__attachment_end(attributeData));
COLLADASaxFWL::attachment_AttributeData attrData;
attrData.joint = attributeData.joint;
return mLoader->begin__attachment_end(attrData);
}


bool LibraryKinematicsModelsLoader15::end__attachment_end()
{
SaxVirtualFunctionTest15(end__attachment_end());
return mLoader->end__attachment_end();
}


bool LibraryKinematicsModelsLoader15::begin__translate( const COLLADASaxFWL15::translate__AttributeData& attributeData )
{
SaxVirtualFunctionTest15(begin__translate(attributeData));
COLLADASaxFWL::translate__AttributeData attrData;
attrData.sid = attributeData.sid;
return mLoader->begin__translate(attrData);
}


bool LibraryKinematicsModelsLoader15::end__translate()
{
SaxVirtualFunctionTest15(end__translate());
return mLoader->end__translate();
}


bool LibraryKinematicsModelsLoader15::data__translate( const float* data, size_t length )
{
SaxVirtualFunctionTest15(data__translate(data, length));
return mLoader->data__translate(data, length);
}


bool LibraryKinematicsModelsLoader15::begin__rotate( const COLLADASaxFWL15::rotate__AttributeData& attributeData )
{
SaxVirtualFunctionTest15(begin__rotate(attributeData));
COLLADASaxFWL::rotate__AttributeData attrData;
attrData.sid = attributeData.sid;
return mLoader->begin__rotate(attrData);
}


bool LibraryKinematicsModelsLoader15::end__rotate()
{
SaxVirtualFunctionTest15(end__rotate());
return mLoader->end__rotate();
}


bool LibraryKinematicsModelsLoader15::data__rotate( const float* data, size_t length )
{
SaxVirtualFunctionTest15(data__rotate(data, length));
return mLoader->data__rotate(data, length);
}


bool LibraryKinematicsModelsLoader15::begin__technique____technique_type( const COLLADASaxFWL15::technique____technique_type__AttributeData& attributeData )
{
SaxVirtualFunctionTest15(begin__technique____technique_type(attributeData));
COLLADASaxFWL::technique__AttributeData attrData;
attrData.profile = attributeData.profile;
return mLoader->begin__technique(attrData);
}


bool LibraryKinematicsModelsLoader15::end__technique____technique_type()
{
SaxVirtualFunctionTest15(end__technique____technique_type());
return mLoader->end__technique();
}



}
