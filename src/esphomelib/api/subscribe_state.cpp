#include "esphomelib/defines.h"

#ifdef USE_API

#include "esphomelib/api/subscribe_state.h"
#include "esphomelib/log.h"

ESPHOMELIB_NAMESPACE_BEGIN

namespace api {

static const char *TAG = "api.subscribe_state";

#ifdef USE_BINARY_SENSOR
bool InitialStateIterator::on_binary_sensor(binary_sensor::BinarySensor *binary_sensor) {
  if (!binary_sensor->has_state())
    return true;

  return this->client_->send_binary_sensor_state(binary_sensor, binary_sensor->state);
}
#endif
#ifdef USE_COVER
bool InitialStateIterator::on_cover(cover::Cover *cover) {
  if (!cover->has_state())
    return true;

  return this->client_->send_cover_state(cover);
}
#endif
#ifdef USE_FAN
bool InitialStateIterator::on_fan(fan::FanState *fan) {
  return this->client_->send_fan_state(fan);
}
#endif
#ifdef USE_LIGHT
bool InitialStateIterator::on_light(light::LightState *light) {
  return this->client_->send_light_state(light);
}
#endif
#ifdef USE_SENSOR
bool InitialStateIterator::on_sensor(sensor::Sensor *sensor) {
  if (!sensor->has_state())
    return true;

  return this->client_->send_sensor_state(sensor, sensor->state);
}
#endif
#ifdef USE_SWITCH
bool InitialStateIterator::on_switch(switch_::Switch *switch_) {
  return this->client_->send_switch_state(switch_, switch_->state);
}
#endif
#ifdef USE_TEXT_SENSOR
bool InitialStateIterator::on_text_sensor(text_sensor::TextSensor *text_sensor) {
  if (!text_sensor->has_state())
    return true;

  return this->client_->send_text_sensor_state(text_sensor, text_sensor->state);
}
#endif
InitialStateIterator::InitialStateIterator(StoringController *controller, APIConnection *client)
    : ComponentIterator(controller), client_(client) {

}

APIMessageType SubscribeStatesRequest::message_type() const {
  return APIMessageType::SUBSCRIBE_STATES_REQUEST;
}

} // namespace api

ESPHOMELIB_NAMESPACE_END

#endif //USE_API