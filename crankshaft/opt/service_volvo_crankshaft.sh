#!/bin/bash

SERIAL_DEVICE=/dev/ttyACM0
ANDROID_CONNECTED="+"
ANDROID_DISCONNECTED="-"

while true; do
    if [ -c $SERIAL_DEVICE ]; then
        if [ -f /tmp/android_device ]; then
            echo $ANDROID_CONNECTED > $SERIAL_DEVICE
        else
            echo $ANDROID_DISCONNECTED > $SERIAL_DEVICE
        fi
        
        sleep 0.1

        read -t 1 RESPONSE < $SERIAL_DEVICE

        if [ "${RESPONSE/$'\r'}" == "CMD SHUTDOWN" ]; then
            sudo halt
        fi
    fi

    sleep 1
done

exit 0
