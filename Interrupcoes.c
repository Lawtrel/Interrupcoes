#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define WS2812_PIN 7
#define NUM_LEDS 25
#define RED_PIN 13 
#define GREEN_PIN 11
#define BLUE_PIN 12
#define BUTTON_PIN 5
#define BUTTON_PIN2 6
const uint8_t animacao[10][25] = {
    // 0
    {
        1,1,1,1,1,
        1,0,0,0,1,
        1,0,0,0,1,
        1,0,0,0,1,
        1,1,1,1,1
    },
    // 1
    {
        0,0,1,0,0,
        0,1,1,0,0,
        0,0,1,0,0,
        0,0,1,0,0,
        0,0,1,0,0
    },
    // 2
    {
        1,1,1,1,1,
        0,0,0,0,1,
        1,1,1,1,1,
        1,0,0,0,0,
        1,1,1,1,1
    },
    // 3
    {
        1,1,1,1,1,
        0,0,0,0,1,
        0,1,1,1,1,
        0,0,0,0,1,
        1,1,1,1,1
    },
    // 4 
    {
        0,1,0,1,0,
        0,1,0,1,0,
        0,1,1,1,0,
        0,0,0,1,0,
        0,1,0,0,0
    },
    // 5
    {
        1,1,1,1,1,
        1,0,0,0,0,
        1,1,1,1,1,
        0,0,0,0,1,
        1,1,1,1,1
    },
    // 6
    {
        1,1,1,1,1,
        1,0,0,0,0,
        1,1,1,1,1,
        1,0,0,0,1,
        1,1,1,1,1
    },
    // 7
    {
        1,1,1,1,1,
        0,0,0,0,1,
        0,1,0,0,0,
        0,0,1,0,0,
        0,0,0,1,0
    },
    // 8
    {
        1,1,1,1,1,
        1,0,0,0,1,
        1,1,1,1,1,
        1,0,0,0,1,
        1,1,1,1,1
    },
    // 9
    {
        1,1,1,1,1,
        1,0,0,0,1,
        1,1,1,1,1,
        0,0,0,0,1,
        1,1,1,1,1
    }
};


volatile uint8_t numero = 0;
volatile bool atualizar_display = false;
PIO pio = pio0;
uint sm = 0;

struct repeating_timer blink_timer;
void init_rgb_led();
void init_buttons();
void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, float freq);
void atualizar_led_matrix();
bool blink_callback(struct repeating_timer *t);
void button_isr(uint gpio, uint32_t events);
int64_t debounce_callback(alarm_id_t id, void *user_data);

int main()
{
    stdio_init_all();
    init_rgb_led();
    init_buttons();

    // Inicializa o programa PIO
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000);

    add_repeating_timer_ms(100, blink_callback, NULL, &blink_timer);
    while (true) {
        if (atualizar_display) {
            atualizar_led_matrix();
            atualizar_display = false;
        }
        sleep_ms(10);
    }
}

void init_rgb_led() {
    gpio_init(RED_PIN);
    gpio_set_dir(RED_PIN, GPIO_OUT);
    gpio_init(GREEN_PIN);
    gpio_set_dir(GREEN_PIN, GPIO_OUT);
    gpio_init(BLUE_PIN);
    gpio_set_dir(BLUE_PIN, GPIO_OUT);
    gpio_put(GREEN_PIN, 0);
    gpio_put(BLUE_PIN, 0);
}

bool blink_callback(struct repeating_timer *t) {
    gpio_put(RED_PIN, !gpio_get(RED_PIN));  
    return true;
}

void init_buttons() {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_isr);
    gpio_init(BUTTON_PIN2);
    gpio_set_dir(BUTTON_PIN2, GPIO_IN);
    gpio_pull_up(BUTTON_PIN2);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN2, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_isr);
}

void button_isr(uint gpio, uint32_t events) {
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL, false);
    add_alarm_in_ms(50, debounce_callback, (void*)(uintptr_t)gpio, true);
}

int64_t debounce_callback(alarm_id_t id, void *user_data) {
    uint gpio = (uint)(uintptr_t)user_data;
    if (!gpio_get(gpio)) {
        if (gpio == BUTTON_PIN) {
            numero = (numero + 1) % 10;
        } else if (gpio == BUTTON_PIN2) {
            numero = (numero == 0) ? 9 : numero - 1;
        }
        atualizar_display = true;
    }
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL, true);
    return 0;
}

void atualizar_led_matrix() {
    uint32_t colors[NUM_LEDS];
    const uint8_t *pattern = animacao[numero];

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int index = i * 5 + j;
            int corrigido = ( 4- i) * 5 + j; // inverte as linhas
            colors[corrigido] = pattern[index] ? 0x00FFFF  : 0x000000;
        }
    }

    for (int i = 0; i < NUM_LEDS; i++) {
        uint32_t color = colors[i];
        pio_sm_put_blocking(pio, sm, colors[i]);
    }
}

void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, float freq) {
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    // Configuração do programa
    pio_sm_config c = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin); // Usa o pino sideset
    sm_config_set_out_shift(&c, true, true, 24); // 24 bits (3 bytes) por LED
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX); // Usa apenas o FIFO TX

    // Calcula o prescaler para o clock
    float prescaler = clock_get_hz(clk_sys) / (10.f * freq); // 10 ciclos por bit
    sm_config_set_clkdiv(&c, prescaler);

    // Inicializa a state machine
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}