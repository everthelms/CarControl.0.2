#include<stdio.h>

#include <string>
#include <iostream>

#include "RCMessage.h"
#include "RCMailBox.h"
#include "RCData.h"
#if defined(_MSC_VER)
#pragma comment(lib, "RCLibrary.lib")
#endif

#include "CarControl.h"


#define EOF (-1)

extern "C"
{
#if defined(_MSC_VER)
	__declspec(dllexport) void __cdecl run(std::string parameter)
	{
#else
	void run(std::string parameter)
	{
#endif
		std::cout << "CarControl.0.2 started\n";

		RCMessage msg(parameter);
		std::string creatorID = msg["from"];
		RCMailBox theMailbox(creatorID);
		RCData rcdata;

		std::string moduleName = "CarControl.0.2";
        std::cout << "CarControl.0.2 1\n";

		CarControl theCarControl;
		theCarControl.Init();
		std::string latestKeyboardInput = "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";

        std::cout << "CarControl.0.2 2\n";


		std::vector<std::string> control_out_recevier_modules;
		std::vector<std::string> control_out_recevier_inputs;


		theMailbox.send(creatorID, { "confirmcreate", {{"type", moduleName},{"id", theMailbox.Id}} });
        std::cout << "CarControl.0.2 3\n";
		// *** Here starts the Messageloop ***
		while (true)
		{
            theMailbox.receive(msg, 1000);  // 1000 us is also the tact to send motor commands

            if (msg.command == "connect")
            {
                std::string sourceoutput = msg["sourceoutput"];
                std::string targetmodule = msg["targetmodule"];
                std::string targetinput = msg["targetinput"];


                if (sourceoutput == "control_out")
                {
                    control_out_recevier_modules.push_back(targetmodule);
                    control_out_recevier_inputs.push_back(targetinput);
                }
            }

			if (msg.command == "destroy")
			{
				theMailbox.send(creatorID, { "confirmdestruction", {{"from", theMailbox.Id}} });

				break;
			}

			if (msg.command  == "keyboard_in")
			{
				latestKeyboardInput=  msg["data"];
				std::cout << latestKeyboardInput << "\n";
			}


            //todo: als message schicken mit data={....}

			for (int i = 0; i < control_out_recevier_modules.size(); i++)
			{
				// Send commands to car
				std::string command = theCarControl.StepSteering(latestKeyboardInput);
				theMailbox.send(control_out_recevier_modules[i], control_out_recevier_inputs[i] + "?data={" + command + "}");

				command = theCarControl.StepThrottle(latestKeyboardInput);
				theMailbox.send(control_out_recevier_modules[i], control_out_recevier_inputs[i] + "?data={" + command + "}");

			}
		}
	}

#ifdef __cplusplus
}
#endif
