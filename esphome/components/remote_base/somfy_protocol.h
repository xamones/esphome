#pragma once

#include "esphome/core/component.h"
#include "remote_base.h"

namespace esphome {
namespace remote_base {

struct SomfyData {
  uint32_t address;
  uint16_t control_code;
  uint16_t rolling_code;

  bool operator==(const SomfyData &rhs) const { return address == rhs.address && control_code == rhs.control_code && rolling_code == rhs.rolling_code; }
};

class SomfyProtocol : public RemoteProtocol<SomfyData> {
 public:
  void encode(RemoteTransmitData *dst, const SomfyData &data) override;
  optional<SomfyData> decode(RemoteReceiveData src) override;
  void dump(const SomfyData &data) override;
};

DECLARE_REMOTE_PROTOCOL(Somfy)

template<typename... Ts> class SomfyAction : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(uint32_t, address)
  TEMPLATABLE_VALUE(uint16_t, control_code)
  TEMPLATABLE_VALUE(uint16_t, rolling_code)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    SomfyData data{};
    data.address = this->address_.value(x...);
    data.control_code = this->control_code_.value(x...);
    data.rolling_code = this->rolling_code_.value(x...);
    SomfyProtocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome
