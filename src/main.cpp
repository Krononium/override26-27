#include "main.h"
#include "api.h"
#include <cmath>

using namespace std;
using namespace pros;
using namespace competition;

Controller master(pros::E_CONTROLLER_MASTER);
MotorGroup rightMG({-1, -2});   // Creates a motor group with reversed ports 1 & 2
MotorGroup leftMG({18, 19});  // Creates a motor group with forwards port 18 & 19

Motor inOutMotor({-17}); // Creates a motor with port 17 to handle intake/extake
Motor cascadeLiftMotor({-16}); // Creates a motor with reversed port 16 to handle the cascade lift. 
Motor clawMotor({10});  // Creates a motor with port 10 to handle the claw
Motor clawRotationMotor({-9}); // Creates a motor with reversed port 9 to handle the claw

Rotation clawRotationSensor({8}); // Creates a rotation sensor with port 8 to find the claw's angle

int maxVel = 600;
/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
/*
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}
*/
/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");	
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}


//Contains the autonomous movements in a switch case such as:
//moving forward, turning both directions, slowing down, or other autonomous 
//movements that we may need/want
void passiveMove(int dir){
	switch(dir){
		//A forward movement at a slow speed
		case 1:
			rightMG.move_velocity(maxVel/6);
			leftMG.move_velocity(maxVel/6);
			break;
		//Stopping the velocity entirely
		default:
			rightMG.move_velocity(0);
			leftMG.move_velocity(0);
			break;
	}
}
/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */

void autonomous() {
	// passiveMove(1);
	// delay(1000);
	// passiveMove(0);
}

//Contains the primary control scheme (arcade style) for driver control to be used as a
//task in the opcontrol method
void twoStick(){
	while (true) {
		// Arcade control scheme
		double leftStick = -master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);    // Gets amount forward/backward from left joystick
		double rightStick = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);  // Gets the turn left/right from right joystick
		leftMG.move_voltage((leftStick - rightStick) * 94.48818898);                      // Sets left motor voltage
		rightMG.move_voltage((leftStick + rightStick) * 94.48818898);                     // Sets right motor voltage
		pros::delay(20);                               // Run for 20 ms then update
	}
}

// Function that handles intake/extake
void inOut() { 
	while (true) {
		if (master.get_digital(E_CONTROLLER_DIGITAL_R2)) { // Intakes while holding R2 
			inOutMotor.move_velocity(maxVel); // Sets the forward voltage
		
		} else if (master.get_digital(E_CONTROLLER_DIGITAL_R1)) { // Extakes while holding R1
			inOutMotor.move_velocity(-(maxVel)); // Sets the backwards voltage

		} else { // If neither buttons are being pressed
			inOutMotor.move_velocity(0); // Stops the motor
		}
		pros::delay(20);
		
	}
}

// Function to handle the raising and lowering of the cascade lift
void cascadeLift() {
	while (true) {
		if (master.get_digital(E_CONTROLLER_DIGITAL_X)) { // Moves cascade lift up when holding the x button
			cascadeLiftMotor.move_velocity(maxVel * 3); // Sets velocity of the cascade lift to go up

		} else if (master.get_digital(E_CONTROLLER_DIGITAL_B)) { // Moves cascade lift down when holding the b button
			cascadeLiftMotor.move_velocity(-(maxVel * 3)); // Sets velocity of the cascade lift to go down

		} else {
			cascadeLiftMotor.move_velocity(0); // Stops the motor
		}
		pros::delay(20);
	}
}

// Function to handle the claw
void claw() {
	while (true) {
		if (master.get_digital(E_CONTROLLER_DIGITAL_L2)) { // If button L2 is being held down
			clawMotor.move_velocity(maxVel / 3); // Set forward velocity for the claw motor

		} else if (master.get_digital(E_CONTROLLER_DIGITAL_L1)) { // If button L1 is being held down
		 	clawMotor.move_velocity(-(maxVel / 3)); // Set backwards velocity for the claw motor

		} else { // If neither button is being held down
		 	clawMotor.move_velocity(0); // Stops the motor
		}
		
		pros::delay(20);
	}
}

// Legacy function to handle the rotation of the claw if the rotation sensor version breaks
void clawRotationLegacy() {
	/* USE THIS IF WE WANT MANUAL CLAW ROTATION
	
	while (true) {
		if (master.get_digital(E_CONTROLLER_DIGITAL_UP)) { // If d-pad button up is being pressed
			clawRotationMotor.move_velocity(maxVel / 15); // Sets forward velocity for the claw rotation motor

		} else if (master.get_digital(E_CONTROLLER_DIGITAL_DOWN)) { // If d-pad button down is being pressed
			clawRotationMotor.move_velocity(-(maxVel / 15)); // Sets backwards velocity for the claw rotation motor

		} else { // If neither button is being held down
			clawRotationMotor.move_velocity(0); // Stops the claw rotation motor
		}

		pros::delay(20);
	 } */

	
	// CLAW ROTATIOIN WITH PRESETS
	bool clawUp = true; // Boolean checker to make sure you can't go up when already up, and down when already down

	while (true) {
		if (master.get_digital(E_CONTROLLER_DIGITAL_UP) && !clawUp) { // If d-pad button up gets pressed and the claw is not up
			clawRotationMotor.move_velocity(maxVel / 13); // Sets the speed of the claw rotation motor to make it go up
			pros::delay(500); // Runs for 500 ms
			clawRotationMotor.move_velocity(0); // Stops the claw rotation motor

			clawUp = true; // Changes the boolean value to true so you can't go up again

		} else if (master.get_digital(E_CONTROLLER_DIGITAL_DOWN) && clawUp) { // If d-pad button down gets pressed and the claw is currently up
			clawRotationMotor.move_velocity(-(maxVel / 15)); // Sets the speed of the claw rotation motor to make it go down

			clawUp = false; // Changes the boolean value to false so you can't go down again
		} else if (master.get_digital(E_CONTROLLER_DIGITAL_A)) { // Third preset to make claw reach the toggle 
			clawRotationMotor.move_velocity(-(maxVel / 6)); // Moves claw all the way down to confirm it's location
			pros::delay(150); // Runs for 150 ms

			clawRotationMotor.move_velocity(maxVel / 10); // Moves claw back up to reach the toggle
			pros::delay(600); // Runs for 700 ms

			clawRotationMotor.move_velocity(0);

		}
		pros::delay(20);
	}

}

// Helper function to get the claw's angle in a rounded, whole number 
int getClawDegrees() {
	// Try clawRotationSensor.get_position(); as well
	std::int32_t rawDegrees = clawRotationSensor.get_angle(); // Gets the raw angle as centidegrees
	double degrees = rawDegrees / 100.0; // Turns the centidegrees into actual degrees
	return static_cast<int>(std::round(degrees)); // Rounds the degrees to a whole number and casts it to an integer
}

// Helper funtion that moves the claw to a specific angle
void moveClawToAngle(int angle) {
	int currentDegrees = getClawDegrees(); // Gets the current angle

	if (currentDegrees <= angle + 5 && currentDegrees >= angle - 5) {
		return;
	}

	if (currentDegrees < angle) { // If the current angle is lower then the target then it needs to go up
		clawRotationMotor.move_velocity(maxVel / 60); // Sets the motor's velocity to make the claw go up
		while (getClawDegrees() < angle) { // Loops and waits 20 ms each turn until the claw reaches or is greater than the angle
			pros::delay(20);

		}

		clawRotationMotor.move_velocity(0); // Stops the motor

	} else if (currentDegrees > angle) { // If the current angle is higher then the target then it needs to go down
		clawRotationMotor.move_velocity(-(maxVel / 60)); // Sets the motor's velocity to make the claw go down
		while (getClawDegrees() > angle) { // Loops and waits 20 ms each turn until the claw reaches or is less than th eangle
			pros::delay(20);
			
		}

		clawRotationMotor.move_velocity(0); // Stops the motor

	} // If those two are false, then the claw is already at the specified angle, so it does nothing
}

// More efficient function to handle the rotation of the claw by using a rotation sensor
void clawRotation() {
	int midPosition = 134; // Degree value that determines the specific angle to reach the middle position preset
	int highPosition = 170; // Degree value that determines the specific angle to reach the top position preset
	int lowPosition = 53; // Degree value that determines the specific angle to reach the bottom position preset

	moveClawToAngle(midPosition); // Makes claw start at mid position

	int maintainAngle = midPosition; // Variable to make the claw maintain its location, starts at mid position since the claw will start at the mid position

	while (true) {
		int currentDegrees = getClawDegrees(); // Gets the claw's current angle

		if (master.get_digital(E_CONTROLLER_DIGITAL_UP)) { // Checks if the up arrow on the d-pad is being pressed
			// Causes the claw to go up

			if (currentDegrees <= midPosition + 5 && currentDegrees >= midPosition - 5) { // Checks if the current position of the claw is at the mid position
				moveClawToAngle(highPosition);
				maintainAngle = highPosition; // Changes the maintain position to the high position

			} else if (currentDegrees <= highPosition + 5 && currentDegrees >= highPosition - 5) {// Checks if the claw is at the high position
				// Does nothing so you can't go up when you are already at the max height

			} else { // Claw is at the bottom
				moveClawToAngle(midPosition);
				maintainAngle = midPosition; // Changes the maintain position to the mid position
				
			}

		} else if (master.get_digital(E_CONTROLLER_DIGITAL_DOWN)) { // Checks if the bottom arrow on the d-pad is being pressed
			// Causes the claw to go down

			if (currentDegrees <= midPosition + 10 && currentDegrees >= midPosition - 10) { // Checks if the current position of the claw is at the mid position
				clawRotationMotor.move_velocity(-(maxVel / 15)); // Sets the speed of the claw rotation motor to make it go down indefinitely
				maintainAngle = 0;  // Changes the maintain position to the bottom position
				

			} else if (currentDegrees <= highPosition + 5 && currentDegrees >= highPosition - 5) { // Checks if the claw is at the high position
				moveClawToAngle(midPosition);
				maintainAngle = midPosition; // Changes the maintain position to the mid position

			} else  if (currentDegrees <= lowPosition + 5 && currentDegrees >= lowPosition - 5) { // Claw is at the bottom
				// Does nothing so you can't go lower when you are already at the lowest
				maintainAngle = 0; // Changes the maintain position so it stays down
			
			} else {
				clawRotationMotor.move_velocity(-(maxVel / 15)); // Makes it go down for safety
				maintainAngle = 0;
			}
		} else { // Nothing pressed, maintain position
			if (maintainAngle != 0) { // Checks if the maintain angle is at high position or low position
				moveClawToAngle(maintainAngle); // Calls move claw to angle to make it maintain the angle

			} else { // If the maintain position is to go down
				clawRotationMotor.move_velocity(-(maxVel / 15)); // Sets the speed of the claw rotation motor to make it go down indefinitely
			}
			
		}

		pros::lcd::print(1, "Current Degrees: %d", currentDegrees);	// Prints the claw's current angle for debugging
		pros::delay(20);
	}
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	Task movement(twoStick);
	Task intakeExtake(inOut);
	Task cascadeHandling(cascadeLift);
	Task clawHandling(claw);
	Task clawRotationHandling(clawRotation);
}
