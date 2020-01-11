#include <iostream>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

void HelloWorld() {
	std::cout << "Hello world" << std::endl;
}

int main() {
	boost::signals2::signal<void()> sig;
	sig.connect(&HelloWorld);
	
	sig();
}