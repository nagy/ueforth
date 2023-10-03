#! /usr/bin/env ueforth

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
  2dup \ debug
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
    evalfin @ 1 = if
      sockfd resp outoffset @ 0 received sizeof(sockaddr_in) sendto drop
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
;

: hear-init ( -- )
  reader-task start-task
  sender-task start-task
  begin 1000 ms ['] udp catch -1 = until
;

: hear-1  ( -- )
  readd @ 0 > if
    0 outoffset !
    msg readd @ evaluate
    0 readd !
    1 evalfin !
  then
;

: hear-loop ( -- )
  0 readd !
  begin ['] hear-1 catch drop pause again
;

hear
