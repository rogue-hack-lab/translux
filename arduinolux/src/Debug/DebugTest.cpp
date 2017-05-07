#include"../StringConverter.h"
#include <string>
#include <iostream>

int main() {
	std::string testString = "Hello there world!";
	do {
		char* convertedString = StringConverter::ExtractCharArrayFromString(testString);

		std::cout << convertedString << "\n";

		std::cin >> testString;
	} while (testString != "");
}