#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <algorithm>
#include <map>
#include <regex>
#include <utility>
#include <unordered_map>
#include <vector>
#include <memory>
#include <fstream>
#include <sys/stat.h>
#include <cctype>
#include <experimental/string_view>
#include <set>
#include <cstdint>
#include <sstream>
#include <cstdio>
#include <filesystem>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <any>
#include "crc32c/crc32c.h"

using namespace std::filesystem;
namespace po = boost::program_options;
namespace Utils {

    std::string readAllText(const std::string& path) {
        std::ifstream infile(path);
        if (infile.fail()) {
            throw std::invalid_argument{ "Unable to read file at path: '" + path + "'" };
        }
        infile.ignore(std::numeric_limits<std::streamsize>::max());
        std::streamsize size = infile.gcount();
        infile.clear();
        infile.seekg(0, std::ifstream::beg);
        std::string contents(size, ' ');
        infile.read(&contents[0], size);
        return contents;
    }


    void writeAllText(const std::string& path, const std::string& contents) {
        std::ofstream outfile(path);
        if (outfile.fail()) {
            throw std::invalid_argument{ "Unable to write file at path: '" + path + "'" };
        }
        outfile << contents;
    }
    static std::vector<char> readAllBytes(const std::string& filename)
    {
        std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
        std::ifstream::pos_type pos = ifs.tellg();

        if (pos == 0) {
            return std::vector<char>{};
        }

        std::vector<char>  result(pos);

        ifs.seekg(0, std::ios::beg);
        ifs.read(&result[0], pos);

        return result;
    }
}

using namespace Utils;

std::string ssystem (const char *command) {
    char tmpname [L_tmpnam];
    std::tmpnam ( tmpname );
    std::string scommand = command;
    std::string cmd = scommand + " >> " + tmpname;
    std::system(cmd.c_str());
    std::ifstream file(tmpname, std::ios::in | std::ios::binary );
    std::string result;
    if (file) {
        while (!file.eof()) result.push_back(file.get());
        file.close();
    }
    remove(tmpname);
    return result;
}
// int flag = {1 -- for md5, 2 -- for sha};
static int flag = 3;
std::string algo(const std::string& pathToFile){
      return crc32c::Crc32c(pathToFile);
}



int main(int argc, char** argv) {
    if (argc == 1) {
            std::cerr << "No files provided and/or options\n";
            exit(1);
    }

    po::options_description description("Usage");
    description.add_options()
            ("help", "NO ONE CAN HELP YOU HAHAHA")
            ("version", "1.4.8.7")
            ("file", po::value<std::string>(), "Writing to this file")
            ("read", po::value<std::vector<std::string>>(), "Checking these files' cksums");
    po::positional_options_description p;
    p.add("read", -1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help")){
        std::cout << description << std::endl;
    }
    if (vm.count("version")){
        std::cout << "1.4.8.7" << std::endl;
    }
    if (vm.count("crc32")){
        flag = 3;
    }


    if (vm.count("file")){
        std::string filename = vm["file"].as<std::string>();
        std::string filename_path = "./" + filename;
        if (!std::filesystem::exists(filename_path)){
            std::cout << "FILE DIDN'T EXIST. CREATING ONE!\n";
            FILE * file_ptr = fopen(filename.c_str(), "w");
            fclose(file_ptr);
        }
        if (std::filesystem::exists(filename_path) && !std::filesystem::is_directory(filename_path)) {
            std::ofstream out(filename_path, std::ofstream::out);
            if (out.is_open()) {
                for (const auto &x: std::filesystem::directory_iterator("./")) {
                    if (!std::filesystem::is_directory(x.path().string())) {
                        out << x.path().string().substr(2, x.path().string().size() - 1) << " : " << algo(x.path().string()) << "\n";
                    }
                }
            }
            out.close();
            std::cout << filename << " was successfully filled\n\n";
    }
}

    if (vm.count("read")){
        for (const auto& x : vm["read"].as<std::vector<std::string>>()){
            if (!std::filesystem::is_directory("./" + x) && x != "cksum.ini")
                std::cout << x << " : " << algo("./" + x) << std::endl;
        }
    }

}
