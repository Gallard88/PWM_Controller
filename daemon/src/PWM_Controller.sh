#!/bin/bash
# Provides:          PWM_Controller
# Required-Start:    $syslog
# Required-Stop:     $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Example TFTP_Server
# Description:       Start/Stops TFTP Server

start() {
  # Start TFTP Server
  /usr/sbin/PWM_Controller
}

stop() {
  # Stop TFTP_Server
  killall PWM_Controller
}

case "$1" in 
    start)
        start
        ;;
    stop)
        stop
        ;;
    retart)
        stop
        start
        ;;
    *)
echo "Usage: $0 {start|stop|restart}"
esac
exit 0
