//
// Created by jinyang on 22-12-9.
//

#include "DeviceManager.h"

namespace magic_sheep {
    void DeviceManager::init() {
        DeviceTable::init();
        DeviceInfoTable::init();
    }

    void DeviceManager::connect_device(std::string_view kind, std::string_view name) {
        DeviceTable::device_connect(kind);
        DeviceInfoTable::connect_device(kind, name, 0x123789);
    }

    int DeviceManager::request_device(std::string_view kind, std::string_view name) {
        DeviceTable::request_device(kind);
        DeviceInfoTable::allocate(kind, name, getpid(), 0x123654);
        return 0x963741;
    }

    void DeviceManager::release_device(std::string_view kind, std::string_view name) {
        DeviceTable::release_device(kind);
        DeviceInfoTable::release(kind, name);
    }

    void DeviceManager::disconnect_device(std::string_view kind, std::string_view name) {
        DeviceTable::device_disconnect(kind);
        DeviceInfoTable::disconnect_device(kind, name);
    }

    std::vector<DeviceInfoTable::device_info> DeviceManager::show_all_device() {
        return DeviceInfoTable::show_info();
    }
} // magic_sheep