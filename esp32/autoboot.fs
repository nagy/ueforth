\ Copyright 2021 Bradley D. Nelson
\
\ Licensed under the Apache License, Version 2.0 (the "License");
\ you may not use this file except in compliance with the License.
\ You may obtain a copy of the License at
\
\     http://www.apache.org/licenses/LICENSE-2.0
\
\ Unless required by applicable law or agreed to in writing, software
\ distributed under the License is distributed on an "AS IS" BASIS,
\ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
\ See the License for the specific language governing permissions and
\ limitations under the License.

internals definitions

( Change default block source on arduino )
: arduino-default-use s" /spiffs/blocks.fb" open-blocks ;
' arduino-default-use is default-use

( Setup remember file )
: arduino-remember-filename   s" /spiffs/myforth" ;
' arduino-remember-filename is remember-filename

( Check for autoexec.fs and run if present.
  Failing that, try to revive save image. )
: autoexec
  also wifi
  r| also wdts | evaluate
  r| wifi_mode_sta wifi.mode | evaluate
  r| z" cjdns-q" 0 wifi.begin | evaluate
  r| wifisetpsnone drop | evaluate \ powersave
  r| $6503a8c0 $0103a8c0 $00ffffff 0 wifi.config | evaluate
  \ r| hear-init | evaluate
  \ r| hear-loop | evaluate
;
' autoexec ( leave on the stack for fini.fs )

forth definitions
