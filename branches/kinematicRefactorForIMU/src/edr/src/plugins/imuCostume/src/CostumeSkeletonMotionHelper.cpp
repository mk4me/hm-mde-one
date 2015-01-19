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
	for (auto & m : mapping)
	{
		const auto & d = sensorsData.find(m.first)->second;
		m.second->estimate(d.accelerometer, d.gyroscope, d.magnetometer, deltaT);
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
	delete pd;
}

int CostumeSkeletonMotionHelper::exec()
{
	timer.start(0);
	pd->exec();

	return pd->wasCanceled() == true ? QDialog::Rejected : QDialog::Accepted;
}

void CostumeSkeletonMotionHelper::perform()
{
	//PLUGIN_LOG_DEBUG("Performing data check");
	if (observer->modified() == true){
		//PLUGIN_LOG_DEBUG("New data processing");
		IMU::SensorsStreamData data;
		sensorsStream->data(data);
		double deltaTime = (data.timestamp > previousTime) ? (data.timestamp - previousTime) : (std::numeric_limits<imuCostume::CostumeCANopenIO::Timestamp>::max() - previousTime + data.timestamp);
		deltaTime /= 1000.0;
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

		if (pd->value() == pd->maximum()){
			cancel();			
		}
		else{

			pd->setValue(pd->value() + 1);
			previousTime = data.timestamp;
		}
	}
}

void CostumeSkeletonMotionHelper::cancel()
{
	timer.stop();
}