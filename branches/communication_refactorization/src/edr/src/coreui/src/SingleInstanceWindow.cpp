#include "CoreUiPCH.h"
#include <coreui/SingleInstanceWindow.h>
#include <QtCore/QSharedMemory>

using namespace coreUI;

class SingleInstanceWindow::SingleInstanceWindowImpl
{
public:
	//! TODO - postfix guida powinien byæ konfigurowalny z poziomu cmake
	SingleInstanceWindowImpl(const std::string & viewUniqueID)
		: sm(new QSharedMemory(QString::fromStdString(viewUniqueID) + "_{E3F0B695-7E6D-44BF-BDDF-603D2C33B1EF}"))
	{

	}

	~SingleInstanceWindowImpl()
	{
		if(sm->isAttached() == true){
			sm->detach();
		}
	}

	const bool isSingle() const
	{
		if(sm->attach(QSharedMemory::ReadOnly)){
			sm->detach();
			return false;
		}

		if(sm->create(1))
			return true;

		return false;
	}

private:

	mutable utils::shared_ptr<QSharedMemory> sm;
};

SingleInstanceWindow::SingleInstanceWindow(const std::string & viewUniqueID)
	: impl(new SingleInstanceWindowImpl(viewUniqueID))
{

}

SingleInstanceWindow::~SingleInstanceWindow()
{

}

const bool SingleInstanceWindow::isSingle() const
{
	return impl->isSingle();
}