#include <AFMotor.h>
#include <NewPing.h>
#include <Servo.h>

#define TRIG_PIN A0
#define ECHO_PIN A1
#define MAX_DISTANCE 200
#define MAX_SPEED 190  // sets speed of DC motors
#define BASE_SPEED 120 // base speed for adaptive control

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);
Servo myservo;

boolean goesForward = false;
int distance = 100;
int speedSet = 0;

void setup()
{
    myservo.attach(10);
    myservo.write(115);
    delay(1000);
    distance = getAverageDistance();
}

void loop()
{
    int distanceR = 0;
    int distanceL = 0;

    distance = readPing();

    if (distance <= 15)
    {
        avoidObstacle();
    }
    else
    {
        moveForward(distance);
    }
    delay(40);
}

// Helper functions
int getAverageDistance()
{
    int total = 0;
    for (int i = 0; i < 4; i++)
    {
        total += readPing();
        delay(100);
    }
    return total / 4;
}

int readPing()
{
    delay(50);
    int cm = sonar.ping_cm();
    return cm == 0 ? MAX_DISTANCE : cm;
}

// Obstacle handling functions
void avoidObstacle()
{
    moveStop();
    delay(100);
    moveBackward();
    delay(300);
    moveStop();
    delay(200);

    int distanceR = lookRight();
    int distanceL = lookLeft();
    delay(100);

    if (distanceR > distanceL)
    {
        turnRight();
    }
    else if (distanceL > distanceR)
    {
        turnLeft();
    }
    else
    {
        // if both sides are equal, make a random choice or turn right by default
        turnRight();
    }
    moveStop();
}

// Servo functions for looking around
int lookRight()
{
    myservo.write(50);
    delay(400);
    int distance = readPing();
    myservo.write(115);
    delay(100);
    return distance;
}

int lookLeft()
{
    myservo.write(170);
    delay(400);
    int distance = readPing();
    myservo.write(115);
    delay(100);
    return distance;
}

// Movement functions
void moveStop()
{
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    motor3.run(RELEASE);
    motor4.run(RELEASE);
}

void moveForward(int distance)
{
    int speedAdjust = BASE_SPEED + (MAX_SPEED - BASE_SPEED) * (distance / MAX_DISTANCE);
    speedAdjust = min(speedAdjust, MAX_SPEED); // ensure speed doesn’t exceed MAX_SPEED

    if (!goesForward)
    {
        goesForward = true;
        motor1.run(FORWARD);
        motor2.run(FORWARD);
        motor3.run(FORWARD);
        motor4.run(FORWARD);
    }

    setMotorSpeeds(speedAdjust);
}

void moveBackward()
{
    goesForward = false;
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(BACKWARD);
    motor4.run(BACKWARD);
    setMotorSpeeds(BASE_SPEED);
}

void turnRight()
{
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(BACKWARD);
    motor4.run(BACKWARD);
    delay(400);
}

void turnLeft()
{
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
    delay(400);
}

void setMotorSpeeds(int speed)
{
    motor1.setSpeed(speed);
    motor2.setSpeed(speed);
    motor3.setSpeed(speed);
    motor4.setSpeed(speed);
}
