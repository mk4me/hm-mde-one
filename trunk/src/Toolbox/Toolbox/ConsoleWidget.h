#ifndef CONSOLE_WIDGET_H
#define CONSOLE_WIDGET_H

#include <QtGui/QWidget>

#include "ui_Console.h"
//#include "Logger.h"

class ConsoleWidget: public QWidget, Ui::Console
{
  Q_OBJECT
public:
  ConsoleWidget(void);
  ~ConsoleWidget(void);

  void print(std::string str); 

public slots: 
  virtual void echo();
};

#endif