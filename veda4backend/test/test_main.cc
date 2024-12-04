#define DROGON_TEST_MAIN
#include <unistd.h>
#include <drogon/drogon_test.h>
#include <drogon/drogon.h>


int main(int argc, char** argv) 
{
    // jenkins test3
    using namespace drogon;

    std::promise<void> p1;
    std::future<void> f1 = p1.get_future();

    //Load config file
    //drogon::app().loadConfigFile("../config.json");


    // Start the main loop on another thread
    std::thread thr([&]() {
        // Queues the promise to be fulfilled after starting the loop
        drogon::app().addListener("127.0.0.1", 5555);
        drogon::app().loadConfigFile("../../config.yaml");
        app().getLoop()->queueInLoop([&p1]() { p1.set_value(); });
        app().run();

    });

    // The future is only satisfied after the event loop started
    f1.get();
    int status = test::run(argc, argv);

    // Ask the event loop to shutdown and wait
    app().getLoop()->queueInLoop([]() { app().quit(); });
    thr.join();
    return status;
}
