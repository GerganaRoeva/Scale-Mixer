#include <HX711.h>
#include <LiquidCrystal.h>

#define pinDATA 2
#define pinSCK 3

#define pinRS 8
#define pinEN 9
#define pinD4 4
#define pinD5 5
#define pinD6 6
#define pinD7 7
#define pinBL 10

#define relay 11

#define button A0



LiquidCrystal lcd(pinRS,  pinEN,  pinD4,  pinD5,  pinD6,  pinD7);
HX711 scale;

float y1 = 437.0; //known weight
long x1 = 0L;
long x0 = 0L;
float avg_size = 10.0; // amount of averages for each mass measurement
float tara = 0;

void setup() {
	Serial.begin(9600); // prepare serial port

  	scale.begin(pinDATA, pinSCK);
  	lcd.begin(16, 2);
	delay(1000);  // allow load cell and hx711 to settle	

	pinMode(relay, OUTPUT);

	// tare procedure
	for (int i = 0; i < (int)avg_size; i++){
		delay(100);
		x0 += scale.read();
	}
 
  	x0 /= long(avg_size);

 	lcd.setCursor(0,0);
	lcd.println("Add Calibrated Mass");
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("      Add       ");
	lcd.setCursor(0,1);
	lcd.print("    the BOLL    ");


	// calibration procedure (mass should be added equal to y1)
	int i = 1;
	while(true)
	  if (scale.read() >= x0 + 10000){
	    i++;
	    delay(2000);
	    for (int j = 0; j < (int)avg_size; j++){
	      	x1 += scale.read();
	    }
	    x1 /= (long)avg_size;
	    break;
	  }

	Serial.println("Calibration Complete");
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("    Ready to    ");
	lcd.setCursor(0,1);
	lcd.print("      cook      ");
}

void loop()
{
	long reading = 0;
	for (int j = 0; j < (int)avg_size; j++)
	{
		reading += scale.read();
	}
	reading /= (long)avg_size;
  
  
  // calculating mass based on calibration and linear fit
	float ratio_1 = (float) (reading-x0);
	float ratio_2 = (float) (x1-x0);
	float ratio = ratio_1/ratio_2;
	float mass = y1*ratio;

	int x;
	x = analogRead(button); // read from buttons

	if(x >= 0 && x < 60){ // button rigth to set 0
    	tara = mass;
    	Serial.print("SET TO 0");
    	Serial.print(".");
    	lcd.setCursor(0,0);
    	lcd.print("    SET TO 0    ");
    	lcd.setCursor(0,1);
    	lcd.print("      .         ");    
    	delay(300);
    	Serial.print(".");
    	lcd.setCursor(0,0);
    	lcd.print("    SET TO 0    ");
    	lcd.setCursor(0,1);
    	lcd.print("      ..        "); 
    	delay(300);
    	Serial.println(".");
    	lcd.setCursor(0,0);
    	lcd.print("    SET TO 0    ");
    	lcd.setCursor(0,1);
    	lcd.print("      ...       "); 
    	delay(300);   
  	}

	if (x >= 60 && x < 200) { // turn on mixer up button
		digitalWrite(relay, HIGH);
	}

	if (x >= 200 && x < 400) { // turn off mixer down button
		digitalWrite(relay, LOW); 
	}	

    Serial.print(mass - tara);
    Serial.println(" g");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    MESURED:    ");
    lcd.setCursor(0,1);
    lcd.print(mass - tara);
    lcd.print(" g");
};
