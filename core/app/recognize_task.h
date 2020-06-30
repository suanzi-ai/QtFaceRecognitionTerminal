#ifndef RECOGNIZE_TASK_H
#define RECOGNIZE_TASK_H

#include <QObject>
#include "pingpangbuffer.h"
#include "person.h"

namespace suanzi {

class RecognzieTask : QObject {

    Q_OBJECT
 public:
    RecognzieTask(QObject *parent = nullptr);
    ~RecognzieTask();

 private slots:
    void rxFrame(PingPangBuffer<MmzImage> *buffer);


 signals:
    void txResult(Person person);

 private:

};

}  // namespace suanzi

#endif
