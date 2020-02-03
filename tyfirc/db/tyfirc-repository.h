// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares abstract class for repository and implementor interface database
// repository.
#pragma once
#include <vector>
#include <functional>
#include <initializer_list>
#include <mysqlx/xdevapi.h>

#include <iostream>

#define CATCH_AND_WRAP_IN_REPO_EXCEPTION	  \
	catch (const std::exception& e) {					\
		throw RepositoryException(e);						\
	}

namespace tyfirc {

namespace db {

// Always contains inner exception.
struct RepositoryException : std::exception {
	RepositoryException(const std::exception& inner_ex) 
			: std::exception(inner_ex) {}
};

// Used to access elements of some repository.
// Throws RepositoryException that always contains inner
// exception which contains the source of error.
template <typename T, typename Collection = std::vector<T>>
class IRepository {
public:
	virtual T ReadById(int id) = 0;
	virtual Collection ReadAll() = 0;
	virtual Collection ReadWhere(std::function<bool(const T&)>) = 0;
	// Returns id of new created object.
	virtual int Insert(T t) = 0;
	virtual void Update(T t) = 0;
	virtual void Delete(T t) = 0;
	virtual void DeleteById(int id) = 0;
	virtual ~IRepository() {};
};

struct DbUser {
	int userid;
	std::string username;
	std::string password;
};

// Collection must contain method push_back.
// More info about structure of db is in ctor comment.
template <typename Collection = std::vector<DbUser>>
class DbUsersRepository : public IRepository<DbUser, Collection> {
public:
	// Expects connection str to mysql server that contains database 'tyfirc'
	// with table 'users' with following structure:
	//
	// userid INT AUTO_INCREMENT PRIMARY KEY,
	// username VARCHAR(255) UNIQUE NOT NULL,
	// password VARCHAR(255) NOT NULL
	//
	// To create such database use tyfirc-db-init.sql file in this directory.
	//
	// If it is not possible to create a valid session or to USE tyfirc, errors
	// are thrown from constructor.
	DbUsersRepository(std::string con_str) : session_{ con_str } {
		// TODO: static_assert Collection is collection
		mysqlx::SqlResult res = session_.sql("USE tyfirc").execute();
	}
	
	// Returns DbUser specified by id. If user is not found, default
	// DbUser is returned.
	DbUser ReadById(int id) override {
		try {
			mysqlx::SqlResult sql_res = session_.sql(
				"SELECT * "
				"FROM users "
				"WHERE userid = ?;")
				.bind(id).execute();

			if (!sql_res.hasData())
				return DbUser{ 0, nullptr, nullptr };

			auto row = sql_res.fetchOne();
			DbUser user;
			user.userid = id;
			user.username = static_cast<std::string>(row.get(1));
			user.password = static_cast<std::string>(row.get(2));
			return user;
		}
		CATCH_AND_WRAP_IN_REPO_EXCEPTION
	}

	Collection ReadAll() override {
		try {
			mysqlx::SqlResult sql_res = session_.sql(
				"SELECT * "
				"FROM users;")
				.execute();

			if (!sql_res.hasData())
				return Collection{};

			Collection res{};
			for (auto row : sql_res.fetchAll()) {
				DbUser user;
				user.userid = static_cast<int>(row.get(0));
				user.username = static_cast<std::string>(row.get(1));
				user.password = static_cast<std::string>(row.get(2));
				res.push_back(user);
			}
			return res;
		}
		CATCH_AND_WRAP_IN_REPO_EXCEPTION
	}

	// Highly unoptimized.
	Collection ReadWhere(std::function<bool(const DbUser&)> condition) override {
		// Don't wrap two times
		Collection all;
		try {
			all = ReadAll();
		}
		catch (const RepositoryException& exc) {
			throw;
		}

		try {
			Collection res{};
			std::for_each(all.cbegin(), all.cend(),
					[&res, &condition](const DbUser& user) {
					if (condition(user))
						res.push_back(user);
			});
			return res;
		}
		CATCH_AND_WRAP_IN_REPO_EXCEPTION
	}

	// Returns result of operation.
	int Insert(DbUser u) override {
		try {
			session_.sql(
					"INSERT INTO users(userid, username, password) "
					"VALUES(?, ?, ?); ")
					.bind(u.userid, u.username, u.password).execute();
			auto new_id_res = session_.sql("SELECT @@identity AS id;").execute();
			return static_cast<int>(new_id_res.fetchOne().get(0));
		}
		CATCH_AND_WRAP_IN_REPO_EXCEPTION
	}

	void Update(DbUser u) override {
		try {
			session_.sql(
					"UPDATE users "
					"SET username = ?, password = ? "
					"WHERE userid = ?")
					.bind(u.username, u.password, u.userid).execute();
		}
		CATCH_AND_WRAP_IN_REPO_EXCEPTION
	}

	void Delete(DbUser u) override {
		try {
			session_.sql(
					"DELETE FROM users "
					"WHERE userid = ?;")
					.bind(u.userid).execute();
		}
		CATCH_AND_WRAP_IN_REPO_EXCEPTION
	}

	void DeleteById(int id) override {
		try {
			session_.sql(
				"DELETE FROM users "
				"WHERE userid = ?;")
				.bind(id).execute();
		}
		CATCH_AND_WRAP_IN_REPO_EXCEPTION
	}

private:
	mysqlx::Session session_;
};

}  // namespace db

}  // namespace tyfirc