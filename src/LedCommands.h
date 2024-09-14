#ifndef LEDCOMMANDS_H
#define LEDCOMMANDS_H
#include <NimBLEDevice.h>

/**
 * @brief A class to craft and send commands to ELK-BLEDOM LED strip
 * 
 */
class LedCommands {
private:
    NimBLEClient* m_client;
    NimBLERemoteCharacteristic* m_characteristic;
  
    bool get_characteristic_handle(const std::string& address, const std::string& service_uuid, const std::string& characteristic_uuid);
    bool send_command(const String& command);
    void delete_client();

    static std::vector<uint8_t> hex_string_to_bytes(const String& hex_string);

public:
    LedCommands(const std::string& address, const std::string& service_uuid, const std::string& characteristic_uuid);
    ~LedCommands();

    bool is_initialized() const;

    void set_brightness(const int& brightness);
    void set_effect_speed(const int& speed);
    void set_mode_temperature(const int& temperature);
    void set_mode_effect(const int& effect);
    void set_power(const bool& is_on);
    void set_color_for_rgb_mode(const int& r, const int& g, const int& b);

    bool get_power_state();
};
#endif // LEDCOMMANDS_H