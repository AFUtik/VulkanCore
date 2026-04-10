#include <iostream>
#include "Engine.hpp"

#include <string>

int main()
{
	Engine engine{};

	try {
		engine.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return 0;
}
