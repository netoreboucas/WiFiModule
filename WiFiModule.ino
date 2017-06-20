void setup()
{
  Serial1.begin(115200);
  Serial.begin(115200);
}

void loop()
{
  if (Serial1.available() > 0) {
    Serial.write(Serial1.read());
  }
  
  if (Serial.available() > 0) {
    Serial1.write(Serial.read());
  }
}
