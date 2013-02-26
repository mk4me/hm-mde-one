/********************************************************************
	created:	2013/02/22
	created:	22:2:2013   11:38
	filename: 	DataSink.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__DATASINK_H__
#define HEADER_GUARD_NEWVDF__DATASINK_H__

#include <plugins/newVdf/IDataSink.h>
#include <plugins/newVdf/Export.h>

namespace df { class ISinkNode; }

namespace vdf {

class PLUGIN_NEWVDF_EXPORT DataSink : public IDataSink
{
public:
	typedef boost::function<df::ISinkNode* (const df::ISinkNode*)> Creator;
	DataSink(df::ISinkNode* impl, core::UniqueID id, const std::string& name, Creator creator );
	//! \return Nazwa elementu logicznego
	virtual const std::string getName() const;;
	virtual df::INode* getPrototype() const;
	virtual df::INode* create() const;
	virtual core::UniqueID getID() const;
	vdf::IWorkflowItemBase* createClone(void) const;
	virtual const std::string& getDescription() const;
	virtual void getInputInfo( std::vector<InputInfo>& info ) const;

	QDialog * getOutputConfigurationDialog(void);
	bool  empty(void) const;
	QIcon*  getIcon(void) const;

private:
	df::ISinkNode* _impl;
	std::string _desc;
	std::string _name;
	core::UniqueID _id;
	Creator _creator;
};
typedef core::shared_ptr<DataSink> DataSinkPtr;
typedef core::shared_ptr<const DataSink> DataSinkConstPtr;

}

#endif
