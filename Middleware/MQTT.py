import random
import time
import collections
from paho.mqtt import client as mqtt_client


broker = 'test.mosquitto.org'
port = 1883
topic_data = "AGGO/SS01/Temperature01"
topic_avg = "AGGO/SS01/Temperature01/Avg"
topic_max = "AGGO/SS01/Temperature01/Max"
topic_min = "AGGO/SS01/Temperature01/Min"
# generate client ID with pub prefix randomly
client_id = f'python-mqtt-{random.randint(0, 1000)}'
username = ''
password = ''

LENGTH_DATA = 3600*24 #24 hours data

data = collections.deque(maxlen=LENGTH_DATA)

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    #client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client

def publish(client, data, topic):
    msg = f"{data}"
    result = client.publish(topic, msg)
    status = result[0]
    if status == 0:
        print(f"Send `{msg}` to topic `{topic}`")
    else:
        print(f"Failed to send message to topic {topic}")

def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        try:
            data.append(float(msg.payload.decode("utf-8")))
        except:
            None
    client.subscribe(topic_data)
    client.on_message = on_message

if __name__ == '__main__':
    client = connect_mqtt()
    client.loop_start()
    time.sleep((5)) # wait 5 sec
    subscribe(client)
    while True:

        try:
            temp_max = round(max(data),2)
            temp_min = round(min(data),2)
            temp_avg = round((0 if len(data) == 0 else sum(data)/len(data)), 2)
            publish(client, temp_max, topic_max)
            publish(client, temp_min, topic_min)
            publish(client, temp_avg, topic_avg)
        except:
            print("Waiting for incoming data.")
            temp_max = 0.0
            temp_min = 0.0
            temp_avg = 0.0
        time.sleep((5))

        