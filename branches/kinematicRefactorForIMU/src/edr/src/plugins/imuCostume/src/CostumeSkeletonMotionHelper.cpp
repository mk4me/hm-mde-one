#include "PCH.h"
#include "CostumeSkeletonMotionHelper.h"
#include <chrono>
#include <thread>
#include <QtWidgets/QProgressDialog>
#include <corelib/ILog.h>
#include <corelib/IPlugin.h>

void estimate(IMU::IIMUDataSource::OrientationEstimationAlgorithmsMapping & mapping,
	const IMU::SensorsData & sensorsData, const double deltaT)
{
	for (auto & d : sensorsData)
	{
		mapping.find(d.first)->second->estimate(d.second.accelerometer, d.second.gyroscope, d.second.magnetometer, deltaT);
	}
}

CostumeSkeletonMotionHelper::CostumeSkeletonMotionHelper(SensorsStreamPtr sensorsStream,
	IMU::IIMUDataSource::CostumeProfileInstance * costumeProfile,
	const unsigned int maxSamples,
	const unsigned int calibratinStageChangeValue, QWidget * parent)
	: sensorsStream(sensorsStream), costumeProfile(costumeProfile),
	observer(new threadingUtils::ResetableStreamStatusObserver),
	calibratinStageChangeValue(calibratinStageChangeValue), previousTime(0), first(true)
{
	sensorsStream->attachObserver(observer);
	pd = new QProgressDialog(parent);
	pd->setWindowTitle(tr("Costume initialization"));
	pd->setRange(0, maxSamples);
	pd->setValue(0);
	pd->setLabelText(tr("Orientation estimation algorithms"));
	connect(pd, SIGNAL(canceled()), this, SLOT(cancel()));
	connect(&timer, SIGNAL(timeout()), this, SLOT(perform()));
}

CostumeSkeletonMotionHelper::~CostumeSkeletonMotionHelper()
{
	sensorsStream->detachObserver(observer);
}

int CostumeSkeletonMotionHelper::exec()
{
	timer.start(0);
	return pd->exec();
}

void CostumeSkeletonMotionHelper::perform()
{
	PLUGIN_LOG_DEBUG("Performing data check");
	if (observer->modified() == true){
		PLUGIN_LOG_DEBUG("New data processing");
		IMU::SensorsStreamData data;
		sensorsStream->data(data);
		double deltaTime = deltaTime = (data.timestamp > previousTime) ? (data.timestamp - previousTime) : (std::numeric_limits<imuCostume::CostumeCANopenIO::Timestamp>::max() - previousTime + data.timestamp);
		if (first == true){
			first = false;
			deltaTime = 0.0;
		}
		
		if (pd->value() == calibratinStageChangeValue) {
			pd->setLabelText(tr("Calibrating"));
		}

		if (pd->value() < calibratinStageChangeValue){
			estimate(costumeProfile->sensorsOrientationEstimationAlgorithms, data.sensorsData, deltaTime);
		} else{
			bool ret = costumeProfile->calibrationAlgorithm->calibrate(data.sensorsData, deltaTime);
			if (ret == true){
				pd->setValue(pd->maximum());
			}
		}

		pd->setValue(pd->value() + 1);

		previousTime = data.timestamp;

		if (pd->value() == pd->maximum()){
			cancel();
			pd->accept();
		}
	}
}

void CostumeSkeletonMotionHelper::cancel()
{
	timer.stop();
}