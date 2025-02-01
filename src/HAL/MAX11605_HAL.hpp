#include "hardware/i2c.h"
#include "pico/stdlib.h"

typedef enum channel
{
    channel_0 = 0b0000,
    channel_1 = 0b0001,
    channel_2 = 0b0010,
    channel_3 = 0b0011,
    channel_4 = 0b0100,
    channel_5 = 0b0101,
    channel_6 = 0b0110,
    channel_7 = 0b0111,
    channel_8 = 0b1000,
    channel_9 = 0b1001,
    channel_10 = 0b1010,
    channel_11 = 0b1011
} channel_t;

typedef enum scan
{
    SWEEP = 0b00,
    REPEAT_8 = 0b01,
    UPPER_HALF = 0b10,
    SINGLE = 0b11,
}
scan_t;

typedef enum select
{
    VDD = 0b000,
    EXTERNAL = 0b010,
    INTERNAL_ANALOG_AUTOSHUTDOWN = 0b100,
    INTERNAL_ANALOG_ON = 0b101,
    INTERNAL_REF_ON = 0b110,
}
select_t;


class MAX11605
{
    public:
        MAX11605(i2c_inst_t *inst, scan_t sc, channel_t chan, select_t sel, bool single_ended, bool clk_external, bool bipolar, bool reset, uint8_t addr, uint8_t scl, uint8_t sda, uint32_t freq, uint32_t timer_freq);
        ~MAX11605();

        int8_t Write(uint8_t* payload, size_t len);
        int8_t Read();
        int8_t ReadChannel(channel_t chan);

        void SetNewData(bool value);
        void SetScan(scan_t s);
        void SetChannel(channel_t chan);
        void SetSelect(select_t sel);
        void SetReset(bool res);
        
        bool GetNewData();
        scan_t GetScan();
        channel_t GetChannel();
        select_t GetSelect();
        bool  GetReset();

        uint8_t buff[12];

    private:
        i2c_inst_t *i2c;

        scan_t sc;
        channel_t chan;
        select_t sel;
        bool singleEnded;
        
        bool clkExternal;
        bool bipolar;
        bool reset;
        
        uint8_t maxSize;
        uint8_t addr;

        uint8_t scl;
        uint8_t sda;
        uint32_t freq;

        bool newData;

        struct repeating_timer timer;

        uint8_t
        ConfigToByte();
        uint8_t SetupToByte();
};