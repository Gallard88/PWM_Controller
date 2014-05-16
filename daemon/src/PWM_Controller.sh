#!/bin/bash
# Provides:          PWM_Controller
# Required-Start:    $syslog
# Required-Stop:     $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Example PWM Controller /dev/ttyS1
# Description:       Start/Stops PWM Controller daemon

start() {
  # Start PWM Controller daemon
  /usr/sbin/PWM_Controller /dev/ttyUSB0
}

stop() {
  # Stop PWM Controller daemon
  killall PWM_Controller
  killall PWM_Log
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
