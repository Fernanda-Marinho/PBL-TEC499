# Sistemas Digitais

Este projeto tem como objetivo criar um console dedicado, utilizando a plataforma de desenvolvimento DE1-SoC. A proposta é desenvolver um jogo inspirado no Tetris, integrando programação em C com interações diretas com o hardware. O controle das peças é feito por meio de um acelerômetro, que capta os movimentos do jogador, enquanto botões físicos permitem executar ações como iniciar, pausar e continuar. Além disso, a interface VGA é utilizada para projetar o jogo em um monitor externo.

# Equipe:
- [Camila de Araújo Bastos](https://github.com/Caamilab)
- [Fernanda Marinho Silva](https://github.com/Fernanda-Marinho/)
- Nome 3

# Executando o Projeto
### Clonando o Repositório
```bash
git clone  https://github.com/Fernanda-Marinho/PBL-TEC499 
cd PBL-TEC499/

```
### Executando
```bash
cd problema1/ 
sudo make run
``` 

# Lógica do jogo

# VGA

A funcionalidade de vídeo no projeto é implementada por meio de um conjunto de operações que gerenciam a saída de vídeo na tela VGA.

## Lógica de Funcionamento

Primeiramente, a conexão com o sistema de vídeo é estabelecida, garantindo que todas as operações subsequentes possam ser realizadas corretamente. Em seguida, é realizado um processo de limpeza do buffer de vídeo, removendo qualquer conteúdo que possa ter sido previamente desenhado. Isso assegura que a visualização na tela seja clara e livre de interferências.

Uma vez que a tela está limpa, retângulos podem ser desenhados com coordenadas e cores específicas, permitindo a personalização dos elementos gráficos que serão apresentados ao usuário. Após completar os desenhos desejados, o conteúdo armazenado no buffer é atualizado na tela, tornando visíveis todas as alterações realizadas.

Por fim, é importante encerrar a conexão com o sistema de vídeo, liberando os recursos utilizados durante a execução. Esse fechamento é crucial para evitar vazamentos de memória e garantir que o sistema retorne ao seu estado original após a conclusão das operações de vídeo.

# Botão

O sistema implementa uma thread dedicada ao monitoramento do estado dos botões, garantindo que a interação do usuário com o jogo seja detectada em tempo real.

## Lógica de Funcionamento

A thread de monitoramento é responsável por verificar continuamente o estado dos botões, garantindo que quaisquer alterações sejam capturadas instantaneamente. Para isso, a função de monitoramento entra em um loop infinito, onde uma função específica é chamada repetidamente para verificar se algum botão foi pressionado.

Durante a execução do loop, um mutex é utilizado para proteger o acesso ao estado de pausa/início do jogo, assegurando que as alterações nesse estado sejam feitas de forma segura e evitando condições de corrida. Isso é crucial para a integridade do estado do jogo, especialmente em situações onde múltiplos eventos podem ocorrer simultaneamente.

Para evitar sobrecarga do sistema e permitir que outros processos sejam executados, a thread faz uma pausa de 10 milissegundos entre as leituras dos botões. Essa abordagem equilibrada permite uma resposta rápida às interações do usuário, ao mesmo tempo que mantém a eficiência do sistema.

# Mapeamento de Memória
Para possibilitar o acesso ao acelerômetro (ADXL345) presente na placa DE1-SOC, é necessário mapear a memória física para o espaço de endereçamento virtual. Esse processo envolve duas etapas principais.

## Passo 1: Abertura do `/dev/mem`
O arquivo especial `<code>/dev/mem</code>`, em sistemas Linux, permite interação direta com o hardware, contornando as proteções do kernel. Para acessá-lo, são concedidas permissões de leitura e escrita, além de garantir sincronização de I/O.

## Passo 2: Mapeamento da Memória
Após a abertura do arquivo, o próximo passo é mapear os endereços do **System Manager (SYSMGR)** e do controlador **I2C0**. Durante esse processo, define-se o endereço base, que marca o ponto de partida na memória física, e o tamanho da área a ser mapeada (span), indicando a extensão da memória a ser acessada. O sistema operacional atribui um endereço virtual à área mapeada, permitindo leitura e escrita. O compartilhamento entre processos também é habilitado, possibilitando que diferentes partes do sistema alterem essa região de memória simultaneamente, garantindo uma comunicação eficiente entre componentes.

Com o mapeamento completo, deve-se configurar tanto o Pinmux quanto o controlador I2C0 para estabelecer a comunicação com o acelerômetro.

# Configuração do Pin Mux
Depois de mapear a base de endereços, os valores dos offsets de cada registrador são somados à base para acessar regiões específicas e realizar as configurações necessárias. A seguir estão os registradores essenciais para garantir o roteamento correto dos sinais I2C para o controlador I2C0:

- **GENERALIO7 (offset: 0x127):** Este registrador é ajustado para o valor "1".
- **GENERALIO8 (offset: 0x128):** Semelhante ao GENERALIO7, este registrador também é configurado com o valor "1".
- **I2C0USEFPGA (offset: 0x1C1):** Este registrador é configurado para "0".

# Configuração do I2C0
Para ajustar o controlador I2C0 e permitir a comunicação com o acelerômetro ADXL345, os seguintes registradores são acessados, utilizando seus respectivos offsets:

- **I2C0_ENABLE (0x1B):** Inicialmente, o controlador I2C0 é desativado para interromper quaisquer transmissões ativas.
- **I2C0_ENABLE_STATUS (0x27):** Após a desativação, o status é verificado até que o processo seja concluído.
- **I2C0_CON (0x00):** Este registrador é ajustado para operar o I2C0 no modo mestre, utilizando endereçamento de 7 bits e operando em modo rápido (400 kbps).
- **I2C0_TAR (0x01):** Aqui é configurado o endereço do acelerômetro, desativando comandos especiais e usando endereçamento de 7 bits.
- **I2C0_FS_SCL_HCNT (0x07):** Este registrador define o período alto do sinal SCL, garantindo um tempo mínimo de 0,6 µs, além de um ajuste de 0,3 µs.
- **I2C0_FS_SCL_LCNT (0x08):** Define o período baixo do sinal SCL, assegurando um tempo mínimo de 1,3 µs, também com o ajuste adicional de 0,3 µs.
- **I2C0_ENABLE (0x1B):** Após todas as configurações, o controlador I2C0 é reativado para iniciar a comunicação.
- **I2C0_ENABLE_STATUS (0x27):** O status é novamente verificado até que o controlador esteja pronto para operar.

# Configuração do Acelerômetro ADXL345
Uma vez que a comunicação com o acelerômetro foi estabelecida, é preciso configurá-lo. Isso envolve a modificação de diversos registradores, permitindo a personalização de parâmetros como a faixa de medição, taxa de dados e interrupções para eventos de atividade e inatividade.

## Faixa de Medição e Resolução
O registrador **DATA_FORMAT (0x31)** é responsável por configurar a faixa de medição do ADXL345. A faixa foi ajustada para ±16g, a maior disponível, o que permite ao dispositivo detectar acelerações mais intensas. Além disso, a resolução total está ativada, garantindo precisão independente da faixa selecionada.

## Taxa de Dados de Saída
A frequência de amostragem do acelerômetro é controlada pelo registrador **BW_RATE (0x2C)**, que foi configurado para uma taxa de dados de saída de 200 Hz. Isso significa que o acelerômetro fornecerá novas leituras a cada 5 ms, ideal para detecção rápida de movimentos.

## Interrupções de Atividade e Inatividade
Para otimizar a detecção de movimentos, o ADXL345 permite o uso de interrupções para indicar atividade ou inatividade.

- **THRESH_ACT (0x24):** Define o limiar para a detecção de atividade, que foi configurado em um valor baixo, 0x04, para detectar movimentos pequenos.
- **THRESH_INACT (0x25):** Define o limiar de inatividade, com um valor menor, 0x02, para detectar a ausência de movimento de forma sensível.
- **TIME_INACT (0x26):** Estabelece o tempo necessário para considerar que o dispositivo está inativo, configurado com o valor 0x02, permitindo uma resposta rápida à ausência de movimento.
- **ACT_INACT_CTL (0x27):** Controla o acoplamento AC ou DC para os sinais de atividade e inatividade. Todos os eixos foram configurados com acoplamento AC (valor 0xFF), ajustando a detecção a variações rápidas de aceleração.
- **INT_ENABLE (0x2E):** Habilita as interrupções para eventos de atividade e inatividade, o que permite ao sistema reagir quando o acelerômetro detecta movimento ou sua ausência.

## Controle de Energia
A gestão de energia é feita pelo registrador **POWER_CTL (0x2D)**. O dispositivo é inicialmente colocado em modo de standby (espera), o que garante que nenhuma medição seja realizada até que todas as configurações estejam concluídas. Uma vez configurado, o modo de medição é ativado, permitindo que o ADXL345 comece a monitorar os movimentos.

## Leitura dos Dados

Para iniciar a leitura dos eixos, um comando é enviado ao acelerômetro, solicitando acesso ao registrador de dados. O endereço inicial utilizado pelo ADXL345 é **`0x32`**, que marca o início da sequência de dados dos eixos. Esse endereço é transmitido para o registrador **`I2C0_DATA_CMD`** (offset: `0x00000004`), adicionando-se **0x400** para indicar o envio do sinal START no barramento I2C.

Em seguida, são enviados sinais de leitura para obter um total de seis bytes (dois por eixo: X, Y e Z). Cada requisição é feita através do registrador **`I2C0_DATA_CMD`**, utilizando o valor **`0x100`** para indicar a leitura.

Após o acelerômetro responder, o registrador **`I2C0_RXFLR`** (offset: `0x0000001E`) é consultado para verificar a disponibilidade de dados no buffer de recepção. Assim que os dados estiverem prontos, eles são lidos diretamente do registrador **`I2C0_DATA_CMD`**.

## Combinação dos Bits

Os dados obtidos para os três eixos (X, Y e Z) são organizados em seis bytes, com dois bytes dedicados a cada eixo:
- Um byte corresponde aos 8 bits menos significativos (**LSB**).
- O outro byte representa os 8 bits mais significativos (**MSB**).

Para combinar os dois bytes de cada eixo e formar um valor de 16 bits:
- O byte mais significativo (MSB) é deslocado 8 bits para a esquerda.
- Esse valor é então combinado com o byte menos significativo (LSB) usando uma operação de "OU" bit a bit.

## Interpretação dos Dados dos Eixos

As informações extraídas da leitura são utilizadas no código para determinar o movimento das peças. A inclinação da placa para a direita ou esquerda é interpretada com base nos valores obtidos, permitindo assim o controle das peças no jogo.

# Recursos
O projeto é implementado na placa de desenvolvimento Intel® DE1-SoC, uma plataforma versátil projetada para experimentos em organização de computadores e sistemas embarcados. A arquitetura da DE1-SoC é composta por dois principais componentes: o Hard Processor System (HPS) e o FPGA, ambos integrados no chip Cyclone® V SoC.

O HPS abriga um processador ARM Cortex A9 dual-core, que é responsável pelo processamento de dados, juntamente com uma porta de memória DDR3 e um conjunto de dispositivos periféricos. O FPGA, por sua vez, implementa dois processadores Intel Nios® II, além de várias portas periféricas, como memória, módulos de temporização, áudio, vídeo, PS/2, conversão analógica-digital, e portas paralelas conectadas a interruptores e luzes.

Para o desenvolvimento do software, foi utilizada a linguagem de programação C, além do Visual Studio Code (VSCode), um editor de código-fonte que oferece suporte a várias linguagens de programação.

# Testes
Para garantir a funcionalidade e a eficácia do sistema, foram realizados os seguintes testes respectivamente:

| Ação                                        | Resultado                                                  |
|---------------------------------------------|-----------------------------------------------------------|
| Botão 1                                      | Iniciar o jogo.                                          |
| Inclinar a placa para a direita             | Mover a peça para a direita.                             |
| Inclinar a placa para a esquerda            | Mover a peça para a esquerda.                            |
| Deixar a placa centralizada                 | Fazer a peça cair sem alterar seu movimento.            |
| Botão 0                                      | Pausar o jogo.                                          |
| Botão 0                                     | Continuar o jogo.                                       |
| Formar uma linha completa                   | Quebrar a linha e somar 100 pontos.                     |
| Permitir que uma peça chegue ao topo da tela | Game over.     

### Iniciar o jogo  
<img src="media/initgame.gif" alt="Iniciar o jogo" width="270"/>

### Mover peça para a esquerda  
<img src="media/left.gif" alt="Mover peça para a esquerda" width="270"/>

### Peça centralizada  
<img src="media/center.gif" alt="Peça centralizada" width="270"/>

### Mover peça para a direita  
<img src="media/right.gif" alt="Mover peça para a direita" width="270"/>

### Pausa no jogo  
<img src="media/pause.gif" alt="Pausa no jogo" width="270"/>

### Pontuação  
<img src="media/score.gif" alt="Pontuação" width="270"/>

### Game over  
<img src="media/gameover.gif" alt="Game over" width="270"/>

# Conclusão
Para a realização deste projeto, foi fundamental aplicar conhecimentos de interação hardware-software, a fim de desenvolver um jogo funcional. Também foi necessário compreender os princípios básicos da arquitetura da DE1-SoC, utilizando sua interface de comunicação I2C, saída de vídeo VGA e periféricos como botões. Ao final, o projeto atingiu os objetivos estabelecidos, proporcionando uma experiência de jogo completa.
