import json, sys, csv, datetime, time
import paho.mqtt.client as mqtt
from jsonschema import validate

#recieving one sensor value and setpoint(possibly empty) at a time
schema_file = "dout_header.json" #defalut schema
subscription_list_file = "Topics\sub_list.txt"
hostname = "agrasp.wifizone.org" #80, topic: test
file_to_write = "" 
timestamp = "" #datetime.datetime.now()
valid_json = False
sub_list = []
json_in = ""

def setup_client():
	client = mqtt.Client()
	#client.username_pw_set(username="ec2-user",password="agrasp-admin")
	client.on_connect = on_connect
	client.on_message = on_message
	client.connect("agrasp.wifizone.org", 80) #iot.eclipse.org - 1883 / EC2:8883
	return client
	
def check_header(data, file):
	try:
		with open(file, 'r') as read_file:
			reader = csv.reader(read_file)
			data_label = str(data["data"][0]["d_label"])
			header_row = next(reader)
			print(header_row)
			if (len(header_row) == 0 or data_label not in header_row ):
				print("No mathcing data type found")
				return False
			read_file.close()
		return True
	except IOError:
		print("Could not open " + file + "\n")
		return False
	return False

def parse_data(data, file):
	timestamp = datetime.datetime.now()
	write_data = data["data"][0]["d_value"]
	row = [timestamp, write_data]
	#row.insert(0, timestamp)
	try:
		with open(file, 'a', newline='') as wf:
			writer = csv.writer(wf)
			writer.writerow(row)
			wf.close()
		return True
	except IOError:
		print("Could not write to " + file + "\n")
		return False
	return False
	
def check_setpoint(data):
	#compare header of setpoint
	spt_file = "setpoints\\" + str(data["device_id"]) + "_setpoint.csv"
	print(spt_file)
	if len(data["setpoint"]) == 0:
		print("This data type has no setpoint")
		return True
	data_spt_header = data["setpoint"][0]["d_label"]
	data_spt_val = int(data["setpoint"][0]["d_value"])
	try:
		#compare setpoint values
		with open(spt_file, 'r+') as f:
			csv_reader = csv.reader(f)
			header_row = next(csv_reader)
			indx = header_row.index(data_spt_header)
			data_row = next(csv_reader)
			if ( not (int(data_row[indx]) == data_spt_val)):
				print(data_row[indx])
				print(data_spt_val)
				print("Mismatch b/w setpoitns, sending correct setpoint")
				pub_topic = user + "/" + data["device_id"] + "/" + "setpoint/" + data["device_type"]
				payload_to_send = {"device_id":data["device_id"], "device_type":data["device_type"], "setpoint":[{"d_label":header_row[indx],"d_value":data_row[indx]}]}
				client.publish(pub_topic,json.dumps(payload_to_send).encode(),1)
			f.close()
	except IOError:
		print("Could not check " + spt_file + "\n")
	return True
	
def update_sub_list():
	print("Checking for updates in subscription list")
	try:
		with open(subscription_list_file, 'r+') as f:
			for line in f:
				clean_line = line.rstrip()
				if (clean_line, 0) not in sub_list:
					print("Adding " + clean_line + " to sublist") 
					sub_list.append((clean_line,0))
			f.close()
	except IOError:
		print("Could not open " + subscription_list_file + "\n")
		client.loop_stop()

def on_connect(client, userdata, flags, rc):
	if rc == 0:
		print("Connection successful!\n")
		#client.subscribe(sub_list)
	else:
		print("Connection failed, with code=" + str(rc) + "\n")

def on_message(client, userdata, msg):
	print("message recieved")
	topic = msg.topic
	m_decode = str(msg.payload.decode("utf-8", "ignore"))
	json_in = json.loads(m_decode)
	user = topic.split('/')[0]
	try:
		with open(schema_file, 'r+') as s:
			print("begin validating")
			schema = json.load(s)
			validate(json_in, schema)
			valid_json = True
	except IOError:	
		print("Could not open " + schema_file + "/n")
	if(valid_json):
		file_to_write = "sensor_data\\" + str(json_in["device_id"]) + "\\" + str(json_in["data"][0]["d_label"]) + ".csv"
		print(file_to_write)
		if check_header(json_in, file_to_write):
			print("Checking Setpoint")
			check_setpoint(json_in)
			print("Parsing Data")
			parse_data(json_in, file_to_write)
		else:
			print("Type of data provided did not match type in csv file")
	else:
		print("JSON recieved was not of the correct format")
	
def main():
	client = setup_client()
	#while True:
	client.loop_start()
		#update_sub_list()
	client.subscribe("tanner/1111/sensor/test")

		#r=client.subscribe(sub_list)
		#print(r)
	time.sleep(5)
	client.loop_stop()
		
if __name__ == "__main__": main()