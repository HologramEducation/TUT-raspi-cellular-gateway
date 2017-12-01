import time
# python library for publishing and subscribing to MQTT data
import paho.mqtt.client as mqtt

# This script is running on the same machine the MQTT Broker (server)
# is running on. Localhost will work fine, otherwise use the Broker IP.
Broker = "localhost" 

# Topics are how MQTT organize data. 
# Clients publish data to a specific topic(s)
# Clients also can subscribe to a topic(s) and wild cards
# Topics and subtopics are organized in a folders-ish structure 
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

# Istantiate a new Client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Connect to the broker, by default broker runs on port 1883 
client.connect(Broker, 1883, 60)

# Paho's loop that keeps data streaming
client.loop_start()

# We create our own loop to keep this script running 
while True:
    time.sleep(5)