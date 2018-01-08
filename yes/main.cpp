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
#include <vector>
#include <thread>

#include <coreutils/param.hpp>


int main(int argc, char **argv) {
    coreutils::parameter param("yes", "", "1.0.0.0-dev", "");

    param.parse(argc, argv);

    if(param.has_help()) {
        std::cout<<param.help()<<std::endl;
        return EXIT_SUCCESS;

    }else if(param.has_version()) {
        std::cout<<param.formed_version()<<std::endl;
        return EXIT_SUCCESS;
    }

    std::string say="y";
    if(!param.arguments().empty()) {
        say=param.arguments()[0];
    }

    const char * const _say=say.c_str();

    while(true) {
        puts(_say);
    }

    return EXIT_SUCCESS;
}
