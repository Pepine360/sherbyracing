#include "MAX11605_HAL.hpp"

static bool timer_callback(repeating_timer_t *t)
{
    MAX11605* max = (MAX11605*)t->user_data;
    max->Read();
    max->SetNewData(true);

    return true;
}

MAX11605::MAX11605(i2c_inst_t *inst, scan_t s, channel_t c, select_t sl, bool se, bool ce, bool bi, bool r, uint8_t address, uint8_t sc, uint8_t sd, uint32_t f, uint32_t timer_freq)
{
    this->chan = c;
    this->sc = s;
    this->singleEnded = se;

    this->sel = sl;
    this->clkExternal = ce;
    this->bipolar = bi;
    this->reset = r;

    this->i2c = inst;
    this->addr = address;
    this->scl = sc;
    this->sda = sd;
    this->freq = f;


    i2c_init(this->i2c, this->freq);

    gpio_set_function(this->sda, GPIO_FUNC_I2C);
    gpio_set_function(this->scl, GPIO_FUNC_I2C);

    uint8_t payload[2] = {SetupToByte(), ConfigToByte()};

    i2c_write_blocking(this->i2c, this->addr, payload, 2, false);

    uint32_t timer_delay = (uint32_t)((1.f / timer_freq) * 1000);

    if (!add_repeating_timer_ms(timer_delay, timer_callback, (void *)this, &timer))
    {
        panic("Failed to create ADC timer");
    }
}

MAX11605::~MAX11605(){}

int8_t MAX11605::Write(uint8_t *payload, size_t len)
{
    if (this->i2c == nullptr)
        return PICO_ERROR_GENERIC;

    i2c_write_blocking(this->i2c, this->addr, payload, len, false);
}

int8_t MAX11605::Read()
{
    if (this->i2c == nullptr)
        return PICO_ERROR_GENERIC;

    uint8_t len;

    switch (this->sc)
    {
    case SWEEP:
        len = (int)this->chan;
        break;
    
    case REPEAT_8:
        len = 8;
        break;

    case UPPER_HALF:
        if (this->chan == channel_6)
            len = 1;
        else
        {
            len = (int)this->chan - (int)channel_6;
            if (len < 0)
                len = 1;
        }
        break;

    default:
        len = 1;
        break;
    }
    
    i2c_read_blocking(this->i2c, this->addr, this->buff, len, false);

    return 1;
}

int8_t MAX11605::ReadChannel(channel_t chan)
{
    if (this->i2c == nullptr)
        return PICO_ERROR_GENERIC;

    this->chan = chan;
    uint8_t payload = ConfigToByte();

    i2c_write_blocking(this->i2c,this->addr, &payload, 1, false);
    sleep_us(10);
    i2c_read_blocking(this->i2c, this->addr, this->buff, 1, false);
}

void MAX11605::SetNewData(bool value)
{
    this->newData = value;
}

bool MAX11605::GetNewData()
{
    return this->newData;
}

uint8_t MAX11605::ConfigToByte()
{
    return 0b00000000 | this->singleEnded | ((int)this->chan << 1) | ((int)this->sc << 5);
}

uint8_t MAX11605::SetupToByte()
{
    return 0b10000000 | (this->reset << 1) | (this->bipolar << 2) | (this->clkExternal << 3) | ((int)this->sel << 4);
}
