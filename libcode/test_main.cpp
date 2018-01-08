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

#include <coreutils/code/convert.hpp>
#include <iostream>

int main() {
    const std::string str8=u8"abcあいう𠮷野家";
    const std::u16string str16=u"abcあいう𠮷野家";
    const std::u32string str32=U"abcあいう𠮷野家";

    std::cout<<std::boolalpha;

    namespace cvt=coreutils::codecvt;

    /* check utf8 -> utf16, utf32 */{
        std::u16string u16;
        cvt::utf16_from_utf8(u16, str8);
        std::cout<<"UTF-8 -> UTF-16: "<<(u16==str16)<<std::endl;

        std::u32string u32;
        cvt::utf32_from_utf8(u32, str8);
        std::cout<<"UTF-8 -> UTF-32: "<<(u32==str32)<<std::endl;
    }

    /* check utf16 -> utf8, utf32 */ {
        std::string u8;
        cvt::utf8_from_utf16(u8, str16);
        std::cout<<"UTF-16 -> UTF-8: "<<(u8==str8)<<std::endl;

        std::u32string u32;
        cvt::utf32_from_utf16(u32, str16);
        std::cout<<"UTF-16 -> UTF-32: "<<(u32==str32)<<std::endl;
    }

    /* check utf32 -> utf8, utf16 */ {
        std::string u8;
        cvt::utf8_from_utf32(u8, str32);
        std::cout<<"UTF-32 -> UTF-8: "<<(u8==str8)<<std::endl;

        std::u16string u16;
        cvt::utf16_from_utf32(u16, str32);
        std::cout<<"UTF-32 -> UTF-16: "<<(u16==str16)<<std::endl;
    }
}
