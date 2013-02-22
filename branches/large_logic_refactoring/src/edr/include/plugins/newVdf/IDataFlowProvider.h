/********************************************************************
	created:	2013/02/22
	created:	22:2:2013   10:42
	filename: 	IDataFlowProvider.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__IDATAFLOWPROVIDER_H__
#define HEADER_GUARD_NEWVDF__IDATAFLOWPROVIDER_H__

#include <list>
#include <plugins/newVdf/IWorkflowItemBase.h>
#include <plugins/newVdf/DataProcessor.h>
#include <plugins/newVdf/DataSource.h>
#include <plugins/newVdf/DataSink.h>

namespace df { class IProcessingNode; class ISourceNode; class ISinkNode; };

//class _DataProcessor : public core::IDataProcessor
//{
//public:
//	typedef boost::function<df::ISourceNode* (void)> Creator;
//	_DataProcessor(df::IProcessingNode* impl, core::UniqueID id, const std::string& name, Creator creator) :
//	_impl(impl), _id(id), _name(name), _creator(creator) {}
//	//! \return Nazwa elementu logicznego
//	virtual const std::string getName() const { return _name; };
//	virtual void getOutputInfo( std::vector<OutputInfo> & output ) const 
//	{
//		int size = _impl->outputSize();
//		for (int i = 0; i < size; i++) {
//			OutputInfo pin;
//			pin.name = "out" + boost::lexical_cast<std::string>(i);
//			output.push_back(pin);
//		}
//	}
//
//	virtual core::UniqueID getID() const { return _id; }
//	core::IWorkflowItemBase* createClone(void) const { UTILS_ASSERT(false); return nullptr; }
//	virtual const std::string& getDescription() const {  return _desc; }
//	virtual df::INode* getPrototype() const { return _impl; }
//	virtual df::INode* create() const { return _creator(); }
//	virtual void getInputInfo( std::vector<InputInfo>& info ) const 
//	{
//		int size = _impl->inputSize();
//		for (int i = 0; i < size; i++) {
//			InputInfo pin;
//			pin.name = "in" + boost::lexical_cast<std::string>(i);
//			info.push_back(pin);
//		}
//	}
//
//private:
//	df::IProcessingNode* _impl;
//	std::string _desc;
//	std::string _name;
//	core::UniqueID _id;
//	Creator _creator;
//};

//class _DataSource : public core::IDataSource
//{
//public:
//	typedef boost::function<df::ISourceNode* (void)> Creator;
//	_DataSource(df::ISourceNode* impl, core::UniqueID id, const std::string& name, Creator creator) :
//	_impl(impl), _id(id), _name(name), _creator(creator) {}
//	//! \return Nazwa elementu logicznego
//	virtual const std::string getName() const { return _name; };
//	virtual df::INode* getPrototype() const { return _impl; }
//	virtual df::INode* create() const { return _creator(); }
//	virtual void getOutputInfo( std::vector<OutputInfo> & output ) const
//	{
//		int size = _impl->outputSize();
//		for (int i = 0; i < size; i++) {
//			OutputInfo pin;
//			pin.name = "out" + boost::lexical_cast<std::string>(i);
//			output.push_back(pin);
//		}
//	}
//
//	virtual QDialog * getOutputConfigurationDialog() { return nullptr; }
//	virtual bool empty() const{ UTILS_ASSERT(false); return false; }
//	//! \return Ikona źródła
//	virtual QIcon* getIcon() const { return nullptr; }
//	virtual core::UniqueID getID() const {  return _id; }
//	core::IWorkflowItemBase* createClone(void) const { UTILS_ASSERT(false); return nullptr; }
//	virtual const std::string& getDescription() const { UTILS_ASSERT(false); return _desc;}
//
//private:
//	df::ISourceNode* _impl;
//	std::string _desc;
//	std::string _name;
//	core::UniqueID _id;
//	Creator _creator;
//};

//class _DataSink : public core::IDataSink
//{
//public:
//	typedef boost::function<df::ISinkNode* (void)> Creator;
//	_DataSink(df::ISinkNode* impl, core::UniqueID id, const std::string& name, Creator creator ) :
//	_impl(impl), _id(id), _name(name), _creator(creator) {}
//	//! \return Nazwa elementu logicznego
//	virtual const std::string getName() const { return _name; };
//	virtual df::INode* getPrototype() const { return _impl; }
//	virtual df::INode* create() const { return _creator(); }
//	virtual core::UniqueID getID() const { return _id; }
//	core::IWorkflowItemBase* createClone(void) const { UTILS_ASSERT(false); return nullptr; }
//	virtual const std::string& getDescription() const { return _desc; }
//	virtual void getInputInfo( std::vector<InputInfo>& info ) const 
//	{
//		int size = _impl->inputSize();
//		for (int i = 0; i < size; i++) {
//			InputInfo pin;
//			pin.name = "in" + boost::lexical_cast<std::string>(i);
//			info.push_back(pin);
//		}
//	}
//
//	QDialog * getOutputConfigurationDialog(void) { return nullptr; }
//	bool  empty(void) const { UTILS_ASSERT(false); return false;}
//	QIcon*  getIcon(void) const { UTILS_ASSERT(false); return nullptr; }
//
//private:
//	df::ISinkNode* _impl;
//	std::string _desc;
//	std::string _name;
//	core::UniqueID _id;
//	Creator _creator;
//};

#define VDF_BEGIN public: virtual void provideDataFlowItems(std::list<vdf::IWorkflowItemBasePtr>& items) {
#define VDF_END }

//! Dodaje elementu przetwarzającego zadanego typu do pluginu.
#define VDF_ADD_DATA_PROCESSOR(className, uid)                           \
	items.push_back( vdf::IDataProcessorPtr(new vdf::DataProcessor(new (className), (uid), (#className), [&]() { return new (className); })) );

//! Dodaje źródło zadanego typu do pluginu.
#define VDF_ADD_DATA_SOURCE(className, uid)                           \
	items.push_back( vdf::IDataSourcePtr(new vdf::DataSource(new (className), (uid), (#className), [&]() { return new (className); })) );

//! Dodaje sink zadanego typu do pluginu.
#define VDF_ADD_DATA_SINK(className, uid)                           \
	items.push_back( vdf::IDataSinkPtr(new vdf::DataSink(new (className), (uid), (#className), [&]() { return new (className); })) );


namespace vdf {

class IDataFlowProvider
{
public:
	virtual ~IDataFlowProvider() {}
	virtual void provideDataFlowItems(std::list<IWorkflowItemBasePtr>& items) = 0;
};
typedef core::shared_ptr<IDataFlowProvider> IDataFlowProviderPtr;
typedef core::shared_ptr<const IDataFlowProvider> IDataFlowProviderConstPtr;
}

#endif
