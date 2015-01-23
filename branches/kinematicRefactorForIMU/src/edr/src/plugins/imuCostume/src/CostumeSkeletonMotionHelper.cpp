#include "PCH.h"
#include "CostumeSkeletonMotionHelper.h"
#include <chrono>
#include <thread>
#include <QtWidgets/QProgressDialog>
#include <corelib/ILog.h>
#include <corelib/IPlugin.h>

void CostumeSkeletonMotionHelper::estimate(std::map<imuCostume::Costume::SensorID, AlgoProgress> & algos,
	const IMU::SensorsData & sensorsData, const uint32_t currentTime)
{
	for (auto & m : algos)
	{
		auto it = sensorsData.find(m.first);
		if (it != sensorsData.end()){
			double deltaTime = (currentTime > m.second.lastTime) ? (currentTime - m.second.lastTime) : (std::numeric_limits<imuCostume::CostumeCANopenIO::Timestamp>::max() - m.second.lastTime + currentTime);
			deltaTime /= 1000.0;

			if (m.second.counter == 0 && m.second.lastTime == 0){
				deltaTime = 0.0;
			}

			m.second.lastTime = currentTime;
			++(m.second.counter);
			m.second.algo->estimate(it->second.accelerometer, it->second.gyroscope, it->second.magnetometer, deltaTime);
		}
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
	for (const auto & sa : costumeProfile->sensorsOrientationEstimationAlgorithms)
	{
		AlgoProgress ap;
		ap.counter = 0;
		ap.lastTime = 0;
		ap.algo = sa.second;

		algorithmsProgress.insert(std::map<imuCostume::Costume::SensorID, AlgoProgress>::value_type(sa.first, ap));
	}	

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

void CostumeSkeletonMotionHelper::resetCounters()
{
	for (auto & val : algorithmsProgress)
	{
		val.second.counter = 0;
	}
}

unsigned int CostumeSkeletonMotionHelper::minCounter() const
{
	unsigned int ret = std::numeric_limits<unsigned int>::max();

	for (const auto & val : algorithmsProgress)
	{
		ret = std::min(ret, val.second.counter);
	}

	return ret;
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

			resetCounters();
		}

		estimate(algorithmsProgress, data.sensorsData, data.timestamp);

		if (pd->value() >= calibratinStageChangeValue){		
			bool ret = costumeProfile->calibrationAlgorithm->calibrate(data.sensorsData, deltaTime);
			if (ret == true){
				pd->setValue(pd->maximum());
			}
		}

		if (pd->value() == pd->maximum()){
			cancel();			
		}
		else {
			previousTime = data.timestamp;

			if (pd->value() < calibratinStageChangeValue){

				pd->setValue(minCounter());
			}
			else{
				pd->setValue(minCounter() + calibratinStageChangeValue);
			}
		}
	}
}

void CostumeSkeletonMotionHelper::cancel()
{
	timer.stop();
}