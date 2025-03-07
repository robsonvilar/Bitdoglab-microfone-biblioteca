#include <stdio.h>
#include "pico/stdlib.h"
#include "mic.h"

int main() {
    stdio_init_all();
    sleep_ms(5000); // Espera para abrir o monitor serial.

    mic_init(); // Inicializa o microfone
    printf("Iniciando leitura do microfone...\n");

    while (true) {
        mic_sample(); // Realiza uma amostragem
        float voltage = mic_get_voltage(); // Converte para tensão
        printf("Sinal do Microfone new: %.4f V\n", voltage);
        sleep_ms(500);
    }
}
