#include "DicomPCH.h"
#include "DicomSourceWidget.h"
#include "ui_DicomSource.h"
#include <QtWidgets/QFileDialog>
#include <corelib/Filesystem.h>
#include "DicomSource.h"
#include <QtWidgets/QMessageBox>

//#include <hmdbserviceslib/WSConnection.h>
#include <hmdbserviceslib/IBasicQueriesWS.h>
#include <hmdbserviceslib/BasicQueriesWS.h>
#include "hmdbserviceslib/BasicUpdatesWS.h"
//#include "hmdbserviceslib/DownloadHelper.h"
#include "hmdbserviceslib/IFileStoremanWS.h"
#include "hmdbserviceslib/FileStoremanWS.h"
//#include "hmdbserviceslib/FtpsConnection.h"
//#include "hmdbserviceslib/UploadHelper.h"

using namespace dicom;

DicomSourceWidget::DicomSourceWidget(DicomSource* source) :
dicomSource(source),
ui(new Ui::DicomSource)
{
	ui->setupUi(this);
	connect(ui->openProjectButton, SIGNAL(clicked()), this, SLOT(onOpenProject()));
	connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(onSendTest()));
}

void dicom::DicomSourceWidget::onOpenProject()
{
	QString dirPath = QFileDialog::getExistingDirectory(this);
	if (dirPath.isNull() == false) {
		QDir fileDir(dirPath);
		QFileInfoList fileList = fileDir.entryInfoList();

		QRegExp rxlen("[0-9]{4}-[0-9]{2}-[0-9]{2}-S[0-9]{4}\\.xml");
		for(const auto & info : fileList) {
			QString filename = info.fileName();
			if (rxlen.exactMatch(filename)) {
				core::Filesystem::Path path(info.filePath().toStdString());
				if (core::Filesystem::pathExists(path)) {
					dicomSource->openInternalDataMainFile(path);
					return;
				}
			}
		}
		QMessageBox::warning(this, tr("Warning"), tr("Given folder was not proper"));
	}
}

void dicom::DicomSourceWidget::onSendTest()
{
	/*
	namespace ws = hmdbServices;
	std::string caPath = (plugin::getPaths()->getResourcesPath() / "v21.pjwstk.edu.pl.crt").string();
	ws::WSSecureConnectionPtr queriesConnection = utils::make_shared<ws::WSSecureConnection>();
	queriesConnection->setCAPath(caPath);
	queriesConnection->setUrl("https://v21.pjwstk.edu.pl/HMDB/res/BasicQueriesWSStandalone.wsdl");
	queriesConnection->setCredentials("usg-medusa20", "usg-medusamw");

	ws::FtpsConnectionPtr  motionFtps = utils::make_shared<ws::FtpsConnection>();
	motionFtps->setUrl("ftps://v21.pjwstk.edu.pl/");
	motionFtps->setCredentials("testUser", "testUser");

	ws::MotionBasicQueriesWSPtr queriesService(new ws::MotionBasicQueriesWS());
	queriesService->setConnection(queriesConnection);
	//std::string performers = queriesService->listPerformersXML();

	ws::WSSecureConnectionPtr updateConnection = utils::make_shared<ws::WSSecureConnection>();
	updateConnection->setCAPath(caPath);
	updateConnection->setUrl("https://v21.pjwstk.edu.pl/HMDB/BasicUpdatesWS.svc?wsdl");
	updateConnection->setCredentials("usg-medusa20", "usg-medusamw");

	ws::MotionBasicUpdatesWSPtr updateService(new ws::MotionBasicUpdatesWS());
	updateService->setConnection(updateConnection);

	ws::WSSecureConnectionPtr storemanConnection = utils::make_shared<ws::WSSecureConnection>();
	storemanConnection->setUrl("https://v21.pjwstk.edu.pl/HMDB/FileStoremanWS.svc?wsdl");
	storemanConnection->setCredentials("usg-medusa20", "usg-medusamw");
	storemanConnection->setCAPath(caPath);
	ws::MotionFileStoremanWSPtr fileStoreman = utils::make_shared<ws::MotionFileStoremanWS>();
	fileStoreman->setConnection(storemanConnection);

	//ws::BasicDownloadHelper fileDownloadHelper;
	//fileDownloadHelper.configure(fileStoreman, motionFtps);
	//fileDownloadHelper.setDownload(22831, "E:/Testtt.xml");
	//auto ret = fileDownloadHelper.get();

	ws::BasicUploadHelper fileUploaderHelper;
	fileUploaderHelper.configure(motionFtps);
	fileUploaderHelper.setFileUpload("C:/Users/Wojciech/Desktop/testImport1_6/2013-09-03-S0005/2013-09-03-S0005-T0001.xml", "BDR/635228989817457423/2013-08-21-S0007-T0013.xml");
	auto res = fileUploaderHelper.put();

	ws::FtpsConnectionPtr  writerFps = utils::make_shared<ws::FtpsConnection>();
	writerFps->setUrl("ftps://v21.pjwstk.edu.pl/");
	writerFps->setCredentials("testUser", "testUser");

	ws::BasicUploadHelper fileUploaderHelper;
	fileUploaderHelper.configure(fileStoreman, writerFps);
	fileUploaderHelper.setFileUpload("C:/Users/Wojciech/Desktop/testImport1_6/2013-09-03-S0005/2013-09-03-S0005-T0001.xml", "BDR/w/2013-08-21-S0007-T0013.userusg1.xml");
	auto res = fileUploaderHelper.put();
	auto errorMsg = fileUploaderHelper.errorMessage();

	int res2 = fileStoreman->storeTrialFile(3906, "/BDR/w", "test xml file", "2013-08-21-S0007-T0013.xml");
	//int res = fileStoreman->storeTrialFile(3906, "BDR/635228989817457423", "test xml file", "2013-08-21-S0007-T0013.xml"  );
	*/
}