// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Used to track current users in room.
#include "tyfirc-chatroom.h"

namespace tyfirc {

namespace server {

void tyfirc::server::ChatRoom::DeliverScMessage(const ScMessage& msg) {
	for (auto& session : sessions_) {
		session->WriteScMessage(msg);
	}
}

void ChatRoom::AddSession(std::shared_ptr<Session> session) {
	sessions_.insert(session);
}

void ChatRoom::RemoveSession(std::shared_ptr<Session> session) {
	sessions_.erase(session);
}

}  // namespace server

}  // namespace tyfirc
