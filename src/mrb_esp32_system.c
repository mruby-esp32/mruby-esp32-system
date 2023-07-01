#include <mruby.h>
#include <mruby/value.h>

#include <stdio.h>

#include "esp_idf_version.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "esp_chip_info.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static mrb_value
mrb_esp32_system_delay(mrb_state *mrb, mrb_value self) {
  mrb_int delay;
  mrb_get_args(mrb, "i", &delay);

  vTaskDelay(delay / portTICK_PERIOD_MS);

  return self;
}

static mrb_value
mrb_esp32_system_available_memory(mrb_state *mrb, mrb_value self) {
  return mrb_fixnum_value(esp_get_free_heap_size());
}

static mrb_value
mrb_esp32_system_sdk_version(mrb_state *mrb, mrb_value self) {
  return mrb_str_new_cstr(mrb, esp_get_idf_version());
}

static mrb_value
mrb_esp32_system_restart(mrb_state *mrb, mrb_value self) {
  esp_restart();
  return self;
}

static mrb_value
mrb_esp32_system_deep_sleep_for(mrb_state *mrb, mrb_value self) {
  mrb_int sleep_time;
  mrb_get_args(mrb, "i", &sleep_time);

  esp_deep_sleep(sleep_time);

  return self;
}

static mrb_value
mrb_esp32_esp_timer_get_time(mrb_state *mrb, mrb_value self) {
  return mrb_float_value(mrb, esp_timer_get_time());
}

static mrb_value
mrb_esp32_get_chip_model(mrb_state *mrb, mrb_value self) {
  esp_chip_info_t info;
  esp_chip_info(&info);
  return mrb_fixnum_value(info.model);
}

void
mrb_mruby_esp32_system_gem_init(mrb_state* mrb) {
  // ESP32 Ruby module
  struct RClass *esp32_module = mrb_define_module(mrb, "ESP32");

  // ESP32::System
  struct RClass *esp32_system_module = mrb_define_module_under(mrb, esp32_module, "System");
  mrb_define_module_function(mrb, esp32_system_module, "delay", mrb_esp32_system_delay, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, esp32_system_module, "available_memory", mrb_esp32_system_available_memory, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, esp32_system_module, "sdk_version", mrb_esp32_system_sdk_version, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, esp32_system_module, "restart", mrb_esp32_system_restart, MRB_ARGS_NONE());
  mrb_define_module_function(mrb, esp32_system_module, "deep_sleep_for", mrb_esp32_system_deep_sleep_for, MRB_ARGS_REQ(1));
  mrb_define_module_function(mrb, esp32_system_module, "chip_model", mrb_esp32_get_chip_model, MRB_ARGS_NONE());

  // ESP32::Timer
  struct RClass *esp32_timer_module = mrb_define_module_under(mrb, esp32_module, "Timer");
  mrb_define_module_function(mrb, esp32_timer_module, "get_time", mrb_esp32_esp_timer_get_time, MRB_ARGS_NONE());
  
  // ESP32::Constants
  struct RClass *constants = mrb_define_module_under(mrb, esp32_module, "Constants");
  
  // Pass a C constant through to mruby, defined inside ESP32::Constants.
  #define define_const(SYM) \
  do { \
    mrb_define_const(mrb, constants, #SYM, mrb_fixnum_value(SYM)); \
  } while (0)

  //
  // ESP32::System.chip_model returns a constant from the esp_chip_model_t enum:
  // https://github.com/espressif/esp-idf/blob/master/components/esp_hw_support/include/esp_chip_info.h
  //
  // Define constants from the enum in mruby:
  define_const(CHIP_ESP32);
  define_const(CHIP_ESP32S2);
  define_const(CHIP_ESP32S3);
  define_const(CHIP_ESP32C3);
  define_const(CHIP_ESP32H2);
  define_const(CHIP_ESP32C2);
  #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
    define_const(CHIP_ESP32C6);
    define_const(CHIP_POSIX_LINUX);
  #endif
  // define_const(CHIP_ESP32P4);
}

void
mrb_mruby_esp32_system_gem_final(mrb_state* mrb) {
}
