#include <string>
#include "Viewport.h"
#ifdef _DEBUG
	#include<iostream>
#endif //_DEBUG
#pragma once

class CommandParser
{
	enum CommandType{
		NONE,
		ROTATE,
		MOVE
	};

protected:
	std::string input_buffer = "";
	CommandType cmd = NONE;
	int param_index = 0;
	int max_params = 0;
	int* param_buffer;
	Viewport* vp;

public:
	std::string cmdline_bar = ">_";
	std::string output_bar = "...";

public:
	CommandParser(Viewport* viewport) : vp(viewport) {}

	void input(char a) {
		if (a == ' ') {
			if (param_index == 0) {
				parseCommand();
			}
			else if(param_index >= max_params) {
				output_bar = "press Enter to confirm";
			}
			else {
				parseParam();
			}
		}
		else if (a == '\n' || a == '\r') {
			if (param_index > 0) parseParam();
			else parseCommand();
			execute();
		}
		else if (a == '\b') {
			if (input_buffer.length() > 0) {
				input_buffer.erase(input_buffer.length() - 1, std::string::npos);
			}
			else if (param_index > 0) {
				param_index--;
			}
		}
		else {
			input_buffer += a;

		}
		
		cmdline_bar = ">" + input_buffer + "_";
	}

protected:
	void parseCommand() {
		if (input_buffer == "r") {
			cmd = ROTATE;
			max_params = 2;
#ifdef _DEBUG
			std::cout << "Command: rotate" << std::endl;
#endif //_DEBUG
		}
		else if (input_buffer == "m") {
			cmd = MOVE;
			max_params = 1;
#ifdef _DEBUG
			std::cout << "Command: move" << std::endl;
#endif //_DEBUG
		}
		else {
			cmd = NONE;
		}

#ifdef _DEBUG
		std::cout << "max_params: " << max_params << std::endl;
#endif

		if (max_params != 0) {
			param_buffer = new int[max_params];
			param_index++;
		}

		input_buffer = "";
	}

	void parseParam() {
		if (input_buffer.length() == 0) return;
		param_buffer[param_index - 1] = parseInt();
		input_buffer = "";
		param_index++;
	}

	int parseInt() {
		try {
			return std::stoi(input_buffer);
		}
		catch (std::invalid_argument) {
			output_bar = "Invalid argument. Value 0 used.";
			return 0;
		}
		
	}

	void execute() {
		switch (cmd) {
		case ROTATE:
			if (param_buffer[0] < 0 || param_buffer[0] >= MAX_DIMS || param_buffer[1] < 0 || param_buffer[1] >= MAX_DIMS) {
				output_bar = "Max " + std::to_string(MAX_DIMS) + " dimensions. Axis index must be from 0 to " + std::to_string(MAX_DIMS-1);
				break;
			}
			float rhistory;
			rhistory= vp->getRotationHistory();
			vp->setRotationAxis(param_buffer[0], param_buffer[1]);
			break;
		case MOVE:
			if (param_buffer[0] < 0 || param_buffer[0] >= MAX_DIMS) {
				output_bar = "Max " + std::to_string(MAX_DIMS) + " dimensions. Axis index must be from 0 to " + std::to_string(MAX_DIMS - 1);
				break;
			}
			float mhistory = vp->getMovementHistory();
			vp->setMoveAxis(param_buffer[0]);
			break;
		}

#ifdef _DEBUG
		std::cout << "Executed. Params: ";
		for (int i = 0; i < max_params; i++) {
			std::cout << param_buffer[i] << " ";
		}
		std::cout << std::endl;
#endif

		if(max_params > 0) delete[] param_buffer;
		param_buffer = nullptr;
		input_buffer = "";
		cmdline_bar = ">_";
		param_index = 0;
		max_params = 0;
		cmd = NONE;
	}
};

