import time
import paho.mqtt.client as mqtt
from Hologram.HologramCloud import HologramCloud

hologram = HologramCloud(dict(), network='cellular')

Broker = "localhost"
sub_topic = "node/value"

# when connecting to mqtt do this;
def on_connect(node, userdata, flags, rc):
    print "Connected with result code "+str(rc)
    client.subscribe(sub_topic)

# when receiving a mqtt message do this;
def on_message(node, userdata, msg):
    message = str(msg.payload)
    print msg.topic+" "+message

    response = hologram.sendMessage(message, topics=[msg.topic])
    print response

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(Broker, 1883, 60)
client.loop_start()

while True:
    time.sleep(5)
