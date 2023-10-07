#include "CarControl.h"
#include <string>
#include <chrono>


int CarControl::Init()
{
	//Steering
	minsteeringpulsewidth = 1150;
	midsteeringpulsewidth = 1500;
	maxsteeringpulsewidth = 1850;
	currentSteeringwheelPosition = midsteeringpulsewidth;

	//Throttle
	minThrottleFowardPulsewidth = 1600;
	maxThrottleFowardPulsewidth = 1900;
	neutralThrottlePulsewidth = 1500;

	reverseimpulseThrottlePulsewidth = 1000;
	minreverseThrottlePulsewidth = 1400;
	maxreverseThrottlePulsewidth = 1200;

	currentThrottlePosition = neutralThrottlePulsewidth;

	nanosecondsIntoBackGearInProgress = 0;


	lastCommandSteering = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	lastCommandThrottle = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	return 0;
}



std::string CarControl::StepSteering(std::string keyboardinput)
{
	// check how long (in ns) since the last Step() call
	long long now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	long long waitnanoseconds = now - lastCommandSteering;   // TODO waitnanoseconds muss die wartezeit seit dem letzen aufruf von run sein

	double timefactor = waitnanoseconds / 1000000;

	// get the input in a variable to make reading easier
	const char* buttonArray = keyboardinput.c_str();

	bool buttonLeftpressed = false; //'a'
	bool buttonRightpressed = false; //'d'

	if (buttonArray[0x41] == 'D') buttonLeftpressed = true;
	if (buttonArray[0x44] == 'D') buttonRightpressed = true;

	if (buttonLeftpressed && buttonRightpressed)
	{
		;
	}
	else if (buttonLeftpressed)
	{
		if (currentSteeringwheelPosition < maxsteeringpulsewidth)
		{
			currentSteeringwheelPosition += 2* timefactor;
		}
	}
	else if (buttonRightpressed)
	{
		if (currentSteeringwheelPosition > minsteeringpulsewidth)
		{
			currentSteeringwheelPosition -= 2 * timefactor;
		}
	}
	else
	{
		if (currentSteeringwheelPosition < midsteeringpulsewidth) currentSteeringwheelPosition += 2 * timefactor;
		if (currentSteeringwheelPosition > midsteeringpulsewidth) currentSteeringwheelPosition -= 2 * timefactor;

	}

	//gpio_in?data=18;12345  steering

	// Steering
	std::string command = "gpio_in?data=18;" + std::to_string(currentSteeringwheelPosition);

	//std::string command = "STEERING=" + std::to_string(currentSteeringwheelPosition);
	lastCommandSteering = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	return command;
}

std::string CarControl::StepThrottle(std::string keyboardinput)
{
	// check how long (in ns) since the last Step() call
	long long now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	long long waitnanoseconds = now - lastCommandThrottle;   // TODO waitnanoseconds muss die wartezeit seit dem letzen aufruf von run sein

	double timefactor = waitnanoseconds / 1000000;

	// get the input in a variable to make reading easier
	const char* buttonArray = keyboardinput.c_str();

	bool buttonForwardpressed = false; //'w'
	bool buttonBackwardpressed = false; //'s'

	if (buttonArray[0x57] == 'D') buttonForwardpressed = true;
	if (buttonArray[0x53] == 'D') buttonBackwardpressed = true;

	// throttle
	if (buttonForwardpressed && buttonBackwardpressed)
	{
		//nanosecondsIntoBackGearInProgress =0;
	}
	else if (buttonForwardpressed)
	{
		if (currentThrottlePosition < minThrottleFowardPulsewidth) currentThrottlePosition = minThrottleFowardPulsewidth;
		if (currentThrottlePosition < maxThrottleFowardPulsewidth) currentThrottlePosition += .1 * timefactor;
		nanosecondsIntoBackGearInProgress = 0;
	}
	else if (buttonBackwardpressed)
	{
		if (currentThrottlePosition > minThrottleFowardPulsewidth) currentThrottlePosition -= .2 * timefactor;
		else
		{

			nanosecondsIntoBackGearInProgress += waitnanoseconds;
		}

		// in case we are in switch to backwards gear or in backward gear
		if (nanosecondsIntoBackGearInProgress > 0)
		{
			if (nanosecondsIntoBackGearInProgress < 2 * 1000000000)   // Break for 2 seconds
			{
				currentThrottlePosition = neutralThrottlePulsewidth;
			}
			else if (nanosecondsIntoBackGearInProgress < 2.020 * 1000000000)   // Switch to max reverse for 20 ms
			{
				currentThrottlePosition = reverseimpulseThrottlePulsewidth;
			}
			else if (nanosecondsIntoBackGearInProgress < 2.200 * 1000000000)   // Switch to max reverse for 20 ms
			{
				currentThrottlePosition = minreverseThrottlePulsewidth;
			}
			else
			{
				if (currentThrottlePosition > minreverseThrottlePulsewidth) currentThrottlePosition = minreverseThrottlePulsewidth;
				if (currentThrottlePosition > maxreverseThrottlePulsewidth)  // dont drive to fast backwards
				{
					currentThrottlePosition -= 0.1 * timefactor;
				}
			}
		}
	}
	else   // No button pressed
	{
		if (currentThrottlePosition > minThrottleFowardPulsewidth) currentThrottlePosition -= .1 * timefactor;
		else
		{
			currentThrottlePosition = neutralThrottlePulsewidth;
			//nanosecondsIntoBackGearInProgress = 0;
		}
		if (currentThrottlePosition < minreverseThrottlePulsewidth) currentThrottlePosition += .1 * timefactor;
		else
		{
			currentThrottlePosition = neutralThrottlePulsewidth;
		}
	}


	//gpio_in?data=15;12345  throttle

	// Steering
	std::string command = "gpio_in?data=15;" + std::to_string(currentThrottlePosition);

	//std::string command = "THROTTLE=" + std::to_string(currentThrottlePosition);
	lastCommandThrottle = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	return command;
}


