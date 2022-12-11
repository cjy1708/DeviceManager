//
// Created by jinyang on 22-12-2.
//

#ifndef DEVICEMANAGER_DEVICEINFOTABLE_H
#define DEVICEMANAGER_DEVICEINFOTABLE_H

#include <string>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <regex>
#include <optional>

namespace magic_sheep {

    /**
     * (kind)\t(name)\t(physical_address)\t(pid)\t(in_precess_virtual_address)
     */
    class DeviceInfoTable {
    private:
        inline static const std::string path{"./etc/device_info_table"};
        inline static std::mutex read_write_mutex{ };

        /**
         * 修改或者添加表格信息
         * @param kind 设备类型
         * @param name 设备名称
         * @param physical_address 物理地址
         * @param pid 使用者pid
         * @param in_precess_virtual_address 进程中虚拟地址
         * @return
         */
        static int
        modify_append_table(std::string_view kind,
                            std::string_view name,
                            std::optional<uint64_t> physical_address = std::nullopt,
                            std::optional<pid_t> pid = std::nullopt,
                            std::optional<uint64_t> in_precess_virtual_address = std::nullopt);
        static void
        reformat_file();

    public:
        using device_info = struct DeviceInfo{
            std::string_view kind;
            std::string_view name;
            uint64_t physical_address;
            pid_t pid;
            uint64_t in_precess_virtual_address;

            DeviceInfo(std::string_view kind,
                       std::string_view name,
                       uint64_t physical_address,
                       pid_t pid,
                       uint64_t in_precess_virtual_address) {
                this->kind = kind;
                this->name = name;
                this->physical_address = physical_address;
                this->pid = pid;
                this->in_precess_virtual_address = in_precess_virtual_address;
            }
        };
        static void init();
        static int allocate(std::string_view kind, std::string_view name,
                            pid_t pid, uint64_t in_precess_virtual_address);
        static int release(std::string_view kind, std::string_view name);
        /**
         * 返回所有已连接信息
         * @return
         */
        static std::vector<DeviceInfo> show_info();

        static int connect_device(std::string_view kind, std::string_view name, uint64_t physical_address);
        static int disconnect_device(std::string_view kind, std::string_view name);
    };

} // magic_sheep

#endif //DEVICEMANAGER_DEVICEINFOTABLE_H
