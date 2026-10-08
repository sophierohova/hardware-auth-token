void setup(){
  Serial.begin(115200); 
}

void loop() {
  if (Serial.available() > 0){
    byte challengeByte = Serial.read();
    byte responseByte = ~challengeByte;
    Serial.write(responseByte);
  }
}
