#include "main.h"
#include "api.h"

using namespace std;
using namespace pros;
using namespace competition;

Controller master(pros::E_CONTROLLER_MASTER);
MotorGroup leftMG({-1, -2});    // Creates a motor group with reversed ports 1 & 2
MotorGroup rightMG({3, 4});  // Creates a motor group with forwards port 3 & 4
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
	autonomous();
	
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
	passiveMove(1);
	delay(1000);
	passiveMove(0);
}

//Contains the primary control scheme (arcade style) for driver control to be used as a
//task in the opcontrol method
void twoStick(){
	while (true) {
		// Arcade control scheme
		double leftStick = master.get_analog(E_CONTROLLER_ANALOG_LEFT_Y);    // Gets amount forward/backward from left joystick
		double rightStick = master.get_analog(E_CONTROLLER_ANALOG_RIGHT_X);  // Gets the turn left/right from right joystick
		leftMG.move_voltage(leftStick - rightStick);                      // Sets left motor voltage
		rightMG.move_voltage(leftStick + rightStick);                     // Sets right motor voltage
		pros::delay(20);                               // Run for 20 ms then update
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
}
