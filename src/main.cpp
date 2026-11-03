#include <avr/io.h>
#include <util/delay.h>

#define OLED_ADDR 0x3C
#define SCREEN_W 128
#define SCREEN_H 64
#define PAGES (SCREEN_H / 8)

uint8_t framebuffer[SCREEN_W * PAGES];

void i2c_init()
{
    TWSR = 0x00;        // prescaler =1
    TWBR = 0x0c;        // scl= 400khz
    TWCR = (1 << TWEN); // open all register
}

void i2c_start()
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT)|(1<<TWEN);
    while (!(TWCR & (1 << TWINT))) ;
}

void i2c_stop()
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

// Command OLED
void oled_send_cmd(uint8_t cmd)
{
    i2c_start();
    i2c_write((OLED_ADDR << 1) | 0);
    i2c_write(0x00);
    i2c_write(cmd);
    i2c_stop();
}

void oled_send_data(uint8_t data)
{
    i2c_start();
    i2c_write((OLED_ADDR << 1) | 0);
    i2c_write(0x40);
    i2c_write(data);
    i2c_stop();
}

void olen_init()
{
    _delay_ms(100);

    oled_send_cmd(0xD5);
    oled_send_cmd(0x80);
    oled_send_cmd(0xA8);
    oled_send_cmd(0x3F);
    oled_send_cmd(0xAE);
    oled_send_cmd(0xD3);
    oled_send_cmd(0x00);
    oled_send_cmd(0x40);
    oled_send_cmd(0x8D);
    oled_send_cmd(0x14);
    oled_send_cmd(0x20);
    oled_send_cmd(0x00);
    oled_send_cmd(0xA1);
    oled_send_cmd(0xC8);
    oled_send_cmd(0xDA);
    oled_send_cmd(0x12);
    oled_send_cmd(0x81);
    oled_send_cmd(0xCF);
    oled_send_cmd(0xD9);
    oled_send_cmd(0xF1);
    oled_send_cmd(0xDB);
    oled_send_cmd(0x40);
    oled_send_cmd(0xA4);
    oled_send_cmd(0xA6);
    oled_send_cmd(0xAF);
}

void fb_clear()
{
    for (uint16_t i = 0; i < SCREEN_W * PAGES; i++)
    {
        framebuffer[i] = 0x00;
    }
}

void fb_set_pixel(uint8_t x, uint8_t y, uint8_t on)
{
    if ((x >= SCREEN_W) || (y >= SCREEN_H))
        return;
    uint8_t page = y / 8;
    uint8_t bit_pos = y % 8;
    uint16_t index = SCREEN_W * page + x;

    if (on)
    {
        framebuffer[index] |= (1 << bit_pos);
    }
    else
    {
        framebuffer[index] &= ~(1 << bit_pos);
    }
}
void fb_flush()
{
    for (uint8_t page = 0; page < PAGES; page++)
    {
        oled_send_cmd(0xB0 + page);
        oled_send_cmd(0x00);
        oled_send_cmd(0x10);
        /* code */
        for (uint8_t col = 0; col < SCREEN_W; col++)
        {
            oled_send_data(framebuffer[page * SCREEN_W + col]);
        }
    }
}

int main()
{
    i2c_init();
    olen_init();
    fb_clear();

    fb_set_pixel(10, 10, 1);
    fb_set_pixel(11, 11, 1);
    fb_set_pixel(12, 12, 1);

    fb_flush();
    while (1)
    {
        /* code */
    }
}