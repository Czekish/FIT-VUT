# -*- coding: utf-8 -*-
#!/usr/bin/env python3

# Importovani knihoven pro praci se sockety, zpracovani json a command-line parametru
import socket
import json
import sys

# Kontrola poctu parametru
if len(sys.argv) != 3:
    sys.stderr.write('Byl zadan neplatny pocet parametru!\n')
    exit(1)

# Ziskani parametru
API_KEY = str(sys.argv[1])
CITY = str(sys.argv[2])

# Vytvoreni socketu
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)                 

# Konstanty host a port, kam se budeme pripojovat
HOST = "api.openweathermap.org"
PORT = 80

# Obsah pozadavku, ktery se odesle na server
request = "GET http://api.openweathermap.org/data/2.5/weather?q=%s&APPID=%s HTTP/1.1\r\n" % (CITY, API_KEY)
request = request + "Host: %s\r\n\r\n" % (HOST)

try: 
    s.connect((HOST, PORT))
    s.sendall(request.encode()) # Odesle data do socketu
    data = s.recv(4096).decode("utf-8") # recv vraci byte object, dekodujeme
except:
    sys.stderr.write('Problem with the connectivity occured!\n')
    exit(1)
finally:
    s.close()

# Oriznuti http hlavicky a prevod na json objekt
json_data = json.loads(data.split("\n")[11:][0])

code = int(json_data['cod'])

if code == 200:
    # OK
    name = json_data['name']
    description = json_data['weather'][0]['description']
    temp_celsius = str(round(json_data['main']['temp']-273.15,1))
    humidity = str(round(float(json_data['main']['humidity']),1))
    pressure = str(round(float(json_data['main']['pressure']),1))
    wind_speed = str(round(float(json_data['wind']['speed'])*3.6,1))
    try:
        wind_deg = str(round(float(json_data['wind']['deg']),1))
    except KeyError:
        wind_deg = '---';

    output = name + '\n' + description + '\n' + 'temp: ' + temp_celsius + u'\u2103' + '\n' + 'humidity: ' + humidity + ' %' + '\n' + 'pressure: ' + pressure + ' hPa' + '\n' + 'wind-speed: ' + wind_speed + 'km/h' + '\n' + 'wind-deg: ' + wind_deg

    print(output)

elif code == 404:
    # Bylo zadano neplatne mesto
    sys.stderr.write('City not found!\n')
    exit(1)

elif code == 401:
    # Byl zadan neplatny API klic
    sys.stderr.write('Invalid API key!\n')
    exit(1)

else:
    # Jina, neocekavana chyba
    sys.stderr.write('Unexpected error!\n')
    exit(1)
    
