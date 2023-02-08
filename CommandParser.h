#include <string>
#include "Viewport.h"
#ifdef _DEBUG
	#include<iostream>
#endif //_DEBUG
#pragma once

void buildTestCube(VertexArray& array, const int dims);
void buildTetraedr(VertexArray& array, const int dims);
void buildSphere(VertexArray& array, const int dims);

class CommandParser
{
	enum CommandType{
		NONE,
		ROTATE,
		MOVE,
		BUILD,
		MODE
	};

protected:
	std::string input_buffer = "";
	CommandType cmd = NONE;
	int param_index = 0;
	int max_params = 0;
	int* param_buffer;
	std::string param_string = "";
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
			output_bar = "rotate[axis1, axis2]";
#ifdef _DEBUG
			std::cout << "Command: rotate" << std::endl;
#endif //_DEBUG
		}
		else if (input_buffer == "m") {
			cmd = MOVE;
			max_params = 1;
			output_bar = "move[axis]";
#ifdef _DEBUG
			std::cout << "Command: move" << std::endl;
#endif //_DEBUG
		}
		else if (input_buffer == "mode") {
			cmd = MODE;
			max_params = 1;
			output_bar = "<ortho|hyper|cut>";
		}
		else if (input_buffer == "build") {
			cmd = BUILD;
			max_params = 2;
			output_bar = "<cube|tetr|sphere>";
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
		switch (cmd) {
		case MOVE:
		case ROTATE:
			param_buffer[param_index - 1] = parseInt();
			break;
		case MODE:
			param_string = input_buffer;
			break;
		case BUILD:
			if (param_index == 1) {
				param_string = input_buffer;
				output_bar = "<dim: 1.." + std::to_string(MAX_DIMS) + ">";
			}
			else if (param_index == 2) {
				param_buffer[0] = parseInt();
			}
		}
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
			output_bar = "Rotation " + std::to_string(vp->getRotationHistory()) + " was fixed. New axis set.";
			vp->setRotationAxis(param_buffer[0], param_buffer[1]);
			break;
		case MOVE:
			if (param_buffer[0] < 0 || param_buffer[0] >= MAX_DIMS) {
				output_bar = "Max " + std::to_string(MAX_DIMS) + " dimensions. Axis index must be from 0 to " + std::to_string(MAX_DIMS - 1);
				break;
			}
			output_bar = "Movement " + std::to_string(vp->getMovementHistory()) + " was fixed. New axis set.";
			vp->setMoveAxis(param_buffer[0]);
			break;
		case MODE:
			if (param_string == "cut") {
				vp->projection = CUT;
				output_bar = "Cut mode";
			}
			else if (param_string == "ortho" || param_string == "flat") {
				vp->projection = ORTHO;
				output_bar = "Ortho projection mode";
			}
			else if (param_string == "hyper" || param_string == "bihyper") {
				vp->projection = BIHYPERBOLIC;
				output_bar = "Bihyperbolic projection mode";
			}
			else {
				output_bar = "No such mode '" + param_string + "'";
			}
			break;
		case BUILD:
			if (param_buffer[0] > MAX_DIMS || param_buffer[0] < 1) {
				output_bar = "Invalid dimension number. Must be from 1 to " + std::to_string(MAX_DIMS);
				break;
			}
			*vp->shape = VertexArray();
			if (param_string == "cube") {
				buildTestCube(*vp->shape, param_buffer[0]);
			}else if (param_string == "tetr") {
				buildTetraedr(*vp->shape, param_buffer[0]);
			}
			else if (param_string == "sphere") {
				buildSphere(*vp->shape, param_buffer[0]);
			}
			else {
				output_bar = "Can't recognize such shape!";
			}
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
		param_string = "";
	}
};

