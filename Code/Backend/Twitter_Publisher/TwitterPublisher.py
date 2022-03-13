import tweepy
from datetime import datetime
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS

token = "YOU_INFLUXDB_TOKEN"
org = "YOUR_INFLUXDB_ORG"
bucket = "YOUR_INFLUXDB_BUCKET"
query = 'from(bucket: "WeatherStation") |> range(start: -1h) |> last()|> filter(fn: (r) => r._measurement != "HeaterStatus" and r._measurement != "Voltage")'

client =  InfluxDBClient(url="http://IP_OF_YOUR_SERVER:8086", token=token, org=org)

result = client.query_api().query(query, org=org)

results = []
for table in result:
    for record in table.records:
        results.append((record.get_measurement(), record.get_value()))

        #print(record.get_measurement())
        #print(record.get_value())
        if record.get_measurement() == "Temperature":
            Temperature = record.get_value()
        elif record.get_measurement() == "Humidity":
            Humidity = record.get_value()
        elif record.get_measurement() == "Pressure":
            Pressure = record.get_value()
        elif record.get_measurement() == "Speed":
            Speed = record.get_value()
        elif record.get_measurement() == "WindDir":
            WindDir = record.get_value()
        elif record.get_measurement() == "Precipitation":
            Precipitation = record.get_value()
        elif record.get_measurement() == "WindChill":
            WindChill = record.get_value()
        elif record.get_measurement() == "UV":
            UV = record.get_value()
        elif record.get_measurement() == "Light":
            Light = record.get_value()
        elif record.get_measurement() == "HeatIndex":
            HeatIndex = record.get_value()
        elif record.get_measurement() == "DewPoint":
            DewPoint = record.get_value()
        else:
            record.get_measurement()


client.close()
 
now = datetime.now()
current_time = now.strftime("%H:%M")



if WindDir > 22.5 and WindDir < 67.5 : 
      WindDirS = "ze severovýchodu"
    
elif WindDir > 67.5 and WindDir < 112.5 : 
      WindDirS = "z východu"
    
elif WindDir > 112.5 and WindDir < 157.5 : 
      WindDirS = "z jihovýchodu"
    
elif WindDir > 157.5 and WindDir < 202.5 : 
      WindDirS = "z jihu"
    
elif WindDir > 202.5 and WindDir < 247.5 : 
      WindDirS = "z jihozápadu"
    
elif WindDir > 247.5 and WindDir < 292.5 : 
      WindDirS = "ze západu"
    
elif WindDir > 292.5 and WindDir < 337.5 : 
      WindDirS = "ze severozápadu"
    
else:
      WindDirS = "ze severu"
    
if Speed == 0:
    Wind = "Nyní nefouká"
else:
     Wind = "Fouká " + str(WindDirS) + " rychlostí " + str(Speed) + " m/s"


def main():
    twitter_auth_keys = {
        "consumer_key"        : "YOUR_CONSUMER_KEY",
        "consumer_secret"     : "YOUR_CONSUMER_SECRET",
        "access_token"        : "YOUR_ACCESS_TOKEN",
        "access_token_secret" : "YOUR_ACCESS_TOKEN_SECRET"
    }
 
    auth = tweepy.OAuthHandler(
            twitter_auth_keys['consumer_key'],
            twitter_auth_keys['consumer_secret']
            )
    auth.set_access_token(
            twitter_auth_keys['access_token'],
            twitter_auth_keys['access_token_secret']
            )
    api = tweepy.API(auth)
 
    tweet = "Nyní je " + str(current_time) + "\n\n" + "Teplota: " + str(Temperature) + " °C\n"  + "Pocitová teplota: " + str(HeatIndex) + " °C\n" + "Rosný bod: " + str(DewPoint) + " °C\n\n" + "Relativní vlhkost: " + str(Humidity) + " %\n" + "Relativní tlak: " + str(Pressure) + " hPa\n\n" + str(Wind) + "\n\n" + "Intenzita osvětlení: " + str(Light) + " lux\n" + "UV intenzita: " + str(UV) + " mW/cm²\n\n" + "Srážky za hodinu: " + str(Precipitation) + " mm\n"
    status = api.update_status(status=tweet)
 
if __name__ == "__main__":
    main()
