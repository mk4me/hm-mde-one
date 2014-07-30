/********************************************************************
	created:  2014/07/23	14:39:32
	filename: DataConfigurationWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__DATACONFIGURATIONWIDGET_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__DATACONFIGURATIONWIDGET_H__

#include <QtGui/QWidget>

namespace Ui
{
	class DataConfigurationWidget;
}

class DataConfigurationWidget : public QWidget
{

	Q_OBJECT
		Q_PROPERTY(QString url READ url WRITE setUrl RESET resetUrl NOTIFY urlChanged)
		Q_PROPERTY(QString certificatePath READ certificatePath WRITE setCertificatePath RESET resetCertificatePath NOTIFY certificatePathChanged)

public:
	DataConfigurationWidget(QWidget * parent = nullptr, Qt::WindowFlags f = 0);

	virtual ~DataConfigurationWidget();

	const QString url() const;
	const QString certificatePath() const;

	public slots:

	void setUrl(const QString & url);
	void setCertificatePath(const QString & caPath);

	void resetUrl();
	void resetCertificatePath();

	void openCertificateFile();

signals:

	void urlChanged(const QString & url);
	void certificatePathChanged(const QString & caPath);

private:

	Ui::DataConfigurationWidget * ui;
};

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__DATACONFIGURATIONWIDGET_H__