#!/bin/bash
echo "Real Time Data Exchange UR"
cd /home/riki/python-scripts/UR-RTDE
/usr/bin/python record.py --robothost "192.168.0.21" --serverhost "127.0.0.1" --serverport 11994
