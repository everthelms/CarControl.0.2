#pragma once

#include <string>

class CarControl
{
public:
	int Init();
	std::string StepSteering(std::string keyboardinput);
	std::string StepThrottle(std::string keyboardinput);
	

private:
	
	//Steering
	double minsteeringpulsewidth;
	double midsteeringpulsewidth;
	double maxsteeringpulsewidth;
	double currentSteeringwheelPosition;

	//Throttle
	double minThrottleFowardPulsewidth;
	double maxThrottleFowardPulsewidth;
	double neutralThrottlePulsewidth;

	double reverseimpulseThrottlePulsewidth;
	double minreverseThrottlePulsewidth;
	double maxreverseThrottlePulsewidth;

	double currentThrottlePosition;

	long long nanosecondsIntoBackGearInProgress;
	long long lastCommandSteering;
	long long lastCommandThrottle;

};

