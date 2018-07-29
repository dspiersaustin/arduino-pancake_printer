/*
Pancake Printer Program

  Input Devices
    1)  Wii nunchuck
        a)  nunchuk_joyangle() - to control direction of motors and speed
        b)  nunchuk_zbutton()  - to control when the servo opens pancake batter control
        c)  nunchuk_cbutton()  - to control when the DC motors are enabled 
            0 = off
            1 = on
          
        d) nunchuk_joyrad()   - to detect and control when wii is back to center
            values range from 0 - 100   (Center value is 3-5)
        --------------------------------
        To be implemented in the future:
        -------------------------------- 
    2)  Add a limiter switch to detect when y-axis is at full forward
  
  Output Devices
    1)  x-axis DC motor - controlled with L293D H-bridge
    2)  y-axis DC motor - controlled with L293D H-bridge        
        --------------------------------
        To be implemented in the future:
        --------------------------------
    3)  servo

    Harware 
      L293D H-bridge to control speed and direction of 

    Library:
    http://www.timteatro.net/2012/02/10/a-library-for-using-the-wii-nunchuk-in-arduino-sketches/  

Version   Author     Comments
v11       DS/NS      Add documentation to current working version
v12       ""         Add variables, code, function to run motor 2
*/

// A Pin            L293D H Bridge                 Arduino Pin
// ~ 6    Enable 1  -- 1--U--16 -- Vss (3.3v)      + strip to A+ 3.3v    
// ~ 5    Input1    -- 2     15 -- Input4          ~pin 10
// M      Output1   -- 3     14 -- Output4         to DC motor (M) top
// -      GND       -- 4     13 -- GND             - strip
// -      GND       -- 5     12 -- GND             - strip
// M      Output2   -- 6     11 -- Output3         to DC motor top
// ~ 3    Input2    -- 7     10 -- Input3          ~pin 9
// +      Vs (12v)  -- 8-----09 -- Enable 2        ~pin 11
 
//Set Pins for Y-axis motor at top of printer
int enablePin2 = 11; //connected L293D Lp9 EN2
int in3Pin = 10;    //connected L293D Lp15 IN4
int in4Pin = 9;     //connected L293D Lp10 IN3

//Set Pins for X-axis motor at bottom of printer
int enablePin1 = 6; //connected L293D Lp1 EN1
int in1Pin = 5;    //connected L293D Lp2  IN1
int in2Pin = 3;     //connected L293D Lp7 IN2

//define global vars
  //motor variables
  boolean y_direction_positive = true;
  boolean x_direction_positive = true;
  int speed_y = 0;
  int speed_x = 0;

  //add variables set speed increments
  // Upper Motor
  int speed_y_stop = 0;
  int speed_y_slow = 150;
  int speed_y_med_slow = 160;
  int speed_y_med = 170;
  int speed_y_fast = 205;

  // Lower Motor
  int speed_x_stop = 0;
  int speed_x_slow = 150;
  int speed_x_med_slow = 160;
  int speed_x_med = 170;
  int speed_x_fast = 205;  

  //add to adjust speed of motors
  int speed_test = 0;
  int potPin = 0;
  
  //wii variables
  int angle = 0;
  int zbutton = 0; // set z button to off
  int cbutton = 0; // set c button to off0.0
  float joyrad = 0.0;
  int joyrad_int = 0;  //convert joyrad from float to int 

//wii
#include <Wire.h>
#include <wiinunchuck.h>   //library wiinunchuck
 
void setup()
{
//wii
  // Initialize the nunchuk
  nunchuk_setpowerpins();
  nunchuk_init();
  nunchuk_send_request();
  
  // Open the serial port at 9600 baud
  Serial.begin(9600);
  Serial.println("Ready");
//wii

  //Set pin mode y-axis  
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enablePin2, OUTPUT);

  //Set pin mode x-axis  
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enablePin1, OUTPUT);
}
 
void loop()
{
  //test speed of motors set from potPin
  speed_test = analogRead(potPin) / 4;
  
 //wii - start
if (nunchuk_get_data() > 0) {
    // at this point, nunchuk_buf contains data
    angle = nunchuk_joyangle() - 90;

    //get additional values from data buffer
/*  unsigned int nunchuk_cbutton()
    unsigned int nunchuk_zbutton() 
    int nunchuk_joy_x()     
    int nunchuk_joy_y()
    //Calibrated X and Y    
    int nunchuk_cjoy_x()
    int nunchuk_cjoy_y()
    //Angles
    int nunchuk_joyangle() - it keeps track of which quadrant we are in by the signs of the opposite and adjacent lengths, and also handles vectors one the axes gracefully.
    int nunchuk_rollangle() //the angle created by tipping an upright nunchuk to the left or right as viewed from behind. More precisely, with the z-axis pointing up from the joystick, the y-axis point out of the c-button and the x-axis pointing from the right side, roll is the angle created between the z-axis and the nunchuk when it is rotated about the y-axis. 
*/
    
    if (angle < 0) {
      angle += 360;
    }
   
//Set variables based on wii data
    zbutton = nunchuk_zbutton();
    cbutton = nunchuk_cbutton();
    joyrad = nunchuk_joyrad();
    joyrad_int = int(joyrad);   //distance away from center of wii on joystick

    //Print speed_test only - comment out after test
    Serial.print("Test Speed=");
    Serial.println(speed_test);  //analogRead(potPin) / 4

/*    //Print all values
    Serial.print("zfn=");
    Serial.print(nunchuk_zbutton());
    Serial.print(" zvar=");
    Serial.print(zbutton);    
    Serial.print(" cfn=");
    Serial.print(nunchuk_cbutton());
    Serial.print(" cvar=");
    Serial.print(cbutton);    
    Serial.print(" angle=");
    Serial.print(angle);    
    Serial.print(" joyrad=");
    Serial.print(nunchuk_joyrad());
    Serial.print(" joyradvar=");
    Serial.print(joyrad);
    Serial.print(" joyrad_int var=");
    Serial.print(joyrad_int);
    Serial.print(" rollangle= ");
    Serial.println(nunchuk_rollangle());
*/
    
    //delay(500);
    delay(50);
}  
 //wii - end


//   Y-MOTOR SPEED and DIRECTION   (Moves the upper motor front to back)

/*  Temp comment during speed test 
//set direction of upper motor (y-axis) based on angle
  if (angle >= 0 and angle <= 90) {
    y_direction_positive = false;
  }
  else if (angle <=360 and angle >=270) {
    y_direction_positive = false;
  }
  else {
    y_direction_positive = true;
  }
*/

//Comment block below after speed test - start
//Use C button to reverse direction during speed test
  if (cbutton == 0) {
    y_direction_positive = false;
  }
  else {
    y_direction_positive = true;
  }
//Comment block below after speed test - end
  
  //150   ultra slow
  //160   slow
  //170   med slow
  //205   medfast
  //254   too fast

//--------------------  
//       0 360          205   medfast
//     23     337          
//---------------------  
//   45         315     170   med slow
//--------------------- 
// 68            292    160   slow
//--------------------- 
// 87             273
//90               270  0     stopped
// 93             267
//---------------------- 150   ultra slow 
// 113           247    
//---------------------  160   slow            
//  135         225     
//---------------------- 170   med slow  
//   158     202              
//        180            205   medfast 
//---------------------- 
  
  //set the speed of the upper motor (y-axis) based on angle
 if (joyrad_int >= 0 and joyrad_int <= 6) {
    speed_y = speed_y_stop;   }   
  else if (angle >= 0 and angle <= 23) {
    speed_y = speed_y_fast;   }  
  else if (angle > 23  and angle <= 45) {
    speed_y = speed_y_med;   }
  else if (angle > 45  and angle <= 68) {
    speed_y = speed_y_med_slow;   }
  else if (angle > 68  and angle < 87) {
    speed_y = speed_y_slow;   }
  else if (angle >= 87  and angle <= 93) {
    speed_y = speed_y_stop;   }
  else if (angle > 93  and angle <= 113) {
    speed_y = speed_y_slow;   }
  else if (angle > 113  and angle <= 135) {
    speed_y = speed_y_med_slow;   }
  else if (angle > 135  and angle <= 158) {
    speed_y = speed_y_med;   }
  else if (angle > 158  and angle <= 202) {
    speed_y = speed_y_fast;   }
  else if (angle > 202  and angle <= 225) {
    speed_y = speed_y_med;   }
  else if (angle > 225  and angle <= 247) {
    speed_y = speed_y_med_slow;   } 
  else if (angle > 247  and angle < 267) {
    speed_y = speed_y_slow;   }
  else if (angle >= 267  and angle <= 273) {
    speed_y = speed_y_stop;   }
  else if (angle > 273  and angle <= 292) {
    speed_y = speed_y_med_slow;   }
  else if (angle > 292  and angle <= 315) {
    speed_y = speed_y_med;     }    
  else if (angle > 315  and angle <= 360) {
    speed_y = speed_y_fast;  }
  else {
    speed_y = speed_y_stop;
  }
  
//   X-MOTOR SPEED and DIRECTION   (Moves the frame left-right)
  
/*  Temp comment during speed test 
  //set direction of lower motor (x-axis) based on angle
    if (angle >= 180 and angle <= 360) {
    x_direction_positive = true;
  }
    else {
    x_direction_positive = false;
  }
*/

//Comment block below after speed test - start
//Use C button to reverse direction during speed test
  if (cbutton == 0) {
    x_direction_positive = false;
  }
  else {
    x_direction_positive = true;
  }
//Comment block below after speed test - end
  
  //set the speed of the lower motor (x-axis) based on angle
  if (joyrad_int >= 0 and joyrad_int <= 6) {
    speed_x = speed_x_stop;   }
  else if (angle >= 0 and angle <= 23) {
    speed_x = speed_x_stop;   }  
  else if (angle > 23  and angle <= 45) {
    speed_x = speed_x_slow;   }
  else if (angle > 45  and angle <= 68) {
    speed_x = speed_x_med_slow;   }
  else if (angle > 68  and angle < 87) {
    speed_x = speed_x_med;   }
  else if (angle >= 87  and angle <= 93) {
    speed_x = speed_x_fast;   }
  else if (angle > 93  and angle <= 113) {
    speed_x = speed_x_med;   }
  else if (angle > 113  and angle <= 135) {
    speed_x = speed_x_med_slow;   }
  else if (angle > 135  and angle <= 158) {
    speed_x = speed_x_slow;   }
  else if (angle > 158  and angle <= 202) {
    speed_x = speed_x_stop;   }
  else if (angle > 202  and angle <= 225) {
    speed_x = speed_x_slow;   }
  else if (angle > 225  and angle <= 247) {
    speed_x = speed_x_med_slow;   } 
  else if (angle > 247  and angle < 267) {
    speed_x = speed_x_med;   }
  else if (angle >= 267  and angle <= 273) {
    speed_x = speed_x_fast;   }
  else if (angle > 273  and angle <= 292) {
    speed_x = speed_x_med;   }
  else if (angle > 292  and angle <= 315) {
    speed_x = speed_x_med_slow;     }    
  else if (angle > 315  and angle <= 337) {
    speed_x = speed_x_slow;  }
  else {
    speed_x = speed_x_stop;
  }

/* Comment for speed test  
  //send commands to motors
    setMotor_y(speed_y, y_direction_positive);
    setMotor_x(speed_x, x_direction_positive);
*/

//Speed test - start
   // setMotor_y(speed_test, y_direction_positive);
//Speed test - end    
  
  //set servo for batter based on c button

  
}
 
void setMotor_y(int speed_y, boolean y_direction_positive)
{
  analogWrite(enablePin2, speed_y);
  digitalWrite(in3Pin, ! y_direction_positive);
  digitalWrite(in4Pin, y_direction_positive);
}

void setMotor_x(int speed_x, boolean x_direction_positive)
{
  analogWrite(enablePin1, speed_x);
  digitalWrite(in1Pin, ! x_direction_positive);
  digitalWrite(in2Pin, x_direction_positive); 
}
