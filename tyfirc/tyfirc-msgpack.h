// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares messages structs
#pragma once

#include <vector>
#include <string>
#include <chrono>


namespace tyfirc {

// Temp
struct User {
	std::string username;
};

struct Message {
	User user;
	std::string text;
	std::chrono::system_clock::time_point time;
};

// Lambda for comparing two msgs by date.
// I could be static member BUT this solution is more concise.
//auto comparator_by_date = [](const Message& msg1, const Message& msg2) { return msg1.time < msg2.time; };


// Collection of message. 
// Guarantees that messages are sorted in ascending order by date.
//
// We don't use priority queue because we often need to access all 
// elements by order.
//
// We don't provide iterators because we're lazy for the moment.
class MessagePack {
 public:
	// Ctors
	MessagePack() {}
	explicit MessagePack(std::vector<Message> msgs);

	// Inserts argument message copies into this collection. 
	// Sorted order is preserved.
	void Insert(MessagePack msgs);

	// Deletes msg with specified index and returns its copy.
	// Does nothing and returns default Message if index is invalid.
	Message Delete(int index);

	// Index must be in bound otherwise throws out_of_range exception.
	const Message& operator[](int index) {
		return msgs_[index];
	}

	int size() const { return msgs_.size(); }
	const std::vector<Message> msgs() const { return msgs_; }
private:
	std::vector<Message> msgs_;
};
}  // namespace tyfirc