// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declaration of class that reads and saves ScMessages blocking thread.
#pragma once
#include "tyfirc-chatreader.h"

namespace tyfirc {

namespace client {

void ChatReader::ConnectHandler(ScMessageType type, void(*handler)(ScMessage)) {
	short type_ind = static_cast<short>(type);
	signals_[type_ind].connect(handler);
}

void ChatReader::ConnectHandlerOnDiscard(void(*handler)(std::string)) {
	discard_signal_.connect(handler);
}

void ChatReader::Run() {

}

}  // namespace client

}  // namespace tyfirc