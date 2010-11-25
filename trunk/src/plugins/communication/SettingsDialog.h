#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QFormLayout>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFileDialog>

class SettingsDialog : public QDialog
{
public:
	SettingsDialog(QWidget* parent = 0);
	virtual ~SettingsDialog(void);
	const QString getFtpLogin() const;
	const QString getFtpPassword() const;
	const QString getFtpUri() const;
	const QString getWsdlLogin() const;
	const QString getWsdlPassword() const;
	const QString getWsdlFSSDocument() const;
	const QString getWsdlBQSDocument() const;
	const QString getWsdlBUSDocument() const;
	void setFtpLogin(const QString& login);
	void setFtpPassword(const QString& password);
	void setFtpUri(const QString& uri);
	void setWsdlLogin(const QString& login);
	void setWsdlPassword(const QString& password);
	void setWsdlFSSDocument(const QString& uri);
	void setWsdlBQSDocument(const QString& uri);
	void setWsdlBUSDocument(const QString& uri);

	private slots:
		void fssOpenFile();
		void bqsOpenFile();
		void busOpenFile();
private:
	Q_OBJECT
	QGroupBox* ftpGroup;
	QGroupBox* wsdlGroup;
	QLabel* wsdlLoginLabel;
	QLabel* wsdlPasswordLabel;
	QLabel* wsdlFSSLabel;
	QLabel* wsdlBQSLabel;
	QLabel* wsdlBUSLabel;
	QLineEdit* ftpLoginLineEdit;
	QLineEdit* ftpPasswordLineEdit;
	QLineEdit* ftpUriLineEdit;
	QLineEdit* wsdlLoginLineEdit;
	QLineEdit* wsdlPasswordLineEdit;
	QLineEdit* wsdlFSSLineEdit;
	QLineEdit* wsdlBQSLineEdit;
	QLineEdit* wsdlBUSLineEdit;
	QPushButton* fssButton;
	QPushButton* bqsButton;
	QPushButton* busButton;
	QFormLayout* ftpLayout;
	QDialogButtonBox* buttonsBox;
};
#endif
