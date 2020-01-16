// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declaration of server-client message class.
#pragma once
#include <map>

namespace tyfirc {

// Message types for server-client communication (see ScMessage below
// for more info.
enum class ScMessageType {
	LOGIN = 0,	//  message must contain 'username' and 'password' properties
	LOGIN_SUCCESS,
	LOGIN_FAILURE,
	REGISTER,  // message must contain 'username' and 'password' properties
	REGISTER_SUCCESS,
	REGISTER_FAILURE,
	END  // not message type
};

std::string ScMessageTypeToStr(ScMessageType type);
ScMessageType ScMessageTypeFromStr(const std::string& msg);

// Class provides wrapper for server-client message that is similar to
// http request/response message.
// 
// Server-Client message should contain its type on first line (ex. LOGIN, 
// REGISTER). Then you can specify any number of property-value pairs each
// on distinct line in next form:
// <property>: <value>
// Only first colon separate property from value, so you can use colons in your
// value.
// Property names shouldn't repeat.
// ScMessageType enum comments describe mandatory properties for each msg type.
//
// Example of message for logging in:
//
// LOGIN
// username: somename
// password: 12345678
//
// Sc in class name stands for server-client.
class ScMessage {
 public:
	ScMessage(ScMessageType type = ScMessageType::LOGIN) : msg_type_{type} {}

	// Creates ScMessage from string. Throws invalid_argument exception if
	// string is invalid.
	static ScMessage FromString(const std::string&);
	
	explicit operator std::string();

	void SetProperty(std::string property, std::string value) { 
		properties_[property] = value;
	}
	void SetType(ScMessageType type) {	msg_type_ = type;	}

 private:
	ScMessageType msg_type_;
	std::map<std::string, std::string> properties_;
};

}	 // namespace tyfirc