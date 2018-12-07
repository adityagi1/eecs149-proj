import json, sys, csv, datetime, time
import paho.mqtt.client as mqtt
from jsonschema import validate

schema_file = "dout_header.json" #defalut schema
subscription_list_file = "Topics\sub_list.txt"
hostname = "agrasp.wifizone.org"
file_to_write = "" 
timestamp = "" #datetime.datetime.now()
valid_json = False
sub_list = []
json_in = ""

def setup_client():
	client = mqtt.Client("C1")
	#client.username_pw_set(username="ec2-user",password="agrasp-admin")
	client.on_connect = on_connect
	client.connect("iot.eclipse.org", 1883) #iot.eclipse.org - 1883 / EC2:8883
	client.on_message = on_message
	return client
	
def check_header(data, file):
	try:
		with open(file, 'r+') as read_file:
			reader = csv.reader(read_file)
			if (len(reader[0]) == 0 or len(reader[0]) != len(data["data"])):
				print("Header is not up-to-date\n")
				read_file.close()
				update_header(data, read_file)
		read_file.close()
		return True
	except IOError:
		print("Could not open " + file + "\n")
		return False
	return False

def updateheader(data, file):
	print("Updating header")
	header_row = [pt["d_label"] for pt in data["data"]]
	header_row.insert(0, "Timestamp")
	writer = csv.writer(file)
	writer.writerow(header_row)
	return True

def parse_data(data, file):
	row = [pt["d_value"] for pt in data["data"]]
	row.insert(0, timestamp)
	try:
		with open(file, 'w') as wf:
			writer = csv.writer(file)
			writer.writerow(row)
		wf.close()
		return True
	except IOError:
		print("Could not write to " + file + "\n")
		return False
	return False
	
def check_setpoint():
	return True
	
def update_sub_list():
	print("Checking for updates in subscription list")
	try:
		with open(subscription_list_file, 'r') as f:
			for line in f:
				sub_list.append(line)
		f.close()
	except IOError:
		print("Could not open " + subscription_list_file + "\n")
		client.loop_stop()
	for topic in sub_list:
		client.subscribe(topic)

def on_connect(client, userdata, flags, rc):
	if rc == 0:
		print("Connection successful!\n")
	else:
		print("Connection failed, with code=" + str(rc) + "\n")

def on_message(client, userdata, msg):
	topic = msg.topic
	m_decode = str(msg.payload.decode("utf-8", "ignore"))
	json_in = json.loads(m_decode)
	try:
		with open(schema, 'r') as s:
			valid_json = validate(json_in, s)
	except IOError:
		print("Could not open " + schema_file + "/n")
	if(valid_json): 
		file_to_write = str("Sensor Data/" + data_out[device_id]) + ".csv"
		check_header(json_in, file_to_write)
		parse_data(json_in, file_to_write)
	else:
		print("JSON recieved was not of the correct format")
	
def main():
	client = setup_client()
	while True:
		client.loop_start()
		update_sub_list()
		time.sleep(5)
		client.loop_stop()
		
if __name__ == "__main__": main()