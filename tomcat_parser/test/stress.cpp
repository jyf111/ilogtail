#include <chrono>
#include <iostream>

#include "parser.h"
#include "random_log.h"
#include "regex_parser.h"

int main() {
    tomcat_parser::Parser parser;
    tomcat_parser::RegexParser regex_parser;
    tomcat_parser::RandomLog random_log;
    for (const auto& format :
         {"%h %l %u %t \"%r\" %s %B",
          "%h %l %u \"%r\" %s %B \"%{Referer}i\" \"%{User-Agent}i\""}) // the generator for %b has a bug
    {
        const int LOG_NUM = 10000;
        const int LOG_MIN_LEN = 50;
        std::vector<std::string> input;
        input.reserve(LOG_NUM);
        size_t tot_len = 0;
        for (int i = 0; i < LOG_NUM; i++) {
            input.push_back(random_log.GenLog(format, LOG_MIN_LEN));
            tot_len += input.back().size();
        }
        std::cerr << "Average log length: " << tot_len / LOG_NUM << '\n';

        std::cerr << "Parser process:\n";
        std::vector<tomcat_parser::Log> output1;
        {
            auto begin = std::chrono::high_resolution_clock::now();
            parser.Process(format, input, output1);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
            std::cout << "Parser used " << duration.count() << " ms" << std::endl;
        }

        std::cerr << "Regex parser process:\n";
        std::vector<tomcat_parser::Log> output2;
        {
            auto begin = std::chrono::high_resolution_clock::now();
            regex_parser.Process(format, input, output2);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
            std::cout << "Regex parser used " << duration.count() << " ms" << std::endl;
        }
        for (size_t i = 0; i < LOG_NUM; i++) {
            const auto& content1 = output1[i].Contents();
            const auto& content2 = output2[i].Contents();
            bool equal = content1.size() == content2.size();
            for (size_t j = 0; j < content1.size() && equal; j++) {
                equal &= (content1[j].Key() == content2[j].Key()) && ((content1[j].Value() == content2[j].Value()));
            }
            if (!equal) {
                std::cerr << "Unequal log: " << input[i] << '\n';
                std::cerr << "Parser result:\n";
                for (size_t j = 0; j < content1.size(); j++) {
                    std::cerr << content1[j].Key() << ": " << content1[j].Value() << '\n';
                }
                std::cerr << "Regex parser result:\n";
                for (size_t j = 0; j < content2.size(); j++) {
                    std::cerr << content2[j].Key() << ": " << content2[j].Value() << '\n';
                }
            }
        }
    }
    return 0;
}