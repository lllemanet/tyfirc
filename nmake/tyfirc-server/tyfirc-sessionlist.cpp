// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Provides vector with thread-safe simple add and delete operations.
#include <list>
#include "tyfirc-sessionlist.h"
#include "tyfirc-misc.h"


namespace tyfirc {

namespace server {

void SessionList::AddSession(std::shared_ptr<Session> new_session, 
		std::function<void(const Message&)> f) {
	sessions_mutex_.lock();
	auto connection = new_session->BindOnMessage(f);
	sessions_.emplace_back(new_session, connection);
	sessions_mutex_.unlock();
}

typename std::list<SessionList::SessionConPair>::iterator
SessionList::CleanSession(
		typename std::list<SessionConPair>::iterator deleted) {
	sessions_mutex_.lock();
	auto next = internal::next(deleted);
	deleted->second.disconnect();
	sessions_.erase(deleted);
	sessions_mutex_.unlock();
	return next;
}

}  // namespace server

}  // namespace tyfirc