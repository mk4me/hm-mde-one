#include "CorePCH.h"
#include "ConsoleWidget.h"
#include <QtGui/QScrollBar>

ConsoleWidget::ConsoleWidget(void): 
  Ui::ConsoleWidget()
, QWidget()
{
  setupUi(this); 
}

ConsoleWidget::~ConsoleWidget(void)
{
}

void ConsoleWidget::onEcho()
{
  QString text = lineEdit->text(); 
  //std::string s = text.toUtf8(); 
  //log << s;
  textEdit->insertPlainText(text); 
  textEdit->insertHtml("<br>"); 

  QScrollBar *sb = textEdit->verticalScrollBar();
  sb->setValue(sb->maximum());

  lineEdit->clear(); 
}

void ConsoleWidget::print(const std::string& str)
{
    textEdit->insertPlainText( QString::fromStdString(str) ); 
    QScrollBar *sb = textEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}