#include <gtest/gtest.h>

#include <cstdio>
#include <cstring>
#include <string>

#include "../../examples/companion_radio/NodePrefs.h"

class ReplayStream : public Stream {
  const char* _text;
  int _pos = 0;
  int _len;

public:
  explicit ReplayStream(const char* text) : _text(text), _len(strlen(text)) { }

  int available() override { return _len - _pos; }
  int read() override { return _pos < _len ? _text[_pos++] : -1; }
  int peek() override { return _pos < _len ? _text[_pos] : -1; }
};

class CaptureStream : public Stream {
  std::string _text;

  size_t emit(long long value) {
    char text[24];
    int length = snprintf(text, sizeof(text), "%lld", value);
    return write(reinterpret_cast<const uint8_t*>(text), length);
  }

public:
  size_t write(uint8_t value) override {
    _text.push_back(static_cast<char>(value));
    return 1;
  }

  size_t write(const uint8_t* buffer, size_t size) override {
    _text.append(reinterpret_cast<const char*>(buffer), size);
    return size;
  }

  size_t print(unsigned char value, int = DEC) override { return emit(value); }
  size_t print(int value, int = DEC) override { return emit(value); }
  size_t print(unsigned int value, int = DEC) override { return emit(value); }
  size_t print(long value, int = DEC) override { return emit(value); }
  size_t print(unsigned long value, int = DEC) override { return emit(value); }
  size_t print(long long value, int = DEC) override { return emit(value); }
  size_t print(unsigned long long value, int = DEC) override { return emit(value); }

  const std::string& text() const { return _text; }
};

#if 0
// Re-enable test once we can SET fem_ values in companion
TEST(CompanionNodePrefs, RxGainSettingsRoundTripIndependently) {
  NodePrefs saved;
  saved.rx_boosted_gain = 0;
  saved.radio_fem_rxgain = 1;
  saved.radio_fem_txgain = 0;

  CaptureStream output;
  ASSERT_TRUE(saved.saveSerial(output));
  EXPECT_NE(std::string::npos, output.text().find("rxgain:0"));
  EXPECT_NE(std::string::npos, output.text().find("fem_rxgain:1"));
  EXPECT_NE(std::string::npos, output.text().find("fem_txgain:0"));

  ReplayStream input("{radio:{rxgain:1,fem_rxgain:0,fem_txgain:1}}");
  NodePrefs loaded;
  loaded.rx_boosted_gain = 0;
  loaded.radio_fem_rxgain = 1;
  loaded.radio_fem_txgain = 0;

  ASSERT_TRUE(loaded.loadSerial(input));
  EXPECT_EQ(1, loaded.rx_boosted_gain);
  EXPECT_EQ(0, loaded.radio_fem_rxgain);
  EXPECT_EQ(1, loaded.radio_fem_txgain);
}
#endif

// TEAM: guards against the custom fields silently resetting on reboot.
// These live outside upstream's prefs set, so they must be explicitly
// registered in NodePrefs::structure() to survive saveSerial/loadSerial.
TEST(CompanionNodePrefs, TeamCustomFieldsRoundTrip) {
  NodePrefs saved;
  saved.flood_max = 3;
  saved.autonomous_enabled = 1;
  saved.autonomous_channel_hash = 0x2A;
  saved.autonomous_interval_sec = 120;
  saved.autonomous_min_distance_m = 250;

  CaptureStream output;
  ASSERT_TRUE(saved.saveSerial(output));
  // Assert the exact nested blocks: "int" is also a GPSPrefs key, so a loose
  // substring match would not prove the value came from the autonomous object.
  EXPECT_NE(std::string::npos, output.text().find("repeat:{disable:1,f_max:3}"));
  EXPECT_NE(std::string::npos, output.text().find("auto:{en:1,chan:42,int:120,dist:250}"));

  ReplayStream input("{repeat:{disable:0,f_max:7},auto:{en:1,chan:9,int:45,dist:1000}}");
  NodePrefs loaded;
  ASSERT_TRUE(loaded.loadSerial(input));
  EXPECT_EQ(7, loaded.flood_max);
  EXPECT_TRUE(loaded.isRepeatEn());
  EXPECT_EQ(1, loaded.autonomous_enabled);
  EXPECT_EQ(9, loaded.autonomous_channel_hash);
  EXPECT_EQ(45, loaded.autonomous_interval_sec);
  EXPECT_EQ(1000, loaded.autonomous_min_distance_m);
}

// Defaults must be safe when no prefs file exists yet.
TEST(CompanionNodePrefs, TeamCustomFieldDefaults) {
  NodePrefs fresh;
  EXPECT_EQ(0, fresh.flood_max);              // forwarding off until app sets it
  EXPECT_EQ(0, fresh.autonomous_enabled);
  EXPECT_EQ(30, fresh.autonomous_interval_sec);
  EXPECT_EQ(0, fresh.autonomous_min_distance_m);
  EXPECT_FALSE(fresh.isRepeatEn());           // disable_fwd defaults to 1
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
