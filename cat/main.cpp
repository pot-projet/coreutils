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

#include <iostream>

#include <coreutils/param.hpp>

#include "read.hpp"
#include "process.hpp"

int main(int argc, char **argv) {
    coreutils::parameter param("cat", "[OPTION]... [FILE]...", "1.0.0.0-dev", "conCATenate");
    param.add_flag({"-A", "--show-all"},         "Same with -vET");
    param.add_flag({"-b", "--number-nonblank"},  "Show line number to non-blank lines");
    param.add_flag({"-e"},                       "Same with -vE");
    param.add_flag({"-E", "--show-ends"},        "Display '$' symbol at end of line");
    param.add_flag({"-n", "--number"},           "Show line number");
    param.add_flag({"-s", "--squeeze-blank"},    "Compresses consecutive empty lines");
    param.add_flag({"-t", "-T", "--show-tabs"},  "Display TAB(\\t) as '^I'");
    param.add_flag({"-u"},                       "");
    param.add_flag({"-v", "--show-nonprinting"}, "Display not printable");

    param.parse(argc, argv);

    if(param.has_help()) {
        std::cout<<param.help()<<std::endl;
        return EXIT_SUCCESS;

    }else if(param.has_version()) {
        std::cout<<param.formed_version()<<std::endl;
        return EXIT_SUCCESS;
    }

    auto lines=coreutils::cat::read(param.arguments());
    coreutils::cat::compress_blank(lines);
    coreutils::cat::line_number(lines);

    for(const auto& line : lines) {
        std::cout<<line<<std::endl;
    }

    return EXIT_SUCCESS;
}
