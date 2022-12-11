//
// Created by jinyang on 22-12-1.
//

#include <iostream>
#include "inc/DeviceTable.h"

namespace magic_sheep {
    void DeviceTable::modify_variables(std::string_view kind, int idle_increment, int no_idle_increment) {
        using std::fstream;
        std::lock_guard<std::mutex> lock_guard(read_write_mutex);
        fstream device_table;
        device_table.open(path, std::ios::out | std::ios::in | std::ios::ate);

        if (!device_table.is_open()) {
            device_table.open(path, std::ios::out | std::ios::in | std::ios::ate | std::ios::trunc);
        }

        std::string line_buffer;
        std::regex pattern{R"((\w+))"};
        int idle_devices{};
        int no_idle_devices{};
        device_table.seekg(0, std::ios::beg);
        while (device_table) {
            // 读取每行，分别读取 设备类型 可用设备数 已用设备数
            std::getline(device_table, line_buffer);
            if (line_buffer.empty()) continue;
            std::sregex_token_iterator begin{line_buffer.begin(),
                                             line_buffer.end(), pattern}, end;
            if (begin == end) {
                throw std::runtime_error("file parse error, please check the file in " + path);
            }
            if (begin->str() != kind) {
                if (device_table.eof()) {
                    line_buffer.clear();
                }
                continue;
            }
            ++begin;
            if (begin == end) {
                throw std::runtime_error("file parse error, please check the file in " + path);
            }
            idle_devices = std::stoi(begin->str()); // 发现新设备，空闲设备加一
            ++begin;
            if (begin == end) {
                throw std::runtime_error("file parse error, please check the file in " + path);
            }
            no_idle_devices = std::stoi(begin->str());
            break;
        }
        if (idle_devices + idle_increment < 0
            && no_idle_devices + no_idle_increment < 0) {
            throw std::runtime_error(
                    std::string{
                            "illegal modify the devices number, number must greater than zero: Before idle_devices("}
                    + std::to_string(idle_devices) + ") no_idle_devices("
                    + std::to_string(no_idle_devices) + "), After idle_devices("
                    + std::to_string(idle_devices + idle_increment) + ") no_idle_devices("
                    + std::to_string(no_idle_devices + no_idle_increment) + ")"
            );
        }
        if (device_table) {
            // 流未结束
            device_table.seekp(-static_cast<long>(line_buffer.length() + 1), std::ios::cur);
        } else {
            device_table.close();
            device_table.open(path, std::ios_base::out | std::ios_base::app);
        }
        device_table << kind << '\t'
                     << idle_devices + idle_increment << '\t'
                     << no_idle_devices + no_idle_increment << std::endl;
        device_table.flush();
        device_table.close();
    }

    void DeviceTable::init() {
        std::filesystem::path table_path{path};
        if (!std::filesystem::exists(table_path.parent_path()))
        {
            if (!std::filesystem::create_directories(table_path.parent_path())) {
                throw std::runtime_error("can't create director: ./etc, please check");
            }
        }
    }

    int DeviceTable::device_connect(std::string_view kind) {
        modify_variables(kind, 1, 0);
        return 0;
    }

    int DeviceTable::device_disconnect(std::string_view kind) {
        modify_variables(kind, -1, 0);
        return 0;
    }

    int DeviceTable::request_device(std::string_view kind) {
        modify_variables(kind, -1, 1);
        return 0;
    }

    int DeviceTable::release_device(std::string_view kind) {
        modify_variables(kind, 1, -1);
        return 0;
    }

    void DeviceTable::reformat_file() {
        using std::ifstream;
        using std::ofstream;

        ifstream old_file;
        ofstream new_file;

        old_file.open(path);
        if (!old_file) {
            throw std::runtime_error("not file: " + path);
        }
        new_file.open(path + ".tmp", std::ios_base::out | std::ios_base::app);
        if (!new_file) {
            throw std::runtime_error("not file: " + path + ".tmp");
        }

        std::string line_buffer;
        while (std::getline(old_file, line_buffer)) {
            if (line_buffer.empty() || std::regex_match(line_buffer, std::regex(R"(\s+)"))) {
                continue;
            }
            new_file << line_buffer << std::endl;
        }
        new_file.flush();
        new_file.close();
        old_file.close();

        std::filesystem::path old_name(path);
        remove(old_name);
        std::filesystem::path new_name(path + ".tmp");
        rename(new_name, old_name);
    }
} // magic_sheep