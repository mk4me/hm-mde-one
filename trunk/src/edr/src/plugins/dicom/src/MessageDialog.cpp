#include "DicomPCH.h"
#include "MessageDialog.h"

MessageDialog::MessageDialog(QWidget * parent, const QString & title, const QString & content)
	: QDialog(parent)
{
	setupUi(this);
	titleLabel->setText(title);
	textEdit->setText(content);
	textEdit->setAcceptRichText(false);
	setModal(true);
}

MessageDialog::~MessageDialog()
{

}

const QString MessageDialog::getText() const
{
	return textEdit->toPlainText();
}