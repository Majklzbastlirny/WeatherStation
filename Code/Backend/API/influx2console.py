from datetime import datetime
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS

token = "H4hYgXBEUTFRivBBLdo0Pk_ca1qCAmN8O-LRGjRLRoVitVGcSfervwQQOKa8Jka7rwDa2m7EDlqZn_MmNqsezQ=="
org = "MajklovaBastlirna"
bucket = "WeatherStation"
query = 'from(bucket: "WeatherStation") |> range(start: -1h) |> last()|> filter(fn: (r) => r._measurement != "HeaterStatus" and r._measurement != "Voltage")'

client =  InfluxDBClient(url="http://192.168.0.8:8086", token=token, org=org)
      
result = client.query_api().query(query, org=org)

results = []
for table in result:
    for record in table.records:
        results.append((record.get_measurement(), record.get_value()))

listToStr = '-'.join([str(elem) for elem in results])
listToStr = listToStr.replace(",", ":")
listToStr = listToStr.replace("('", "\"")
listToStr = listToStr.replace("\'", "\"")
listToStr = listToStr.replace(")", "")
listToStr = listToStr.replace(": ", ":")
listToStr = listToStr.replace("-\"", ",\"")

print("{" + listToStr + "}") 
client.close()