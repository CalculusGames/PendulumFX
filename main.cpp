#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmdfx.hpp>
#include <string>
#include <cstdlib>
#include <random>

#include "pendulum.hpp"

int main(int argc, char** argv) {
	bool noInteractive = false;
	int cliCount = 0;
	double cliGravity = 9.81;
	double cliMultiplier = 1.0;
	double cliDamping = -1.0; // negative means not set
	double cliCoupling = -1.0;
	bool cliDrawTrails = false;
	std::vector<int> cliColors;
	std::vector<double> cliLengths;
	std::vector<double> cliMasses;

	int startY = CmdFX::Canvas::getHeight() / 4;

	for (int ai = 1; ai < argc; ++ai) {
		std::string a = argv[ai];
		if (a == "--no-interactive" || a == "--noint") {
			noInteractive = true;
		} else if ((a == "--count" || a == "-c") && ai + 1 < argc) {
			cliCount = std::atoi(argv[++ai]);
		} else if ((a == "--gravity" || a == "-g") && ai + 1 < argc) {
			cliGravity = std::atof(argv[++ai]);
		} else if ((a == "--multiplier" || a == "-m") && ai + 1 < argc) {
			cliMultiplier = std::atof(argv[++ai]);
		} else if ((a == "--damping") && ai + 1 < argc) {
			cliDamping = std::atof(argv[++ai]);
		} else if ((a == "--coupling") && ai + 1 < argc) {
			cliCoupling = std::atof(argv[++ai]);
		} else if (a == "--draw-trails" || a == "--trails" || a == "-t") {
			cliDrawTrails = true;
		} else if (a == "--color" && ai + 1 < argc) {
			std::string colorStr = argv[++ai];
			int color = std::strtol(colorStr.c_str(), nullptr, 16);
			cliColors.push_back(color);
		} else if ((a == "--length" || a == "-l") && ai + 1 < argc) {
			cliLengths.push_back(std::atof(argv[++ai]));
		} else if ((a == "--mass" || a == "-ms") && ai + 1 < argc) {
			cliMasses.push_back(std::atof(argv[++ai]));
		}
	}

	int pendulumCount = 0;
	if (noInteractive && cliCount > 0) {
		pendulumCount = cliCount;
	} else if (noInteractive && cliCount == 0) {
		pendulumCount = 2; // sensible default
	} else {
		std::cout << "Enter number of pendulums: ";
		if (!(std::cin >> pendulumCount) || pendulumCount <= 0) {
			std::cerr << "Invalid number of pendulums. Expected an integer." << std::endl;
			return 1;
		}
	}

	std::vector<Pendulum> pendulums;
	if (noInteractive) {
		// create default pendulums when non-interactive
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> angleDist(-M_PI, M_PI); // random angles between -180deg and 180deg

		for (int i = 0; i < pendulumCount; i++) {
			double length = (i < static_cast<int>(cliLengths.size())) ? cliLengths[i] : (10.0 + i * 2.0);
			double mass = (i < static_cast<int>(cliMasses.size())) ? cliMasses[i] : (1.0 + i * 0.5);
			double angle = angleDist(gen);
			int color = (i < static_cast<int>(cliColors.size())) ? cliColors[i] : (0xFFFFFF - (i * 0x222222));
			int x = CmdFX::Canvas::getWidth() / 2;
			int y = startY; // all start from same pivot initially

			pendulums.emplace_back(length, mass, angle, x, y, color, cliDrawTrails && (i == pendulumCount - 1));
			if (i > 0) {
				pendulums[i].setParent(&pendulums[i - 1]);
			}
		}
	} else {
		for (int i = 0; i < pendulumCount; i++) {
			double length, mass, angle;
			std::cout << "Pendulum #" << (i + 1) << " - Enter length: ";
			if (!(std::cin >> length)) {
				std::cerr << "Invalid input for length. Expected a decimal value." << std::endl;
				return 1;
			}

			std::cout << "Pendulum #" << (i + 1) << " - Enter mass: ";
			if (!(std::cin >> mass)) {
				std::cerr << "Invalid input for mass. Expected a decimal value." << std::endl;
				return 1;
			}

			std::cout << "Pendulum #" << (i + 1) << " - Enter initial angle (radians): ";
			if (!(std::cin >> angle)) {
				std::cerr << "Invalid input for angle. Expected a decimal value." << std::endl;
				return 1;
			}

			int color;
			std::cout << "Pendulum #" << (i + 1) << " - Enter color (hex, default FFFFFF): ";
			std::string colorInput;
			std::getline(std::cin >> std::ws, colorInput);
			if (colorInput.empty()) {
				color = 0xFFFFFF; // default white
			} else {
				color = std::strtol(colorInput.c_str(), nullptr, 16);
				if (color == 0 && colorInput != "0") {
					std::cerr << "Invalid input for color. Expected a hex value (e.g., FF00FF)" << std::endl;
					return 1;
				}
			}

			int x = CmdFX::Canvas::getWidth() / 2;
			int y = startY; // all start from same pivot
			pendulums.emplace_back(length, mass, angle, x, y, color);
			if (i > 0) {
				pendulums[i].setParent(&pendulums[i - 1]);
			}
		}
	}

	double gravity = 9.81;
	double timeMultiplier = 1.0;

	if (noInteractive) {
		gravity = cliGravity;
		timeMultiplier = cliMultiplier;
	} else {
		std::cout << "Change gravity [9.81] ? (y/n): ";
		char changeGravity;
		std::cin >> changeGravity;
		if (changeGravity == 'y' || changeGravity == 'Y') {
			std::cout << "Enter new gravity value: ";
			if (!(std::cin >> gravity)) {
				std::cerr << "Invalid input for gravity. Expected a decimal value." << std::endl;
				return 1;
			}
		}

		std::cout << "Change time multiplier [1] ? (y/n): ";
		char changeTimeStep;
		std::cin >> changeTimeStep;
		if (changeTimeStep == 'y' || changeTimeStep == 'Y') {
			std::cout << "Enter new time multiplier value: ";
			if (!(std::cin >> timeMultiplier)) {
				std::cerr << "Invalid input for time multiplier. Expected a decimal value." << std::endl;
				return 1;
			}
		}
	}

	// damping and coupling (exposed as CLI or interactive tunables)
	if (cliDamping >= 0.0) {
		Pendulum::setGlobalDamping(cliDamping);
	} else if (!noInteractive) {
		std::cout << "Change damping [0.01] ? (y/n): ";
		char changeDamp;
		std::cin >> changeDamp;
		if (changeDamp == 'y' || changeDamp == 'Y') {
			double d;
			std::cout << "Enter new damping value: ";
			if (!(std::cin >> d)) {
				std::cerr << "Invalid input for damping. Expected a decimal value." << std::endl;
				return 1;
			}
			Pendulum::setGlobalDamping(d);
		}
	}

	if (cliCoupling >= 0.0) {
		Pendulum::setGlobalCoupling(cliCoupling);
	} else if (!noInteractive) {
		std::cout << "Change coupling [0.5] ? (y/n): ";
		char changeCoup;
		std::cin >> changeCoup;
		if (changeCoup == 'y' || changeCoup == 'Y') {
			double c;
			std::cout << "Enter new coupling value: ";
			if (!(std::cin >> c)) {
				std::cerr << "Invalid input for coupling. Expected a decimal value." << std::endl;
				return 1;
			}
			Pendulum::setGlobalCoupling(c);
		}
	}

	std::cout << "Starting simulation... Press Ctrl+C to exit." << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(2));

	CmdFX::Canvas::clearScreen();

	const double baseFrame = 0.016; // ~60 FPS
	const int baseFrameMicros = static_cast<int>(baseFrame * 1e6);
	while (true) {
		double dt = baseFrame * timeMultiplier * 3;

		// build vector of pointers for the chain
		std::vector<Pendulum*> chain;
		for (auto& p : pendulums) {
			chain.push_back(&p);
		}

		Pendulum::stepMultiPendulum(chain, gravity, dt);

		std::this_thread::sleep_for(std::chrono::microseconds(baseFrameMicros));
	}

	return 0;
}
