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
#include <fstream>
#include "IMUPerspective.h"
#include "CostumeSkeletonMotionHelper.h"
#include <corelib/IVisualizerManager.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLayout>
#include <plugins/newChart/INewChartVisualizer.h>
#include <corelib/Exceptions.h>
#include <plugins/imuCostume/Streams.h>

class JointStreamExtractor
{
public:
	JointStreamExtractor(JointStreamExtractor && other) : joint(std::move(other.joint)) {}
	JointStreamExtractor(const JointStreamExtractor & other) : joint(other.joint) {}
	JointStreamExtractor(kinematic::SkeletonState::JointConstPtr joint) : joint(joint) {}

	template<typename T>
	inline static bool verify(const T &) { return true; }

	template<typename T>
	void extract(const T & src, IMU::JointStream::value_type & jointData) const {
		jointData.first = src.first;
		jointData.second.globalOrientation = joint->value.globalOrientation();
		jointData.second.localOrientation = joint->value.localOrientation();
		jointData.second.globalPosition = joint->value.globalPosition();
		jointData.second.localPosition = joint->value.localPosition();
	}

private:
	kinematic::SkeletonState::JointConstPtr joint;
};

TIMEMEMBER_EXTRACTOR(globalOrientation);
TIMEMEMBER_EXTRACTOR(localOrientation);
TIMEMEMBER_EXTRACTOR(globalPosition);
TIMEMEMBER_EXTRACTOR(localPosition);

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

template<typename StreamType, typename SensorDataType, typename Extractor>
core::HierarchyDataItemPtr extracImuStreamVector(core::VariantsList & domainData,
	utils::shared_ptr<StreamType> stream,
	SensorDataType & sd,
	const std::string & paramName,
	const QString & units, const double minY, const double maxY)
{
	auto vec3Stream = utils::make_shared<threadingUtils::StreamAdapterT<StreamType::value_type, IMU::Vec3Stream::value_type, Extractor>>(stream);

	auto ow = core::Variant::wrapp<IMU::Vec3Stream>(vec3Stream);
	ow->setMetadata("core/name", paramName);
	sd.domainData.push_back(ow);
	domainData.push_back(ow);

	std::vector<core::VariantConstPtr> vecDataObjects;
	std::list<core::IHierarchyItemPtr> hierarchyItems;

	for (unsigned int j = 0; j < 3; ++j){
		auto scalarStream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::Vec3Stream::value_type, ScalarStream::value_type, IMU::CompoundArrayExtractor>>(vec3Stream, IMU::CompoundArrayExtractor(j));

		ow = core::Variant::wrapp<ScalarStream>(scalarStream);
		ow->setMetadata("core/name", vectorParameterName(j));
		sd.domainData.push_back(ow);
		domainData.push_back(ow);
		vecDataObjects.push_back(ow);

		hierarchyItems.push_back(utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(paramName), QString::fromStdString(vectorParameterName(j))));
	}

	auto helper = utils::make_shared<NewStreamVector3ItemHelper>(ow, vecDataObjects[0], vecDataObjects[1], vecDataObjects[2], QString::fromStdString(paramName), units, minY, maxY);

	auto imuElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(paramName), QString::fromStdString(paramName), helper);

	for (auto & hi : hierarchyItems)
	{
		imuElement->appendChild(hi);
	}

	return imuElement;
}

template<typename StreamType, typename SensorDataType, typename Extractor>
core::HierarchyDataItemPtr extracImuStreamQuat(core::VariantsList & domainData,
	utils::shared_ptr<StreamType> stream,
	SensorDataType & sd,
	const std::string & paramName)
{
	auto orientStream = utils::shared_ptr<IMU::QuatStream>(new threadingUtils::StreamAdapterT<StreamType::value_type, IMU::QuatStream::value_type, Extractor>(stream));

	auto ow = core::Variant::wrapp<IMU::QuatStream>(orientStream);
	ow->setMetadata("core/name", QObject::tr("Orientation").toStdString());
	sd.domainData.push_back(ow);
	domainData.push_back(ow);

	std::vector<core::VariantConstPtr> vecDataObjects;
	std::list<core::IHierarchyItemPtr> hierarchyItems;

	for (unsigned int j = 0; j < 4; ++j){
		auto scalarStream = utils::shared_ptr<ScalarStream>(new threadingUtils::StreamAdapterT<IMU::QuatStream::value_type, ScalarStream::value_type, IMU::CompoundArrayExtractor>(orientStream, IMU::CompoundArrayExtractor(j)));

		ow = core::Variant::wrapp<ScalarStream>(scalarStream);
		ow->setMetadata("core/name", vectorParameterName(j));
		sd.domainData.push_back(ow);
		domainData.push_back(ow);
		vecDataObjects.push_back(ow);

		hierarchyItems.push_back(utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QString::fromStdString(vectorParameterName(j)), QString::fromStdString(vectorParameterName(j))));
	}

	auto helper = utils::make_shared<NewStreamQuaternionItemHelper>(ow, vecDataObjects[0], vecDataObjects[1], vecDataObjects[2], vecDataObjects[3], QObject::tr("Orientation"), -1, 1);

	auto imuElement = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Orientation"), QObject::tr("Orientation"), helper);

	for (auto & hi : hierarchyItems)
	{
		imuElement->appendChild(hi);
	}

	return imuElement;
}

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
			std::set<imuCostume::CostumeRawIO::CostumeAddress> costumes;
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
						
						costumes.insert(id);

						//costumesDataFrame.insert(CostumesDataFrame::value_type(id, streamBuffer));

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

			if (costumes.empty() == false){
				std::lock_guard<std::recursive_mutex > lock(synch);			
				for (auto & rec : recordings)
				{
					CostumesDataFrame cdf;
					for (const auto & ctr : rec->costumesToRecord)
					{
						auto it = costumes.find(ctr.first);
						if (it != costumes.end()){
							RecordedCostumeData rcd;

							auto cIT = costumesData.find(ctr.first);

							utils::shared_ptr<SkeletonWithStreamData> kinematicStream;
							cIT->second.domainData.back()->tryGet(kinematicStream);
							::SkeletonStateStream::value_type sdata;
							kinematicStream->states->data(sdata);

							IMU::SensorsStreamData cssd;
							cIT->second.completeImuStream->data(cssd);
							rcd.timestamp = cssd.timestamp;
							
							//zapis danych tego kostiumu
							for (const auto & sID : ctr.second)
							{
								auto jointName = cIT->second.profile->sensorsDescriptions.find(sID)->second.jointName;

								auto idx = kinematicStream->nodesMapping.right.find(jointName)->get_left();

								RecordedSensorData rsd;
								auto sdIT = cIT->second.sensorsData.find(sID);
								auto ow = sdIT->second.domainData.front();
								utils::shared_ptr<IMUStream> imuStream;
								ow->tryGet(imuStream);
								IMUStream::value_type data;
								imuStream->data(data);

								rsd.accelerometer = data.second.accelerometer;
								rsd.gyroscope = data.second.gyroscope;
								rsd.magnetometer = data.second.magnetometer;
								rsd.orientation = data.second.orientation;

								auto it = sdIT->second.domainData.begin();
								std::advance(it, 18);

								ow = *it;								
								ow->tryGet(imuStream);								
								imuStream->data(data);

								rsd.estimatedOrientation = data.second.orientation;

								//todo - z kinematic
								rsd.jointOrientation = sdata[idx].rotation;

								rcd.sensorsData.insert(std::map<imuCostume::Costume::SensorID, RecordedSensorData>::value_type(sID, rsd));
							}
							cdf.insert(CostumesDataFrame::value_type(ctr.first, rcd));
						}
					}

					rec->costumesDataBuffer.pushData(std::move(cdf));
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
						auto upro = utils::make_shared<IMU::CostumeProfile>(SerializableCostumeProfile::unpack(pro, this));
						costumesProfiles_.insert(CostumesProfiles::value_type(upro->name, upro));
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

					auto sPro = SerializableCostumeProfile::pack(*pro.second);
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

std::string sensorParameterName(const unsigned int idx)
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

std::string vectorParameterName(const unsigned int idx)
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
	//core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>("Raw Data", "Raw communication data", QIcon());	
	//hierarchyTransaction->addRoot(root);

	const auto & sc = cData.sensorsConfiguration;
	auto ow = core::Variant::create<RawDataStream>();
	ow->setMetadata("core/name", QObject::tr("Raw Data").toStdString());
	ow->set(cData.rawDataStream);
	cData.domainData.push_back(ow);

	//core::HierarchyDataItemPtr item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Raw stream"), QObject::tr("Raw Ethernet costume frames"));
	//root->appendChild(item);

	cData.CANopenStream.reset(new threadingUtils::StreamAdapterT<RawDataStream::value_type, CANopenFramesStream::value_type, RawToCANopenExtractor>(cData.rawDataStream, RawToCANopenExtractor()));
	ow = core::Variant::create<CANopenFramesStream>();
	ow->setMetadata("core/name", QObject::tr("CANopen Data").toStdString());
	ow->set(cData.CANopenStream);
	cData.domainData.push_back(ow);

	//item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("CANopen stream"), QObject::tr("Extracted CANopen frames stream"));
	//root->appendChild(item);

	cData.costumeStream.reset(new threadingUtils::StreamAdapterT<CANopenFramesStream::value_type, CostumeStream::value_type, CANopenDataExtractor>(cData.CANopenStream, CANopenDataExtractor()));
	ow = core::Variant::create<CostumeStream>();
	ow->setMetadata("core/name", QObject::tr("Costume Data").toStdString());
	ow->set(cData.costumeStream);
	cData.domainData.push_back(ow);

	//item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Sensors stream"), QObject::tr("Grouped sensors data and status stream"));
	//root->appendChild(item);

	//return root;
	return core::HierarchyItemPtr();
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

	fillRawCostumeData(cData);

	try{
		auto t = memoryDM->transaction();
		for (const auto & ow : cData.domainData){
			t->addData(ow);
		}

		it->second = cData;

		//auto rootItem = utils::make_shared<core::HierarchyItem>(QObject::tr("Costume %1:%2 data").arg(QString::fromStdString(it->first.ip)).arg(it->first.port), "", QIcon());
		//rootItem->appendChild(hItem);		
		//memoryDM->hierarchyTransaction()->addRoot(rootItem);
		///it->second.hierarchyRootItem = rootItem;
	}
	catch (std::exception & e){
		PLUGIN_LOG_ERROR(e.what());		
	}
	catch (...){
		PLUGIN_LOG_ERROR("Unknown error while loading raw costume data");
	}
}
void IMUCostumeDataSource::loadCalibratedCostume(const CostumeID & id,
	CostumeProfilePtr profile)
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

	if (profile->sensorsDescriptions.size() > imusCount)
	{
		throw std::runtime_error("Mismatch in number of loaded sensors orientation estimation algorithms and costume configuration");
	}

	CostumeData cData = it->second;

	fillRawCostumeData(cData);

	auto root = utils::make_shared<core::HierarchyItem>(QObject::tr("Costume %1:%2 data").arg(QString::fromStdString(it->first.ip)).arg(it->first.port), "", QIcon());
	//core::HierarchyItemPtr root = utils::make_shared<core::HierarchyItem>(QObject::tr("Unpacked Data"), QObject::tr("Unpacked IMU sensors data"), QIcon());

	//adapter na pe�ne dane z kostiumu nap�dzaj�ce strumie� z estymacj�
	cData.completeImuStream.reset(new ExtractedCostumeStreamAdapter(cData.costumeStream, CostumeIMUExtractor(cData.sensorsConfiguration)));

	auto ow = core::Variant::create<IMU::SensorsStream>();
	ow->setMetadata("core/name", QObject::tr("Complete unpacked IMU data").toStdString());
	ow->set(IMU::SensorsStreamPtr(cData.completeImuStream));
	cData.domainData.push_back(ow);

	core::IHierarchyItemPtr item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("IMU stream"), QObject::tr("IMU sensors data stream"));
	root->appendChild(item);

	unpackSensorsStream(cData.completeImuStream, cData.sensorsData, item, cData.domainData, profile);

	auto estimatedData = utils::make_shared<threadingUtils::StreamProcessorT<IMU::SensorsStreamData, OrientationEstimator>>(cData.completeImuStream,
		OrientationEstimator(profile));

	ow = core::Variant::create<IMU::SensorsStream>();
	ow->setMetadata("core/name", QObject::tr("Filtered IMU stream").toStdString());
	ow->set(IMU::SensorsStreamPtr(estimatedData));
	cData.domainData.push_back(ow);

	item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Filtered IMU stream"), QObject::tr("IMU filtered orientation stream"));
	root->appendChild(item);

	unpackSensorsStream(estimatedData, cData.sensorsData, item, cData.domainData, profile);

	cData.skeletonMotion = utils::make_shared<IMU::CostumeSkeletonMotion>();
	ow = core::Variant::create<IMU::CostumeSkeletonMotion>();
	ow->setMetadata("core/name", QObject::tr("Skeleton").toStdString());
	ow->set(cData.skeletonMotion);
	cData.domainData.push_back(ow);

	//item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Skeleton"), QObject::tr("Calibrated and filtrated skeleton"));
	//root->appendChild(item);

	//mapowanie pozycji wektora do nazwy jointa w szkielecie i stanie

	auto visitor = [&cData](kinematic::JointConstPtr joint, kinematic::Joint::size_type) -> void
			{
				cData.skeletonMotion->dataToModelMapping.insert(DataIndexToJointMapping::value_type(cData.skeletonMotion->dataToModelMapping.size(), joint->value.name));
			};

	utils::TreeNode::visitLevelOrder(profile->skeleton->root, visitor);

	cData.skeletonMotion->skeleton = profile->skeleton;
	cData.skeletonMotion->stream.reset(new RealMotionStream(estimatedData,
		ExtractCostumeMotion(profile, cData.skeletonMotion->dataToModelMapping)));

	ow = core::Variant::create<MotionStream>();
	ow->setMetadata("core/name", QObject::tr("Skeleton stream").toStdString());
	ow->set(MotionStreamPtr(cData.skeletonMotion->stream));
	cData.domainData.push_back(ow);

	//item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Skeleton stream"), QObject::tr("Calibrated and filtrated skeleton stream"));
	//root->appendChild(item);

	{
		//dane dla kinematic
		auto kStream = utils::make_shared<SkeletonWithStreamData>();
		kStream->skeleton = profile->skeleton;
		kStream->nodesMapping = kinematic::SkeletonState::createMapping(*profile->skeleton);
		//SkeletonStateStream

		auto skeletonState = kinematic::SkeletonState::create(*profile->skeleton);

		std::list<kinematic::SkeletonState::JointConstPtr> joints;

		auto jointsGrabberVisitor = [&joints](kinematic::SkeletonState::JointConstPtr joint, std::size_t level)
		{
			if (joint != nullptr){
				joints.push_back(joint);
			}
		};

		utils::TreeNode::visitLevelOrder(skeletonState.root(), jointsGrabberVisitor);

		auto sStateStream = utils::make_shared<threadingUtils::StreamAdapterT<MotionStream::value_type, IMU::SkeletonStateStream::value_type, KinematicStreamExtractor>>(cData.skeletonMotion->stream, KinematicStreamExtractor(std::move(skeletonState)));

		cData.domainData.push_back(core::Variant::wrapp<IMU::SkeletonStateStream>(sStateStream));

		kStream->states.reset(new threadingUtils::StreamAdapterT<IMU::SkeletonStateStream::value_type, ::SkeletonStateStream::value_type, IMU::TimeRemoverExtractor>(sStateStream));

		ow = core::Variant::create<SkeletonWithStreamData>();
		ow->setMetadata("core/name", QObject::tr("Kinematic stream").toStdString());
		ow->set(kStream);
		cData.domainData.push_back(ow);

		item = utils::make_shared<core::HierarchyDataItem>(ow, QIcon(), QObject::tr("Kinematic stream"), QObject::tr("Kinematic 3D stream"));
		root->appendChild(item);

		item = utils::make_shared<core::HierarchyItem>(QObject::tr("Active joints"), QObject::tr("Joints with sensors"));
		root->appendChild(item);
		//TODO
		//dodać gałęzie z jointami i ich danymi: lokalny kąt w szkielecie, globalny kąt w świecie, lokalna pozycja względem rodzica, globalna pozycja

		for (const auto & sd : profile->sensorsDescriptions)
		{
			auto it = std::find_if(joints.begin(), joints.end(), [&sd](kinematic::SkeletonState::JointConstPtr joint)
			{
				if (joint != nullptr){
					if (joint->value.name() == sd.second.jointName){
						return true;
					}
				}

				return false;
			});

			if (it != joints.end()){

				auto sIT = cData.sensorsData.find(sd.first);

				//TODO
				//dodać gałąź jointa active!!

				auto stream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::SkeletonStateStream::value_type, JointStream::value_type, JointStreamExtractor>>(sStateStream, JointStreamExtractor(*it));
				cData.domainData.push_back(core::Variant::wrapp<JointStream>(stream));
				auto jitem = utils::make_shared<core::HierarchyItem>(QString::fromStdString((*it)->value.name()), QString::fromStdString((*it)->value.name()));
				item->appendChild(jitem);			

				//global orient
				{
					jitem->appendChild(extracImuStreamQuat<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorglobalOrientation>(cData.domainData, stream,
						sIT->second, QObject::tr("Global orientation").toStdString()));
				}

				//local orient
				{
					jitem->appendChild(extracImuStreamQuat<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalOrientation>(cData.domainData, stream,
						sIT->second, QObject::tr("Local orientation").toStdString()));
				}

				//local pos
				{
					jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorglobalPosition>(cData.domainData, stream,
						sIT->second, QObject::tr("Global position").toStdString(), QString(), std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max()));
				}

				//global pos
				{
					jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalPosition>(cData.domainData, stream,
						sIT->second, QObject::tr("Local position").toStdString(), QString(), std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max()));
				}

				joints.erase(it);
			}
		}

		item = utils::make_shared<core::HierarchyItem>(QObject::tr("Inactive joints"), QObject::tr("Joints with no sensors attached"));
		root->appendChild(item);

		for (const auto & joint : joints){

			auto idIT = std::find_if(profile->sensorsDescriptions.begin(), profile->sensorsDescriptions.end(), [&joint](const IMU::CostumeProfile::SensorsDescriptions::value_type & sd)
			{
				return sd.second.jointName == joint->value.name();
			});

			//TODO
			//dodać gałąź jointa inactive!!
			auto sIT = cData.sensorsData.find(idIT->first);

			//TODO
			//dodać gałąź jointa active!!

			auto stream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::SkeletonStateStream::value_type, JointStream::value_type, JointStreamExtractor>>(sStateStream, JointStreamExtractor(joint));
			cData.domainData.push_back(core::Variant::wrapp<JointStream>(stream));
			auto jitem = utils::make_shared<core::HierarchyItem>(QString::fromStdString(joint->value.name()), QString::fromStdString(joint->value.name()));
			item->appendChild(jitem);

			//global orient
			{
				jitem->appendChild(extracImuStreamQuat<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorglobalOrientation>(cData.domainData, stream,
					sIT->second, QObject::tr("Global orientation").toStdString()));
			}

			//local orient
			{
				jitem->appendChild(extracImuStreamQuat<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalOrientation>(cData.domainData, stream,
					sIT->second, QObject::tr("Local orientation").toStdString()));
			}

			//local pos
			{
				jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorglobalPosition>(cData.domainData, stream,
					sIT->second, QObject::tr("Global position").toStdString(), QString(), std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max()));
			}

			//global pos
			{
				jitem->appendChild(extracImuStreamVector<JointStream, IMU::IMUCostumeDataSource::SensorData, TimeMemberExtractorlocalPosition>(cData.domainData, stream,
					sIT->second, QObject::tr("Local position").toStdString(), QString(), std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max()));
			}
		}
	}

	try{
		{
			auto t = memoryDM->transaction();
			for (const auto & ow : cData.domainData){
				t->addData(ow);
			}
		}

		cData.profile = profile;
		it->second = cData;
		
		//auto troot = utils::make_shared<core::HierarchyItem>(QObject::tr("Costume %1:%2 data").arg(QString::fromStdString(it->first.ip)).arg(it->first.port), "", QIcon());
		//troot->appendChild(hItem);
		//troot->appendChild(root);
		//memoryDM->hierarchyTransaction()->addRoot(troot);
		memoryDM->hierarchyTransaction()->addRoot(root);

		//it->second.hierarchyRootItem = troot;
		it->second.hierarchyRootItem = root;
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

void IMUCostumeDataSource::registerOrientationEstimationAlgorithm(IIMUOrientationEstimationAlgorithmPtr algorithm)
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

void IMUCostumeDataSource::registerCostumeCalibrationAlgorithm(IMUCostumeCalibrationAlgorithmPtr algorithm)
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

void IMUCostumeDataSource::registerMotionEstimationAlgorithm(IMUCostumeMotionEstimationAlgorithmPtr algorithm)
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

void IMUCostumeDataSource::registerSkeletonModel(SkeletonPtr skeleton)
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
		profile.skeleton == nullptr || profile.sensorsDescriptions.empty() == true){

		ret = false;
	}

	return ret;
}

void IMUCostumeDataSource::registerCostumeProfile(CostumeProfilePtr profile)
{
	if (verifyProfile(*profile) == false){
		throw std::runtime_error("Incomplete profile");
	}

	std::lock_guard<std::recursive_mutex> lock(synch);

	if (costumesProfiles_.find(profile->name) != costumesProfiles_.end()){
		throw std::runtime_error("Profile already registered");
	}

	//produjemy rejestrowac brakujace elementy profilu

	if (profile->skeleton != nullptr){
		if (skeletonModels_.find(profile->skeleton->id) == skeletonModels_.end()){
			skeletonModels_.insert(SkeletonModels::value_type(profile->skeleton->id, profile->skeleton));
		}
	}

	//algorytm kalibracji
	if (calibrationAlgorithms_.find(profile->calibrationAlgorithm->ID()) == calibrationAlgorithms_.end())
	{
		calibrationAlgorithms_.insert(CostumeCalibrationAlgorithms::value_type(profile->calibrationAlgorithm->ID(), profile->calibrationAlgorithm));
	}

	//algorytm estymacji ruchu
	if (motionEstimationAlgorithms_.find(profile->motionEstimationAlgorithm->ID()) == motionEstimationAlgorithms_.end())
	{
		motionEstimationAlgorithms_.insert(CostumeMotionEstimationAlgorithms::value_type(profile->motionEstimationAlgorithm->ID(), profile->motionEstimationAlgorithm));
	}

	// algorytmy estymacji
	for (const auto & ea : profile->sensorsDescriptions)
	{
		if (orientationEstimationAlgorithms_.find(ea.second.orientationEstimationAlgorithm->ID()) == orientationEstimationAlgorithms_.end()){
			orientationEstimationAlgorithms_.insert(OrientationEstimationAlgorithms::value_type(ea.second.orientationEstimationAlgorithm->ID(), ea.second.orientationEstimationAlgorithm));
		}
	}

	//rejestrujemy profil
	costumesProfiles_.insert(CostumesProfiles::value_type(profile->name, profile));
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
	core::IHierarchyItemPtr root,
	core::VariantsList & domainData,
	CostumeProfileConstPtr profile)
{
	for (auto & sd : sensorsData)
	{		
		if (profile->sensorsDescriptions.find(sd.first) == profile->sensorsDescriptions.end()){
			continue;
		}

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
			imuItem->appendChild(extracImuStreamVector<IMUStream, IMU::IMUCostumeDataSource::SensorData, IMU::TimeMemberExtractoraccelerometer>(domainData, imuStream,
				sd.second, sensorParameterName(0), QString("m/s^2"), -15.0, 15.0));			
		}

		//GYRO
		{
			imuItem->appendChild(extracImuStreamVector<IMUStream, IMU::IMUCostumeDataSource::SensorData, IMU::TimeMemberExtractorgyroscope>(domainData, imuStream,
				sd.second, sensorParameterName(1), QString("rad/s"), -0.3, 0.3));
		}

		//MAG
		{
			imuItem->appendChild(extracImuStreamVector<IMUStream, IMU::IMUCostumeDataSource::SensorData, IMU::TimeMemberExtractormagnetometer>(domainData, imuStream,
				sd.second, sensorParameterName(2), "", -2.5, 2.5));
		}

		//ORIENT
		{
			imuItem->appendChild(extracImuStreamQuat<IMUStream, IMU::IMUCostumeDataSource::SensorData, IMU::TimeMemberExtractororientation>(domainData, imuStream,
				sd.second, QObject::tr("Orientation").toStdString()));
		}	
	}
}
