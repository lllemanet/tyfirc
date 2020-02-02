// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares abstract class for repository and implementor interface database
// repository.
#pragma once
#include <vector>
#include <functional>

namespace tyfirc {

namespace db {

template <typename T, typename Collection = std::vector<T>>
class IRepository {
public:
	virtual T ReadById() = 0;
	virtual Collection ReadAll() = 0;
	virtual Collection ReadWhere(std::function<bool(const T&)>) = 0;
	// Returns result of operation.
	virtual bool Insert(T t) = 0;
	virtual bool Update(T t) = 0;
	virtual bool Delete(T t) = 0;
	virtual bool DeleteById(int id) = 0;
	virtual IRepository() {};
};

struct DbUser {
	int userid;
	std::string username;
	std::string password;
};

template <typename Collection = std::vector<DbUser>>
class UsersRepository : public IRepository<DbUser, Collection> {
public:
	// Expects connection str to mysql server that contains database 'tyfirc'
	// with table 'users' with following structure:
	//
	// userid INT AUTO_INCREMENT PRIMARY KEY,
	// username VARCHAR(255) UNIQUE NOT NULL,
	// password VARCHAR(255) NOT NULL
	//
	// Throws invalid_argument if structure is not preserved.
	// Throws ConnectionFailException if connection cannot be established.
	UsersRepository(std::string connection_str);

	DbUser ReadById() override;
	Collection  ReadAll() override;
	Collection ReadWhere(std::function<bool(const DbUser&)>) override;
	// Returns result of operation.
	bool Insert(DbUser u) override;
	bool Update(DbUser u) override;
	bool Delete(DbUser u) override;
	bool DeleteById(int id) override;
};

}  // namespace db

}  // namespace tyfirc