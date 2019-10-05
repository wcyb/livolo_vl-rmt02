/*
 * (c) 2019 Wojciech Cybowski
 * https://github.com/wcyb/livolo_vl-rmt02
 */
#define TXPIN 52//you change this pin to suit your needs
#define LED 13

const uint8_t btnA[3] = {0xD7, 0x17, 0x0F};//data to send for each of the buttons
const uint8_t btnB[3] = {0xD7, 0x17, 0x07};
const uint8_t btnC[3] = {0xD7, 0x17, 0x0B};
const uint8_t btnD[3] = {0xD7, 0x17, 0x33};

void sendByte(const uint8_t& byteToSend, const bool& start = false, const bool& repStart = false);
void sendCommand(const uint8_t dataArray[]);

void setup() {
  pinMode(TXPIN, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(TXPIN, LOW);
  digitalWrite(LED, LOW);
  Serial.begin(115200);
  Serial.println(F("READY"));
}

void loop() {
  if (Serial.available())
  {
    switch (Serial.read())
    {
      case 'A':
        digitalWrite(LED, HIGH);
        sendCommand(btnA);
        Serial.println(F("A sent"));
        break;

      case 'B':
        digitalWrite(LED, HIGH);
        sendCommand(btnB);
        Serial.println(F("B sent"));
        break;

      case 'C':
        digitalWrite(LED, HIGH);
        sendCommand(btnC);
        Serial.println(F("C sent"));
        break;

      case 'D':
        digitalWrite(LED, HIGH);
        sendCommand(btnD);
        Serial.println(F("D sent"));
        break;
    }
    digitalWrite(LED, LOW);
  }
}

void sendByte(const uint8_t& byteToSend, const bool& start = false, const bool& repStart = false)
{
  bool clk = false;//clock

  if (repStart)//adjust rep start time to get 0,5ms pulse length
  {
    digitalWrite(TXPIN, HIGH);
    delayMicroseconds(190);
  }
  else if (start)//first bit in the first byte of command is longer than normal - it is about 0,5ms pulse
  {
    digitalWrite(TXPIN, HIGH);
    delayMicroseconds(350);
  }

  for (int16_t i = 1; i <= 128; i <<= 1)//apply Manchester encoding to data
  {
    for (uint8_t j = 0; j < 2; j++)
    {
      clk = !clk;
      if (static_cast<bool>(i & byteToSend) ^ clk) digitalWrite(TXPIN, LOW);
      else digitalWrite(TXPIN, HIGH);
      delayMicroseconds(150);//wait to get correct bit rate - about 3300
    }
  }
}

void sendCommand(const uint8_t dataArray[])
{
  for (uint8_t rep = 0; rep < 128; rep++)//repeat data 128 times
  {
    for (uint8_t i = 0; i < 3; i++)
    {
      sendByte(dataArray[i], (i == 0) ? true : false, (i == 0 && rep > 0) ? true : false);//set start to true if the first byte of packet is sent and set repStart to true if it is any next packet
    }
  }
  digitalWrite(TXPIN, LOW);//make sure than TX pin will be LOW after transmitting
}
