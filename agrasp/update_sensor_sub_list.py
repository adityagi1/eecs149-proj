#update list of topics
import json, sys, csv, datetime, time

subscription_list_file = "Topics/sub_list.txt"
new_topics = []

def check_file(topic):
	try:
		with open(subscription_list_file, 'r+') as f:
			lines = [line.rstrip('\n') for line in f]
			for line in lines:
				if str(line) == topic:
					print("Topic: " + topic + " was already present\n")
					return False
			f.close()
		return True
	except IOError:
		print("Could not open " + subscription_list_file + "\n")
	return True
	
def main():
	try:
		with open(subscription_list_file, 'a+') as f:
			f.writelines(new_topics)
			f.close()
	except IOError:
		print("Could not open " + subscription_list_file + "\n")

if __name__ == "__main__": 
	for x in range(1,len(sys.argv)):
		if check_file(sys.argv[x]):
			new_topics.append(str(sys.argv[x] + "\n"))
	main()
