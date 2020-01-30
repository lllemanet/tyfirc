// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Used to track current users in room.
#pragma once
#include <set>
#include "tyfirc-scmessage.h"
#include "tyfirc-session.h"

namespace tyfirc {

namespace server {
class Session;

class ChatRoom {
public:
	void DeliverScMessage(const ScMessage& msg);

	void AddSession(std::shared_ptr<Session> session);
	void RemoveSession(std::shared_ptr<Session> session);

private:
	std::set<std::shared_ptr<Session>> sessions_;
};

}  // namespace server

}  // namespace tyfirc