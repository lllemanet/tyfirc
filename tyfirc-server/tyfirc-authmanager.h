// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Super simple auth manager.
#pragma once
#include <string>
#include <map>

namespace tyfirc {

namespace server {

// Interface for auth manager.
class IAuthManager {
 public:
	virtual bool Login(const std::string& username, const std::string& password)=0;
	virtual bool Register(const std::string& username, const std::string& password)=0;
};

// Opens and reads auth file to string on initializiaion. Next, you work with
// local info. On destructor overwrites original file with local info.
class FileAuthManager : public IAuthManager {
 public:
	FileAuthManager();

	// filepath - path to file with username-password pairs.
	// Reads and safe auth info.
	// Throw invelid_argument if file is invalid.
	void Setup(std::string filepath);
	bool Login(const std::string& username, const std::string& password) override;
	bool Register(const std::string& username, const std::string& password) override;
	~FileAuthManager();	// Overwrites file with new values.
 private:
	void MapAuthInfoStr(const std::string&);

	std::string auth_filename_;
	bool is_file_outdated_ = false;	// should file be overwritten?
	std::map<std::string, std::string> auth_info_;	// username-password pairs
};

}  // namespace server

}  // namespace tyfirc