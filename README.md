# AquaTank Sensor Data IDE and Fiware Integration

Bem-vindo ao repositório da IDE e integração Fiware responsável pelos dados dos sensores do AquaTank. Este repositório contém a pasta da IDE do Arduino, onde você pode encontrar o código para a IDE dos sensores, e a pasta do Fiware, que inclui uma coleção Postman para facilitar a integração com o Fiware.

## Pré-requisitos

Antes de começar, certifique-se de ter o Arduino IDE instalado em seu sistema. Se ainda não tiver, você pode baixá-lo [aqui](https://www.arduino.cc/en/software). Além disso, é necessário fazer o download das bibliotecas localizadas na pasta "biblioteca-ide-file" deste repositório.

## Configuração da IDE Arduino

1. Baixe a pasta (`ide-file`) do repositório.
2. Abra o arquivo `.ino` dentro da pasta no Arduino IDE.
3. Faça o download das bibliotecas presentes na pasta "biblioteca-ide-file" deste repositório.
4. Importe todas as bibliotecas para a IDE.
5. Selecione o modelo de placa como "DOIT ESP32 DEVKIT V1" em "Board".
6. Certifique-se de que todos os sensores estão conectados corretamente às portas do ESP32.
7. Selecione a porta à qual o ESP32 está conectado em "Port".
8. Clique em "Upload" para executar a IDE no ESP32.

## Configuração do Fiware com Postman

O processo para configurar o Fiware é simples e é necessário apenas para realizar alterações específicas na coleta de dados do AquaTank. Siga os passos abaixo:

1. Crie uma conta no [Postman](https://www.postman.com/), caso ainda não tenha uma.
2. Crie um workspace no Postman.
3. Clique em "Import" e importe o arquivo "FIWARE-AquaTank.postman_collection.json" da pasta "fiware-file" deste repositório.

Agora você está pronto para utilizar a IDE do Arduino para coletar dados dos sensores do AquaTank e integrar esses dados com o Fiware usando a coleção Postman fornecida. Divirta-se!
