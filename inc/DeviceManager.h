//
// Created by jinyang on 22-12-9.
//

#ifndef DEVICEMANAGER_DEVICEMANAGER_H
#define DEVICEMANAGER_DEVICEMANAGER_H

#include <string>
#include <vector>
#include <unistd.h>
#include <DeviceInfoTable.h>
#include <DeviceTable.h>

namespace magic_sheep {

    class DeviceManager {
    public:
        /**
         * 初始化目录，如果程序运行中出现目录被删除，任何对文件的操作都会报错
         */
        static void init();
        /**
         * 设备连接，注册设备信息，并写入文件持久化
         * device_table 将空闲设备数加一
         * device_info_table 注册设备，使用进程的pid为0,进程内相对地址为0
         * @param kind 设备类型
         * @param name 设备名称
         */
        static void connect_device(std::string_view kind, std::string_view name);
        /**
         * 进程请求设备
         * device_table 将空闲设备数减一，非空闲设备数加一
         * 将device_info_table中的对应设备的pid设备对应进程的pid，把应用相对地址填入
         * @param kind 设备类型
         * @param name 设备名称
         * @return
         */
        static int request_device(std::string_view kind, std::string_view name);
        /**
         * 进程释放设备
         * device_table 将空闲设备数加一，非空闲设备数减一
         * 如果占用设备的pid与申请释放设备进程的pid一直，则允许释放，否则释放失败
         * @param kind 设备类型
         * @param name 设备名称
         */
        static void release_device(std::string_view kind, std::string_view name);
        /**
         * 设备断开连接
         * device_table 将空闲设备数减一
         * 从device_info_table中删除对应行的数据
         * @param kind 设备类型
         * @param name 设备名称
         */
        static void disconnect_device(std::string_view kind, std::string_view name);

        static std::vector<DeviceInfoTable::device_info > show_all_device();
    };

} // magic_sheep

#endif //DEVICEMANAGER_DEVICEMANAGER_H
