import paho.mqtt.client as mqtt
import mysql.connector
from mysql.connector import Error

# Configuración del broker MQTT y la base de datos
BROKER_ADDRESS = "192.168.1.101"  # Cambia a la IP de tu broker MQTT
TOPIC = "hello/world"

# Conectar a la base de datos MySQL
def connect_to_database():
    try:
        connection = mysql.connector.connect(
            host="localhost",          # Cambia a la IP de tu servidor MySQL si no está en localhost
            user="tu_usuario",         # Cambia a tu usuario de MySQL
            password="tu_contraseña",  # Cambia a tu contraseña de MySQL
            database="ProyectoIOT"
        )
        if connection.is_connected():
            print("Conexión a la base de datos exitosa")
        return connection
    except Error as e:
        print("Error al conectar a MySQL", e)
        return None

# Guardar el dato en la base de datos
def save_data(distance):
    connection = connect_to_database()
    if connection:
        cursor = connection.cursor()
        query = "INSERT INTO DatosSensor (distancia_cm) VALUES (%s)"
        cursor.execute(query, (distance,))
        connection.commit()
        cursor.close()
        connection.close()
        print(f"Distancia {distance} cm guardada en la base de datos")

# Función que se ejecuta cuando se conecta al broker MQTT
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Conectado al broker MQTT con éxito")
        client.subscribe(TOPIC)
    else:
        print(f"Error al conectar, código de error: {rc}")

# Función que se ejecuta cuando se recibe un mensaje del broker
def on_message(client, userdata, msg):
    try:
        # Convertir el payload a texto y extraer el valor de distancia
        distancia_str = msg.payload.decode()
        distancia = float(distancia_str.split(": ")[1].split(" ")[0])  # Extraer el número de la cadena
        print(f"Distancia recibida: {distancia} cm")

        # Guardar la distancia en la base de datos
        save_data(distancia)
    except Exception as e:
        print("Error al procesar el mensaje:", e)

# Configuración del cliente MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Conectar al broker MQTT
client.connect(BROKER_ADDRESS, 1883, 60)

# Iniciar el loop de recepción de mensajes
client.loop_forever()
