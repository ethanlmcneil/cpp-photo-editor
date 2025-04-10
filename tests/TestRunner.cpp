#include "TestRunner.h"

int TestRunner::failureCount = 0;

void TestRunner::runTest(const std::string& testName, std::function<void()> testFunction) {
    try {
        testFunction();
        std::cout << "[PASS] " << testName << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[FAIL] " << testName << " - " << e.what() << std::endl;
        failureCount++;
    } catch (...) {
        std::cerr << "[FAIL] " << testName << " - Unknown error occurred" << std::endl;
        failureCount++;
    }
}

int TestRunner::getFailureCount() {
    return failureCount;
}