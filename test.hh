#include <string>
#include <sstream>
#include <vector>
#include <functional>
namespace drift
{
    class test_runner_base
    {
        static void error(const std::string&);
        static void pass(const std::string&);
        static void warn(const std::string&);
    public:
        static unit_test
        {
            unit_test() = default;
            unit_test(std::function<test_sig> fn, const std::string& msg, const std::string& diag):
                test(fn), message(msg), diagnostic(diag) { }
            
            template<class T> virtual void eq(T l, T r, const std::string& msg = "")
            {
                if (l != r) {
                    ++assertions_failed;
                    std::ostringstream ss("");
                    ss << msg << " : " <<  l << " not equivalent to " << r;
                    error(ss.str());
                }
                ++assertions_run;
            }
        private:
            std::function<void(unit_test&)> fn;
            std::size_t assertions_run = 0ULL;
            std::size_t assertions_failed = 0ULL;
            std::string message = "";
            std::string diagnostic = "";
        }

        void run();
        void test(const std::string& msg, std::function<void(unit_test&)>, const std::string& = "");
    private:
        virtual void setup() = 0;
        virtual void tear_down() = 0;
    private:
        std::vector<unit_test> tests;
    };
}