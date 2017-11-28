import time
import paho.mqtt.client as mqtt

Broker = "localhost"

sub_topic = "node/value"

# when connecting to mqtt do this;
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe(sub_topic)

# when receiving a mqtt message do this;
def on_message(client, userdata, msg):
    message = str(msg.payload)
    print(client)
    print(msg.topic+" "+message)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(Broker, 1883, 60)
client.loop_start()

while True:
    time.sleep(3)