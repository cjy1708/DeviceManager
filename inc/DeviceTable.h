//
// Created by jinyang on 22-12-1.
//

#ifndef DEVICEMANAGER_DEVICETABLE_H
#define DEVICEMANAGER_DEVICETABLE_H

#include <string>
#include <filesystem>
#include <exception>
#include <fstream>
#include <regex>
#include <mutex>

namespace magic_sheep {

    /**
     * (kind)\t(sum)\t
     */
    class DeviceTable {
    private:
        /**
         * 文件目录
         */
        inline const static std::string path = "./etc/device_table";
        /**
         * 防止多线程导致文件被错误修改
         */
        inline static std::mutex read_write_mutex{ };

        static void modify_variables(std::string_view kind, int idle_increment, int no_idle_increment);
        static void reformat_file();
    public:
        DeviceTable() = delete;
        /**
         * 检查父文件夹是否存在，不存在则创建
         */
        static void init();
        static int device_connect(std::string_view kind);
        static int device_disconnect(std::string_view kind);
        static int request_device(std::string_view kind);
        static int release_device(std::string_view kind);
    };

} // magic_sheep

#endif //DEVICEMANAGER_DEVICETABLE_H
