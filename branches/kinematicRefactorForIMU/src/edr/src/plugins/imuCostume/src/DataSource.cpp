#include "PCH.h"
#include "DataSource.h"
#include <boost/lexical_cast.hpp>
#include <corelib/Filesystem.h>
#include <QtCore/QCoreApplication>
#include <corelib/PluginCommon.h>
#include "IMUCostumeListWidget.h"
#include <QtCore/QCoreApplication>
#include <corelib/HierarchyHelper.h>
#include <corelib/HierarchyDataItem.h>
#include <plugins/hmdbCommunication/TreeItemHelper.h>
#include <iosfwd>
#include "IMUPerspective.h"
#include "CostumeSkeletonMotionHelper.h"
#include <corelib/IVisualizerManager.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLayout>
#include <plugins/newChart/INewChartVisualizer.h>
#include <corelib/Exceptions.h>

//! klasa pomocnicza przy tworzeniu wykresów z wektora 3-elementowego
class NewStreamVector3ItemHelper : public core::WrappedItemHelper
{
public:
	//! Konstruktor pobiera obiekt z wrapperem wektora 3D
	//! \param wrapper 
	core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager)
	{
		core::IVisualizerManager::VisualizerPrototypes prototypes;
		manager->getVisualizerPrototypes(typeid(ScalarStream), prototypes, true);
		core::VisualizerPtr visualizer(prototypes.front()->create());

		QWidget * visWidget = visualizer->getOrCreateWidget();
		visWidget->layout()->setContentsMargins(2, 0, 2, 2);
		INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
		if (!chart) {
			UTILS_ASSERT(false);
			throw core::runtime_error("Wrong visualizer type!");
		}
		else {
			chart->setTitle(QString("%1 [%2]").arg(name).arg(units));
			chart->setAxisScale(INewChartVisualizer::AxisX, 0, 5);
			chart->setAxisScale(INewChartVisualizer::AxisY, yMin, yMax);
		}
		return visualizer;
	}

	void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series)
	{
		static int number = 0;
		// hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
		std::string suffix = boost::lexical_cast<std::string>(number++);
		std::string p = path.toStdString();

		visualizer->getOrCreateWidget();

		auto serieX = visualizer->createSerie(xwrapper->data()->getTypeInfo(), xwrapper);
		serieX->serie()->setName("X_" + suffix);
		auto serieY = visualizer->createSerie(ywrapper->data()->getTypeInfo(), ywrapper);
		serieY->serie()->setName("Y_" + suffix);
		auto serieZ = visualizer->createSerie(zwrapper->data()->getTypeInfo(), zwrapper);
		serieZ->serie()->setName("Z_" + suffix);

		INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX->serie());
		INewChartSerie* chartSerieY = dynamic_cast<INewChartSerie*>(serieY->serie());
		INewChartSerie* chartSerieZ = dynamic_cast<INewChartSerie*>(serieZ->serie());

		chartSerieX->setColor(QColor(255, 0, 0));
		chartSerieY->setColor(QColor(0, 255, 0));
		chartSerieZ->setColor(QColor(0, 0, 255));

		series.push_back(serieX);
		series.push_back(serieY);
		series.push_back(serieZ);
	}

	NewStreamVector3ItemHelper(const core::VariantConstPtr& wrapper,
		const core::VariantConstPtr& xwrapper,
		const core::VariantConstPtr& ywrapper,
		const core::VariantConstPtr& zwrapper,
		const QString & name, const QString & units,
		const double yMin, const double yMax) :
		WrappedItemHelper(wrapper), xwrapper(xwrapper), ywrapper(ywrapper),
		zwrapper(zwrapper), name(name), units(units), yMin(yMin), yMax(yMax)
	{
	}

	std::vector<utils::TypeInfo> getTypeInfos() const
	{
		std::vector<utils::TypeInfo> ret;
		ret.push_back(typeid(ScalarStream));
		return ret;
	}

private:
	const QString name;
	const QString units;
	const core::VariantConstPtr xwrapper;
	const core::VariantConstPtr ywrapper;
	const core::VariantConstPtr zwrapper;
	const double yMin;
	const double yMax;
};

//! klasa pomocnicza przy tworzeniu wykresów z wektora 3-elementowego
class NewStreamQuaternionItemHelper : public core::WrappedItemHelper
{
public:
	//! Konstruktor pobiera obiekt z wrapperem wektora 3D
	//! \param wrapper 
	core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager)
	{
		core::IVisualizerManager::VisualizerPrototypes prototypes;
		manager->getVisualizerPrototypes(typeid(ScalarStream), prototypes, true);
		core::VisualizerPtr visualizer(prototypes.front()->create());

		QWidget * visWidget = visualizer->getOrCreateWidget();
		visWidget->layout()->setContentsMargins(2, 0, 2, 2);
		INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
		if (!chart) {
			UTILS_ASSERT(false);
			throw core::runtime_error("Wrong visualizer type!");
		}
		else {
			chart->setTitle(name);
			chart->setAxisScale(INewChartVisualizer::AxisX, 0, 5);
			chart->setAxisScale(INewChartVisualizer::AxisY, yMin, yMax);
		}
		return visualizer;
	}

	void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series)
	{
		static int number = 0;
		// hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
		std::string suffix = boost::lexical_cast<std::string>(number++);
		std::string p = path.toStdString();

		visualizer->getOrCreateWidget();

		auto serieX = visualizer->createSerie(xwrapper->data()->getTypeInfo(), xwrapper);
		serieX->serie()->setName("X_" + suffix);
		auto serieY = visualizer->createSerie(ywrapper->data()->getTypeInfo(), ywrapper);
		serieY->serie()->setName("Y_" + suffix);
		auto serieZ = visualizer->createSerie(zwrapper->data()->getTypeInfo(), zwrapper);
		serieZ->serie()->setName("Z_" + suffix);
		auto serieW = visualizer->createSerie(wwrapper->data()->getTypeInfo(), wwrapper);
		serieZ->serie()->setName("W_" + suffix);

		INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX->serie());
		INewChartSerie* chartSerieY = dynamic_cast<INewChartSerie*>(serieY->serie());
		INewChartSerie* chartSerieZ = dynamic_cast<INewChartSerie*>(serieZ->serie());
		INewChartSerie* chartSerieW = dynamic_cast<INewChartSerie*>(serieW->serie());

		chartSerieX->setColor(QColor(255, 0, 0));
		chartSerieY->setColor(QColor(0, 255, 0));
		chartSerieZ->setColor(QColor(0, 0, 255));
		chartSerieW->setColor(QColor(0, 0, 0));

		series.push_back(serieX);
		series.push_back(serieY);
		series.push_back(serieZ);
		series.push_back(serieW);
	}

	NewStreamQuaternionItemHelper(const core::VariantConstPtr& wrapper,
		const core::VariantConstPtr& xwrapper,
		const core::VariantConstPtr& ywrapper,
		const core::VariantConstPtr& zwrapper,
		const core::VariantConstPtr& wwrapper,
		const QString & name, const double yMin, const double yMax) :
		WrappedItemHelper(wrapper), xwrapper(xwrapper), ywrapper(ywrapper),
		zwrapper(zwrapper), wwrapper(wwrapper), name(name), yMin(yMin), yMax(yMax)
	{
	}

	std::vector<utils::TypeInfo> getTypeInfos() const
	{
		std::vector<utils::TypeInfo> ret;
		ret.push_back(typeid(ScalarStream));
		return ret;
	}

private:
	const QString name;	
	const core::VariantConstPtr xwrapper;
	const core::VariantConstPtr ywrapper;
	const core::VariantConstPtr zwrapper;
	const core::VariantConstPtr wwrapper;
	const double yMin;
	const double yMax;
};

utils::SamplesStatus::StatusMap createStatusMap()
{
	utils::SamplesStatus::StatusMap ret;

	ret.insert(utils::SamplesStatus::StatusMap::value_type(0.0, IMU::IIMUDataSource::ConnectionStatus::OFFLINE));
	ret.insert(utils::SamplesStatus::StatusMap::value_type(0.35, IMU::IIMUDataSource::ConnectionStatus::CONNECTION_PROBLEMS));
	ret.insert(utils::SamplesStatus::StatusMap::value_type(0.5, IMU::IIMUDataSource::ConnectionStatus::ONLINE));

	return ret;
}

typedef threadingUtils::StreamAdapterT<IMU::SensorsStreamData, IMU::MotionStream::value_type, IMU::ExtractCostumeMotion> RealMotionStream;

enum
{
	AccIdx = 0,
	GyroIdx = 1,
	MagIdx = 2,
	OrientationIdx = 3
};

using namespace IMU;

IMUCostumeDataSource::IMUCostumeDataSource()
	: memoryDM(nullptr), finish(false)
{
	//char data[] = { -128, -72, -1, 79, 58, -85, 111, 0, 47, -123, -52, 37, 124, 45, 4, 25, -118, 88, 8, 23, 5, 10, 23, 5, 0, 101, -128, 20, 79, 0, 103, 34, 1, 101, 111, -4, -121, 0, -32, -4, 2, 101, -4, -1, -69, -1, 62, 0, 3, -123, -20, 19, 26, 28, -70, 5, -90, 103, 4, 101, -45, -7, 92, -19, 60, 35, 5, 101, -22, 1, 73, 0, -68, -5, 6, 101, 27, -7, 82, 5, 116, 8, 7, -123, 106, 89, -11, 26, 109, 6, 112, -57, 8, 101, -31, 4, 107, 28, -4, -29, 9, 101, 115, 0, 43, -4, -102, 1, 10, 101, -10, -1, -19, -1, -19, -1, 11, -123, 122, -40, -43, 85, 66, -53, 42, -16, 12, 101, -58, 22, -28, -8, 25, 32, 13, 101, -62, -4, 18, 2, -101, -2, 14, 101, 0, 0, 86, 0, 12, 0, 15, -123, -37, 37, -36, 34, -50, 2, 120, 96, 16, 101, -84, 39, -19, -7, -119, 4, 17, 101, -9, -5, 19, -1, -73, -2, 18, 101, 21, 0, -5, -1, 7, 0, 19, -123, -42, 66, -62, -33, -80, 65, -68, -47, 20, 101, 4, 32, -26, 13, -67, -21, 21, 101, -18, -4, -71, -1, -81, 2, 22, 101, 7, 0, -18, -1, -32, -1, 23, -123, 65, 11, 23, 77, -55, 54, -59, 53, 24, 101, 4, 32, 85, 6, 85, 23, 25, 101, 26, -3, -11, 0, 14, -3, 26, 101, 10, 0, -21, -1, -18, -1, 27, -123, 111, 78, 17, 21, 15, 45, -62, 57, 28, 101, 107, 1, 6, 10, 24, 39, 29, 101, 50, 0, 86, -3, -124, -4, 30, 101, 5, 0, -24, -1, 14, 0, 31, -123, -15, 83, 38, -12, -9, -8, 65, -69, 32, 101, -110, 5, 10, -39, 112, -7, 33, 101, -4, 0, 4, 4, -78, 1, 34, 101, 12, 0, 15, 0, 5, 0, 35, -123, 16, 66, 38, 81, 101, -19, -45, 25, 36, 101, -113, -19, -30, -3, 65, -36, 37, 101, -110, 0, 62, 0, -49, 3, 38, 101, -27, -1, -24, -1, 6, 0, 39, -123, 72, -26, 108, 1, 63, 106, -61, -4, 40, 101, -4, -29, -1, -23, -57, 17, 41, 101, -66, 2, 54, 3, 111, -1, 42, 101, -27, -1, 39, 0, 4, 0, 43, -123, -30, 76, -52, 3, -24, -58, -78, 52, 44, 101, 121, 33, 20, 1, 37, 22, 45, 101, -104, -4, 126, 1, -92, -1, -128, 0 };


	//imuCostume::CostumeCANopenIO::Data d = imuCostume::CostumeCANopenIO::extractData(&data, 416);
}

IMUCostumeDataSource::~IMUCostumeDataSource()
{
}

void IMUCostumeDataSource::init(core::IMemoryDataManager * memoryDM,
	core::IStreamDataManager * streamDM,
	core::IFileDataManager * fileDM)
{
	this->memoryDM = memoryDM;
	refreshThread = plugin::getThreadPool()->get(name(), "Costumes data reader");
}

void updateStatus(volatile IIMUDataSource::ConnectionStatus & destStatus,
	const utils::SamplesStatus & srcStatus)
{
	destStatus = (IIMUDataSource::ConnectionStatus)srcStatus.status();
}

void IMUCostumeDataSource::resfreshCostumesData()
{
	while (finish == false){
		CostumesData locCostumesData;
		{
			std::lock_guard<std::recursive_mutex > lock(synch);
			locCostumesData = costumesData;
		}

		if (locCostumesData.empty() == true){
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		else{

			imuCostume::CostumeCANopenIO::Frame frame;
			uint16_t length;

			CostumesDataFrame costumesDataFrame;

			for (auto & cd : locCostumesData)
			{				
				try{
					bool success = cd.second.rawCostume->receive(frame.buffer, length, 100);

					if (success == true){

						{
							std::lock_guard<std::recursive_mutex > lock(synch);
							cd.second.samplesStatus->positiveSample();
						}						

						if (length < 3 || frame.structure.header.structure.type != 0x80){
							continue;
						}

						imuCostume::ProtocolSendBufferHelper::Buffer streamBuffer;
						streamBuffer.buffer.reset(new uint8_t[length], utils::array_deleter<uint8_t>());
						streamBuffer.length = length;
						std::memcpy(streamBuffer.buffer.get(), frame.buffer.data(), length);
						//raw data
						//PLUGIN_LOG_DEBUG("Raw push");
						cd.second.rawDataStream->pushData(streamBuffer);
						//PLUGIN_LOG_DEBUG("After raw push");

						//potencjalne dane do zapisu przy nagrywaniu
						CostumeID id;
						id.ip = cd.second.rawCostume->ip();
						id.port = cd.second.rawCostume->port();
						costumesDataFrame.insert(CostumesDataFrame::value_type(id, streamBuffer));

						//konwertuje dane do czujnikow kompletnych

						auto s = imuCostume::Costume::extractSensorsData(frame.structure.data.data(), length - sizeof(imuCostume::CostumeCANopenIO::Header));

						std::map<imuCostume::Costume::SensorID, imuCostume::Costume::SensorDataPtr> sdat;

						for (auto ss : s)
						{
							sdat.insert(std::map<imuCostume::Costume::SensorID, imuCostume::Costume::SensorDataPtr>::value_type(ss->id(), ss));
						}

						{
							std::lock_guard<std::recursive_mutex > lock(synch);

							//nie ma danych to sensory tez maja false probke
							for (auto & sd : cd.second.sensorsData)
							{
								auto it = sdat.find(sd.first);
								if (it != sdat.end()){
									if (it->second->type() == imuCostume::Costume::IMU){
										auto ptr = utils::dynamic_pointer_cast<imuCostume::Costume::IMUSensor>(it->second);
										if (ptr->dataStatus() >= 0x07){
											sd.second.samplesStatus->positiveSample();
										}
										else if (ptr->dataStatus() > 0x00 && ptr->dataStatus() < 0x08){
											sd.second.samplesStatus->positiveNegativeSample();
										}
										else{
											sd.second.samplesStatus->negativeSample();
										}
									}
									else{
										sd.second.samplesStatus->positiveSample();
									}
								}
								else{
									sd.second.samplesStatus->negativeSample();
								}							
							}
						}

						//PLUGIN_LOG_DEBUG("Costume " << cd.second.rawCostume->ip() << " data received");												
					}
					else{

						cd.second.samplesStatus->negativeSample();

						//nie ma danych to sensory tez maja false probke
						for (auto & sd : cd.second.sensorsData)
						{
							sd.second.samplesStatus->negativeSample();
						}

						PLUGIN_LOG_DEBUG("Costume " << cd.second.rawCostume->ip() << " timeout on data receive");
					}					
				}
				catch (...){
					PLUGIN_LOG_DEBUG("Costume " << cd.second.rawCostume->ip() << " failed to receive data");
				}
			}

			if (costumesDataFrame.empty() == false){
				std::lock_guard<std::recursive_mutex > lock(synch);
				for (auto & rec : recordings)
				{
					CostumesDataFrame recCDF;
					for (const auto & id : rec->costumesToRecord)
					{
						auto it = costumesDataFrame.find(id);
						if (it != costumesDataFrame.end()){
							recCDF.insert(*it);
						}
					}

					rec->costumesDataBuffer.pushData(std::move(recCDF));
				}
			}
		}
	}
}

bool IMUCostumeDataSource::lateInit()
{
	refreshThread.run(&IMUCostumeDataSource::resfreshCostumesData, this);

	try{
		auto p = plugin::getResourcesPath() / "profiles.pro";

		if (core::Filesystem::pathExists(p) == true){
			std::ifstream f(p.string());

			//próbujemy czytać profile
			if (f.is_open() == true){

				while (f.eof() == false){

					auto pro = SerializableCostumeProfile::deserialize(f);

					if (SerializableCostumeProfile::verify(pro, this) != SerializableCostumeProfile::Failed){
						auto upro = SerializableCostumeProfile::unpack(pro, this);
						costumesProfiles_.insert(CostumesProfiles::value_type(upro.name, upro));
					}
				}
			}			
		}
	}
	catch (...){
		PLUGIN_LOG_ERROR("Failed to read profiles");
	}

	return true;
}

void IMUCostumeDataSource::finalize()
{
	finish = true;

	if (refreshThread.joinable() == true){
		refreshThread.join();
		refreshThread = core::Thread();
	}

	if (costumesProfiles_.empty() == false){

		try{
			auto p = plugin::getResourcesPath() / "profiles.pro";
		
			std::ofstream f(p.string());

			//próbujemy zapisać profile
			if (f.is_open() == true){

				for (const auto & pro : costumesProfiles_){

					auto sPro = SerializableCostumeProfile::pack(pro.second);
					SerializableCostumeProfile::serialize(f, sPro);
					f << std::endl;
				}
					
				f.close();
			}			
		}
		catch (...){
			PLUGIN_LOG_ERROR("Failed to write profiles");
		}
	}


	OrientationEstimationAlgorithms().swap(orientationEstimationAlgorithms_);
	CostumeCalibrationAlgorithms().swap(calibrationAlgorithms_);
	CostumeMotionEstimationAlgorithms().swap(motionEstimationAlgorithms_);
	SkeletonModels().swap(skeletonModels_);
	CostumesProfiles().swap(costumesProfiles_);
}

void IMUCostumeDataSource::update(double deltaTime)
{
}

QWidget* IMUCostumeDataSource::getWidget()
{
	return new IMUCostumeWidget(this);
}

QWidget* IMUCostumeDataSource::getControlWidget()
{
	return nullptr;
}

QWidget* IMUCostumeDataSource::getSettingsWidget()
{
	return nullptr;
}

void IMUCostumeDataSource::getOfferedTypes(utils::TypeInfoList & offeredTypes) const
{	
	offeredTypes.push_back(typeid(RawDataStream));
	offeredTypes.push_back(typeid(CANopenFramesStream));
	offeredTypes.push_back(typeid(CostumeStream));
	offeredTypes.push_back(typeid(IMUStream));
	offeredTypes.push_back(typeid(Vec3Stream));
	offeredTypes.push_back(typeid(ScalarStream));
}

const bool IMUCostumeDataSource::refreshCostumes()
{
	static const uint8_t MaxSamplesCount = 125;

	auto costumes = imuCostume::CostumeRawIO::listAvailableCostumes();
	if (costumes.empty() == true){
		costumes = imuCostume::CostumeRawIO::listAvailableCostumes();
	}

	std::set<CostumeID> refreshedIPs;
	std::set<CostumeID> currentIPs;

	for (const auto & c : costumes){
		refreshedIPs.insert(c);
	}

	std::lock_guard<std::recursive_mutex > lock(synch);

	for (const auto & c : costumesData){
		currentIPs.insert(c.first);
	}

	std::vector<CostumeID> toAdd(costumes.size());
	std::vector<CostumeID> toRemove(costumesData.size());

	auto removeIT = std::set_difference(currentIPs.begin(), currentIPs.end(), refreshedIPs.begin(), refreshedIPs.end(), toRemove.begin());
	auto addIT = std::set_difference(refreshedIPs.begin(), refreshedIPs.end(), currentIPs.begin(), currentIPs.end(), toAdd.begin());

	toAdd.resize(addIT - toAdd.begin());
	toRemove.resize(removeIT - toRemove.begin());

	/*
	auto it = toRemove.begin();
	//TODO
	//infinit loop
	while (it != toRemove.end()){
		auto cIT = costumesData.find(*it);
		if (cIT != costumesData.end()){
			//TODO - unload jak by� za�adowany
			
			//if (cIT->second.domainData.empty() == false){
			//	it = toRemove.erase(it);
			//}
			//else{
			//	++it;
			//}
			
		}
		else{
			it = toRemove.erase(it);
		}
	}
	*/

	CostumesData additionalCostumesData;

	for (const auto & id : toAdd){
		try{
			CostumeData cData;
			cData.rawCostume.reset(new imuCostume::CostumeRawIO(id.ip, id.port));
			cData.rawCostume->setSamplingDelay(10);
			cData.rawDataStream.reset(new RawDataStream);
			cData.samplesStatus = utils::make_shared<utils::SamplesStatus>(100, createStatusMap());
			cData.samplesStatus->positiveSample();
			configureCostume(cData);
			innerRefreshCostumeSensorsConfiguration(cData, MaxSamplesCount);

			additionalCostumesData.insert(CostumesData::value_type(id, cData));
		}
		catch (...){
			//TODO
			//info o nieudanej probie za�adowania kostiumu
		}
	}

	for (const auto & id : toRemove){		
		costumesData.erase(id);
	}

	costumesData.insert(additionalCostumesData.begin(), additionalCostumesData.end());

	return costumes.empty() == false;
}

void IMUCostumeDataSource::configureCostume(CostumeDescription & cd)
{
	imuCostume::ProtocolSendBufferHelper psbh;
	imuCostume::CostumeCANopenIO::prepareCostumeConfiguration(psbh);
	imuCostume::Costume::prepareCostumeConfiguration(psbh);

	while (psbh.empty() == false)
	{
		const auto buff = psbh.createBuffer();
		cd.rawCostume->send(buff.buffer.get(), buff.length);
	}
}

bool IMUCostumeDataSource::innerRefreshCostumeSensorsConfiguration(CostumeData & data, const uint8_t MaxSamplesCount)
{	
	bool ret = false;
	imuCostume::CostumeCANopenIO::Frame frame = { 0 };
	uint16_t length = 0;	

	for (unsigned int i = 0; i < MaxSamplesCount; ++i){		
		if (data.rawCostume->receive(frame.buffer, length, 100) == true){
			if (length > 2 && frame.structure.header.structure.type == 0x80){
				auto sc = imuCostume::Costume::sensorsConfiguration(frame.structure.data.data(), length - sizeof(imuCostume::CostumeCANopenIO::Header));
				ret = true;
				for (const auto & st : sc){
					auto it = data.sensorsConfiguration.find(st.first);
					if (it == data.sensorsConfiguration.end()){
						data.sensorsConfiguration.insert(st);
					}
					else{
						it->second.insert(st.second.begin(), st.second.end());
					}

					for (const auto & sid : st.second){						
						auto ret = data.sensorsData.insert(std::map<imuCostume::Costume::SensorID, SensorData>::value_type(sid, SensorData()));
						if (ret.second == true){
							ret.first->second.samplesStatus = utils::make_shared<utils::SamplesStatus>(100, createStatusMap());
						}
					}
				}
			}
		}
	}	

	return ret;
}

const unsigned int IMUCostumeDataSource::costumesCout() const
{
	std::lock_guard<std::recursive_mutex > lock(synch);
	return costumesData.size();
}

bool IMUCostumeDataSource::costumeLoaded(const CostumeID & id) const
{
	std::lock_guard<std::recursive_mutex > lock(synch);
	auto it = costumesData.find(id);

	if (it != costumesData.end()){
		return !it->second.domainData.empty();
	}

	return false;
}

std::string IMUCostumeDataSource::sensorParameterName(const unsigned int idx)
{
	switch (idx)
	{
	case AccIdx:			return QObject::tr("Accelerometer").toStdString();
	case GyroIdx:			return QObject::tr("Gyroscope").toStdString();
	case MagIdx:			return QObject::tr("Magnetometer").toStdString();
	case OrientationIdx:	return QObject::tr("Orientation").toStdString();
	default:				return QObject::tr("Unknown").toStdString();
	}
}

std::string IMUCostumeDataSource::vectorParameterName(const unsigned int idx)
{
	switch (idx)
	{
	case 0:	return "x";
	case 1:	return "y";
	case 2:	return "z";
	case 3:	return "w";
	default: return QObject::tr("Unknown").toStdString();
	}
}

core::HierarchyItemPtr IMUCostumeDataSource::fillRawCostumeData(CostumeData & cData)
{
	auto hierarchyTransaction = memoryDM->hierarchyTransaction();
	core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>("Raw Data", "Raw Data", QIcon());	
	//hierarchyTransaction->addRoot(root);

	const auto & sc = cData.sensorsConfiguration;
	auto ow = core::Variant::create<RawDataStream>();
	ow->setMetadata("core/name", QObject::tr("Raw Data").toStdString());
	ow->set(cData.rawDataStream);
	cData.domainData.push_back(ow);

	core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Raw stream"), QObject::tr("Raw stream"));
	root->appendChild(item);

	cData.CANopenStream.reset(new threadingUtils::StreamAdapterT<RawDataStream::value_type, CANopenFramesStream::value_type, RawToCANopenExtractor>(cData.rawDataStream, RawToCANopenExtractor()));
	ow = core::Variant::create<CANopenFramesStream>();
	ow->setMetadata("core/name", QObject::tr("CANopen Data").toStdString());
	ow->set(cData.CANopenStream);
	cData.domainData.push_back(ow);

	item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("CANopen stream"), QObject::tr("CANopen stream"));
	root->appendChild(item);

	cData.costumeStream.reset(new threadingUtils::StreamAdapterT<CANopenFramesStream::value_type, CostumeStream::value_type, CANopenDataExtractor>(cData.CANopenStream, CANopenDataExtractor()));
	ow = core::Variant::create<CostumeStream>();
	ow->setMetadata("core/name", QObject::tr("Costume Data").toStdString());
	ow->set(cData.costumeStream);
	cData.domainData.push_back(ow);

	item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Sensors stream"), QObject::tr("Sensors stream"));
	root->appendChild(item);

	return root;
}

void IMUCostumeDataSource::loadRawCostume(const CostumeID & id)
{
	std::lock_guard<std::recursive_mutex > lock(synch);	
	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		if (it->second.domainData.empty() == false){
			throw std::runtime_error("Costume already loaded");
		}
	}	
	
	CostumeData cData = it->second;

	auto hItem = fillRawCostumeData(cData);

	try{
		auto t = memoryDM->transaction();
		for (const auto & ow : cData.domainData){
			t->addData(ow);
		}

		it->second = cData;
	
		auto name = QObject::tr("Costume %1:%2 data").arg(QString::fromStdString(it->first.ip)).arg(it->first.port);
		it->second.hierarchyRootItem = utils::make_shared<core::HierarchyItem>(name, name, QIcon());
		it->second.hierarchyRootItem->appendChild(hItem);
		memoryDM->hierarchyTransaction()->addRoot(it->second.hierarchyRootItem);
	}
	catch (std::exception & e){
		PLUGIN_LOG_ERROR(e.what());		
	}
	catch (...){
		PLUGIN_LOG_ERROR("Unknown error while loading raw costume data");
	}
}
void IMUCostumeDataSource::loadCalibratedCostume(const CostumeID & id,
	const CostumeProfileInstance & profileInstance)
{
	std::lock_guard<std::recursive_mutex > lock(synch);	
	
	auto it = costumesData.find(id);

	if (it == costumesData.end()){
		throw std::runtime_error("Costume not found");
	}
	
	if (it->second.domainData.empty() == false){
		throw std::runtime_error("Costume already loaded");	
	}

	unsigned int imusCount = 0;	

	auto sIT = it->second.sensorsConfiguration.find(imuCostume::Costume::IMU);
	if (sIT != it->second.sensorsConfiguration.end()){
		imusCount = sIT->second.size();
	}

	if (profileInstance.sensorsOrientationEstimationAlgorithms.size() > imusCount)
	{
		throw std::runtime_error("Mismatch in number of loaded sensors orientation estimation algorithms and costume configuration");
	}

	CostumeData cData = it->second;

	auto hItem = fillRawCostumeData(cData);

	core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>("Unpacked Data", "Unpacked Data", QIcon());

	//adapter na pe�ne dane z kostiumu nap�dzaj�ce strumie� z estymacj�
	cData.completeImuStream.reset(new ExtractedCostumeStreamAdapter(cData.costumeStream, CostumeIMUExtractor(cData.sensorsConfiguration)));

	auto ow = core::Variant::create<IMU::SensorsStream>();
	ow->setMetadata("core/name", QObject::tr("Complete unpacked IMU data").toStdString());
	ow->set(IMU::SensorsStreamPtr(cData.completeImuStream));
	cData.domainData.push_back(ow);

	core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("IMU stream"), QObject::tr("IMU stream"));
	root->appendChild(item);

	unpackSensorsStream(cData.completeImuStream, cData.sensorsData, item, cData.domainData);

	auto estimatedData = utils::make_shared<threadingUtils::StreamProcessorT<IMU::SensorsStreamData, OrientationEstimator>>(cData.completeImuStream,
		OrientationEstimator(profileInstance.sensorsOrientationEstimationAlgorithms));

	ow = core::Variant::create<IMU::SensorsStream>();
	ow->setMetadata("core/name", QObject::tr("Complete filtered IMU data").toStdString());
	ow->set(IMU::SensorsStreamPtr(estimatedData));
	cData.domainData.push_back(ow);

	item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Filtered IMU stream"), QObject::tr("Filtered IMU stream"));
	root->appendChild(item);

	unpackSensorsStream(cData.completeImuStream, cData.sensorsData, item, cData.domainData);

	//TODO doda� kwaterniony po estymacji + skalary

	cData.skeletonMotion = utils::make_shared<IMU::CostumeSkeletonMotion>();
	ow = core::Variant::create<IMU::CostumeSkeletonMotion>();
	ow->setMetadata("core/name", QObject::tr("Complete skeleton motion stream").toStdString());
	ow->set(cData.skeletonMotion);
	cData.domainData.push_back(ow);

	item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Skeleton stream"), QObject::tr("Skeleton stream"));
	root->appendChild(item);

	//mapowanie pozycji wektora do nazwy jointa w szkielecie i stanie

	auto visitor = [&cData](kinematic::JointConstPtr joint, kinematic::Joint::size_type) -> void
			{
				cData.skeletonMotion->dataToModelMapping.insert(DataIndexToJointMapping::value_type(cData.skeletonMotion->dataToModelMapping.size(), joint->value.name));
			};

	kinematic::Joint::visitLevelOrder(profileInstance.skeleton->root, visitor);

	cData.skeletonMotion->skeleton = profileInstance.skeleton;
	cData.skeletonMotion->stream.reset(new RealMotionStream(estimatedData,
		ExtractCostumeMotion(profileInstance.skeleton,
		profileInstance.sensorsMapping,
		cData.skeletonMotion->dataToModelMapping,
		profileInstance.sensorsAdjustments,
		profileInstance.motionEstimationAlgorithm)));

	ow = core::Variant::create<MotionStream>();
	ow->setMetadata("core/name", QObject::tr("Calibrated skeleton motion stream").toStdString());
	ow->set(MotionStreamPtr(cData.skeletonMotion->stream));
	cData.domainData.push_back(ow);

	item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Calibrated skeleton stream"), QObject::tr("Calibrated skeleton stream"));
	root->appendChild(item);

	{
		//dane dla kinematic
		auto kStream = utils::make_shared<SkeletonWithStreamData>();
		kStream->skeleton = profileInstance.skeleton;
		kStream->nodesMapping = kinematic::SkeletonState::createMapping(*profileInstance.skeleton);
		//SkeletonStateStream
		kStream->states.reset(new threadingUtils::StreamAdapterT<IMU::MotionStream::value_type, SkeletonStateStream::value_type, KinematicStreamExtractor>(cData.skeletonMotion->stream, KinematicStreamExtractor(profileInstance.skeleton)));

		ow = core::Variant::create<SkeletonWithStreamData>();
		ow->setMetadata("core/name", QObject::tr("Kinematic stream").toStdString());
		ow->set(kStream);
		cData.domainData.push_back(ow);

		item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Kinematic stream"), QObject::tr("Kinematic stream"));
		root->appendChild(item);
	}

	try{
		{
			auto t = memoryDM->transaction();
			for (const auto & ow : cData.domainData){
				t->addData(ow);
			}
		}

		it->second = cData;

		auto name = QObject::tr("Costume %1:%2 data").arg(QString::fromStdString(it->first.ip)).arg(it->first.port);
		it->second.hierarchyRootItem = utils::make_shared<core::HierarchyItem>(name, name, QIcon());
		it->second.hierarchyRootItem->appendChild(hItem);
		it->second.hierarchyRootItem->appendChild(root);
		memoryDM->hierarchyTransaction()->addRoot(it->second.hierarchyRootItem);
	}
	catch (std::exception & e){
		PLUGIN_LOG_ERROR(e.what());
	}
	catch (...){
		PLUGIN_LOG_ERROR("Unknown error while loading raw costume data");
	}
}

void IMUCostumeDataSource::unloadCostume(const CostumeID & id)
{
	std::lock_guard<std::recursive_mutex > lock(synch);
	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		auto transaction = memoryDM->transaction();

		for (const auto & d : it->second.domainData){
			transaction->tryRemoveData(d);
		}

		//TODO - update hierarchy in analysis
		//core::HierarchyDataItemPtr hierarchyRootItem;
		it->second.skeletonMotion.reset();
		it->second.CANopenStream.reset();
		it->second.costumeStream.reset();
		it->second.completeImuStream.reset();		
		core::VariantsList().swap(it->second.domainData);
	}
	else{
		throw std::runtime_error("Costume not loaded");
	}
}

unsigned int IMUCostumeDataSource::loadedCostumesCount() const
{
	std::lock_guard<std::recursive_mutex > lock(synch);

	unsigned int ret = 0;

	for (const auto & val : costumesData)
	{
		if (val.second.domainData.empty() == false)
		{
			++ret;
		}
	}

	return ret;
}

core::ConstVariantsList IMUCostumeDataSource::costumeData(const CostumeID & id) const
{
	std::lock_guard<std::recursive_mutex > lock(synch);

	core::ConstVariantsList ret;
	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		for (const auto & d : it->second.domainData){
			ret.push_back(d);
		}
	}

	return ret;
}

void IMUCostumeDataSource::startRecording(RecordingOutputPtr recording)
{
	if (recording != nullptr){
		std::lock_guard<std::recursive_mutex > lock(synch);
		recordings.insert(recording);
	}
}

void IMUCostumeDataSource::stopRecording(RecordingOutputPtr recording)
{
	if (recording != nullptr){
		std::lock_guard<std::recursive_mutex > lock(synch);
		recordings.erase(recording);
	}
}

core::HierarchyItemPtr IMUCostumeDataSource::createRootItem()
{
	return utils::make_shared<core::HierarchyItem>(QObject::tr("Costumes data"), QObject::tr("Costumes data"), QIcon());
}

core::HierarchyItemPtr IMUCostumeDataSource::createStreamItem()
{
	return utils::make_shared<core::HierarchyItem>(QObject::tr("Streams"), QObject::tr("Costumes data streams"), QIcon());
}

core::HierarchyItemPtr IMUCostumeDataSource::createRecordItem()
{
	return utils::make_shared<core::HierarchyItem>(QObject::tr("Recordings"), QObject::tr("Costumes data recordings"), QIcon());
}

core::HierarchyItemPtr IMUCostumeDataSource::createIMUsItem()
{
	return utils::make_shared<core::HierarchyItem>(QObject::tr("IMUs"), QObject::tr("IMUs data"), QIcon());
}

void IMUCostumeDataSource::tryCreateRootItem()
{
	if (root == nullptr){
		root = createRootItem();
		auto hierarchyTransaction = memoryDM->hierarchyTransaction();
		hierarchyTransaction->addRoot(root);
	}
}

void IMUCostumeDataSource::tryCreateStreamItem()
{
	if (streamItems == nullptr){
		streamItems = createStreamItem();
		tryCreateRootItem();
		root->appendChild(streamItems);
	}
}

void IMUCostumeDataSource::tryCreateRecordedItem()
{
	if (recordedItems == nullptr){
		recordedItems = createRecordItem();
		tryCreateRootItem();
		root->appendChild(recordedItems);
	}
}

void IMUCostumeDataSource::registerOrientationEstimationAlgorithm(const IIMUOrientationEstimationAlgorithm * algorithm)
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	if (algorithm == nullptr){
		throw std::runtime_error("Uninitialized orientation estimation algorithm");
	}

	if (orientationEstimationAlgorithms_.find(algorithm->ID()) == orientationEstimationAlgorithms_.end()){
		orientationEstimationAlgorithms_.insert(OrientationEstimationAlgorithms::value_type(algorithm->ID(), IIMUOrientationEstimationAlgorithmConstPtr(algorithm)));
	}
	else{
		throw std::runtime_error("Orientation estimation algorithm already registered");
	}
}

void IMUCostumeDataSource::registerCostumeCalibrationAlgorithm(const IMUCostumeCalibrationAlgorithm * algorithm)
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	if (algorithm == nullptr){
		throw std::runtime_error("Uninitialized costume calibration algorithm");
	}

	if (calibrationAlgorithms_.find(algorithm->ID()) == calibrationAlgorithms_.end()){
		calibrationAlgorithms_.insert(CostumeCalibrationAlgorithms::value_type(algorithm->ID(), IMUCostumeCalibrationAlgorithmConstPtr(algorithm)));
	}
	else{
		throw std::runtime_error("Costume calibration algorithm already registered");
	}
}

void IMUCostumeDataSource::registerMotionEstimationAlgorithm(const IMUCostumeMotionEstimationAlgorithm * algorithm)
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	if (algorithm == nullptr){
		throw std::runtime_error("Uninitialized motion estimation algorithm");
	}

	if (motionEstimationAlgorithms_.find(algorithm->ID()) == motionEstimationAlgorithms_.end()){
		motionEstimationAlgorithms_.insert(CostumeMotionEstimationAlgorithms::value_type(algorithm->ID(), IMUCostumeMotionEstimationAlgorithmConstPtr(algorithm)));
	}
	else{
		throw std::runtime_error("Motion estimation algorithm already registered");
	}
}

void IMUCostumeDataSource::registerSkeletonModel(SkeletonConstPtr skeleton)
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	if (skeleton == nullptr){
		throw std::runtime_error("Uninitialized skeleton model");
	}

	auto it = skeletonModels_.find(skeleton->id);	

	if (it == skeletonModels_.end()){
		skeletonModels_.insert(SkeletonModels::value_type(skeleton->id, skeleton));
	}
	else{
		throw std::runtime_error("Skeleton model already registered");
	}
}	

bool verifyProfile(const CostumeProfile & profile)
{
	bool ret = true;

	if (profile.name.empty() == true || profile.calibrationAlgorithm == nullptr ||
		profile.motionEstimationAlgorithm == nullptr ||
		profile.skeleton == nullptr || profile.sensorsMapping.empty() == true ||
		profile.sensorsMapping.size() != profile.sensorsOrientationEstimationAlgorithms.size() ||
		profile.sensorsMapping.size() != profile.sensorsAdjustments.size()){

		ret = false;
	}

	return ret;
}

void IMUCostumeDataSource::registerCostumeProfile(const CostumeProfile & profile)
{
	if (verifyProfile(profile) == false){
		throw std::runtime_error("Incomplete profile");
	}

	std::lock_guard<std::recursive_mutex> lock(synch);

	if (costumesProfiles_.find(profile.name) != costumesProfiles_.end()){
		throw std::runtime_error("Profile already registered");
	}

	//produjemy rejestrowac brakujace elementy profilu

	if (profile.skeleton != nullptr){
		if (skeletonModels_.find(profile.skeleton->id) == skeletonModels_.end()){
			skeletonModels_.insert(SkeletonModels::value_type(profile.skeleton->id, profile.skeleton));
		}
	}

	//algorytm kalibracji
	if (calibrationAlgorithms_.find(profile.calibrationAlgorithm->ID()) == calibrationAlgorithms_.end())
	{
		calibrationAlgorithms_.insert(CostumeCalibrationAlgorithms::value_type(profile.calibrationAlgorithm->ID(), profile.calibrationAlgorithm));
	}

	//algorytm estymacji ruchu
	if (motionEstimationAlgorithms_.find(profile.motionEstimationAlgorithm->ID()) == motionEstimationAlgorithms_.end())
	{
		motionEstimationAlgorithms_.insert(CostumeMotionEstimationAlgorithms::value_type(profile.motionEstimationAlgorithm->ID(), profile.motionEstimationAlgorithm));
	}

	// algorytmy estymacji
	for (const auto & ea : profile.sensorsOrientationEstimationAlgorithms)
	{
		if (orientationEstimationAlgorithms_.find(ea.second->ID()) == orientationEstimationAlgorithms_.end()){
			orientationEstimationAlgorithms_.insert(OrientationEstimationAlgorithms::value_type(ea.second->ID(), ea.second));
		}
	}

	//rejestrujemy profil
	costumesProfiles_.insert(CostumesProfiles::value_type(profile.name, profile));
}

IMUCostumeDataSource::OrientationEstimationAlgorithms IMUCostumeDataSource::orientationEstimationAlgorithms() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return orientationEstimationAlgorithms_;
}

IMUCostumeDataSource::CostumeCalibrationAlgorithms IMUCostumeDataSource::calibrationAlgorithms() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return calibrationAlgorithms_;
}

IMUCostumeDataSource::CostumeMotionEstimationAlgorithms IMUCostumeDataSource::motionEstimationAlgorithms() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return motionEstimationAlgorithms_;
}

IMUCostumeDataSource::SkeletonModels IMUCostumeDataSource::skeletonModels() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return skeletonModels_;
}

IMUCostumeDataSource::CostumesProfiles IMUCostumeDataSource::costumesProfiles() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return costumesProfiles_;
}

IMUCostumeDataSource::CostumeDescription IMUCostumeDataSource::costumeDescription(const CostumeID & id) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);

	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		return it->second;
	}

	return CostumeDescription();
}

const bool IMUCostumeDataSource::costumesEmpty() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	return costumesData.empty();
}

bool IMUCostumeDataSource::refreshCostumeSensorsConfiguration(const CostumeID & id, const uint8_t samplesCount)
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		return innerRefreshCostumeSensorsConfiguration(it->second, samplesCount);
	}

	return false;
}

void IMUCostumeDataSource::resetCostumeStatus(const CostumeID & id)
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.find(id);
	if (it != costumesData.end()){		
		const auto sm = createStatusMap();
		it->second.samplesStatus = utils::make_shared<utils::SamplesStatus>(100, sm);		

		for (auto & ss : it->second.sensorsData)
		{
			ss.second.samplesStatus = utils::make_shared<utils::SamplesStatus>(100, sm);			
		}		
	}
}

void IMUCostumeDataSource::resetSensorStatus(const CostumeID & costumeID,
	const imuCostume::Costume::SensorID sensorID)
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.find(costumeID);
	if (it != costumesData.end()){		

		auto sIT = it->second.sensorsData.find(sensorID);
		if (sIT != it->second.sensorsData.end()){
			sIT->second.samplesStatus = utils::make_shared<utils::SamplesStatus>(100, createStatusMap());			
		}
	}
}

IMUCostumeDataSource::CostumeStatus IMUCostumeDataSource::innerCreateCostumeStatus(const CostumeData & cData)
{
	CostumeStatus cs;
	cs.status = cData.samplesStatus != nullptr ? (ConnectionStatus)cData.samplesStatus->status() : UNKNOWN;

	for (const auto & ss : cData.sensorsData)
	{
		cs.sensorsStatus.insert(std::map<imuCostume::Costume::SensorID, ConnectionStatus>::value_type(ss.first, ss.second.samplesStatus != nullptr ? (ConnectionStatus)ss.second.samplesStatus->status() : UNKNOWN));
	}

	return cs;
}

IMUCostumeDataSource::CostumeStatus IMUCostumeDataSource::costumeStatus(const CostumeID & id) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		return innerCreateCostumeStatus(it->second);
	}

	return CostumeStatus();
}

IMUCostumeDataSource::CostumeDetails IMUCostumeDataSource::costumeDetails(const CostumeID & id) const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	auto it = costumesData.find(id);
	if (it != costumesData.end()){
		CostumeDetails cd;
		cd.descriptionDetails = it->second;
		cd.statusDetails = innerCreateCostumeStatus(it->second);
		return cd;
	}

	return CostumeDetails();
}

IMUCostumeDataSource::CostumesDescriptions IMUCostumeDataSource::costumesDescriptions() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	CostumesDescriptions ret;

	for (const auto & cd : costumesData)
	{
		ret.insert(CostumesDescriptions::value_type(cd.first, cd.second));
	}

	return ret;
}

IMUCostumeDataSource::CostumesStatus IMUCostumeDataSource::costumesStatus() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	CostumesStatus ret;

	for (const auto & cd : costumesData)
	{
		ret.insert(CostumesStatus::value_type(cd.first, innerCreateCostumeStatus(cd.second)));
	}

	return ret;
}

IMUCostumeDataSource::CostumesDetails IMUCostumeDataSource::costumesDetails() const
{
	std::lock_guard<std::recursive_mutex> lock(synch);
	CostumesDetails ret;

	for (const auto & cd : costumesData)
	{
		CostumeDetails c;
		c.descriptionDetails = cd.second;
		c.statusDetails = innerCreateCostumeStatus(cd.second);
		ret.insert(CostumesDetails::value_type(cd.first, c));
	}

	return ret;
}

void IMUCostumeDataSource::unpackSensorsStream(IMU::SensorsStreamPtr stream,
	SensorsData & sensorsData,
	core::HierarchyItemPtr root,
	core::VariantsList & domainData)
{
	for (auto & sd : sensorsData)
	{				
		auto imuStream = utils::shared_ptr<IMUStream>(new threadingUtils::StreamAdapterT<IMU::SensorsStreamData, IMU::IMUStream::value_type, IMU::IMUExtractor>(stream, IMU::IMUExtractor(sd.first)));

		auto ow = core::Variant::create<IMUStream>();
		ow->setMetadata("core/name", QObject::tr("IMU %1 Data").arg(sd.first).toStdString());
		ow->set(imuStream);
		sd.second.domainData.push_back(ow);
		domainData.push_back(ow);

		auto imuItem = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("IMU %1 Data").arg(sd.first), QObject::tr("IMU %1 Data").arg(sd.first));
		root->appendChild(imuItem);

		//ACC
		{
			auto vec3Stream = utils::shared_ptr<Vec3Stream>(new threadingUtils::StreamAdapterT<IMU::IMUStream::value_type, Vec3Stream::value_type, IMU::IMUAccExtractor>(imuStream, IMU::IMUAccExtractor()));

			ow = core::Variant::create<Vec3Stream>();
			ow->setMetadata("core/name", sensorParameterName(0));
			ow->set(vec3Stream);
			sd.second.domainData.push_back(ow);
			domainData.push_back(ow);

			auto imuElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(sensorParameterName(0)), QString::fromStdString(sensorParameterName(0)));
			imuItem->appendChild(imuElement);

			std::vector<core::VariantConstPtr> vecDataObjects;

			for (unsigned int j = 0; j < 3; ++j){
				auto scalarStream = utils::shared_ptr<ScalarStream>(new threadingUtils::StreamAdapterT<Vec3Stream::value_type, ScalarStream::value_type, CompoundArrayExtractor>(vec3Stream, CompoundArrayExtractor(j)));

				ow = core::Variant::create<ScalarStream>();
				ow->setMetadata("core/name", vectorParameterName(j));
				ow->set(scalarStream);
				sd.second.domainData.push_back(ow);
				domainData.push_back(ow);
				vecDataObjects.push_back(ow);

				auto vecElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(vectorParameterName(j)), QString::fromStdString(vectorParameterName(j)));
				imuElement->appendChild(vecElement);
			}

			imuElement->addHelper(core::HierarchyHelperPtr(new NewStreamVector3ItemHelper(imuElement->getData(), vecDataObjects[0], vecDataObjects[1], vecDataObjects[2], imuElement->getName(), "m/s^2", -15, 15)));
		}

		//GYRO
		{
			auto vec3Stream = utils::shared_ptr<Vec3Stream>(new threadingUtils::StreamAdapterT<IMU::IMUStream::value_type, Vec3Stream::value_type, IMU::IMUGyroExtractor>(imuStream, IMU::IMUGyroExtractor()));

			ow = core::Variant::create<Vec3Stream>();
			ow->setMetadata("core/name", sensorParameterName(1));
			ow->set(vec3Stream);
			sd.second.domainData.push_back(ow);
			domainData.push_back(ow);

			auto imuElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(sensorParameterName(1)), QString::fromStdString(sensorParameterName(1)));
			imuItem->appendChild(imuElement);

			std::vector<core::VariantConstPtr> vecDataObjects;

			for (unsigned int j = 0; j < 3; ++j){
				auto scalarStream = utils::shared_ptr<ScalarStream>(new threadingUtils::StreamAdapterT<Vec3Stream::value_type, ScalarStream::value_type, CompoundArrayExtractor>(vec3Stream, CompoundArrayExtractor(j)));

				ow = core::Variant::create<ScalarStream>();
				ow->setMetadata("core/name", vectorParameterName(j));
				ow->set(scalarStream);
				sd.second.domainData.push_back(ow);
				domainData.push_back(ow);
				vecDataObjects.push_back(ow);

				auto vecElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(vectorParameterName(j)), QString::fromStdString(vectorParameterName(j)));
				imuElement->appendChild(vecElement);
			}

			imuElement->addHelper(core::HierarchyHelperPtr(new NewStreamVector3ItemHelper(imuElement->getData(), vecDataObjects[0], vecDataObjects[1], vecDataObjects[2], imuElement->getName(), "rad/s", -0.3, 0.3)));
		}

		//MAG
		{
			auto vec3Stream = utils::shared_ptr<Vec3Stream>(new threadingUtils::StreamAdapterT<IMU::IMUStream::value_type, Vec3Stream::value_type, IMU::IMUMagExtractor>(imuStream, IMU::IMUMagExtractor()));

			ow = core::Variant::create<Vec3Stream>();
			ow->setMetadata("core/name", sensorParameterName(2));
			ow->set(vec3Stream);
			sd.second.domainData.push_back(ow);
			domainData.push_back(ow);

			auto imuElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(sensorParameterName(2)), QString::fromStdString(sensorParameterName(2)));
			imuItem->appendChild(imuElement);

			std::vector<core::VariantConstPtr> vecDataObjects;

			for (unsigned int j = 0; j < 3; ++j){
				auto scalarStream = utils::shared_ptr<ScalarStream>(new threadingUtils::StreamAdapterT<Vec3Stream::value_type, ScalarStream::value_type, CompoundArrayExtractor>(vec3Stream, CompoundArrayExtractor(j)));

				ow = core::Variant::create<ScalarStream>();
				ow->setMetadata("core/name", vectorParameterName(j));
				ow->set(scalarStream);
				sd.second.domainData.push_back(ow);
				domainData.push_back(ow);
				vecDataObjects.push_back(ow);

				auto vecElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(vectorParameterName(j)), QString::fromStdString(vectorParameterName(j)));
				imuElement->appendChild(vecElement);
			}

			imuElement->addHelper(core::HierarchyHelperPtr(new NewStreamVector3ItemHelper(imuElement->getData(), vecDataObjects[0], vecDataObjects[1], vecDataObjects[2], imuElement->getName(), "", -3, 3)));
		}

		auto orientStream = utils::shared_ptr<QuatStream>(new threadingUtils::StreamAdapterT<IMU::IMUStream::value_type, QuatStream::value_type, IMU::IMUOrientExtractor>(imuStream, IMU::IMUOrientExtractor()));
		ow = core::Variant::create<QuatStream>();
		ow->setMetadata("core/name", QObject::tr("Orientation").toStdString());
		ow->set(orientStream);
		sd.second.domainData.push_back(ow);
		domainData.push_back(ow);

		auto imuElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Orientation"), QObject::tr("Orientation"));
		imuItem->appendChild(imuElement);

		std::vector<core::VariantConstPtr> vecDataObjects;

		for (unsigned int j = 0; j < 4; ++j){
			auto scalarStream = utils::shared_ptr<ScalarStream>(new threadingUtils::StreamAdapterT<QuatStream::value_type, ScalarStream::value_type, CompoundArrayExtractor>(orientStream, CompoundArrayExtractor(j)));

			ow = core::Variant::create<ScalarStream>();
			ow->setMetadata("core/name", vectorParameterName(j));
			ow->set(scalarStream);
			sd.second.domainData.push_back(ow);
			domainData.push_back(ow);
			vecDataObjects.push_back(ow);

			auto vecElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(vectorParameterName(j)), QString::fromStdString(vectorParameterName(j)));
			imuElement->appendChild(vecElement);
		}

		imuElement->addHelper(core::HierarchyHelperPtr(new NewStreamQuaternionItemHelper(imuElement->getData(), vecDataObjects[0], vecDataObjects[1], vecDataObjects[2], vecDataObjects[3], imuElement->getName(), -1, 1)));
	}
}
