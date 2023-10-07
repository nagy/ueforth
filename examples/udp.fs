#! /usr/bin/env ueforth

also sockets
also tasks
also internals \ for cmove

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
: make-eval-done ( --  ) 1 evalfin ! ;
: make-eval-undone ( -- ) 0 evalfin ! ;

\ types into msgout buffer instead of on the terminal.
\ increases outoffset by the amount typed.
: resp-type ( a n -- )
  2dup serial-type \ debug \ more than just debug :(
  resp outoffset @ + swap cmove
  dup outoffset @ + outoffset ! \ increase outoffset
;

: udp-reader ( -- )
  begin
    sockfd msg max-msg 0 received received-len recvfrom
    dup 0 >= if
      readd !
    else drop then
    pause
  again ;
' udp-reader 10 10 task udp-reader-task

: udp-sender ( -- )
  begin
    is-eval-done if \ when the evaluation finished, we send
      \ But only if we have something to send.
      \ Otherwise, we are sending a zero-sized packet,
      \ which gets interpreted as a closed connection on some receivers
      outoffset @ 0 > if
        sockfd resp outoffset @ 1024 min 0 received sizeof(sockaddr_in) sendto drop
      then
      make-eval-undone
    then
    pause
  again ;
' udp-sender 10 10 task udp-sender-task

: udp ( -- )
  1 incoming ->port!
  AF_INET SOCK_DGRAM 0 socket to sockfd
  sockfd non-block throw
  sockfd incoming sizeof(sockaddr_in) bind throw ;

: hear-init ( -- )
  begin 1000 ms ['] udp catch -1 = until
  udp-reader-task start-task
  udp-sender-task start-task
  ' resp-type is type ;

: hear-1  ( -- )
  readd @ 0 > if
    0 outoffset ! \ begin new writing cycle
    msg readd @ evaluate
    make-eval-done
  then ;

: hear-loop ( -- )
  begin
    ['] hear-1 catch drop
    0 readd ! \ outside of loop in case the loop errors
    pause
  again ;

hear
