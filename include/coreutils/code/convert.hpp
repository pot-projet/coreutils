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

#ifndef COREUTILS_CODE_CONVERT_HPP
#define COREUTILS_CODE_CONVERT_HPP

#include <string>

namespace coreutils {
    namespace codecvt {
        bool utf8_from_utf32(std::string& dest, const std::u32string& src);
        bool utf32_from_utf8(std::u32string& dest, const std::string& src);
        bool utf16_from_utf32(std::u16string& dest, const std::u32string& src);
        bool utf32_from_utf16(std::u32string& dest, const std::u16string& src);
        bool utf16_from_utf8(std::u16string& dest, const std::string& src);
        bool utf8_from_utf16(std::string& dest, const std::u16string& src);
    } /* codecvt */
} /* coreutils */


#endif //COREUTILS_CODE_CONVERT_HPP
