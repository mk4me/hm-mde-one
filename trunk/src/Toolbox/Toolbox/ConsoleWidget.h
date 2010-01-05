#pragma once
#include <QtGui/QWidget>

#include "UI/ui_Console.h"
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

