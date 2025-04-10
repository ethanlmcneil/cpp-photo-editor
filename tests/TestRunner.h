#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <iostream>
#include <functional>

class TestRunner {
private:
    static int failureCount;

public:
    static void runTest(const std::string& testName, std::function<void()> testFunction);
    static int getFailureCount();
};

#endif // TEST_RUNNER_H