#include "somfy_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *TAG = "remote.somfy";

static const uint32_t HARDWARE_SYNC_US = 2580;
static const uint32_t SOFTWARE_SYNC_US = 4612;
static const uint32_t BIT_US = 650;

static const uint32_t INTERFRAME_GAP_US = 31870;

void SomfyProtocol::encode(RemoteTransmitData *dst, const SomfyData &data) {
  unsigned char payload[7];
  dst->reserve(127);
  dst->set_carrier_frequency(0);

  payload[0] = 0xAE;
  payload[1] = data.control_code << 4;
  payload[2] = data.rolling_code >> 8;
  payload[3] = data.rolling_code;
  payload[4] = data.address >> 16;
  payload[5] = data.address >> 8;
  payload[6] = data.address;

  uint32_t checksum = 0;
  for (int32_t n = 0; n < 7; n++) {
    checksum = checksum ^ payload[n] ^ (payload[n] >> 4);
  }
  checksum &= 0b1111;
  payload[1] |= checksum; 
	for (int32_t n = 1; n < 7; n++) {
    payload[n] ^= payload[n-1];
  }

  for (int32_t n = 0; n < 6; n++) {
		dst->item(HARDWARE_SYNC_US, HARDWARE_SYNC_US);
  }
  dst->item(SOFTWARE_SYNC_US,BIT_US);

  uint32_t prev_level = 0;
  for (int32_t n = 0; n < 7; n++) {
	  for (uint32_t mask = 1UL << 7; mask != 0; mask >>= 1) {
			if (payload[n] & mask) {
				if (prev_level == 0) {
					dst->space(BIT_US*2);
				} else {
					dst->mark(BIT_US);
					dst->space(BIT_US);
				}
				prev_level = 1;
			} else {
				if (prev_level == 1) {
					dst->mark(BIT_US*2);
				} else {
					dst->space(BIT_US);
					dst->mark(BIT_US);
				}
				prev_level = 0;
			}
		}
  }
	if (prev_level == 1 )
		dst->mark(BIT_US);
		
  dst->space(INTERFRAME_GAP_US);
}
optional<SomfyData> SomfyProtocol::decode(RemoteReceiveData src) {
  SomfyData out{
      .address = 0,
      .control_code = 0,
	  .rolling_code = 0,
  };
  return out;
}
void SomfyProtocol::dump(const SomfyData &data) {
  ESP_LOGD(TAG, "Received Somfy: address=0x%06X, control_code=0x%04X, rolling_code=0x%04X", data.address, data.control_code, data.rolling_code);
}

}  // namespace remote_base
}  // namespace esphome
