
#include <msgpack.h>

#include <string>
#include <vector>

#include <functional>
#include <iostream>
#include <memory>

class SerializedBytes {
  public:
    static std::shared_ptr<SerializedBytes>
    Create(char *data, size_t size, std::function<void(char *)> deleter) {
        return std::shared_ptr<SerializedBytes>(
            new SerializedBytes(data, size, deleter));
    }

    ~SerializedBytes() { m_deleter(m_data); }

    const char *Data() { return m_data; }

    size_t Size() { return m_size; }

  private:
    explicit SerializedBytes(char *data, size_t size,
                             std::function<void(char *)> deleter)
        : m_data(data), m_size(size), m_deleter(deleter) {}
    char *m_data;
    const size_t m_size;
    const std::function<void(char *)> m_deleter;
};

enum SettingID : uint64_t {
    none = 0,
    product_name,
    serial_number,
};

class SettingItem {
  private:
    SettingID m_id;
    std::string m_name;

  public:
    explicit SettingItem() : m_id(SettingID::none){};
    const std::string &Name() const { return m_name; }

    void Name(const std::string &name) { m_name = name; }

    std::shared_ptr<SerializedBytes> ToByte() const {
        msgpack_sbuffer sbuf;
        msgpack_packer pk;
        msgpack_sbuffer_init(&sbuf);
        msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

        ToByte(sbuf, pk);

        return SerializedBytes::Create(sbuf.data, sbuf.size,
                                       [](char *buf) { free(buf); });
    }

    void ToByte(msgpack_sbuffer &sbuf, msgpack_packer &pk) const {
        msgpack_pack_int64(&pk, m_id);
        msgpack_pack_str(&pk, m_name.size());
        msgpack_pack_str_body(&pk, m_name.c_str(), m_name.size());
    }
};

class Settings {
  private:
    std::vector<SettingItem> m_setting_items;

  public:
    std::vector<SettingItem> &SettingItems() { return m_setting_items; }

    std::shared_ptr<SerializedBytes> ToByte() const {
        msgpack_sbuffer sbuf;
        msgpack_packer pk;
        msgpack_sbuffer_init(&sbuf);
        msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

        ToByte(sbuf, pk);

        return SerializedBytes::Create(sbuf.data, sbuf.size,
                                       [](char *buf) { free(buf); });
    }

    void ToByte(msgpack_sbuffer &sbuf, msgpack_packer &pk) const {
        msgpack_pack_array(&pk, m_setting_items.size());
        for (auto item : m_setting_items) {
            item.ToByte(sbuf, pk);
        }
    }
};

#ifndef TEST
int main(int args, char **argv) {
    Settings settings;
    SettingItem item;
    item.Name("PT2023");
    settings.SettingItems().push_back(item);
    settings.SettingItems().push_back(item);
    
    auto bytes = settings.ToByte();
    std::cout << "size = " << bytes->Size() << std::endl;
}
#endif