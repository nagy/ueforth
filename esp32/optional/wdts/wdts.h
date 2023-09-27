// Copyright 2023 Daniel Nagy
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 * ESP32forth WDTS v{{VERSION}}
 * Revision: {{REVISION}}
 */

// More documentation
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/wdts.html

#include <esp_task_wdt.h>

#define OPTIONAL_WDTS_VOCABULARY V(wdts)
#define OPTIONAL_WDTS_SUPPORT \
  XV(internals, "wdts-source", WDTS_SOURCE, \
      PUSH wdts_source; PUSH sizeof(wdts_source) - 1) \
  YV(wdts, wdts_init    , {struct esp_task_wdt_config_t wd = {0};wd.timeout_ms = n0;n0 = esp_task_wdt_init(&wd)}) \


{{wdts}}
