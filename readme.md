# Projeto Interrupções para Raspberry Pi Pico W

Este projeto é um exemplo de uso de interrupções e controle de LEDs WS2812 com o Raspberry Pi Pico W. O código é escrito em C e utiliza o SDK do Raspberry Pi Pico.

## Estrutura do Projeto

- **Interrupcoes.c**: Arquivo principal do projeto contendo a lógica de inicialização e controle dos LEDs.
- **CMakeLists.txt**: Arquivo de configuração do CMake para compilar o projeto.
- **ws2812.pio**: Programa PIO para controlar os LEDs WS2812.

## Requisitos

- Raspberry Pi Pico W
- SDK do Raspberry Pi Pico
- CMake 3.13 ou superior

## Configuração do Ambiente

1. Instale o SDK do Raspberry Pi Pico seguindo as instruções da [documentação oficial](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf).
2. Clone este repositório:
    ```sh
    git clone https://github.com/seu-usuario/seu-repositorio.git
    cd seu-repositorio
    ```

## Compilação e Upload

1. Crie um diretório de build e navegue até ele:
    ```sh
    mkdir build
    cd build
    ```

2. Configure o projeto com CMake:
    ```sh
    cmake ..
    ```

3. Compile o projeto:
    ```sh
    make
    ```

4. Conecte seu Raspberry Pi Pico W ao computador e faça o upload do firmware:
    ```sh
    cp Interrupcoes.uf2 /media/seu-usuario/RPI-RP2/
    ```

## Uso

Após carregar o firmware no Raspberry Pi Pico W, os LEDs WS2812 começarão a piscar conforme a lógica implementada no arquivo [Interrupcoes.c](http://_vscodecontentref_/0).

## Arquivos Principais

### [Interrupcoes.c](http://_vscodecontentref_/1)

Este arquivo contém a lógica principal do projeto. Ele inicializa os LEDs RGB, configura os botões e define as funções de callback para as interrupções.

### [CMakeLists.txt](http://_vscodecontentref_/2)

Arquivo de configuração do CMake que define como o projeto deve ser compilado. Ele inclui o SDK do Raspberry Pi Pico e configura as bibliotecas necessárias.

### [ws2812.pio](http://_vscodecontentref_/3)

Programa PIO que define o protocolo de comunicação com os LEDs WS2812. Ele especifica os tempos de pulso para enviar dados aos LEDs.

## Contribuição

Contribuições são bem-vindas! Sinta-se à vontade para abrir issues e pull requests.

## Licença

Este projeto está licenciado sob a licença MIT. Veja o arquivo LICENSE para mais detalhes.