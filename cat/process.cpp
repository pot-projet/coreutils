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

#include <vector>
#include <string>
#include <algorithm>
#include <regex>
#include <iomanip>
#include <cmath>
#include <sstream>

namespace coreutils {
    namespace cat {
        void compress_blank(std::vector<std::string>& lines) {
            constexpr char SEP[2][16]={"---------A5A5A5", "---------5A5A5A"};
            bool which=true;

            for(auto& line : lines) {
                if(!line.empty()) {
                    line+=SEP[which ? 0 : 1];
                    which=!which;
                }
            }
            lines.erase(std::unique(std::begin(lines), std::end(lines)), std::end(lines));

            for(auto& line : lines) {
                if(!line.empty()) {
                    line.resize(line.size()-(sizeof(SEP[0])/sizeof(SEP[0][0]))+1);
                }
            }
        }

        void line_number(std::vector<std::string>& lines) {
            auto digits_count=static_cast<std::size_t>(std::log10(lines.size()))+1;
            std::string format="%"+std::to_string(digits_count)+"ld";

            for(std::size_t i=0; i<lines.size(); ++i) {
                char number[32]={};
                std::sprintf(number, format.c_str(), i+1);
                lines[i]=std::string(number)+"  "+lines[i];
            }
        }

        void line_number_ignore_blank(std::vector<std::string>& lines) {
            auto digits_count=static_cast<std::size_t>(std::log10(lines.size()))+1;
            std::string format="%"+std::to_string(digits_count)+"ld";

            std::size_t num=1;
            for(auto& line : lines) {
                char number[32]={};
                std::sprintf(number, format.c_str(), num);

                if(!line.empty()) {
                    auto l=std::string(number);
                    l+="  ";
                    l+=line;
                    line=l;
                }

                ++num;
            }
        }

        void end_of_line(std::vector<std::string>& lines) {
            for(auto& line : lines) {
                line+="$";
            }
            lines.back().pop_back();
        }

        void replace_char(std::vector<std::string>& lines, const std::regex& from, const std::string& to) {
            for(auto& line : lines) {
                line=std::regex_replace(line, from, to);
            }
        }

        void replace_tab(std::vector<std::string>& lines) {
            static const std::regex TAB(R"(\t)");
            replace_char(lines, TAB, "^I");
        }

        void replace_control(std::vector<std::string>& lines) {
            const auto replace_characters=[&lines](unsigned char ch) {
                if((0x1f<ch && ch<0x7f) || ch=='\t')return;

                std::stringstream ss;
                ss<<"\\x"<<std::hex<<std::setw(2)<<std::setfill('0')<<static_cast<unsigned int>(ch);

                constexpr unsigned char ASCII_OVER=0x80;

                if(ch<=0x1f) {
                    const char CH=ch+'@';
                    const char to[]={'^', CH, '\0'};
                    replace_char(lines, std::regex(ss.str()), to);

                }else if(ch==0x7f) {
                    const char to[]="^?";
                    replace_char(lines, std::regex(ss.str()), to);

                }else if(ch<=(0x1f+ASCII_OVER)) {
                    const char CH=ch+'@'-ASCII_OVER;
                    const char to[]={'M', '-', '^', CH, '\0'};
                    replace_char(lines, std::regex(ss.str()), to);

                }else if(ch<0xff){
                    const char CH=ch-ASCII_OVER;
                    const char to[] = {'M', '-', CH, '\0'};
                    replace_char(lines, std::regex(ss.str()), to);

                }else{
                    const char to[]="M-^?";
                    replace_char(lines, std::regex(ss.str()), to);

                }
            };

            for(int c=0; c<=0xff; ++c) {
                replace_characters(static_cast<unsigned char>(c));
            }
        }
    } /* cat */
} /* coreutils */
