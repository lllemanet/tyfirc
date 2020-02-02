// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares abstract class for repository and implementor interface database
// repository.
#pragma once
#include <vector>
#include <functional>
#include "tyfirc-repository.h"

namespace tyfirc {

namespace db {
template<typename Collection>
UsersRepository<Collection>::UsersRepository(std::string connection_str) {
}

template<typename Collection>
DbUser UsersRepository<Collection>::ReadById() {
	return DbUser();
}

template<typename Collection>
Collection UsersRepository<Collection>::ReadAll() {
	return Collection();
}

template<typename Collection>
Collection UsersRepository<Collection>::ReadWhere(
		std::function<bool(const DbUser&)>) {
	return Collection();
}

template<typename Collection>
bool UsersRepository<Collection>::Insert(DbUser u) {
	return false;
}

template<typename Collection>
bool UsersRepository<Collection>::Update(DbUser u) {
	return false;
}

template<typename Collection>
bool UsersRepository<Collection>::Delete(DbUser u) {
	return false;
}

template<typename Collection>
bool UsersRepository<Collection>::DeleteById(int id) {
	return false;
}

}	 // namespace db

}  // namespace tyfirc