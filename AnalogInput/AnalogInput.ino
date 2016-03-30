// Press command + shift + L to open Serial Plotter

void setup() {
  // Initialize the serial communication
  Serial.begin(115200);
}

void loop() {
  // Send the value of analog input 0
  Serial.println(analogRead(A0));

  // Wait a bit
  delay(10);
}

