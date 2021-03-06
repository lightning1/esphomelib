#include "esphomelib/defines.h"

#ifdef USE_DEEP_SLEEP

#include <Esp.h>
#include "esphomelib/deep_sleep_component.h"
#include "esphomelib/log.h"
#include "esphomelib/helpers.h"
#include "esphomelib/ota_component.h"

ESPHOMELIB_NAMESPACE_BEGIN

static const char *TAG = "deep_sleep";

void DeepSleepComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Deep Sleep...");
  if (this->run_duration_.has_value())
    this->set_timeout(*this->run_duration_, [this](){
      this->begin_sleep_();
    });
}
void DeepSleepComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Setting up Deep Sleep...");
  if (this->sleep_duration_.has_value()) {
    ESP_LOGCONFIG(TAG, "  Sleep Duration: %llu ms", *this->sleep_duration_ / 1000);
  }
  if (this->run_duration_.has_value()) {
    ESP_LOGCONFIG(TAG, "  Run Duration: %u ms", *this->run_duration_);
  }
  if (this->loop_cycles_.has_value()) {
    ESP_LOGCONFIG(TAG, "  Loop Cycles: %u", *this->loop_cycles_);
  }
#ifdef ARDUINO_ARCH_ESP32
  if (this->wakeup_pin_.has_value()) {
    LOG_PIN("  Wakeup Pin: ", *this->wakeup_pin_);
  }
#endif
}
void DeepSleepComponent::loop() {
  if (this->loop_cycles_.has_value()) {
    if (++this->at_loop_cycle_ >= *this->loop_cycles_)
      this->begin_sleep_();
  }

  if (this->next_enter_deep_sleep_)
    this->begin_sleep_();
}
float DeepSleepComponent::get_loop_priority() const {
  return -100.0f; // run after everything else is ready
}
void DeepSleepComponent::set_sleep_duration(uint32_t time_ms) {
  this->sleep_duration_ = uint64_t(time_ms) * 1000;
}
#ifdef ARDUINO_ARCH_ESP32
void DeepSleepComponent::set_wakeup_pin(const GPIOInputPin &pin) {
  this->wakeup_pin_ = pin.copy();
}
void DeepSleepComponent::set_wakeup_pin_mode(WakeupPinMode wakeup_pin_mode) {
  this->wakeup_pin_mode_ = wakeup_pin_mode;
}
void DeepSleepComponent::set_ext1_wakeup(Ext1Wakeup ext1_wakeup) {
  this->ext1_wakeup_ = ext1_wakeup;
}
#endif
void DeepSleepComponent::set_run_cycles(uint32_t cycles) {
  this->loop_cycles_ = cycles;
}
void DeepSleepComponent::set_run_duration(uint32_t time_ms) {
  this->run_duration_ = time_ms;
}
void DeepSleepComponent::begin_sleep_(bool manual) {
  if (this->prevent_ && !manual) {
    this->next_enter_deep_sleep_ = true;
    return;
  }
#ifdef ARDUINO_ARCH_ESP32
  if (this->wakeup_pin_mode_ == WAKEUP_PIN_MODE_KEEP_AWAKE &&
      this->wakeup_pin_.has_value() && !this->sleep_duration_.has_value() && (*this->wakeup_pin_)->digital_read()) {
    // Defer deep sleep until inactive
    if (!this->next_enter_deep_sleep_) {
      this->status_set_warning();
      ESP_LOGW(TAG, "Waiting for pin_ to switch state to enter deep sleep...");
    }
    this->next_enter_deep_sleep_ = true;
    return;
  }
#endif

  ESP_LOGI(TAG, "Beginning Deep Sleep");

  run_safe_shutdown_hooks("deep-sleep");

#ifdef ARDUINO_ARCH_ESP32
  if (this->sleep_duration_.has_value())
    esp_sleep_enable_timer_wakeup(*this->sleep_duration_);
  if (this->wakeup_pin_.has_value()) {
    bool level = !(*this->wakeup_pin_)->is_inverted();
    if (this->wakeup_pin_mode_ == WAKEUP_PIN_MODE_INVERT_WAKEUP && (*this->wakeup_pin_)->digital_read())
      level = !level;
    esp_sleep_enable_ext0_wakeup(gpio_num_t((*this->wakeup_pin_)->get_pin()), level);
  }
  if (this->ext1_wakeup_.has_value()) {
    esp_sleep_enable_ext1_wakeup(this->ext1_wakeup_->mask, this->ext1_wakeup_->wakeup_mode);
  }
  esp_deep_sleep_start();
#endif

#ifdef ARDUINO_ARCH_ESP8266
  ESP.deepSleep(*this->sleep_duration_);
#endif
}
float DeepSleepComponent::get_setup_priority() const {
  return -100.0f;
}
void DeepSleepComponent::prevent_deep_sleep() {
  this->prevent_ = true;
}

ESPHOMELIB_NAMESPACE_END

#endif //USE_DEEP_SLEEP
