void startWIFI(void) {
  delay(100);

  WiFi.begin(ssid, password); // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");
  oldTime = 0;
  int i = 0;
  delay(100);

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(2000);
    Serial.print(++i);
    Serial.print('.');
    delay(100);
  }
  delay(2000);
  Serial.print('\n');
  Serial.print("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer

}
