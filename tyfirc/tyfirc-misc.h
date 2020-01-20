// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Miscallenous functions.
#pragma once
#include <string>
#include <chrono>
#include <vector>

namespace tyfirc {

namespace internal {
std::vector<std::string> Split(const std::string& src, char delimeter);

std::string CurTimeStr(const std::string& format);

std::string TimePointToStr(const std::chrono::system_clock::time_point& point,
		std::string format);
}  //namespace internal

}  // namespace tyfirc