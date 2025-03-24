# Documentação da Biblioteca do Microfone da BitDogLab

## Introdução

Esta biblioteca foi desenvolvida para facilitar a integração de sensores de som em sistemas embarcados utilizando a plataforma **BitDogLab/Raspberry Pi Pico W**, baseada no microcontrolador **RP2040**. O objetivo é fornecer uma interface simplificada para leitura dos sinais do microfone, abstraindo os comandos de baixo nível.

A biblioteca foi escrita na linguagem **C**, com suporte para **CMake**, e foi desenvolvida utilizando o **Visual Studio Code**, juntamente com a extensão **Raspberry Pi Pico**.

## Funcionalidades

A biblioteca oferece um conjunto de funções para a leitura do sinal do microfone:
- `mic_get_voltage()`: Retorna o valor da tensão em **volts**.
- `mic_get_rms()`: Retorna o valor **médio quadrático (RMS)** das amostras captadas.

## Instalação e Configuração

### Requisitos
- Raspberry Pi Pico W (ou equivalente com RP2040)
- Ambiente de desenvolvimento configurado para C/C++ com CMake
- Biblioteca padrão do Raspberry Pi Pico

### Como usar

1. Clone ou baixe os arquivos da biblioteca.
2. Inclua o arquivo `mic.h` no seu código.
3. Compile o projeto utilizando CMake e faça o upload para a placa.

## Exemplo de Uso

O seguinte código demonstra como utilizar a biblioteca para ler e exibir os valores do microfone:

```c
#include <stdio.h>         // Biblioteca padrão para entrada/saída (printf, scanf, etc.)
#include "pico/stdlib.h"   // Biblioteca do Raspberry Pi Pico (GPIO, UART, timers, etc.)
#include "mic.h"           // Biblioteca personalizada para controle do microfone

int main() {
    stdio_init_all(); // Inicializa a comunicação serial do Raspberry Pi Pico
    sleep_ms(5000);   // Aguarda 5 segundos antes de iniciar (tempo para abrir o monitor serial)

    mic_init(); // Inicializa o microfone
    printf("Iniciando leitura do microfone...\n");

    while (true) { // Loop infinito para monitoramento contínuo do microfone
        mic_sample(); // Captura uma amostra do sinal do microfone
        float voltage = mic_get_voltage(); // Converte a amostra para um valor de tensão
        printf("Sinal do Microfone: %.4f V\n", voltage);
        sleep_ms(500); // Aguarda 500 ms antes da próxima leitura
    }
}
```

## Estrutura da Biblioteca

### Arquivo `mic.h` (Cabeçalho da Biblioteca)

O arquivo `mic.h` define as constantes e declarações das funções públicas:

```c
#ifndef MIC_H
#define MIC_H

#include <stdint.h> // Tipos de dados inteiros fixos

#define MIC_CHANNEL 2        // Canal do ADC para o microfone
#define MIC_PIN (26 + MIC_CHANNEL) // Pino GPIO correspondente ao canal do ADC
#define ADC_CLOCK_DIV 96.f   // Divisão do clock do ADC para ajuste da taxa de amostragem
#define SAMPLES 200          // Número de amostras coletadas

void mic_init();        // Inicializa o ADC para leitura do microfone
void mic_sample();      // Coleta amostras do ADC e armazena no buffer
float mic_get_rms();    // Calcula o valor RMS das amostras coletadas
float mic_get_voltage();// Converte o valor RMS para tensão

#endif // MIC_H
```

### Arquivo `mic.c` (Implementação da Biblioteca)

O arquivo `mic.c` contém a lógica para a captura e processamento dos sinais do microfone:

```c
#include "mic.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <math.h>

static uint16_t adc_buffer[SAMPLES]; // Buffer para armazenar as leituras do ADC

void mic_init() {
    adc_gpio_init(MIC_PIN);
    adc_init();
    adc_select_input(MIC_CHANNEL);
    adc_set_clkdiv(ADC_CLOCK_DIV);
}

void mic_sample() {
    for (int i = 0; i < SAMPLES; i++) {
        adc_buffer[i] = adc_read();
    }
}

float mic_get_rms() {
    float avg = 0.f;
    for (uint i = 0; i < SAMPLES; i++) {
        avg += adc_buffer[i] * adc_buffer[i];
    }
    avg /= SAMPLES;
    return sqrtf(avg);
}

float mic_get_voltage() {
    float rms = mic_get_rms();
    return fabs(rms * 3.3f / (1 << 12u) - 1.65f);
}
```

## Explicação das Funções

- `mic_init()`: Inicializa o ADC e configura o pino correto para leitura.
- `mic_sample()`: Coleta um conjunto de amostras e armazena em um buffer.
- `mic_get_rms()`: Calcula o valor RMS das amostras coletadas.
- `mic_get_voltage()`: Converte o valor RMS para uma leitura de tensão em volts.

---

