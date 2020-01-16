// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Exceptions.
#pragma once
#include <stdexcept>

namespace tyfirc {

class ConnectionFailException : std::runtime_error {
 public:
	ConnectionFailException(std::string msg = "") : runtime_error(msg) {}
};

}	 // namespace tyfirc