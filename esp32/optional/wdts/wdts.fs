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

also sockets
also tasks
also internals \ for cmove
also ESP \ for esp_read_mac_cell

1024 constant max-msg
create msg max-msg allot
create resp 10 max-msg * allot
variable outoffset 0 outoffset !
-1 value sockfd
sockaddr incoming
sockaddr received
variable received-len sizeof(sockaddr_in) received-len !
create readd cell allot 0 readd !

\ evaluation finished
variable evalfin 0 evalfin !
: is-eval-done ( -- n ) evalfin @ 1 = ;
: make-eval-done ( -- ) 1 evalfin ! ;
: make-eval-undone ( -- ) 0 evalfin ! ;

defer sendto-target
: simple-sendto-target ( a n -- )
  \ We send only if we have something to send.
  \ Otherwise, we are sending a zero-sized packet,
  \ which gets interpreted as a closed connection on some receivers
  \ Also we send at most 1024 bytes.
  dup 0 > if
    1024 min >r \ n onto r-stack
    >r \ a onto r-stack
    sockfd r> ( a ) r> ( n ) 0 ( flag ) received sizeof(sockaddr_in) sendto drop
  else 2drop then ;
' simple-sendto-target is sendto-target

: receiver-mac ( -- n ) \ partial mac only
  msg 2 + @ ;

: is-my-mac ( -- n )
  receiver-mac esp_read_mac_cell_inverted = ;

\ types into msgout buffer instead of on the terminal
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
  again ;
' reader 10 10 task reader-task

: sender ( -- )
  begin
    is-eval-done if \ when the evaluation finished, we send
      resp outoffset @ sendto-target
      make-eval-undone
    then
    pause
  again ;
' sender 10 10 task sender-task

: udp ( -- )
  1 incoming ->port!
  AF_INET SOCK_DGRAM 0 socket to sockfd
  sockfd non-block throw
  sockfd incoming sizeof(sockaddr_in) bind throw
  -1 throw ;

: hear-init ( -- )
  reader-task start-task
  sender-task start-task
  begin 1000 ms ['] udp catch -1 = until ;

defer handle-message
\ : hear-simple ( -- )
\     0 outoffset !
\     msg readd @ evaluate
\     make-eval-done ;
\ ' hear-simple is handle-message
: hear-addressed ( -- )
  0 outoffset !
  is-my-mac if
    msg 6 +
    readd @ 6 -
    evaluate
    make-eval-done
  else
    \ look into routing table and forward
    receiver-mac esp_read_mac_cell_inverted <> if

      then
  then ;
' hear-addressed is handle-message

: hear-loop ( -- )
  0 readd !
  begin
    readd @ 6 > if
      ['] handle-message catch drop
      0 readd !
    then
    pause
  again ;

[THEN]
forth definitions
