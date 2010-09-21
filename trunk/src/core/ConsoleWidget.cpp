#include "ConsoleWidget.h"
#include <QtGui/QScrollBar>

ConsoleWidget::ConsoleWidget(void): 
  Ui::Console()
, QWidget()
{
  setupUi(this); 

  connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(echo()));
}

ConsoleWidget::~ConsoleWidget(void)
{
}

void ConsoleWidget::echo()
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

void ConsoleWidget::print( std::string str )
{
  textEdit->insertPlainText(str.c_str()); 
  QScrollBar *sb = textEdit->verticalScrollBar();
  sb->setValue(sb->maximum());
}