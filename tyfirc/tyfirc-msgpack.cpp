// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares class for building controller and model environment.
#pragma once
#include <vector>
#include <algorithm>
#include "tyfirc-msgpack.h"

namespace tyfirc {
MessagePack::MessagePack(std::vector<Message> msgs) : msgs_(msgs) {
	if (msgs_.size() > 1) {
		auto comparator_by_date = [](Message& msg1, Message& msg2) { return msg1.time < msg2.time; };
		std::sort(msgs_.begin(), msgs_.end(),
				[](const Message& msg1, const Message& msg2) { return msg1.time < msg2.time; });
	}
}

Message MessagePack::Delete(int index) {
	if (index < 0 || index > msgs_.size() - 1)
		return Message{};
	
	Message res{ msgs_[index] };
	msgs_.erase(msgs_.begin() + index);
	return res;
}

void MessagePack::Insert(MessagePack new_msgs) {
	if (new_msgs.size() == 0) return;
	if (msgs_.size() == 0) {
		msgs_ = new_msgs.msgs_;
	}

	if (new_msgs.size() == 1) {
		auto new_msg = new_msgs[0];
		auto next_msg = std::find_if(msgs_.cbegin(), msgs_.cend(),
				[new_msg](const Message& cur) { return new_msg.time < cur.time; });
		msgs_.insert(next_msg, new_msg);
	}
	else {
		auto msgs_begin_time = msgs_.begin()->time;
		auto msgs_end_time = (msgs_.end() - 1)->time;
		auto new_msgs_begin_time = new_msgs.msgs_.begin()->time;
		auto new_msgs_end_time = (new_msgs.msgs_.end() - 1)->time;

		if (new_msgs_begin_time >= msgs_end_time)	//if after this msgs
			msgs_.insert(msgs_.end(), new_msgs.msgs_.begin(), new_msgs.msgs_.end());
		else if (new_msgs_end_time <= msgs_begin_time)	// if before this msgs
			msgs_.insert(msgs_.begin(), new_msgs.msgs_.begin(), new_msgs.msgs_.end());
		else {
			std::vector<Message> merged_collection{};
			std::merge(msgs_.begin(), msgs_.end(), new_msgs.msgs_.begin(),
					new_msgs.msgs_.end(), std::back_inserter(merged_collection),
					[](const Message& msg1, const Message& msg2) { return msg1.time < msg2.time; });
			msgs_ = merged_collection;
		}
	}
}

}	//namespace tyfirc