// (c)2020 Ruslan Shemietov
// This code is licensed under MIT license (see LICENSE.txt for details)

//
// Declares class for building controller and model environment.

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include "tyfirc-irclientapp.h"

namespace tyfirc {

bool IrcClientApp::Setup() {
	return true;
}

}  // namespace tyfirc