//
// Created by mateusz on 22.12.2025.
//

//
// Created by mateusz on 22.12.2025.
//


#include "actors/LogCollector.h"

#include <filesystem>
#include <gtest/gtest.h>
#include "../utils/test_name.h"
#include "../utils/run_once.h"
#include "ipcs/key.h"

namespace fs = std::filesystem;

TEST(LogCollector, ProcessControl) {
    MockQueue<Message> msq;
    Logger log(DEBUG, &msq);

    MessageQueue<Message> target_msq(make_key(LOGGING_DIR, "test", log), true);
    Logger target(DEBUG, &target_msq);

    // target logger should be at least of level WARN - to avoid test file pollution
    auto collector = std::make_unique<LogCollector>("test", log);
    ProcessController proc(std::move(collector), log, true, true);

    const ProcessStats *stats = proc.stats();

    // initialize with empty stats!
    ASSERT_EQ(CREATED, stats->state);
    ASSERT_EQ(0, stats->loops);
    ASSERT_EQ(0, stats->reloads);

    target.warn("TEST");
    run_once(proc);

    ASSERT_EQ(true, fs::is_directory("logging"));

    std::string logFilename;
    for (const auto& e : fs::directory_iterator("logging")) {
        auto name = e.path().filename().string();
        if (name.rfind("test", 0) == 0) {
            logFilename = name;
            break;
        }
    }
    ASSERT_NE("", logFilename);
    log.info("file: %s", logFilename.c_str());

    proc.stop();
    usleep(TICK);
    ASSERT_EQ(STOPPED, stats->state);

    auto file = std::ifstream("logging/" + logFilename);
    ASSERT_EQ(true, file.is_open());

    std::string logMessage;
    std::getline(file, logMessage);
    ASSERT_NE("", logMessage);
}
