// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declaration of message structs.
#pragma once
#include <vector>
#include <string>
#include <chrono>


namespace tyfirc {

struct Message {
	std::string username;
	std::string text;
	std::chrono::system_clock::time_point time;

	static const std::string time_format;
	static std::string Serialize(const Message&);
	// Throw invalid_argument if string is invalid. It must be in form:
	// <username>\n<date>\n<text>\0
	// where date should satisfy Message::time_format format.
	// Text can contain \n symbols
	static Message Deserialize(const std::string&);
};

// Collection of message. 
// Guarantees that messages are sorted in ascending order by date.
//
// We don't use priority queue because we often need to access all 
// elements sequentially.
// Iterators will be made in never release.
class MessagePack {
 public:
	MessagePack() {}
	explicit MessagePack(std::vector<Message> data);

	// Inserts argument message copies into this collection. 
	// Sorted order is preserved.
	void Insert(MessagePack data);

	// Deletes msg with specified index and returns its copy.
	// Does nothing and returns default Message if index is invalid.
	Message Delete(int index);

	// Index must be in bound otherwise throws out_of_range exception.
	const Message& operator[](int index) {
		return data_[index];
	}

	int size() const { return data_.size(); }
	const std::vector<Message> data() const { return data_; }
private:
	std::vector<Message> data_;
};

}  // namespace tyfirc