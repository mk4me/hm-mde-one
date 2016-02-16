#include <dataaccessorlib/FunctionFeature.h>

using namespace dataaccessor;

//! \tparam IsFun Informacja czy dane przedstawiają funkcję czy nie
template<bool IsFunc>
class FunctionFeatureT : public IFunctionFeature
{
public:
	//! konstruktor domyślny
	FunctionFeatureT() {}

	virtual ~FunctionFeatureT() {}
	//! \return Czy funkcja jest parzysta
	virtual bool isFunction() const override final { return IsFunc; }
};

const utils::shared_ptr<IFunctionFeature> FunctionFeature::functionFeature = utils::make_shared<FunctionFeatureT<true>>();
const utils::shared_ptr<IFunctionFeature> FunctionFeature::nonFunctionFeature = utils::make_shared<FunctionFeatureT<false>>();