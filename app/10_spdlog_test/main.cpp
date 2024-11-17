#include <string>
#include <iostream>
#include <filesystem>

#include <unistd.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <spdlog/sinks/udp_sink.h>
#include <spdlog/sinks/tcp_sink.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

using namespace std;
using namespace std::filesystem;

int main(int argc,char *argv[]){
    spdlog::stopwatch sw;
    spdlog::debug("Elapsed {}", sw);
    std::filesystem::path path="/root/Linux/app/10_spdlog_test/build/test";
    if(std::filesystem::exists(path)){
        std::cout<<"exists.\n";
    }else{
        std::cout<<"not exists.\n";
        std::filesystem::create_directory(path);
    }
    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);

    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
    spdlog::info("Support for floats {:03.2f}", 1.23456);
    spdlog::info("Positional args are {1} {0}..", "too", "supported");
    spdlog::info("{:<30}", "left aligned");

    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
    spdlog::debug("This message should be displayed..");
     // change log pattern
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

    // create a color multi-threaded logger
    auto console = spdlog::stdout_color_mt("console");
    auto err_logger = spdlog::stderr_color_mt("stderr");
    spdlog::get("console")->info("loggers can be retrieved from a global registry using the spdlog::get(logger_name)");
    auto logger = spdlog::daily_logger_mt("daily_logger", "daily.txt", 19, 48);
    auto max_size = 1024 * 1024 * 100;
    auto max_files = 10;
    auto rotating_logger = spdlog::rotating_logger_mt("some_logger_name", "rotating.txt", max_size, max_files);
    auto callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>([](const spdlog::details::log_msg &msg) {
        //for example you can be notified by sending an email to yourself
        std::string message(msg.payload.begin(), msg.payload.end());
        spdlog::info("received:"+message);
    });
    callback_sink->set_level(spdlog::level::warn);

    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    spdlog::logger logger_callback("custom_callback_logger", {console_sink, callback_sink});

    logger_callback.info("some info log");
    logger_callback.error("critical issue"); // will notify you

    spdlog::sinks::udp_sink_config cfg("127.0.0.1", 8888);
    auto my_logger = spdlog::udp_logger_mt("udplog", cfg);
    my_logger->set_level(spdlog::level::debug);
    return 0;
}