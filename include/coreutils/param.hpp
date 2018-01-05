//
// Created by Miya Moto on 2017/11/15.
//

#ifndef COREUTILS_PARAM_HPP
#define COREUTILS_PARAM_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <regex>
#include <sstream>

namespace coreutils {
    class parameter {
    private:
        constexpr bool PARAM_TYPE_FLAG=true, PARAM_TYPE_ARGS=false;
    private:
        std::vector<std::string> _params;

        using _defined_parameter_type=std::tuple<bool, std::vector<std::string>, std::string>;
        std::vector<_defined_parameter_type> _defined;
        std::string _version, _command_description, _command_name, _command_type;
    public:
        explicit parameter(std::string command_name, std::string command_type, std::string version, std::string command_description)
                : _version(std::move(version)),
                  _command_description(std::move(command_description)),
                  _command_name(std::move(command_name)),
                  _command_type(std::move(command_type)) {
            add_flag({"h", "help"}, "Show command help");
            add_flag({"v", "version"}, "Command version");
        }
        parameter(const parameter&)=default;
        parameter(parameter&&)=default;

        parameter& operator=(const parameter&)=default;
        parameter& operator=(parameter&&)=default;

        ~parameter()=default;
    public:
        std::string help() const {
            std::stringstream ss;
            ss<<_command_name<<" "<<_command_type<<'\n'
              <<_command_description<<'\n'
              <<"\n"<<std::endl;
            for(const auto& def : _defined) {
                ss<<"  ";
                const auto& params=std::get<1>(def);
                for(const auto& param : params) {
                    ss<<param<<" ";
                }
                if(std::get<0>(def)==PARAM_TYPE_ARGS) {
                    ss<<"= <value> ";
                }
                ss<<std::get<2>(def)<<std::endl;
            }
            return ss.str();
        }
        std::string version() const {
            return _version;
        }

    private:
        auto get_and_check_impl(const std::string& fp) {
            auto param_itr=std::find_if(std::begin(_defined), std::end(_defined), [&](const _defined_parameter_type& def) {
                const auto& flags=std::get<1>(def);
                return std::find(std::begin(flags), std::end(flags), fp)!=std::end(flags);
            });
            if(param_itr==std::end(_defined))return std::string::const_iterator();
            return std::find_if(std::begin(_params), std::end(_params), [&](const std::string& param) {
                const auto& p=std::get<1>(*param_itr);
                return std::find(std::begin(p), std::end(p), param)!=std::end(p);
            });
        }
    public:
        bool check(const std::string& flag) {
            return get_and_check_impl(flag)!=std::end(_params);
        }

        std::string get(const std::string& flag) {
            auto arg_itr=get_and_check_impl(flag);
        }

    public:
        void parse(int argc, char **argv) {
            std::string arg;
            std::regex ch_param(R"(-[^- ]+)");
            std::regex str_param(R"(--[^- ]+)");

            for(int i=0; i<argc; ++i) {
                arg=argv[i];
                if(std::regex_match(arg, ch_param)) {
                    for(auto itr=std::begin(arg)+1; itr!=std::end(arg); ++itr) {
                        _params.emplace_back(*itr);
                    }
                }else if(std::regex_match(arg, str_param)) {
                    auto equal_symbol=arg.find('=');
                    _params.emplace_back(arg.substr(2, equal_symbol));
                    if(equal_symbol!=std::string::npos) {
                        _params.emplace_back(arg.substr(equal_symbol));
                    }
                }
            }
        }

        parameter& add_flag(std::vector<std::string> flags, std::string description) {
            _defined_parameter_type content={PARAM_TYPE_FLAG, std::move(flags), std::move(description)};
            _defined.emplace_back(content);
            return *this;
        }
        parameter& add_argument(std::vector<std::string> args, std::string description) {
            _defined_parameter_type content={PARAM_TYPE_ARGS, std::move(args), std::move(description)};
            _defined.emplace_back(content);
            return *this;
        }
    };
} // coreutils

#endif //COREUTILS_PARAM_HPP
