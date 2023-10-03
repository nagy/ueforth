\ Copyright 2023 Daniel Nagy
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

vocabulary wdts   wdts definitions
transfer wdts-builtins
DEFINED? wdts_init [IF]

also sockets also tasks

1024 constant max-msg
create msg max-msg allot
create resp 10 max-msg * allot
variable outoffset 0 outoffset !
variable evalfin 0 evalfin !
-1 value sockfd
sockaddr incoming
sockaddr received
variable received-len sizeof(sockaddr_in) received-len !
create readd cell allot 0 readd !

\ types into msgout buffer instead of on the terminal
internals \ for cmove
: resp-type ( a n -- )
  2dup \ debug \ more than just debug :(
  resp outoffset @ + swap cmove
  dup outoffset @ + outoffset !
  serial-type \ debug
;
' resp-type is type

: reader ( -- )
  begin
    sockfd msg max-msg 0 received received-len recvfrom
    dup 0 >= if
      readd !
    else drop then
    pause
  again
;
' reader 10 10 task reader-task

: sender ( -- )
  begin
    evalfin @ 1 = if \ when the evaluation finished, we send
      \ But only if we have something to send.
      \ Otherwise, we are sending a zero-sized packet,
      \ which gets interpreted as a closed connection on some receivers
      outoffset @ 0 > if
        sockfd resp outoffset @ 1024 min 0 received sizeof(sockaddr_in) sendto drop
      then
      0 evalfin !
    then
    pause
  again
;
' sender 10 10 task sender-task

: udp ( -- )
  1 incoming ->port!
  AF_INET SOCK_DGRAM 0 socket to sockfd
  sockfd non-block throw
  sockfd incoming sizeof(sockaddr_in) bind throw
  -1 throw
;

: hear-init ( -- )
  reader-task start-task
  sender-task start-task
  begin 1000 ms ['] udp catch -1 = until
;

: hear-1 ( -- )
  readd @ 0 > if
    0 outoffset !
    msg readd @ evaluate
    1 evalfin !
  then
;

: hear-loop ( -- )
  0 readd !
  begin
    ['] hear-1 catch drop
    0 readd !
    pause
  again
;

[THEN]
forth definitions
