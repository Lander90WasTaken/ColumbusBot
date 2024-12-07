
  
#include <Arduino.h>
#include <Servo.h>
Servo servo;
const int servoPin = 11; // PWM output for servo

// Ultrasonic Module pins
const int trigPin = A0; // 10 microsecond high pulse causes chirp , wait 50 us
const int echoPin = A1; // Width of high pulse indicates distance

// Motor control pins : L298N H bridge
const int enAPin = 6; // Left motor PWM speed control
const int in1Pin = 7; // Left motor Direction 1
const int in2Pin = 5; // Left motor Direction 2
const int in3Pin = 4; // Right motor Direction 1
const int in4Pin = 2; // Right motor Direction 2
const int enBPin = 3; // Right motor PWM speed contro

enum Motor { LEFT, RIGHT };
bool tooCloseRight = 0;
bool tooCloseLeft;
bool tooCloseStraight;
int angleThatsToClose = 0;


// Set motor speed: 255 full ahead, −255 full reverse , 0 stop
void go( enum Motor m, int speed){     
  digitalWrite (m == LEFT ? in1Pin : in3Pin , speed > 0 ? HIGH : LOW );
  digitalWrite (m == LEFT ? in2Pin : in4Pin , speed <= 0 ? HIGH : LOW );
  analogWrite(m == LEFT ? enAPin : enBPin, speed < 0 ? -speed : speed );
}

unsigned int readDistance (){
  digitalWrite ( trigPin , HIGH );
  delayMicroseconds (10);
  digitalWrite ( trigPin , LOW );
  unsigned long period = pulseIn ( echoPin, HIGH );
  return period * 343 / 2000;
  // Speed of sound in dry air , 20C is 343 m/s
  // pulseIn returns time in microseconds (10ˆ−6)
  // 2d = p * 10ˆ−6 s * 343 m/s = p * 0.00343 m = p * 0.343 mm/us
}

void setup () {
  Serial.begin(9600);
  pinMode(trigPin , OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite ( trigPin , LOW);
  pinMode(enAPin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(enBPin, OUTPUT);
  servo.attach ( servoPin );
  servo.write (90);
  go(LEFT, 0);
  go(RIGHT, 0);
}

void loop () {
  // Stop
    go(LEFT, 0);
    go(RIGHT, 0);
    delay (500);

  //scan the area
  tooCloseLeft = false;
  tooCloseRight = false;
  tooCloseStraight = false;
  for (int angle = 0; angle <= 180; angle += 30) { // Scan from 0 to 180 degrees in 15 degree increments
    servo.write(angle);
    delay(500); // Allow time for servo to move
    unsigned int distance = readDistance();
    Serial.print("Angle: ");
    Serial.print(angle);
    Serial.print(" - Distance: ");
    Serial.println(distance);
    if (distance < 300 and angle < 60.5){
      tooCloseRight = true;
      angleThatsToClose = angle;
      break;
    }
    else if (distance < 300 and 60.5 < angle < 120.5){
      tooCloseStraight = true;
      angleThatsToClose = angle;
      break;
    }
    else if (distance < 300 and 120.5 < angle < 150.5){
      tooCloseLeft = true;
      angleThatsToClose = angle;
      break;
    }
  }

  if (tooCloseRight) {
    Serial.println("< Something's nearby: back up left");
    // Slow down/Stop
    go(LEFT, 0);
    go(RIGHT, 0);
    delay (200);
    
    // back up  
    go(LEFT, -120);
    go(RIGHT, -120);
    delay (500);    

    // rotate 90deg  
    go(LEFT, -185);
    go(RIGHT, 0);
    delay (500);         
    
    // stop
    go(LEFT, 0);
    go(RIGHT, 0);
    delay (500);   
  }
  else if (tooCloseStraight) {
    // Slow down/Stop
    go(LEFT, 0);
    go(RIGHT, 0);
    delay (200);
    
    // back up  
    go(LEFT, -120);
    go(RIGHT, -120);
    delay (500);    

    // rotate 180deg  
    go(LEFT, -185);
    go(RIGHT, 185);
    delay (500);         
    
    // stop
    go(LEFT, 0);
    go(RIGHT, 0);
    delay (500);   
  }
  else if (tooCloseLeft) {
    // Slow down/Stop
    go(LEFT, 0);
    go(RIGHT, 0);
    delay (200);
    
    // back up  
    go(LEFT, -120);
    go(RIGHT, -120);
    delay (500);    

    // rotate 270deg  
    go(LEFT, -185);
    go(RIGHT, 0);
    delay (500);         
    
    // stop
    go(LEFT, 0);
    go(RIGHT, 0);
    delay (500);   
  }
  else {
    Serial.println("^ Nothing in our way: go forward ");
    go(LEFT, 150);
    go(RIGHT, 150);
  }
  delay (1000); // increase to travel further
}
  