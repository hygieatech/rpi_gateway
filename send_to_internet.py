import requests
import random
import json
import sys
import serial

ser = serial.Serial(
	port='/dev/ttyACM0',
	baudrate=9600
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

def push(data):
	#r = requests.post("{}/push/{}/".format("http://hygiea.pd.io", data['hub_id']), data=json.dumps(data), )
        r = requests.post("{}/push/{}/".format("http://demo.hygiea.tech", data['hub_id']), data=json.dumps(data), )
	r.raise_for_status()

if __name__ == "__main__":
	count = 0
	new_string = False
	new_data= False
	a=b=c=d=0
        #push(get_push_data(a,b,c,5))
        #sys.exit(1)
	while 1:
		a=102
		b=52
		c=202
		push(get_push_data(a,b,c,5))

		