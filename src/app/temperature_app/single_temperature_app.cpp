#include "single_temperature_app.hpp"
#include <cmath>
#include <quface-io/engine.hpp>
#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

SingleTemperatureApp::SingleTemperatureApp(TemperatureTask *temperature_task)
    : TemperatureApp(temperature_task) {}

void SingleTemperatureApp::read_temperature(const QRectF &face_area,
                                            bool valid_face_area) {}
