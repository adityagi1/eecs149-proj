import os 
from garden.models import SetPoint
import time
while True:
	newest = SetPoint.objects.latest("timestamp")
	action= "0"
	if(newest.pump_1):
		action += "1"
	else:
		action += "0"
	if(newest.lights):
        	action += "1"
	else:
        	action += "0"
	if(newest.pump_2):
		action += "1"
	else:
        	action += "0"

	os.system("mosquitto_pub -h agrasp.wifizone.org -p 80 -t \"testing\" -i yes -m \""+ action+"\"")
	time.sleep(2)
