
#include <msgpack.h>

#include <string>
#include <vector>

class SettingItem {
  private:
    std::string m_name;

  public:
    const std::string &Name() const { return m_name; }
    void Name(const std::string &name) { m_name = name; }
    void ToByte(char *buf, size_t size) const {
        msgpack_sbuffer sbuf;
        msgpack_packer pk;

        msgpack_sbuffer_init(&sbuf);
        msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
        msgpack_pack_str(&pk, m_name.size());
        msgpack_pack_str_body(&pk, m_name.c_str(), m_name.size());
    }
};

class Settings {
  private:
    std::vector<SettingItem> m_settings;

  public:
    std::vector<SettingItem> &GetSettings() { return m_settings; }
    void ToByte(char *buf, size_t size) const;
};

#ifndef TEST
int main(int args, char **argv) {
    msgpack_sbuffer sbuf; /* buffer */
    msgpack_packer pk;    /* packer */

    msgpack_sbuffer_init(&sbuf); /* initialize buffer */
    msgpack_packer_init(&pk, &sbuf,
                        msgpack_sbuffer_write); /* initialize packer */
}
#endif