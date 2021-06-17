#include <Arduino.h>
#define DHT11PIN 2 //D4

int microTime[42];
int dt[40];
int cnt;
int flag;

void timeoutHandler()
{
  cnt++;
  if(cnt >=2500)
  {
    cnt = 0;
  }
}

ICACHE_RAM_ATTR void pulseHandler()
{
  cnt++;
  microTime[cnt] = micros();
}

int readDHTInterrupt(int *readTemp, int* readHumid)
{
  detachInterrupt(digitalPinToInterrupt(DHT11PIN));
  microTime[42] = {0,};
  cnt = 0;
  digitalWrite(DHT11PIN, 1);
  pinMode(DHT11PIN, OUTPUT);
  delay(1);
  digitalWrite(DHT11PIN, 0);
  delay(20);
  pinMode(DHT11PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(DHT11PIN), pulseHandler, FALLING);

  for(int i=2; i<42; i++)
    dt[i] = microTime[i]-microTime[i-1];

  *readHumid = 0;
  *readTemp = 0;
  for (int j = 2; j < 11; j++) //Humid
  {
    *readHumid = *readHumid << 1;
    if (dt[j] > 100)
      *readHumid = *readHumid + 1;
    else
      *readHumid = *readHumid + 0;
  }
  for (int k = 18; k < 26; k++) //Temp
  {
    *readTemp = *readTemp << 1;
    if (dt[k] > 100)
      *readTemp = *readTemp + 1;
    else
      *readTemp = *readTemp + 0;
  }
  return 1;
}

int readDHT11Polling(int *readTemp, int* readHumid)
{
  dt[82] = {0,};
  //Phase 1
  digitalWrite(DHT11PIN, 1);
  pinMode(DHT11PIN, OUTPUT);
  delay(1);
  digitalWrite(DHT11PIN, 0);
  delay(20);
  pinMode(DHT11PIN, INPUT_PULLUP);
  
  //Phase 2, 3
  int cnt = 0;
  for (cnt = 0; cnt < 41; cnt++)
  {
    dt[cnt * 2] = micros();
    while (1)
      if (digitalRead(DHT11PIN) == 1) break;
    dt[cnt * 2] = micros() - dt[cnt * 2];

    dt[cnt * 2 + 1] = micros();
    while (1)
      if (digitalRead(DHT11PIN) == 0) break;
    dt[cnt * 2 + 1] = micros() - dt[cnt * 2 + 1];
  }

  //Phase 4
  *readHumid = 0;
  *readTemp = 0;
  for (cnt = 2; cnt < 11; cnt++) //Humid
  {
    *readHumid = *readHumid << 1;
    if (dt[cnt * 2 + 1] > 49)
      *readHumid = *readHumid + 1;
    else
      *readHumid = *readHumid + 0;

  }
  for (cnt = 18; cnt < 26; cnt++) //Temp
  {
    *readTemp = *readTemp << 1;
    if (dt[cnt * 2 + 1] > 49)
      *readTemp = *readTemp + 1;
    else
      *readTemp = *readTemp + 0;
  }
  //for(cnt=0;cnt<41;cnt++)
  //  Serial.printf("cnt:%d, dt[%d, %d]\r\n",cnt,dt[cnt*2],dt[cnt*2+1]);
  return 1;
}

void setup()
{
  Serial.begin(115200);
  delay(100);
}

void loop()
{
  cnt = 0;
  dt[cnt] = micros();
  int readTemp, readHumid;
  readDHTInterrupt(&readTemp, &readHumid);
  
  Serial.printf("Temperature : %d, Humidity : %d\r\n", readTemp, readHumid);
  
  delay(5000);
}
