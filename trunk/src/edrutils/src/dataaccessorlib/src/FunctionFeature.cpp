#include <dataaccessorlib/FunctionFeature.h>

using namespace dataaccessor;

const utils::shared_ptr<IFunctionFeature> FunctionFeature::functionFeature = utils::make_shared<FunctionFeatureT<true>>();
const utils::shared_ptr<IFunctionFeature> FunctionFeature::nonFunctionFeature = utils::make_shared<FunctionFeatureT<false>>();