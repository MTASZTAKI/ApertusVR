@echo OFF
@echo Welcome to the UR RealTimeDataExchange Project (Python)
@echo -----
@echo Starting UR RTDE sampler program
@echo You will see how many samples it already parsed.
@echo -----
C:\python27\python.exe record-nr.py --robothost "192.168.0.22" --serverhost "127.0.0.1" --serverport 11994
pause