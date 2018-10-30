//sets the PIN on the ESP from which we will be reading analog values (A0)
const int analogIn = A0;
//affine model slope parameter
const float a = -0.41;
//affine model bias parameter
const float b = 1.66;
//ADC code returned by the ADC on the ESP
int ADCcode = 0;
//analog voltage calculated by our code
float volt;
//gforce value back calculated from the voltage
float gforce;
//LED lights up whenever the sensor sense acceleration
bool led = 1;

void setup() {
  Serial.begin(9600);
  pinMode(16, OUTPUT);
}

void loop() {

  digitalWrite(16, led);
  led = !led; 
  ADCcode = analogRead(analogIn);
  //obtaining voltage value from ADC code
  volt = (1.66/476) * ((float) ADCcode);
  //gforce value calculated from voltage
  gforce = (volt - b)/a;
  //print gforce value to serial
  Serial.println(gforce);
  delay(500);
}
