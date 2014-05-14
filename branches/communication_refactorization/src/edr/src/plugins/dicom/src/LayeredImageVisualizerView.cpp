#include "DicomPCH.h"
#include "IDicomService.h"
#include "AdnotationsDelegate.h"
#include "LayeredImageVisualizerView.h"
#include "ui_layeredImageVisualizer.h"
#include "LayeredImageVisualizer.h"
#include <QtGui/QGraphicsScene>
#include <QtGui/QPainter>
#include <coreui/WheelGraphicsView.h>
#include <QtGui/QGraphicsItem>
#include <QtGui/QMouseEvent>
#include <webserviceslib/Entity.h>
#include <corelib/IServiceManager.h>
#include "MessageDialog.h"
#include <QtGui/QMessageBox>

using namespace dicom;

LayeredImageVisualizerView::LayeredImageVisualizerView(LayeredImageVisualizer* model, QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/) :
model(model),
QWidget(parent, f),
ui(new Ui::LayeredImageVisualizer()),
adnotationDelegate0(new AdnotationsDelegate(0)),
adnotationDelegate1(new AdnotationsDelegate(1)),
lastView(nullptr)
{
	ui->setupUi(this);
	connect(ui->prevButton, SIGNAL(clicked()), model, SLOT(setPrevSerie()));
	connect(ui->nextButton, SIGNAL(clicked()), model, SLOT(setNextSerie()));
	connect(ui->sliderBar, SIGNAL(valueChanged(int)), model, SLOT(trySetSerie(int)));
	connect(model, SIGNAL(serieChanged()), this, SLOT(refresh()));
	connect(ui->removeButton, SIGNAL(clicked()), model, SLOT(removeSelectedLayers()));
	ui->removeButton->setToolTip(tr("Remove selected tag"));

	ui->treeView->header()->setResizeMode(QHeaderView::ResizeToContents);
	ui->treeView->setItemDelegateForColumn(0, adnotationDelegate0);
	ui->treeView->setItemDelegateForColumn(1, adnotationDelegate1);

	ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(model, SIGNAL(changeSelection(const QModelIndex&)), this, SLOT(changeSelection(const QModelIndex &)));
	connect(ui->treeView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(selectionChanged(const QModelIndex &)));


	coreUI::CoreAction*  undo = new coreUI::CoreAction(tr("Edit"), QIcon(":/dicom/undo.png"), tr("Undo"), this, coreUI::CoreTitleBar::Left);
	coreUI::CoreAction*  redo = new coreUI::CoreAction(tr("Edit"), QIcon(":/dicom/redo.png"), tr("Redo"), this, coreUI::CoreTitleBar::Left);

	QIcon normalIcon(":/dicom/arrowIcon.png");
	normalIcon.addFile(":/dicom/arrowIconDisabled.png", QSize(), QIcon::Disabled);
	coreUI::CoreAction*  nrml = new coreUI::CoreAction(tr("Annotations"), normalIcon, tr("Normal"), this, coreUI::CoreTitleBar::Left);

	coreUI::CoreAction*  move = new coreUI::CoreAction(tr("Annotations"), QIcon(":/dicom/moveIcon.png"), tr("Move"), this, coreUI::CoreTitleBar::Left);
	coreUI::CoreAction*  edit = new coreUI::CoreAction(tr("Annotations"), QIcon(":/dicom/moveIcon.png"), tr("Edit"), this, coreUI::CoreTitleBar::Left);

	coreUI::CoreAction*  curv = new coreUI::CoreAction(tr("Annotations"), QIcon(":/dicom/curveIcon.png"), tr("Add curve"), this, coreUI::CoreTitleBar::Left);
	coreUI::CoreAction*  poly = new coreUI::CoreAction(tr("Annotations"), QIcon(":/dicom/pathIcon.png"), tr("Add polygon"), this, coreUI::CoreTitleBar::Left);

	coreUI::CoreAction*  bone = new coreUI::CoreAction(tr("Annotations"), QIcon(":/dicom/BoneIcon.png"), tr("Tag bone"), this, coreUI::CoreTitleBar::Left);
	coreUI::CoreAction*  skin = new coreUI::CoreAction(tr("Annotations"), QIcon(":/dicom/SkinIcon.png"), tr("Tag skin"), this, coreUI::CoreTitleBar::Left);
	//coreUI::CoreAction*  tend = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/TendonIcon.png"), tr("Tag tendon"), this, coreUI::CoreTitleBar::Left);
	coreUI::CoreAction*  join = new coreUI::CoreAction(tr("Annotations"), QIcon(":/dicom/JointIcon.png"), tr("Tag joint"), this, coreUI::CoreTitleBar::Left);
	coreUI::CoreAction*  infl = new coreUI::CoreAction(tr("Annotations"), QIcon(":/dicom/InflammatoryIcon.png"), tr("Tag region inflamatory synovitis"), this, coreUI::CoreTitleBar::Left);
	//coreUI::CoreAction*  nois = new coreUI::CoreAction(tr("Annotations")  , QIcon(":/dicom/NoiseIcon.png"), tr("Tag noise"), this, coreUI::CoreTitleBar::Left);

	QIcon saveIcon(":/dicom/save.png");
	saveIcon.addFile(":/dicom/saveDisabled.png", QSize(), QIcon::Disabled);
	coreUI::CoreAction*  save = new coreUI::CoreAction(tr("I/O"), saveIcon, tr("Save"), this, coreUI::CoreTitleBar::Left);

	QIcon uploadIcon(":/dicom/upload.png");
	uploadIcon.addFile(":/dicom/uploadDisabled.png", QSize(), QIcon::Disabled);
	coreUI::CoreAction* upld = new coreUI::CoreAction(tr("I/O"), uploadIcon, tr("Upload"), this, coreUI::CoreTitleBar::Left);

	QIcon cropIcon(":/dicom/crop.png");
	cropIcon.addFile(":/dicom/cropDisabled.png", QSize(), QIcon::Disabled);
	coreUI::CoreAction*  crop = new coreUI::CoreAction(tr("Other"), cropIcon, tr("Crop"), this, coreUI::CoreTitleBar::Left);

	QIcon acceptIcon(":/dicom/accept.png");
	acceptIcon.addFile(":/dicom/acceptDisabled.png", QSize(), QIcon::Disabled);

	if (model->userIsReviewer() == true){
		coreUI::CoreAction* acceptAction = new coreUI::CoreAction(tr("Status"), acceptIcon, tr("Accept"), this, coreUI::CoreTitleBar::Left);
		connect(acceptAction, SIGNAL(triggered()), this, SLOT(acceptAnnotation()));

		QIcon rejectIcon(":/dicom/reject.png");
		rejectIcon.addFile(":/dicom/rejectDisabled.png", QSize(), QIcon::Disabled);

		coreUI::CoreAction* rejectAction = new coreUI::CoreAction(tr("Status"), rejectIcon, tr("Reject"), this, coreUI::CoreTitleBar::Left);
		connect(rejectAction, SIGNAL(triggered()), this, SLOT(rejectAnnotation()));
		this->addAction(acceptAction);
		this->addAction(rejectAction);
	}
	else{
		coreUI::CoreAction* toVerifyAction = new coreUI::CoreAction(tr("Status"), acceptIcon, tr("Request verification"), this, coreUI::CoreTitleBar::Left);
		connect(toVerifyAction, SIGNAL(triggered()), this, SLOT(requestAnnotationVerification()));
		this->addAction(toVerifyAction);
	}

	connect(undo, SIGNAL(triggered()), this, SLOT(undo()));
	connect(redo, SIGNAL(triggered()), this, SLOT(redo()));

	connect(nrml, SIGNAL(triggered()), this, SLOT(normalState()));
	connect(move, SIGNAL(triggered()), this, SLOT(moveState()));

	connect(bone, SIGNAL(triggered()), this, SLOT(boneState()));
	connect(skin, SIGNAL(triggered()), this, SLOT(skinState()));
	//connect(tend, SIGNAL(triggered()), this, SLOT(tendonState()));
	connect(join, SIGNAL(triggered()), this, SLOT(jointState()));
	connect(infl, SIGNAL(triggered()), this, SLOT(inflamatoryState()));
	//connect(nois, SIGNAL(triggered()), this, SLOT(noiseState()));

	connect(save, SIGNAL(triggered()), model, SLOT(saveSerie()));
	connect(upld, SIGNAL(triggered()), model, SLOT(uploadSerie()));
	upld->setEnabled(false);
	connect(crop, SIGNAL(triggered()), this, SLOT(crop()));

	// tymczasowo wylaczone
	//this->addAction(undo);
	//this->addAction(redo);

	this->addAction(nrml);
	//this->addAction(move);

	this->addAction(bone);
	this->addAction(skin);
	//this->addAction(tend);
	this->addAction(join);
	this->addAction(infl);
	//this->addAction(nois);

	this->addAction(save);
	this->addAction(upld);
	this->addAction(crop);

	ui->graphicsHolder->setLayout(new QHBoxLayout());
	ui->graphicsHolder->setContentsMargins(0, 0, 0, 0);

	ui->commentGroupBox->setVisible(false);
	ui->noteGroupBox->setVisible(false);

	setActionsEnabled(false);
}

void dicom::LayeredImageVisualizerView::acceptAnnotation()
{
	if (verifySerie() == false || QMessageBox::question(this,
				tr("Annotations acceptance verification"),
				tr("Are You sure You want to permanently accept verified annotations?"),
				QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
		return;
	}

	try{
		//model->trySave();
		model->uploadSerie();
		auto status = webservices::xmlWsdl::AnnotationStatus::Approved;
		model->setStatus(status);
		setAnnotationStatus(status);
		setActionsEnabled(false);
		QMessageBox::information(this, tr("Status change"), tr("Annotation accepted successfully"));
	}
	catch (...){
		QMessageBox::warning(this, tr("Status change"), tr("Annotation status change failed. Please try again or contact software developers"));
	}
}

const bool dicom::LayeredImageVisualizerView::getComment(const QString & title, const QString & content, QString & comment)
{
	MessageDialog dialog(this, title, content);
	auto dr = dialog.exec();
	if (dr == QDialog::Accepted){
		comment = dialog.getText();
		return true;
	}

	return false;
}

void dicom::LayeredImageVisualizerView::rejectAnnotation()
{
	if (verifySerie() == false){
		return;
	}

	QString comment;
	if (getComment(tr("Note"), QString(), comment) == true){
		try{
			model->uploadSerie();
			model->setStatus(webservices::xmlWsdl::AnnotationStatus::Rejected, comment);
			setActionsEnabled(false);
			auto service = core::queryService<IDicomService>(plugin::getServiceManager());
			auto as = service->annotationStatus(model->getCurrentLayerUserName(), model->currnetTrialID());
			setAnnotationStatus(as.status);
			refreshChat(as);
			QMessageBox::information(this, tr("Status change"), tr("Annotation status changed successfully"));
		}
		catch (...){
			QMessageBox::warning(this, tr("Status change"), tr("Annotation status change failed. Please try again or contact software developers"));
		}
	}
}

void dicom::LayeredImageVisualizerView::requestAnnotationVerification()
{
	if (verifySerie() == false){
		return;
	}

	QString comment;
	if (getComment(tr("Comment"), QString(), comment) == true){
		try{
			//model->trySave();
			model->uploadSerie();
			model->setStatus(webservices::xmlWsdl::AnnotationStatus::ReadyForReview, comment);
			setActionsEnabled(false);
			auto service = core::queryService<IDicomService>(plugin::getServiceManager());
			auto as = service->annotationStatus(model->getCurrentLayerUserName(), model->currnetTrialID());
			setAnnotationStatus(as.status);
			refreshChat(as);
			QMessageBox::information(this, tr("Status change"), tr("Annotation sent for verification successfully"));
		}
		catch (...){
			QMessageBox::warning(this, tr("Status change"), tr("Annotation status change failed. Please try again or contact software developers"));
		}
	}
}

void dicom::LayeredImageVisualizerView::setDeletionButtonEnabled(const bool enable)
{
	ui->removeButton->setEnabled(enable);
}

void dicom::LayeredImageVisualizerView::refresh()
{
	int currentSerieNo = model->getCurrentSerieNo();
	int numSeries = model->getNumSeries();

	if (numSeries > 1) {
		ui->sliderContainer->show();
	}
	else {
		ui->sliderContainer->hide();
	}

	ui->sliderBar->blockSignals(true);
	if (currentSerieNo >= 0) {
		ui->sliderBar->setValue(currentSerieNo);
	}
	if (numSeries > 0) {
		ui->sliderBar->setRange(0, numSeries - 1);
		ui->sliderBar->setEnabled(true);
	}
	else {
		ui->sliderBar->setEnabled(false);
	}
	ui->sliderBar->blockSignals(false);

	auto serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());

	if (serie) {
		if (serie->getGraphicsView() != lastView) {
			if (lastView) {
				lastView->hide();
			}
			lastView = serie->getGraphicsView();
			serie->fitToSize();
			if (lastView->parent() != ui->graphicsHolder) {
				ui->graphicsHolder->layout()->addWidget(lastView);
			}
			lastView->show();
		}
		serie->refresh();
		auto treeModel = serie->getLayersModel();
		ui->treeView->setModel(treeModel);
		adnotationDelegate0->setImage(treeModel->getImage());
		adnotationDelegate1->setImage(treeModel->getImage());

		auto expands = treeModel->getExpands();
		for (auto it = expands.begin(); it != expands.end(); ++it) {
			ui->treeView->setExpanded(it->first, it->second);
		}

		int row = 0;
		
		auto idx = treeModel->index(row, 0);

		ui->treeView->setCurrentIndex(idx);

		selectionChanged(idx);
	}
	else {
	}
}

void dicom::LayeredImageVisualizerView::undo()
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
	if (serie) {
		serie->undo();
	}
}

void dicom::LayeredImageVisualizerView::redo()
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
	if (serie) {
		serie->redo();
	}
}

void dicom::LayeredImageVisualizerView::normalState()
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
	if (serie) {
		serie->setNormalState();
	}
}

void dicom::LayeredImageVisualizerView::editSelectedSerie()
{
	QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
	std::set<std::pair<int, int>> rows;
	for (auto it = indexes.begin(); it != indexes.end(); ++it) {
		rows.insert(LayeredModelView::getTagAndIndex(*it));
	}
	if (rows.size() == 1) {
		auto ti = rows.begin();
		model->editSerie(ti->first, ti->second);
	}
}

void dicom::LayeredImageVisualizerView::setActionsEnabled(const bool enable)
{
	auto actionsList = actions();

	for (auto it = actionsList.begin(); it != actionsList.end(); ++it){
		(*it)->setEnabled(enable);
	}

	adnotationDelegate0->setEditionActive(enable);
	adnotationDelegate1->setEditionActive(enable);
}

void dicom::LayeredImageVisualizerView::refreshChat(const IDicomService::AnnotationStatus & as)
{
	if (as.note.empty() == false){
		ui->noteTextEdit->setText(QString::fromStdString(as.note));
		ui->noteGroupBox->setVisible(true);
	}
	else{
		ui->noteGroupBox->setVisible(false);
	}

	if (as.comment.empty() == false){
		ui->commentTextEdit->setText(QString::fromStdString(as.comment));
		ui->commentGroupBox->setVisible(true);
	}
	else{
		ui->commentGroupBox->setVisible(false);
	}
}

const bool dicom::LayeredImageVisualizerView::verifySerie()
{
	QString message(tr("Annotation verification status:\n"));

	int counter = 1;

	if (model->verifyCompletness() == false){
		message += tr("%1: Annotations contain only required properties\n").arg(counter++);
	}

	if (model->verifyImflamatoryLevel() == false){
		message += tr("%1: inflammatory level not set\n").arg(counter++);
	}

	if (model->verifyBloodLevel() == false){
		message += tr("%1: blood level not set\n").arg(counter++);
	}

	if (model->verifyFingerType() == false){
		message += tr("%1: finger type not set\n").arg(counter++);
	}

	if (model->verifyJointType() == false){
		message += tr("%1: joint type not set\n").arg(counter++);
	}

	if (counter > 1){
		message += tr("\nContinue anyway?");
		return QMessageBox::question(this, tr("Incomplete annotation description"), message, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes;
	}

	return true;
}

void dicom::LayeredImageVisualizerView::selectionChanged(const QModelIndex &)
{
	QModelIndexList indexes = ui->treeView->selectionModel()->selectedIndexes();
	std::set<std::pair<int, int>> rows;
	for (auto it = indexes.begin(); it != indexes.end(); ++it) {
		rows.insert(LayeredModelView::getTagAndIndex(*it));
	}
	if (rows.size() == 1) {
		auto ti = rows.begin();
		model->selectLayer(ti->first, ti->second);

		//od�wie�y� stan akcji ze wzgl�du na usera aktualnej adnotacji i jej status

		auto service = core::queryService<IDicomService>(plugin::getServiceManager());
		auto as = service->annotationStatus(model->getCurrentLayerUserName(), model->currnetTrialID());
		setAnnotationStatus(as.status);
		refreshChat(as);

		bool enabled = true;

		if (model->userIsReviewer() == true){
			if (as.status == webservices::xmlWsdl::AnnotationStatus::Approved ||
				as.status == webservices::xmlWsdl::AnnotationStatus::Rejected){
				enabled = false;
			}
		}
		else {
			if (model->getCurrentLayerUserName() == model->getUserName()){
				if (as.status == webservices::xmlWsdl::AnnotationStatus::Approved ||
					as.status == webservices::xmlWsdl::AnnotationStatus::UnderReview ||
					as.status == webservices::xmlWsdl::AnnotationStatus::ReadyForReview){
					enabled = false;
				}
			}
			else{
				enabled = false;
			}
		}

		setActionsEnabled(enabled);
	}

	//ui->treeView->blockSignals(true);
	//ui->treeView->clearSelection();
	//ui->treeView->blockSignals(false);
}

void dicom::LayeredImageVisualizerView::crop()
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
	if (serie) {
		serie->switchCrop();
	}
}

void dicom::LayeredImageVisualizerView::moveState()
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
	if (serie) {
		serie->setMoveState();
	}
}

void dicom::LayeredImageVisualizerView::boneState()
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
	if (serie) {
		serie->setBoneState();
	}
}

void dicom::LayeredImageVisualizerView::skinState()
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
	if (serie) {
		serie->setSkinState();
	}
}

void dicom::LayeredImageVisualizerView::tendonState()
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
	if (serie) {
		serie->setTendonState();
	}
}

void dicom::LayeredImageVisualizerView::jointState()
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
	if (serie) {
		serie->setJointState();
	}
}

void dicom::LayeredImageVisualizerView::inflamatoryState()
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
	if (serie) {
		serie->setInflamatoryState();
	}
}

void dicom::LayeredImageVisualizerView::noiseState()
{
	LayeredSerie* serie = dynamic_cast<LayeredSerie*>(model->getActiveSerie());
	if (serie) {
		serie->setNoiseState();
	}
}

void dicom::LayeredImageVisualizerView::setAnnotationStatus(webservices::xmlWsdl::AnnotationStatus::Type status)
{
	switch (status) {
	case webservices::xmlWsdl::AnnotationStatus::Unspecified:
	case webservices::xmlWsdl::AnnotationStatus::Rejected:
	case webservices::xmlWsdl::AnnotationStatus::UnderConstruction:
		ui->label->setText(tr("ANNOTATIONS - in edition"));
		break;

	case webservices::xmlWsdl::AnnotationStatus::UnderReview:
	case webservices::xmlWsdl::AnnotationStatus::ReadyForReview:
		ui->label->setText(tr("ANNOTATIONS - in verification"));
		break;
	case webservices::xmlWsdl::AnnotationStatus::Approved:
		ui->label->setText(tr("ANNOTATIONS - verified"));
		break;
	}
}

void dicom::LayeredImageVisualizerView::changeSelection(const QModelIndex &mi)
{
	ui->treeView->selectionModel()->select(mi, QItemSelectionModel::ClearAndSelect);
	selectionChanged(mi);
}
