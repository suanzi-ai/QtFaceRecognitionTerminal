#ifndef __OBJECT_TEMP_TASK_H
#define __OBJECT_TEMP_TASK_H

#include <QThread>

namespace suanzi {

class ObjectTempTask : QThread {
	
  Q_OBJECT
 public:
  ObjectTempTask(QObject *parent = nullptr);
  ~ObjectTempTask();

 private:
 	void run();
	
 private slots:
  void rx_ambient_temp(float temp);
 

 private:
  
};

}  // namespace suanzi

#endif