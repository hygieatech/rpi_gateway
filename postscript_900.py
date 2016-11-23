import requests
import random
import json
import sys
import serial
import time
import datetime
import smtplib
from email.MIMEMultipart import MIMEMultipart
from email.MIMEText import MIMEText

fromaddr = "hygieastatus@gmail.com"
toaddr = "ss@hygiea.tech"

#msg['Subject'] = "Status email"

ser = serial.Serial(
	port='/dev/ttyACM0',
	baudrate=115200,
        timeout=15
	)


test = {
	"hub_id": 1,
	"key": "9783653f691bf86982a2d3f19442ef99",
	"secret": "00581d45a610bab1b8d5a5066393005e",
	"sensor_data": [{
		"id": 1,
		"data": random.randint(0,10)
	}, {
			"id": 2,
		"data": random.randint(0,10)
	}, {
			"id": 3,
		"data": random.randint(0,10)
	}, {
			"id": 4,
		"data": random.randint(0,10)
	}]
}

def get_push_data(a, b, c, d):
	return {
	"hub_id": 1,
	"key": "bf327001430f40999e0a493138797d4c",
	"secret": "6a89c9aff18f76c5e29a3d11464f985f",
	"sensor_data": [{
		"id": 1,
		"data": a
	}, {
			"id": 2,
		"data": b
	}, {
			"id": 3,
		"data": c
	}, {
			"id": 4,
		"data": d
	}]
}

dateString = '%Y/%m/%d %H:%M:%S'
def push(data):
	#r = requests.post("{}/push/{}/".format("http://hygiea.pd.io", data['hub_id']), data=json.dumps(data), )
        #r = requests.post("{}/push/{}/".format("http://demo.hygiea.tech", data['hub_id']), data=json.dumps(data), )
	#r.raise_for_status()
	pass

if __name__ == "__main__":
	count = 0
	new_string = False
	new_data= False
	a=b=c=d=0
        #push(get_push_data(a,b,c,5))
        #sys.exit(1)
	b = 1
	c = 2
	d = 20 	
	while 1:
		a=10
		# b=5
		# c=20
		# push(get_push_data(a,b,c,5))
		
		print("printed in a file")
		ser.flushInput()
		response = ser.readline()
		
		#int(response)
		print "read_data", response #,type(response)
		print len(response)
		time.sleep(5)
		if len(response)==12 or len(response)==13:
			new_string=True
		if new_string:
			if int((response[0:1].strip()).rstrip()) == 2:
				a = int((response[9:11].strip()).rstrip())
				print "A" ,a
				#with open('/home/pi/gdrivefs/log_2.txt','a') as f:
				#	print >> f, datetime.datetime.now().strftime(dateString)
				#	f.write((response[0:1].strip()).rstrip())
				#	f.write(" ")
				#	f.write((response[3:7].strip()).rstrip())
				#	f.write(" ")
				#	f.write((response[9:11].strip()).rstrip())
				#	f.write("\n")
				#f.close()
			elif int((response[0:1].strip()).rstrip()) == 3:
				b = int((response[9:11].strip()).rstrip())
				print "B",b
				#with open('/home/pi/gdrivefs/log_3.txt','wa') as f:
				#	print >> f, datetime.datetime.now().strftime(dateString)
				#	f.write((response[0:1].strip()).rstrip())
				#	f.write(" ")
				#	f.write((response[3:7].strip()).rstrip())
				#	f.write(" ")
				#	f.write((response[9:11].strip()).rstrip())
				#	f.write("\n")
				#f.close()
			msg = MIMEMultipart()
			msg['From'] = fromaddr
			msg['To'] = toaddr
			msg['Subject'] = "Status email from " + (response[0:1].strip()).rstrip()
			body = datetime.datetime.now().strftime(dateString) + "  NodeID : " + (response[0:1].strip()).rstrip() + " Battery : " + (response[3:7].strip()).rstrip() + " Level : " + (response[9:11].strip()).rstrip()
			msg.attach(MIMEText(body, 'plain'))
			server = smtplib.SMTP('smtp.gmail.com', 587)
			server.starttls()
			server.login(fromaddr, "hygiea1234")
			text = msg.as_string()
			server.sendmail(fromaddr, toaddr, text)
			server.quit()
			print("email sent")
			# b = b*2 if b*2 < 10 else 1
			c = 2+c if 2+c <10 else 1
			d = d/3 if d/3>1 else 9
			print type(response)
			# import IPython
			# IPython.embed()
			print "len ", a,b
			push(get_push_data(a,b,c,d))
			count =0
			new_string=False
			new_data=False

		# print new_string,count,new_data
	ser.close()
