#include "LedCommands.h"

/**
 * @brief Construct a new Led Commands object
 * 
 * @param address address of the BLE device (e.g. "FF:FF:FF:FF:FF:FF")
 * @param service_uuid uuid of the service for the BLE device (e.g. "A1B2")
 * @param characteristic_uuid uuid of the characteristic for the BLE device (e.g. "1234")
 */
LedCommands::LedCommands(const std::string& address, const std::string& service_uuid, const std::string& characteristic_uuid)
    : m_client(nullptr), m_characteristic(nullptr)
{
    NimBLEDevice::init("");

#ifdef ESP_PLATFORM
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // +9db
#else
    NimBLEDevice::setPower(9); // +9db
#endif

    this->m_client = NimBLEDevice::createClient();
    if (this->m_client == nullptr) {
        // Client creation failure
        return; // Exit constructor 
    }
    if (this->m_client != nullptr && !get_characteristic_handle(address, service_uuid, characteristic_uuid)) {
        // Failure to get characteristic handle
        this->delete_client();
    }
}

/**
 * @brief Get the characteristic handle
 * 
 * @param address Address of the BLE device
 * @param service_uuid UUID of the service
 * @param characteristic_uuid UUID of the characteristic
 * @return true if successful
 * @return false if not successful
 */
const bool LedCommands::get_characteristic_handle(const std::string& address, const std::string& service_uuid, const std::string& characteristic_uuid)
{
    NimBLEUUID serviceUuid(service_uuid);
    if (!this->m_client->connect(NimBLEAddress(address)))
    {
        // Connection failure
        return false;
    }

    NimBLERemoteService *pService = this->m_client->getService(serviceUuid);
    if (pService == nullptr)
    {
        // Service not found
        this->m_client->disconnect();
        return false;
    }

    NimBLERemoteCharacteristic *pCharacteristic = pService->getCharacteristic(characteristic_uuid);
    if (pCharacteristic == nullptr)
    {
        // Characteristic not found
        this->m_client->disconnect();
        return false;
    }

    this->m_characteristic = pCharacteristic;
    return true;
}

/**
 * @brief Destroy the Led Commands object, deletes the client
 * 
 */
LedCommands::~LedCommands()
{
    this->delete_client();
}

/**
 * @brief Check if the LedCommands object is properly initialized.
 * 
 * Checks whether the BLE characteristic handle is valid, which indicates that
 * the object was successfully initialized.
 * 
 * @return true if initialization was successful
 * @return false otherwise.
 */
bool LedCommands::is_initialized() const
{
    return m_characteristic != nullptr;
}

/**
 * @brief Deletes the client and cleans up
 *
 */
void LedCommands::delete_client()
{
    if (this->m_client != nullptr)
    {
        NimBLEDevice::deleteClient(this->m_client);
        this->m_client = nullptr;
    }
}

/**
 * @brief Convert hex string to byte vector
 * 
 * @param hexString hex string to be converted 
 * @return std::vector<uint8_t>
 */
std::vector<uint8_t> LedCommands::hex_string_to_bytes(const String& hex_string)
{
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex_string.length(); i += 2)
    {
        String byteString = hex_string.substring(i, i + 2);
        bytes.push_back((uint8_t)strtol(byteString.c_str(), NULL, 16));
    }
    return bytes;
}

/**
 * @brief Sends a string to the LED strip using BLE
 * 
 * @param command any string command to be sent over to the LED strip
 * @return true if succeeded
 * @return false if not connected or otherwise cannot perform write
 */
const bool LedCommands::send_command(const String& command)
{
    return m_characteristic->writeValue(LedCommands::hex_string_to_bytes(command));
}

/**
 * @brief Set the brightness level
 * 
 * @param brightness 0-100 (0x00-0x64)
 */
void LedCommands::set_brightness(const int& brightness)
{
    String command = "7e0001" + String(brightness, HEX) + "00000000ef";
    this->send_command(command);
}

/**
 * @brief Set the effect speed (auto color change, setting patterns, brightness and speed)
 * 
 * @param speed 0-100 (0x00-0x64)
 */
void LedCommands::set_effect_speed(const int& speed)
{
    String command = "7e0002" + String(speed, HEX) + "00000000ef";
    this->send_command(command);
}

/**
 * @brief Set the mode to temperature
 * 
 * @param temperature 128-138 (0x80 - 0x8a) -> (cold -> warm)
 */
void LedCommands::set_mode_temperature(const int& temperature)
{
    String command = "7e0003" + String(temperature, HEX) + "02000000ef";
    send_command(command);
}

/**
 * @brief Set the mode effect for the LED strip
 * 
 * @param effect 0x80-0x99 based on the following effect list
 * 
 * | Hex Value | Effect        |
 * 
 * | 0x80      | Red          |
 * 
 * | 0x81      | Green        |
 * 
 * | 0x82      | Blue         |
 * 
 * | 0x83      | Yellow       |
 * 
 * | 0x84      | Cyan         |
 * 
 * | 0x85      | Magenta      |
 * 
 * | 0x86      | White        |
 * 
 * | 0x87      | Jumping RGB  |
 * 
 * | 0x88      | Jumping RGBYCMW |
 * 
 * | 0x89      | Gradient RGB |
 * 
 * | 0x8A      | Gradient RGBYCMW |
 * 
 * | 0x8B      | Gradient Red |
 * 
 * | 0x8C      | Gradient Green |
 * 
 * | 0x8D      | Gradient Blue |
 * 
 * | 0x8E      | Gradient Yellow |
 * 
 * | 0x8F      | Gradient Cyan |
 * 
 * | 0x90      | Gradient Magenta |
 * 
 * | 0x91      | Gradient White |
 * 
 * | 0x92      | Gradient Red-Green |
 * 
 * | 0x93      | Gradient Red-Blue |
 * 
 * | 0x94      | Gradient Green-Blue |
 * 
 * | 0x95      | Blinking RGBYCMW |
 * 
 * | 0x96      | Blinking Red |
 * 
 * | 0x97      | Blinking Green |
 * 
 * | 0x98      | Blinking Blue |
 * 
 * | 0x99      | Blinking Yellow |
 * 
 * | 0x9A      | Blinking Cyan |
 * 
 * | 0x9B      | Blinking Magenta |
 * 
 * | 0x9C      | Blinking White |
 */
void LedCommands::set_mode_effect(const int& effect)
{
    String command = "7e0003" + String(effect, HEX) + "03000000ef";
    this->send_command(command);
}

/**
 * @brief Set the power for the LED strip
 * 
 * @param is_on true/false (on/off)
 */
void LedCommands::set_power(const bool& is_on)
{
    String command = is_on ? "7e00040100000000ef" : "7e00040000000000ef";
    this->send_command(command);
}

/**
 * @brief Set the color for LED strip on the rgb mode
 * 
 * @param r red value (0-255 (0x00-0xff))
 * @param g green value (0-255 (0x00-0xff))
 * @param b blue value (0-255 (0x00-0xff))
 */
void LedCommands::set_color_for_rgb_mode(const int& r, const int& g, const int& b)
{
    String command = "7e000503" + String(r, HEX) + String(g, HEX) + String(b, HEX) + "00ef";
    this->send_command(command);
}