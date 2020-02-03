#include <iostream>
#include <string>
#include <thread>
#include "tyfirc-ircserverapp.h"
#include "tyfirc-authmanager.h"
#include "db/tyfirc-repository.h"


int main() {
	using namespace tyfirc;

	db::DbUsersRepository<> repo{ "mysqlx://tyfirc_server:password@localhost" };
	db::DbUser usr = repo.ReadById(3);
	auto res = repo.ReadWhere([](const db::DbUser& u)
		{ return u.userid == 3 || u.userid == 1; });
	db::DbUser u{ 0, "username1449", "password" };

	u.userid = repo.Insert(u);
	u.username = "username1450";
	repo.Update(u);
	repo.Delete(u);


	auto auth_manager(std::make_shared<server::FileAuthManager>());
	auth_manager->Setup("passwords");

	server::IrcServerApp app(8001, auth_manager);
	app.StartAccept();
	app.Run();
}