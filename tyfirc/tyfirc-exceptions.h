#pragma once
#include <stdexcept>

namespace tyfirc {

class ConnectionFailException : std::runtime_error {
	ConnectionFailException(std::string msg) : runtime_error(msg) {}
};

}	 // namespace tyfirc