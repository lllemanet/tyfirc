// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Defines server-client message class methods.
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include "tyfirc-scmessage.h"
#include "tyfirc-misc.h"


namespace {
// One-to-one correspond to ScMessageType order.
std::string sc_types_strings[] = {
			"LOGIN",
			"LOGIN_SUCCESS",
			"LOGIN_FAILURE",
			"REGISTER",
			"REGISTER_SUCCESS",
			"REGISTER_FAILURE",
			"END"
};

// Expects line to be in form '<property>: <value>'. Invalid argument
// exception is thrown otherwise. 
void ParseProperty(const std::string& line, std::string& property,
		std::string& value) {
	size_t col_ind = line.find(':');	//colon index
	if (!(line[col_ind + 1] == ' ' && line[col_ind + 2] != '\n')) {
		throw std::invalid_argument(
				"Property line must be in form '<property>: <value>'");
	}

	property = line.substr(0, col_ind);
	value = line.substr(col_ind + 2);
}

}  // namespace

namespace tyfirc {

constexpr size_t sc_message_buff_size = 1024;

std::string ScMessageTypeToStr(ScMessageType type) {
	return sc_types_strings[static_cast<int>(type)];
}

// If msg is invalid and we reach end, then method will return 
// ScMessageType::END.
ScMessageType ScMessageTypeFromStr(const std::string& msg) {
	auto start = sc_types_strings;
	auto end = sc_types_strings + sc_types_size;
	auto iter = std::find(start, end, msg);

	int ind = iter - start;
	return static_cast<ScMessageType>(ind);
}


std::string ScMessage::ToString() {
	std::string res = ScMessageTypeToStr(msg_type_) + "\n";
	for (auto& a : properties_) {
		res += a.first + ": " + a.second + "\n";
	}
	return res;
}

ScMessage ScMessage::FromString(const std::string& sc_msg) {
	ScMessage res_msg{};
	std::vector<std::string> lines{internal::Split(sc_msg, '\n')};

	// Get type.
	ScMessageType msg_type = ScMessageTypeFromStr(lines[0]);
	if (msg_type == ScMessageType::END) {
		throw std::invalid_argument("Invlid message type.");
	}
	res_msg.SetType(msg_type);

	// Get properties.
	lines.erase(lines.begin());
	for (auto& line : lines) {
		std::string property;
		std::string value;
		ParseProperty(line, property, value);
		res_msg.SetProperty(property, value);
	}

	return res_msg;
}

namespace client {

ScMessage AuthScMessage(ScMessageType type, std::string username,
		std::string password) {
	if (type != ScMessageType::LOGIN && type != ScMessageType::REGISTER)
		throw std::invalid_argument("Auth type must be LOGIN or REGISTER");
	ScMessage res{ type };
	res.SetProperty("username", username);
	res.SetProperty("password", password);
	return res;
}

}  // namespace client

}	 // namespace tyfirc