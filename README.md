# Documentação da biblioteca do microfone da BitDogLab

A presente biblioteca possui o objetivo de auxiliar
o uso do sensor de som da placa bitdoglab. Ela possui 
apenas 2 funções. Uma função que retorna o sinal de tensão do sensor 
e outra que retorna uma média dos dados captados pelo sensor.

- Abaixo encontra-se todo o código comentado.

## Arquivo de testes main.c da biblioteca

```c
#include <stdio.h>         // Biblioteca padrão para entrada/saída (printf, scanf, etc.)
#include "pico/stdlib.h"   // Biblioteca do Raspberry Pi Pico (GPIO, UART, timers, etc.)
#include "mic.h"           // Biblioteca personalizada para controle do microfone

int main() {
    stdio_init_all(); // Inicializa a comunicação serial do Raspberry Pi Pico
    sleep_ms(5000);   // Aguarda 5 segundos antes de iniciar (tempo para abrir o monitor serial)

    mic_init(); // Inicializa o microfone
    printf("Iniciando leitura do microfone...\n"); // Mensagem de inicialização

    while (true) { // Loop infinito para monitoramento contínuo do microfone
        mic_sample(); // Captura uma amostra do sinal do microfone
        float voltage = mic_get_voltage(); // Converte a amostra para um valor de tensão
        printf("Sinal do Microfone: %.4f V\n", voltage); // Exibe a tensão no monitor serial
        sleep_ms(500); // Aguarda 500 ms antes da próxima leitura
    }
}
```

## Arquivo de testes mic.c da biblioteca

```c
#include "mic.h"           // Inclui o cabeçalho do microfone (provavelmente define constantes e funções)
#include "pico/stdlib.h"   // Biblioteca padrão do Raspberry Pi Pico (GPIO, UART, timers, etc.)
#include "hardware/adc.h"  // Biblioteca para manipulação do ADC (Conversor Analógico-Digital)
#include <math.h>          // Biblioteca matemática para operações como raiz quadrada (sqrtf)

// Buffer para armazenar as leituras do ADC
static uint16_t adc_buffer[SAMPLES];

// Função para inicializar o ADC para leitura do microfone
void mic_init() {
    adc_gpio_init(MIC_PIN);       // Configura o pino MIC_PIN como entrada do ADC
    adc_init();                   // Inicializa o ADC
    adc_select_input(MIC_CHANNEL); // Define o canal do ADC a ser utilizado
    adc_set_clkdiv(ADC_CLOCK_DIV); // Configura a divisão de clock do ADC para ajuste da taxa de amostragem
}

// Função para coletar amostras do ADC e armazená-las no buffer
void mic_sample() {
    for (int i = 0; i < SAMPLES; i++) {
        adc_buffer[i] = adc_read(); // Realiza a leitura do ADC e armazena no buffer
    }
}

// Função para calcular a potência média (RMS) das amostras armazenadas no buffer
float mic_get_rms() {
    float avg = 0.f; // Variável para armazenar a soma dos quadrados das leituras
    for (uint i = 0; i < SAMPLES; i++) {
        avg += adc_buffer[i] * adc_buffer[i]; // Soma dos quadrados das amostras
    }
    avg /= SAMPLES; // Calcula a média das amostras
    return sqrtf(avg); // Retorna a raiz quadrada da média (valor RMS)
}

// Função para converter a leitura RMS do ADC para tensão
float mic_get_voltage() {
    float rms = mic_get_rms(); // Obtém o valor RMS das amostras
    return fabs(rms * 3.3f / (1 << 12u) - 1.65f); 
    // Converte o valor RMS para tensão:
    // - Multiplica pelo Vref do ADC (3.3V)
    // - Divide pelo número total de níveis do ADC (2^12 = 4096)
    // - Ajusta o deslocamento do sinal para centralizar em 1.65V (meio da faixa de 3.3V)
}

```

## Arquivo de testes mic.h da biblioteca

```c
#ifndef MIC_H   // Verifica se MIC_H ainda não foi definido
#define MIC_H   // Define MIC_H para evitar múltiplas inclusões da mesma biblioteca

#include <stdint.h> // Biblioteca padrão para tipos de dados inteiros de tamanho fixo (ex: uint16_t)

// Definições de configuração do ADC
#define MIC_CHANNEL 2        // Define o canal do ADC que será usado para o microfone
#define MIC_PIN (26 + MIC_CHANNEL) // Calcula o pino GPIO correspondente ao canal do ADC
#define ADC_CLOCK_DIV 96.f   // Define a divisão do clock do ADC para ajuste da taxa de amostragem
#define SAMPLES 200          // Define o número de amostras coletadas para cada cálculo

// Declaração das funções públicas da biblioteca (para serem usadas em outros arquivos)
void mic_init();        // Inicializa o ADC para leitura do microfone
void mic_sample();      // Coleta amostras do ADC e armazena no buffer
float mic_get_rms();    // Calcula o valor RMS das amostras coletadas
float mic_get_voltage();// Converte o valor RMS para tensão

#endif // MIC_H  // Fim da diretiva de inclusão condicional

```