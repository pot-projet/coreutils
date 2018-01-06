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

#ifndef COREUTILS_PARAM_HPP
#define COREUTILS_PARAM_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <regex>
#include <sstream>
#include <ctime>

namespace coreutils {
    /**
     * Parameter operator
     */
    class parameter {
    private:
        const bool PARAM_TYPE_FLAG=true, PARAM_TYPE_ARGS=false;
    private:
        std::vector<std::string> _params, _arguments;

        using _defined_parameter_type=std::tuple<bool, std::vector<std::string>, std::string>;
        std::vector<_defined_parameter_type> _defined;
        std::string _version, _command_description, _command_name, _command_type, _execution_command;
    public:
        /**
         * Constructor
         * @param command_name Command name(eg. cat, ls).
         * @param command_type Command line description(eg. [OPTION]...)
         * @param version Command version
         * @param command_description Command description
         */
        explicit parameter(std::string command_name, std::string command_type, std::string version, std::string command_description)
                : _version(std::move(version)),
                  _command_description(std::move(command_description)),
                  _command_name(std::move(command_name)),
                  _command_type(std::move(command_type)) {
            add_flag({"--help"}, "Show command help");
            add_flag({"--version"}, "Command version");
        }
        parameter(const parameter&)=default;
        parameter(parameter&&)=default;

        parameter& operator=(const parameter&)=default;
        parameter& operator=(parameter&&)=default;

        ~parameter()=default;
    public:
        /**
         * Get help string.
         * @return help string
         */
        std::string help() const {
            std::stringstream ss;
            ss<<"Usage: "<<_command_name<<" "<<_command_type<<'\n'
              <<_command_description<<'\n'
              <<"\n";
            for(const auto& def : _defined) {
                const auto& params=std::get<1>(def);
                for(const auto& param : params) {
                    ss<<" "<<param;
                }
                if(std::get<0>(def)==PARAM_TYPE_ARGS) {
                    ss<<" = <value> ";
                }
                ss<<": "<<std::get<2>(def)<<std::endl;
            }
            ss<<"";
            return ss.str();
        }

        /**
         * Get version
         * @return version string
         */
        std::string version() const {
            return _version;
        }

        /**
         * Get formed version(Version, Copyright, License)
         * @return formed version string
         */
        std::string formed_version() const {
            std::stringstream ss;
            ss<<"Pot coreutils: "<<_command_name<<" "<<version()<<'\n'
              <<"(C) 2017-2018 Pot Project.\n"
              <<"License: Apache License 2.0 (https://opensource.org/licenses/Apache-2.0)\n";
            return ss.str();
        }

    private:
        auto _get_and_check_impl(const std::string &fp_)const {
            const auto remove_hyphen=[](const std::string& src) -> std::string {
                if(src=="--")return src;
                std::string removed=src;
                if(src[0]=='-') {
                    if(src[1]=='-') {
                        removed=src.substr(2);
                    }else{
                        removed=src.substr(1);
                    }
                }
                return removed;
            };
            std::string fp=remove_hyphen(fp_);

            auto param_itr=std::find_if(std::begin(_defined), std::end(_defined), [&fp, &remove_hyphen](const _defined_parameter_type& def) {
                const auto& flags=std::get<1>(def);
                return std::find_if(std::begin(flags), std::end(flags), [&fp, &remove_hyphen](const std::string& flag){
                    return remove_hyphen(flag)==fp;
                })!=std::end(flags);
            });
            if(param_itr==std::end(_defined))return std::end(_params);
            return std::find_if(std::begin(_params), std::end(_params), [&param_itr, &remove_hyphen](const std::string& param) {
                const auto& p=std::get<1>(*param_itr);
                return std::find_if(std::begin(p), std::end(p), [&param, &remove_hyphen](const std::string& pp){
                    return remove_hyphen(pp)==param;
                })!=std::end(p);
            });
        }
    public:
        /**
         * Check parameter has flag
         * @param flag search string
         * @return has flag
         */
        bool check(const std::string& flag)const {
            return _get_and_check_impl(flag)!=std::end(_params);
        }

        bool has_help()const {
            return check("-h");
        }
        bool has_version()const {
            return check("-v");
        }

        std::string get(const std::string& flag)const {
            auto arg_itr= _get_and_check_impl(flag);
        }

        const std::vector<std::string>& arguments()const noexcept {
            return _arguments;
        }

        const std::string& execution_command() const noexcept {
            return _execution_command;
        }

    public:
        void parse(int argc, char **argv) {
            std::string arg;
            bool two_hyphen=false;
            std::regex ch_param(R"(-[^- ]+)");
            std::regex str_param(R"(--[^- ]+)");

            _execution_command=argv[0];

            for(int i=1; i<argc; ++i) {
                arg=argv[i];
                if(!two_hyphen && std::regex_match(arg, ch_param)) {
                    for(auto itr=std::begin(arg)+1; itr!=std::end(arg); ++itr) {
                        _params.emplace_back(1, *itr);
                    }

                }else if(!two_hyphen && std::regex_match(arg, str_param)) {
                    auto equal_symbol = arg.find('=');
                    _params.emplace_back(arg.substr(2, equal_symbol));
                    if (equal_symbol != std::string::npos) {
                        _params.emplace_back(arg.substr(equal_symbol));
                    }

                }else{
                    if(!two_hyphen && arg=="--") {
                        two_hyphen=true;

                    }else{
                        _arguments.emplace_back(arg);
                    }
                }
            }
        }

        parameter& add_flag(const std::vector<std::string>& flags, const std::string& description) {
            _defined_parameter_type content{parameter::PARAM_TYPE_FLAG, flags, description};
            _defined.emplace_back(content);
            return *this;
        }
        parameter& add_argument(const std::vector<std::string>& args, const std::string& description) {
            _defined_parameter_type content{parameter::PARAM_TYPE_ARGS, args, description};
            _defined.emplace_back(content);
            return *this;
        }
    };
} // coreutils

#endif //COREUTILS_PARAM_HPP
