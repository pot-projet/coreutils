/*
    Copyright 2017-2018 Pot Project.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

namespace coreutils{
    namespace cat {
        std::vector<std::string> read(const std::vector<std::string> &files) {
            std::string buf;
            bool is_stdin;

            std::vector<std::string> content;

            for(const auto &file : files) {
                is_stdin=(file=="/dev/stdin");
                std::ifstream fin(file);
                if (!is_stdin && !fin)return {};

                while(true) {
                    if(is_stdin) {
                        std::getline(std::cin, buf);
                    }else{
                        std::getline(fin, buf);
                    }

                    content.emplace_back(buf);
                    if(is_stdin) {
                        if(std::cin.eof())break;
                    }else{
                        if(fin.eof())break;
                    }
                }
            }
            return std::move(content);
        }
    } /* cat */
} /* coreutils */