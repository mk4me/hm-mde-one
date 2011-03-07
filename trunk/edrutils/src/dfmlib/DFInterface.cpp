#include <dfmlib/DFInterface.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

DFInterface::DFInterface(void) : objectEnable(false)
{
}


DFInterface::~DFInterface(void)
{
}

//bool DFInterface::setEnable(bool enable){
//	m_bEnable = enable;
//	notify();
//	return true;
//}

bool DFInterface::isEnable() const{
	return objectEnable;
}

void DFInterface::reset(){
}

void DFInterface::notify(){
	if(objectEnable == true){
		process();
	}
}

}