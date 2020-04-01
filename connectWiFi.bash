#!/bin/bash

# Declare the echo variables to let you know what's going on
WiFI_RELOAD="Restarting WiFi adapter to make sure it's connected"

# Restart WiFi adapter to connect it to the XT1575 network
echo $WiFI_RELOAD
systemctl daemon-reload
systemctl restart wpa_supplicant@wlan0
ifdown wlan0
ifup wlan0
