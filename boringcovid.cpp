#include "boringcovid.h"

#include <string>
#include <chrono>
#include <QDebug>
#include <thread>

std::string address = "127.0.0.1";
int remote_port     = 5000;
std::string password = "stonejing";

void BoringCovid::StartServer()
{
    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        qDebug() << "output of startserver.";
    }
}

void BoringCovid::Test()
{
	qDebug() << "This is a test";
	return;
}
