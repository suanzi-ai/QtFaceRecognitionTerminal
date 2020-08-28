#include "temperature_task.hpp"

namespace suanzi {

class RandomTask : public TemperatureTask {
  Q_OBJECT
 public:
  RandomTask(QObject *parent = nullptr);

 private:
  void run();

 private:
  float randomt_;
};

}  // namespace suanzi
