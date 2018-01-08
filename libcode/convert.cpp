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

#include <array>

namespace {
    inline bool utf32_to_utf8_impl(const char32_t u32, std::array<char, 4>& u8) {
        if (u32<0 || u32>0x10FFFF) {
            return false;
        }

        if(u32<=0x7f) {
            u8[0]=char(u32);
            u8[1]=0;
            u8[2]=0;
            u8[3]=0;
        }else if(u32<=0x7ff) {
            u8[0]=0xC0 | static_cast<char>(u32 >> 6);
            u8[1]=0x80 | (static_cast<char>(u32) & 0x3F);
            u8[2]=0;
            u8[3]=0;
        }else if(u32<=0xffff) {
            u8[0]=0xE0 | static_cast<char>(u32 >> 12);
            u8[1]=0x80 | (static_cast<char>(u32 >> 6) & 0x3F);
            u8[2]=0x80 | (static_cast<char>(u32) & 0x3F);
            u8[3]=0;
        }else{
            u8[0]=0xF0 | static_cast<char>(u32 >> 18);
            u8[1]=0x80 | (static_cast<char>(u32 >> 12) & 0x3F);
            u8[2]=0x80 | (static_cast<char>(u32 >> 6) & 0x3F);
            u8[3]=0x80 | (static_cast<char>(u32) & 0x3F);
        }

        return true;
    }

    int get_utf8_byte_count(char _ch)noexcept {
        const auto ch= static_cast<std::uint8_t>(_ch);
        if(0<=ch && ch<0x80) {
            return 1;
        }
        if(0xC2<=ch && ch<0xE0) {
            return 2;
        }
        if(0xE0<=ch && ch<0xF0) {
            return 3;
        }
        if(0xF0<=ch && ch<0xF8) {
            return 4;
        }
        return 0;
    }

    bool is_utf8_later_byte(char _ch) {
        const auto ch= static_cast<std::uint8_t>(_ch);
        return 0x80<=ch && ch<0xC0;
    }

    bool utf8_to_utf32_impl(const std::array<char, 4>& u8, char32_t& u32) {
        int num_bytes=get_utf8_byte_count(u8[0]);

        if (num_bytes == 0) {
            return false;
        }
        switch (num_bytes) {
            case 1:
                u32 = char32_t(std::uint8_t(u8[0]));
                break;
            case 2:
                if (!is_utf8_later_byte(u8[1])) {
                    return false;
                }
                if ((std::uint8_t(u8[0]) & 0x1E) == 0) {
                    return false;
                }

                u32 = char32_t(u8[0] & 0x1F) << 6;
                u32 |= char32_t(u8[1] & 0x3F);
                break;
            case 3:
                if (!is_utf8_later_byte(u8[1]) || !is_utf8_later_byte(u8[2])) {
                    return false;
                }
                if ((std::uint8_t(u8[0]) & 0x0F) == 0 &&
                    (std::uint8_t(u8[1]) & 0x20) == 0) {
                    return false;
                }

                u32 = char32_t(u8[0] & 0x0F) << 12;
                u32 |= char32_t(u8[1] & 0x3F) << 6;
                u32 |= char32_t(u8[2] & 0x3F);
                break;
            case 4:
                if (!is_utf8_later_byte(u8[1]) || !is_utf8_later_byte(u8[2]) ||
                    !is_utf8_later_byte(u8[3])) {
                    return false;
                }
                if ((std::uint8_t(u8[0]) & 0x07) == 0 &&
                    (std::uint8_t(u8[1]) & 0x30) == 0) {
                    return false;
                }

                u32 = char32_t(u8[0] & 0x07) << 18;
                u32 |= char32_t(u8[1] & 0x3F) << 12;
                u32 |= char32_t(u8[2] & 0x3F) << 6;
                u32 |= char32_t(u8[3] & 0x3F);
                break;
            default:
                return false;
        }

        return true;
    }

    bool utf32_to_utf16_impl(const char32_t u32, std::array<char16_t, 2>& u16) {
        if(u32<0 || u32>0x10FFFF) {
            return false;
        }

        if(u32<0x10000) {
            u16[0]=char16_t(u32);
            u16[1]=0;
        } else {
            u16[0]=char16_t((u32 - 0x10000) / 0x400 + 0xD800);
            u16[1]=char16_t((u32 - 0x10000) % 0x400 + 0xDC00);
        }

        return true;
    }

    bool is_utf16_high_surrogate(char16_t ch) {
        return 0xd800<=ch && ch<0xdc00;
    }
    bool is_utf16_low_surrogate(char16_t ch) {
        return 0xDC00<=ch && ch<0xE000;
    }

    bool utf16_to_utf32_impl(const std::array<char16_t, 2>& u16, char32_t& u32) {
        if (is_utf16_high_surrogate(u16[0])) {
            if (is_utf16_low_surrogate(u16[1])) {
                u32=0x10000 + (char32_t(u16[0]) - 0xD800) * 0x400 + (char32_t(u16[1]) - 0xDC00);
            } else if (u16[1] == 0) {
                u32 = u16[0];
            } else {
                return false;
            }
        } else if (is_utf16_low_surrogate(u16[0])) {
            if (u16[1] == 0) {
                u32 = u16[0];
            } else {
                return false;
            }
        } else {
            u32 = u16[0];
        }

        return true;
    }
} /* anonymous */

namespace coreutils {
    namespace codecvt {
        bool utf8_from_utf32(std::string& dest, const std::u32string& src) {
            std::array<char, 4> u8={};

            dest.clear();
            dest.reserve(src.size()*4);

            for(const auto& s : src) {
                if(!utf32_to_utf8_impl(s, u8))return false;
                for(const auto& u8_ : u8) {
                    if(u8_!=0)dest.push_back(u8_);
                }
            }
            return true;
        }

        bool utf32_from_utf8(std::u32string& dest, const std::string& src) {
            std::array<char, 4> u8={};

            dest.clear();
            dest.reserve(src.size());

            int size;
            for(std::size_t i=0; i<src.size();) {
                size=get_utf8_byte_count(src[i]);

                for(std::size_t n=0; n<u8.size(); ++n) {
                    u8[n]=src[i+n];
                }

                char32_t u32=U'\0';

                if(!utf8_to_utf32_impl(u8, u32))return false;
                dest.push_back(u32);
                i+=size;
            }
            return true;
        }

        bool utf16_from_utf32(std::u16string& dest, const std::u32string& src) {
            std::array<char16_t , 2> u16={};

            dest.clear();
            dest.reserve(src.size()*4);

            for(const auto& s : src) {
                if(!utf32_to_utf16_impl(s, u16))return false;
                for(const auto& u16_ : u16) {
                    if(u16_!=0)dest.push_back(u16_);
                }
            }
            return true;
        }

        bool utf32_from_utf16(std::u32string& dest, const std::u16string& src) {
            std::array<char16_t , 2> u16={};

            dest.clear();
            dest.reserve(src.size());

            for(std::size_t i=0; i<src.size();) {
                u16[0]=src[i];
                if(is_utf16_high_surrogate(src[i++])) {
                    u16[1]=src[i++];
                }

                char32_t u32=U'\0';

                if(!utf16_to_utf32_impl(u16, u32))return false;
                dest.push_back(u32);
            }
            return true;
        }

        bool utf16_from_utf8(std::u16string& dest, const std::string& src) {
            std::u32string u32;

            return utf32_from_utf8(u32, src) && utf16_from_utf32(dest, u32);
        }

        bool utf8_from_utf16(std::string& dest, const std::u16string& src) {
            std::u32string u32;

            return utf32_from_utf16(u32, src) && utf8_from_utf32(dest, u32);
        }

        bool ascii_from_ebcdic(std::string& dest_ebcdic, const std::string& src_ascii) {
            static constexpr unsigned char TABLE[]={
                    //00    01    02    03    04    05    06    07    08    09    0a    0b    0c    0d    0e    0f
                    0x00, 0x01, 0x02, 0x03, 0xec, 0x09, 0xca, 0x7f, 0xe2, 0xd2, 0xd3, 0x0b, 0x0c, 0x0d, 0x0e, 0xa9,// 0
                    0x10, 0x11, 0x12, 0x13, 0xef, 0xc5, 0x08, 0xcb, 0x18, 0x19, 0xdc, 0xd8, 0x1c, 0x1d, 0x1e, 0x1f,// 1
                    0xb7, 0xb8, 0xb9, 0xbb, 0xc4, 0x0a, 0x17, 0x1b, 0xcc, 0xcd, 0xcf, 0xd0, 0xd1, 0x05, 0x06, 0x07,// 2
                    0xd9, 0xda, 0x16, 0xdd, 0xde, 0xdf, 0xe0, 0x04, 0xe3, 0xe5, 0xe9, 0xeb, 0xb0, 0xb1, 0x9e, 0x1a,// 3
                    0x20, 0xc9, 0x83, 0x84, 0x85, 0xa0, 0xf2, 0x86, 0x87, 0xa4, 0xd5, 0x2e, 0x3c, 0x28, 0x2b, 0xb3,// 4
                    0x26, 0x82, 0x88, 0x89, 0x8a, 0xa1, 0x8c, 0x8b, 0x8d, 0xe1, 0x21, 0x24, 0x2a, 0x29, 0x3b, 0x5e,// 5
                    0x2d, 0x2f, 0xb2, 0x8e, 0xb4, 0xb5, 0xb6, 0x8f, 0x80, 0xa5, 0x7c, 0x2c, 0x25, 0x5f, 0x3e, 0x3f,// 6
                    0xba, 0x90, 0xbc, 0xbd, 0xbe, 0xf3, 0xc0, 0xc1, 0xc2, 0x60, 0x3a, 0x23, 0x40, 0x27, 0x3d, 0x22,// 7
                    0xc3, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0xae, 0xaf, 0xc6, 0xc7, 0xc8, 0xf1,// 8
                    0xf8, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0xa6, 0xa7, 0x91, 0xce, 0x92, 0x0f,// 9
                    0xe6, 0x7e, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0xad, 0xa8, 0xd4, 0x5b, 0xd6, 0xd7,// a
                    0x9b, 0x9c, 0x9d, 0xfa, 0x9f, 0x15, 0x14, 0xac, 0xab, 0xfc, 0xaa, 0xfe, 0xe4, 0x5d, 0xbf, 0xe7,// b
                    0x7b, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0xe8, 0x93, 0x94, 0x95, 0xa2, 0xed,// c
                    0x7d, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0xee, 0x96, 0x81, 0x97, 0xa3, 0x98,// d
                    0x5c, 0xf0, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0xfd, 0xf5, 0x99, 0xf7, 0xf6, 0xf9,// e
                    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xdb, 0xfb, 0x9a, 0xf4, 0xea, 0xff // f
            };
            dest_ebcdic.clear();
            dest_ebcdic.reserve(src_ascii.size());

            for(const auto& ascii : src_ascii) {
                dest_ebcdic.push_back(TABLE[ascii]);
            }

            return true;
        }
    } /* codecvt */
} /* coreutils */
