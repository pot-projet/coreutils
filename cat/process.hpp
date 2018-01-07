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

#ifndef COREUTILS_PROCESS_HPP
#define COREUTILS_PROCESS_HPP

#include <vector>
#include <string>

namespace coreutils {
    namespace cat {
        void compress_blank(std::vector<std::string>& lines);
        void line_number(std::vector<std::string>& lines);
        void line_number_ignore_blank(std::vector<std::string>& lines);
        void end_of_line(std::vector<std::string>& lines);
        void replace_char(std::vector<std::string>& lines, const std::regex& from, const std::string& to);
        void replace_tab(std::vector<std::string>& lines);
        void replace_control(std::vector<std::string>& lines);
    } /* cat */
} /* coreutils */

#endif //COREUTILS_PROCESS_HPP
