import json, sys, csv, datetime, time
import paho.mqtt.client as mqtt

topic = ""
payload = ""
device_id = 0
hostname = "agrasp.wifizone.org"
file_to_write = ""
#command format: -t tanner/private/test/setpoint/s1 -p '{"setpoint":[{"d_label":"Test1","d_value":"1"}]}' -d 1

def setup_client():
	client = mqtt.Client()
	client.on_connect = on_connect
	client.connect("iot.eclipse.org", 1883) #iot.eclipse.org - 1883 / EC2:8883
	return client
	
def on_connect(client, userdata, flags, rc):
	if rc == 0:
		print("Connection successful!\n")
	else:
		print("Connection failed, with code=" + str(rc) + "\n")
		
def update():
	payld_dict = json.loads(payload)
	print(payld_dict)
	file_to_write = "setpoints/" + str(device_id) + "_setpoint.csv"
	header = [pt["d_label"] for pt in payld_dict["setpoint"]]
	setpoints = [pt["d_value"] for pt in payld_dict["setpoint"]]
	try:
		with open(file_to_write, 'w', newline='') as file:
			writer = csv.writer(file)
			writer.writerow(header)
			writer.writerow(setpoints)
			file.close()	
	except IOError:
		print("File not found, creating new file\n")

def main():
	client = setup_client()
	update()
	payload_to_send = payload.encode()
	client.loop_start()
	client.publish(topic,payload_to_send,1)
	time.sleep(5)
	client.loop_stop()

if __name__ == "__main__": 
	if "-t" in sys.argv:	
		topic = str(sys.argv[sys.argv.index("-t") + 1])
	if "-p" in sys.argv:
		payload = str(sys.argv[sys.argv.index("-p") + 1])
	if "-d" in sys.argv:
		device_id =str(sys.argv[sys.argv.index("-d") + 1])
	main()
