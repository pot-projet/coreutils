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

namespace {
    struct param_info {
        bool number_noblank=false, show_ends=false, number=false, squeeze_blank=false,
                show_tabs=false, show_nonprinting=false, new_line=false;
    };
    void decode_params(const coreutils::parameter& param, param_info& pi) {
        if(param.has("-b")) {
            pi.number_noblank=true;
        }
        if(param.has("-n")) {
            if(pi.number_noblank) {
                std::clog<<"-b and -n are specified at the same time.\n"
                         <<"  -b takes precedence."
                         <<std::endl;
            }
            pi.number=true;
        }
        if(param.has("-s")) {
            pi.squeeze_blank=true;
        }
        if(param.has("-N")) {
            pi.new_line=true;
        }

        if(param.has("-A")) {
            pi.show_ends=true;
            pi.show_tabs=true;
            pi.show_nonprinting=true;
        }else{
            if(param.has("-e")) {
                pi.show_ends=true;
                pi.show_nonprinting=true;
            }
            if(param.has("-t")) {
                pi.show_tabs=true;
                pi.show_nonprinting=true;
            }

            if(param.has("-E")) {
                pi.show_ends=true;
            }
            if(param.has("-T")) {
                pi.show_tabs=true;
            }
            if(param.has("-v")) {
                pi.show_nonprinting=true;
            }
        }
    }
} /* anonymous */

int main(int argc, char **argv) {
    coreutils::parameter param("cat", "[OPTION]... [FILE]...", "1.0.0.0-dev", "conCATenate FILEs to standard output");
    param.add_flag({"-A", "--show-all"},         "Same with -vET");
    param.add_flag({"-b", "--number-nonblank"},  "Show line number to non-blank lines");
    param.add_flag({"-e"},                       "Same with -vE");
    param.add_flag({"-E", "--show-ends"},        "Display '$' symbol at end of line");
    param.add_flag({"-n", "--number"},           "Show line number");
    param.add_flag({"-s", "--squeeze-blank"},    "Compresses consecutive empty lines");
    param.add_flag({"-t"},                       "Same with -vT");
    param.add_flag({"-T", "--show-tabs"},        "Display TAB(\\t) as '^I'");
    param.add_flag({"-u"},                       "");
    param.add_flag({"-v", "--show-nonprinting"}, "Display not printable");
    param.add_flag({"-N", "--new-line"},         "New line in end of file");

    param.parse(argc, argv);

    if(param.has_help()) {
        std::cout<<param.help()<<std::endl;
        return EXIT_SUCCESS;

    }else if(param.has_version()) {
        std::cout<<param.formed_version()<<std::endl;
        return EXIT_SUCCESS;
    }

    param_info pi;
    decode_params(param, pi);

    auto lines=coreutils::cat::read(param.arguments());
    if(pi.squeeze_blank) {
        coreutils::cat::compress_blank(lines);
    }

    if(pi.number_noblank) {
        coreutils::cat::line_number_ignore_blank(lines);
    }else if(pi.number) {
        coreutils::cat::line_number(lines);
    }

    if(pi.show_ends) {
        coreutils::cat::end_of_line(lines);
    }

    if(pi.show_tabs) {
        coreutils::cat::replace_tab(lines);
    }

    if(pi.show_nonprinting) {
        coreutils::cat::replace_control(lines);
    }

    for(std::size_t i=0; i<lines.size()-1; ++i) {
        std::cout<<lines[i]<<std::endl;
    }
    std::cout<<lines.back();
    if(pi.new_line) {
        std::cout<<'\n';
    }
    std::cout<<std::flush;

    return EXIT_SUCCESS;
}
