// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Super simple auth manager.
#include <iostream>
#include <fstream>
#include <streambuf>
#include <string>
#include <vector>
#include "tyfirc-authmanager.h"
#include "tyfirc-misc.h"

namespace tyfirc {

namespace server {

FileAuthManager::FileAuthManager() {}

void FileAuthManager::Setup(std::string auth_filename) {
	std::ifstream stream(auth_filename);
	if (stream.fail())
		throw std::invalid_argument("Cannot open file");
	std::string auth_info_str((std::istreambuf_iterator<char>(stream)),
		std::istreambuf_iterator<char>());
	MapAuthInfoStr(auth_info_str);
	stream.close();
	auth_filename_ = auth_filename;
}

bool FileAuthManager::Login(const std::string& username, 
		const std::string& password) {
	try {
		return auth_info_.at(username).compare(password) == 0;
	}
	catch (std::out_of_range&) {
		return false;
	}
}

bool FileAuthManager::Register(const std::string& username, 
		const std::string& password) {
	try {
		auth_info_.at(username);	// if user doesn't exists throws out_of_range
		return false;
	}
	catch (std::out_of_range&) {
		if (!is_file_outdated_)
			is_file_outdated_ = true;
		auth_info_[username] = password;
	}
}

FileAuthManager::~FileAuthManager() {
	if (is_file_outdated_) {
		std::fstream stream(auth_filename_);
		if (stream.fail())
			return;
		for (auto& pair : auth_info_) {
			stream << pair.first << ' ' << pair.second << '\n';
		}
		stream.close();
	}
}

void FileAuthManager::MapAuthInfoStr(
		const std::string& auth_info_str) {
	std::vector<std::string> lines = tyfirc::internal::Split(auth_info_str, '\n');
	for (auto& line : lines) {
		size_t space_ind = line.find(' ');
		if (line.find(' ', space_ind + 1) != std::string::npos) {
			throw std::invalid_argument("Username and password"
					"shouldn't contain white spaces");
		}
		auth_info_[line.substr(0, space_ind)] = 
				line.substr(space_ind + 1, line.size() - space_ind - 1);
	}
}


}  // namespace server

}  // namespace tyfirc