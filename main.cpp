#include <iostream>
#include <string>
#include <DeviceManager.h>

/**
 * 主UI用户交互
 * 1. 新设备连接
 * 2. 申请设备
 * 3. 释放设备
 * 4. 设备断开
 * 5. 显示所有设备
 * 6. 退出
 * @return 返回用户选择的选项
 */
int show_main_ui();

int main() {
    using namespace std;
    using namespace magic_sheep;

    DeviceManager::init();
    int choose;
    do {
        choose = show_main_ui();
        switch (choose) {
            case 1: {
                string kind;
                string name;
                cout << "请输入设备类型: " << flush;
                cin >> kind;
                cout << "请输入设备名称: " << flush;
                cin >> name;
                DeviceManager::connect_device(kind, name);
                break;
            }
            case 2: {
                string kind;
                string name;
                cout << "请输入设备类型: " << flush;
                cin >> kind;
                cout << "请输入设备名称: " << flush;
                cin >> name;
                DeviceManager::request_device(kind, name);
                break;
            }
            case 3: {
                string kind;
                string name;
                cout << "请输入设备类型: " << flush;
                cin >> kind;
                cout << "请输入设备名称: " << flush;
                cin >> name;
                DeviceManager::release_device(kind, name);
                break;
            }
            case 4: {
                string kind;
                string name;
                cout << "请输入设备类型: " << flush;
                cin >> kind;
                cout << "请输入设备名称: " << flush;
                cin >> name;
                DeviceManager::disconnect_device(kind, name);
                break;
            }
            case 5: {
                auto all_device{DeviceManager::show_all_device()};
                if (all_device.empty()) {
                    cout << "not device have connected" << endl;
                    break;
                }
                for (size_t i{ }; i < all_device.size(); ++i) {
                    cout << "[" << i << "]: " << all_device[i].kind << flush << "\t" << flush << all_device[i].name << flush  << "\t" << flush
                         << all_device[i].physical_address << flush  << "\t" << flush
                         << all_device[i].pid << flush  << "\t" << flush  << all_device[i].in_precess_virtual_address << endl;
                }
                break;
            }
            case 6:
                break;
            default:
                cout << "输入错误，清重新输入" << endl;
        }
    } while (choose != 6);

    return 0;
}

int show_main_ui() {
    using namespace std;
    int choose{ -1 };
    cout << "****************************************************" << endl
        <<  "1. 新设备连接" << endl
        <<  "2. 申请设备" << endl
        <<  "3. 释放设备" << endl
        <<  "4. 设备断开" << endl
        <<  "5. 显示所有设备" << endl
        <<  "6. 退出" << endl
        << "****************************************************" << endl
        << "请选择: " << flush;
    cin >> choose;
    return choose;
}