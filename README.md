# Description
Super simple irc from very scratch. Not completed yet (waiting for big thorough refactoring, interaction with user, etc.).

Technologies used: 
- boost::signals2 for events, boost::asio for networking
- openssl for establishing ssl connection
- Visual Studio 2017 and nmake for build

# Build
To build project you need to specify boost and openssl (earlier 1.1.0) include and static lib directories in nmake/makefile file and run makefile in this directory.