# Sistemas Digitais
*(Mudar isso)*
Este projeto tem como objetivo criar um console dedicado, utilizando a plataforma de desenvolvimento DE1-SoC. A proposta é desenvolver um jogo inspirado no Tetris, integrando programação em C com interações diretas com o hardware. O controle das peças é feito por meio de um acelerômetro, que capta os movimentos do jogador, enquanto botões físicos permitem executar ações como iniciar, pausar e continuar. Além disso, a interface VGA é utilizada para projetar o jogo em um monitor externo.

# Equipe:
- [Camila de Araújo Bastos](https://github.com/Caamilab)
- [Fernanda Marinho Silva](https://github.com/Fernanda-Marinho/)
- Nome 3

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
<img src="media/initgame.gif" alt="Iniciar o jogo" width="300"/>

### Mover peça para a esquerda  
![Mover peça para a esquerda](media/left.gif)

### Peça centralizada  
![Peça centralizada](media/center.gif)

### Mover peça para a direita  
![Mover peça para a direita](media/right.gif)

### Pausa no jogo  
![Pausa no jogo](media/pause.gif)

### Pontuação  
![Pontuação](media/score.gif)

### Game over  
![Game over](media/gameover.gif)
