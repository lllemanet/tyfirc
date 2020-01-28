// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Miscallenous functions.
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <type_traits>
#include "tyfirc-misc.h"

namespace tyfirc {

namespace internal {

std::vector<std::string> Split(const std::string& src, char delimeter) {
	std::vector<std::string> tokens{};
	std::istringstream token_stream{ src };
	std::string token;
	while (std::getline(token_stream, token, delimeter)) {
		tokens.push_back(token);
	}
	return tokens;
}


std::string CurTimeStr(const std::string& format) {
	std::time_t now = std::chrono::system_clock::to_time_t(
		std::chrono::system_clock::now());
	std::tm tm;
	::localtime_s(&tm, &now);

	std::string res(30, '\0');
	std::strftime(&res[0], res.size(), format.c_str(), &tm);
	return res;
}

std::string TimePointToStr(const std::chrono::system_clock::time_point& point,
	std::string format) {
	std::time_t tt = std::chrono::system_clock::to_time_t(point);
	std::tm tm;
	::localtime_s(&tm, &tt);
	std::string res(20, '\0');
	std::strftime(&res[0], res.size(), format.c_str(), &tm);
	return res;
}

std::chrono::system_clock::time_point TimePointFromStr(const std::string& str,
		const std::string& format) {
	std::istringstream iss{ str };
	std::tm tm{};
	if (!(iss >> std::get_time(&tm, format.c_str())))
		throw std::invalid_argument("get_time");
	std::chrono::system_clock::time_point timePoint
			{ std::chrono::seconds(std::mktime(&tm)) };
	if (iss.eof())
		return timePoint;
	double zz;
	if (iss.peek() != '.' || !(iss >> zz))
		throw std::invalid_argument("decimal");
	using hr_clock = std::chrono::high_resolution_clock;
	std::size_t zeconds = zz * hr_clock::period::den / hr_clock::period::num;
	return timePoint;	// += hr_clock::duration(zeconds); TODO doesn't work
	
}

}  //namespace internal

}  // namespace tyfirc