// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Provides vector with thread-safe simple add and delete operations.
#pragma once
#include <memory>
#include <list>
#include <boost/signals2.hpp>
#include "tyfirc-msgpack.h"
#include "tyfirc-session.h"

namespace tyfirc {

namespace server {

class SessionList {
 public:
	using SessionConPair =
		std::pair<std::shared_ptr<Session>, boost::signals2::connection>;
	using iterator = typename std::list<SessionConPair>::iterator;

 public:
	// Acquire mutex.
	void AddSession(std::shared_ptr<Session> new_session, 
									std::function<void (const Message&)>);
	// Deletes specified session and return next element. Acquire sessions_mutex_.
	iterator CleanSession(iterator);

	const std::list<SessionConPair>& sessions() { return sessions_; }
	iterator begin() { return sessions_.begin(); }
	iterator end() { return sessions_.end(); }
private:
	std::list<SessionConPair> sessions_;
	std::mutex sessions_mutex_;
};

}  // namespace server

}  // namespace tyfirc
