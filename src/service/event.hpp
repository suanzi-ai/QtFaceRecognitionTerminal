#pragma once

#include <eventpp/eventdispatcher.h>

#include <nlohmann/json.hpp>

namespace suanzi {
using json = nlohmann::json;
typedef const json EventData;
typedef const json EmitCallbackData;
typedef std::function<void(const json &)> ResultCallback;
typedef eventpp::EventDispatcher<std::string,
                                 void(EventData &body, ResultCallback cb)>
    EventEmitter;
typedef std::shared_ptr<EventEmitter> EventEmitterPtr;
}  // namespace suanzi
