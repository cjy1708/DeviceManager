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
        static void init();
        static void connect_device(std::string_view kind, std::string_view name);
        static int request_device(std::string_view kind, std::string_view name);
        static void release_device(std::string_view kind, std::string_view name);
        static void disconnect_device(std::string_view kind, std::string_view name);

        static std::vector<DeviceInfoTable::device_info > show_all_device();
    };

} // magic_sheep

#endif //DEVICEMANAGER_DEVICEMANAGER_H
