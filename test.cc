#include <cstdio>
#include "test.hh"

namespace drift
{
    void test_runner_base::error(const std::string& msg)
    {
        rlutil::setColor(rlutil::RED);
        std::printf("Error: ");
        rlutil::resetColor();
        std::printf("%s\n", msg.c_str());
    }

    void test_runner_base::run()
    {
        for(auto& test : tests)
        {
            setup();
            test.fn(test);
            tear_down();
        }
    }

    void test_runner_base::test(
        const std::string& msg,
        std::function<void(unit_test&)> fn,
        const std::string& diag)
    {
        tests.emplace_back(fn, msg, diag);
    }
}