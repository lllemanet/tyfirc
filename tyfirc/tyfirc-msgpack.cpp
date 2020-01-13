// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Definition of message structs.
#pragma once
#include <vector>
#include <algorithm>
#include "tyfirc-msgpack.h"

namespace tyfirc {
MessagePack::MessagePack(std::vector<Message> data) : data_(data) {
	if (data_.size() > 1) {
		auto comparator_by_date = [](Message& msg1, Message& msg2) { return msg1.time < msg2.time; };
		std::sort(data_.begin(), data_.end(),
				[](const Message& msg1, const Message& msg2) { return msg1.time < msg2.time; });
	}
}

Message MessagePack::Delete(int index) {
	if (index < 0 || index > data_.size() - 1)
		return Message{};
	
	Message res{ data_[index] };
	data_.erase(data_.begin() + index);
	return res;
}

void MessagePack::Insert(MessagePack new_msgs) {
	if (new_msgs.size() == 0) return;

	if (data_.size() == 0) {
		data_ = new_msgs.data_;
		return;
	}

	if (new_msgs.size() == 1) {
		auto new_msg = new_msgs[0];
		auto next_msg = std::find_if(data_.cbegin(), data_.cend(),
				[new_msg](const Message& cur) { return new_msg.time < cur.time; });
		data_.insert(next_msg, new_msg);
	}
	else {
		// TODO better to create BeginTime and EndTime methods
		auto msgs_begin_time = data_.begin()->time;
		auto msgs_end_time = (data_.end() - 1)->time;
		auto new_msgs_begin_time = new_msgs.data_.begin()->time;
		auto new_msgs_end_time = (new_msgs.data_.end() - 1)->time;

		if (new_msgs_begin_time >= msgs_end_time)	//if after this msgs
			data_.insert(data_.end(), new_msgs.data_.begin(), new_msgs.data_.end());
		else if (new_msgs_end_time <= msgs_begin_time)	// if before this msgs
			data_.insert(data_.begin(), new_msgs.data_.begin(), new_msgs.data_.end());
		else {
			std::vector<Message> merged_collection{};
			std::merge(data_.begin(), data_.end(), new_msgs.data_.begin(),
					new_msgs.data_.end(), std::back_inserter(merged_collection),
					[](const Message& msg1, const Message& msg2) { return msg1.time < msg2.time; });
			data_ = merged_collection;
		}
	}
}

}	//namespace tyfirc