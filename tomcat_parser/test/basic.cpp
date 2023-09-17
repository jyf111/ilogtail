#include <chrono>
#include <iostream>

#include "parser.h"
#include "regex_parser.h"

void Run(const std::string& format, const std::vector<std::string>& input) {
    std::cerr << "Parser process:\n";
    tomcat_parser::Parser parser;
    std::vector<tomcat_parser::Log> output;
    {
        auto begin = std::chrono::high_resolution_clock::now();
        parser.Process(format, input, output);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        std::cout << "Parser used " << duration.count() << " ns" << std::endl;
    }

    std::cout << std::endl;
    for (const auto& log : output) {
        for (const auto& content : log.Contents()) {
            std::cout << content.Key() << ": " << content.Value() << '\n';
        }
        std::cout << std::endl;
    }

    std::cerr << "Regex parser process:\n";
    tomcat_parser::RegexParser regex_parser;
    output.clear();
    {
        auto begin = std::chrono::high_resolution_clock::now();
        regex_parser.Process(format, input, output);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        std::cout << "Regex parser used " << duration.count() << " ns\n" << std::endl;
    }
}

int main() {
    {
        const std::string format = "%h %l %u %t \"%r\" %s %b";
        std::vector<std::string> input = {
            R"(192.168.1.1 - user2 [11/Sep/2023:12:00:00 +0000] "POST /page2" 200 2048)",
            R"(10.0.0.1 - user1 [11/Sep/2023:11:00:00 +0000] "DELETE /page2" 404 1024)",
            R"(172.16.0.1 - - [11/Sep/2023:10:00:00 +0000] "PUT /page3" 404 4096)",
            R"(192.168.1.1 - user1 [17/Sep/2023:10:00:00 +0000] "GET /page1" 500 2048)",
            R"(10.0.0.1 - - [17/Sep/2023:11:00:00 +0000] "POST /page3" 200 1024)",
        };

        Run(format, input);
    }
    {
        const std::string format = "%h %l %u \"%r\" %s %b \"%{Referer}i\" \"%{User-Agent}i\"";
        std::vector<std::string> input = {
            R"(192.168.1.1 - user1 "GET /page2" 404 2048 "http://example.com/page1" "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36")",
            R"(10.0.0.1 - - "POST /page1" 200 1024 "http://example.com/page3" "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36")",
            R"(172.16.0.1 - user2 "PUT /page3" 200 4096 "http://example.com/page2" "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36")",
            R"(192.168.1.1 - - "DELETE /page2" 404 2048 "http://example.com/page1" "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36")",
            R"(10.0.0.1 - - "GET /page1" 500 1024 "http://example.com/page2" "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.61 Safari/537.36")",
        };

        Run(format, input);
    }
    return 0;
}