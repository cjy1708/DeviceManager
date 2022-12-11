//
// Created by jinyang on 22-12-2.
//

#include "DeviceInfoTable.h"

namespace magic_sheep {
    void DeviceInfoTable::init() {
        std::filesystem::path table_path{path};
        if (!std::filesystem::exists(table_path.parent_path()))
        {
            if (!std::filesystem::create_directories(table_path.parent_path())) {
                throw std::runtime_error("can't create director: ./etc, please check");
            }
        }
    }

    int
    DeviceInfoTable::modify_append_table(std::string_view kind, std::string_view name,
            std::optional<uint64_t> physical_address, std::optional<pid_t> pid,
            std::optional<uint64_t> in_precess_virtual_address) {
        using std::fstream;
        std::lock_guard<std::mutex> lock_guard(read_write_mutex);
        fstream device_table;
        device_table.open(path, std::ios::out | std::ios::in | std::ios::ate);

        auto check_no_end_throw{[end = std::sregex_token_iterator()]
        (const std::sregex_token_iterator& begin){
            if (begin == end) {
                throw std::runtime_error("file parse error, please check the file in " + path);
            }
        }};

        if (!device_table.is_open()) {
            device_table.open(path, std::ios::out | std::ios::in | std::ios::ate | std::ios::trunc);
        }

        std::string line_buffer;
        std::regex pattern{R"((\w+))"};
        uint64_t physical_address_{physical_address.has_value() ? physical_address.value() : 0};
        pid_t pid_{ };
        uint64_t in_precess_virtual_address_{ };
        device_table.seekg(0, std::ios::beg);
        size_t blank_line{ };
        while (std::getline(device_table, line_buffer))
        {
            if (line_buffer.empty() || std::regex_match(line_buffer, std::regex(R"(\s+)"))) {
                ++blank_line;
                continue;
            }
            // 读取每行，分别读取 设备类型 可用设备数 已用设备数
            std::sregex_token_iterator begin{line_buffer.begin(), line_buffer.end(), pattern};
            // 先查找类型
            if (begin->str() != kind) {
                continue;
            }
            ++begin;
            check_no_end_throw(begin);
            // 在查找设备名称
            if (begin->str() != name) {
                continue;
            }
            ++begin;
            check_no_end_throw(begin);
            // 物理地址
            physical_address_ = physical_address_ != 0
                    ? physical_address_ : std::stoull(begin->str());
            ++begin;
            check_no_end_throw(begin);
            // 持有者pid

            pid_ = std::stoi(begin->str());
            // 如果pid_为0, 并且pid有值，更新；如果pid的值为零，也更新
            if (pid.has_value() && (pid_ == 0 || pid.value() == 0)) {
                pid_ = pid.value();
                in_precess_virtual_address_ = in_precess_virtual_address.value();
            } else if (pid.has_value() && pid.value() != 0 && pid_ != 0) {
                return -1;
            }
            ++begin;
            check_no_end_throw(begin);
            // 进程内虚拟地址

            in_precess_virtual_address_ = in_precess_virtual_address.has_value()
                    ? in_precess_virtual_address.value() : std::stoull(begin->str());

            break;
        }
        if (!device_table) {
            // 到了文件末尾
            device_table.close();
            device_table.open(path, std::ios_base::out | std::ios_base::app);
        } else {
            device_table.seekp(-static_cast<long>(line_buffer.length() + 1), std::ios_base::cur);
        }
        device_table << kind << '\t' << name << '\t'
        << physical_address_ << '\t' << pid_ << '\t' << in_precess_virtual_address_ << std::endl;

        device_table.flush();
        device_table.close();
        reformat_file();

        return 0;
    }

    std::vector<DeviceInfoTable::DeviceInfo> DeviceInfoTable::show_info() {
        using std::ifstream;
        std::vector<device_info> res{ };
        std::lock_guard<std::mutex> lock_guard(read_write_mutex);
        ifstream device_table;
        device_table.open(path);

        if (!device_table.is_open()) {
            return {};
        }

        auto check_no_end_throw{[end = std::sregex_token_iterator()]
                                        (const std::sregex_token_iterator& begin){
            if (begin == end) {
                throw std::runtime_error("file parse error, please check the file in " + path);
            }
        }};

        std::string line_buffer;
        std::regex pattern{R"((\w+))"};
        std::string kind;
        std::string name;
        uint64_t physical_address{ };
        pid_t pid{ };
        uint64_t in_precess_virtual_address{ };
        device_table.seekg(0, std::ios::beg);
        size_t blank_line{ };
        while (std::getline(device_table, line_buffer))
        {
            if (line_buffer.empty() || std::regex_match(line_buffer, std::regex(R"(\s+)"))) {
                ++blank_line;
                continue;
            }
            // 读取每行，分别读取 设备类型 可用设备数 已用设备数
            std::sregex_token_iterator begin{line_buffer.begin(), line_buffer.end(), pattern};
            // 先查找类型
            kind = begin->str();
            ++begin;
            check_no_end_throw(begin);
            // 在查找设备名称
            name = begin->str();
            ++begin;
            check_no_end_throw(begin);
            // 物理地址
            physical_address = std::stoull(begin->str());
            ++begin;
            check_no_end_throw(begin);
            // 持有者pid
            pid =  std::stoi(begin->str());
            ++begin;
            check_no_end_throw(begin);
            // 进程内虚拟地址
            in_precess_virtual_address = std::stoull(begin->str());

            res.emplace_back(kind, name, physical_address, pid, in_precess_virtual_address);
        }
        device_table.close();

        if (blank_line > 5) {
            reformat_file();
        }

        return res;
    }

    int DeviceInfoTable::allocate(std::string_view kind, std::string_view name,
                                  pid_t pid, uint64_t in_precess_virtual_address) {
        modify_append_table(kind, name, std::nullopt, pid, in_precess_virtual_address);
        return 0;
    }

    int DeviceInfoTable::release(std::string_view kind, std::string_view name) {
        modify_append_table(kind, name, std::nullopt, 0, 0);
        return 0;
    }

    int DeviceInfoTable::disconnect_device(std::string_view kind, std::string_view name) {
        using std::fstream;
        std::lock_guard<std::mutex> lock_guard(read_write_mutex);
        fstream device_table;
        device_table.open(path, std::ios::out | std::ios::in | std::ios::ate);

        auto check_no_end_throw{[end = std::sregex_token_iterator()]
        (const std::sregex_token_iterator& begin){
            if (begin == end) {
                throw std::runtime_error("file parse error, please check the file in " + path);
            }
        }};

        if (!device_table.is_open()) {
            return -1;
        }

        std::string line_buffer;
        std::regex pattern{R"((\w+))"};
        pid_t pid_{ };
        device_table.seekg(0, std::ios::beg);

        size_t blank_lines{ };
        while (std::getline(device_table, line_buffer))
        {
            if (line_buffer.empty() || std::regex_match(line_buffer, std::regex(R"(\s+)"))) {
                ++blank_lines;
                continue;
            }
            // 读取每行，分别读取 设备类型 可用设备数 已用设备数
            std::sregex_token_iterator begin{line_buffer.begin(), line_buffer.end(), pattern};
            // 先查找类型
            if (begin->str() != kind) {
                continue;
            }
            ++begin;
            check_no_end_throw(begin);
            // 在查找设备名称
            if (begin->str() != name) {
                continue;
            }
            ++begin;
            check_no_end_throw(begin);
            ++begin;
            check_no_end_throw(begin);
            // 持有者pid

            pid_ = std::stoi(begin->str());
            // 如果pid_为0, 并且pid有值，更新；如果pid的值为零，也更新
            if (pid_ != 0) {
                device_table.close();
                return -1;
            }
            break;
        }
        if (device_table) {
            device_table.seekp(-static_cast<long>(line_buffer.length() + 1), std::ios_base::cur);
            device_table << std::string(line_buffer.length(), ' ');
            device_table.flush();
        }

        device_table.close();

        if (blank_lines > 5) {
            reformat_file();
        }

        return 0;
    }

    int DeviceInfoTable::connect_device(std::string_view kind, std::string_view name, uint64_t physical_address) {
        modify_append_table(kind, name, physical_address, 0, 0);
        return 0;
    }

    void DeviceInfoTable::reformat_file() {
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
            if (line_buffer.empty()
                || !std::regex_match(line_buffer, std::regex(
                        R"(^\w+[ \t]\w+[ \t]\w+[ \t]\w+[ \t]\w+$)"
                        ))) {
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