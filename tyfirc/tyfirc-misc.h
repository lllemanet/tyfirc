// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Miscallenous functions.
#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>



namespace tyfirc {

namespace internal {
std::vector<std::string> Split(const std::string& src, char delimeter) {
	std::vector<std::string> tokens{};
	std::istringstream token_stream{src};
	std::string token;
	while (std::getline(token_stream, token, delimeter)) {
		tokens.push_back(token);
	}
	return tokens;
}

}  //namespace internal

}  // namespace tyfirc