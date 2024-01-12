/**
 * Test car wiring
 */
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/car/two_wheels_car.h>

using eloq::camera;
using eloq::car::Motor;
using eloq::car::TwoWheelsCar;

// replace with your motor pins
Motor left(39, 40);
Motor right(42, 41);
TwoWheelsCar testCar(left, right);


/**
 *
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___CAR TEST___");

    // how many millis motors will run
    testCar.defaultDuration(200);
    testCar.stop();

    Serial.println("Enter one of f (forward), b (backward), l (left), r (right)");
}


/**
 *
 */
void loop() {
    if (!Serial.available())
        return;

    String cmd = Serial.readStringUntil('\n');

    if (cmd.startsWith("f")) testCar.forward();
    else if (cmd.startsWith("b")) testCar.backward();
    else if (cmd.startsWith("l")) testCar.left();
    else if (cmd.startsWith("r")) testCar.right();
}