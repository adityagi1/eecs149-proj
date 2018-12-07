import json, sys, csv, datetime, time
import paho.mqtt.client as mqtt

topic = ""
payload = ""
hostname = "agrasp.wifizone.org"

def setup_client():
	client_setup = mqtt.Client()
	client.on_connect = on_connect
	client.connect("iot.eclipse.org", 1883) #iot.eclipse.org - 1883 / EC2:8883
	return client_setup
	
def on_connect(client, userdata, flags, rc):
	if rc == 0:
		print("Connection successful!\n")
	else:
		print("Connection failed, with code=" + str(rc) + "\n")

def main():
	client = setup_client()
	payload = payload.getBytes()
	client.loop_start()
	client.publish(topic,payload,1)
	time.sleep(5)
	client.loop_stope()

if __name__ == "__main__": 
	if "-t" in sys.argv:	
		topic = str(sys.argv[sys.argv.index("-t") + 1])
	if "-p" in sys.argv:
		payload = str(sys.argv[sys.argv.index("-p") + 1])
	main()