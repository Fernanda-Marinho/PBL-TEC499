<h1>Sistemas Digitais</h1>
<p>(Mudar isso)
   Este projeto tem como objetivo criar um console dedicado, utilizando a plataforma de desenvolvimento DE1-SoC. A proposta é desenvolver um jogo inspirado no Tetris, integrando programação em C com interações diretas com o hardware. O controle das peças é feito por meio de um acelerômetro, que capta os movimentos do jogador, enquanto botões físicos permitem executar ações como iniciar, pausar e continuar. Além disso, a interface VGA é utlilizada para projetar o jogo em um monitor externo. 

</p>

<h1>Equipe:</h1>
<ul>
    <li>Camila de Araújo Bastos</li>
    <li>Nome 2</li>
    <li>Nome 3</li>
</ul>

<div id="mapeamento">
    <h1>Mapeamento de Memória</h1>
    <p>
        Para possibilitar o acesso ao acelerômetro (ADXL345) presente na placa DE1-SOC, é necessário mapear a memória física 
        para o espaço de endereçamento virtual. Esse processo envolve duas etapas principais.
    </p>
    <h2>Passo 1: Abertura do `/dev/mem`</h2>
    <p>
        O arquivo especial <code>/dev/mem</code>, em sistemas Linux, permite interação direta com o hardware, contornando 
        as proteções do kernel. Para acessá-lo, são concedidas permissões de leitura e escrita, além de garantir sincronização de I/O.
    </p>
    <h2>Passo 2: Mapeamento da Memória</h2>
    <p>
        Após a abertura do arquivo, o próximo passo é mapear os endereços do <strong>System Manager (SYSMGR)</strong> e 
        do controlador <strong>I2C0</strong>. Durante esse processo, define-se o endereço base, que marca o ponto de partida 
        na memória física, e o tamanho da área a ser mapeada (span), indicando a extensão da memória a ser acessada. O sistema 
        operacional atribui um endereço virtual à área mapeada, permitindo leitura e escrita. O compartilhamento entre processos 
        também é habilitado, possibilitando que diferentes partes do sistema alterem essa região de memória simultaneamente, 
        garantindo uma comunicação eficiente entre componentes.
    </p>
    <p>
        Com o mapeamento completo, deve-se configurar tanto o Pinmux quanto o controlador I2C0 para estabelecer a comunicação com o acelerômetro.
    </p>
</div>

<div id="configuracao-pinmux">
    <h1>Configuração do Pin Mux</h1>
    <p>
        Depois de mapear a base de endereços, os valores dos offsets de cada registrador são somados à base para acessar regiões 
        específicas e realizar as configurações necessárias. A seguir estão os registradores essenciais para garantir o roteamento 
        correto dos sinais I2C para o controlador I2C0:
    </p>
    <ul>
        <li><strong>GENERALIO7 (offset: 0x127):</strong> Este registrador é ajustado para o valor "1".</li>
        <li><strong>GENERALIO8 (offset: 0x128):</strong> Semelhante ao GENERALIO7, este registrador também é configurado com o valor "1".</li>
        <li><strong>I2C0USEFPGA (offset: 0x1C1):</strong> Este registrador é configurado para "0".</li>
    </ul>
</div>

<div id="configuracao-i2c0">
    <h1>Configuração do I2C0</h1>
    <p>
        Para ajustar o controlador I2C0 e permitir a comunicação com o acelerômetro ADXL345, os seguintes registradores são acessados, utilizando seus respectivos offsets:
    </p>
    <ul>
        <li><strong>I2C0_ENABLE (0x1B):</strong> Inicialmente, o controlador I2C0 é desativado para interromper quaisquer transmissões ativas.</li>
        <li><strong>I2C0_ENABLE_STATUS (0x27):</strong> Após a desativação, o status é verificado até que o processo seja concluído.</li>
        <li><strong>I2C0_CON (0x00):</strong> Este registrador é ajustado para operar o I2C0 no modo mestre, utilizando endereçamento de 7 bits e operando em modo rápido (400 kbps).</li>
        <li><strong>I2C0_TAR (0x01):</strong> Aqui é configurado o endereço do acelerômetro, desativando comandos especiais e usando endereçamento de 7 bits.</li>
        <li><strong>I2C0_FS_SCL_HCNT (0x07):</strong> Este registrador define o período alto do sinal SCL, garantindo um tempo mínimo de 0,6 µs, além de um ajuste de 0,3 µs.</li>
        <li><strong>I2C0_FS_SCL_LCNT (0x08):</strong> Define o período baixo do sinal SCL, assegurando um tempo mínimo de 1,3 µs, também com o ajuste adicional de 0,3 µs.</li>
        <li><strong>I2C0_ENABLE (0x1B):</strong> Após todas as configurações, o controlador I2C0 é reativado para iniciar a comunicação.</li>
        <li><strong>I2C0_ENABLE_STATUS (0x27):</strong> O status é novamente verificado até que o controlador esteja pronto para operar.</li>
    </ul>
</div>

<div id="configuracao-adxl345">
    <h1>Configuração do Acelerômetro ADXL345</h1>
    <p>
        Uma vez que a comunicação com o acelerômetro foi estabelecida, é preciso configurá-lo. Isso envolve a modificação de diversos registradores, permitindo a personalização de parâmetros como a faixa de medição, taxa de dados e interrupções para eventos de atividade e inatividade.
    </p>
    <h2>Faixa de Medição e Resolução</h2>
    <p>
        O registrador <strong>DATA_FORMAT (0x31)</strong> é responsável por configurar a faixa de medição do ADXL345. A faixa foi ajustada para ±16g, a maior disponível, o que permite ao dispositivo detectar acelerações mais intensas. Além disso, a resolução total está ativada, garantindo precisão independente da faixa selecionada.
    </p>
    <h2>Taxa de Dados de Saída</h2>
    <p>
        A frequência de amostragem do acelerômetro é controlada pelo registrador <strong>BW_RATE (0x2C)</strong>, que foi configurado para uma taxa de dados de saída de 200 Hz. Isso significa que o acelerômetro fornecerá novas leituras a cada 5 ms, ideal para detecção rápida de movimentos.
    </p>
    <h2>Interrupções de Atividade e Inatividade</h2>
    <p>
        Para otimizar a detecção de movimentos, o ADXL345 permite o uso de interrupções para indicar atividade ou inatividade.
    </p>
    <ul>
        <li><strong>THRESH_ACT (0x24):</strong> Define o limiar para a detecção de atividade, que foi configurado em um valor baixo, 0x04, para detectar movimentos pequenos.</li>
        <li><strong>THRESH_INACT (0x25):</strong> Define o limiar de inatividade, com um valor menor, 0x02, para detectar a ausência de movimento de forma sensível.</li>
        <li><strong>TIME_INACT (0x26):</strong> Estabelece o tempo necessário para considerar que o dispositivo está inativo, configurado com o valor 0x02, permitindo uma resposta rápida à ausência de movimento.</li>
        <li><strong>ACT_INACT_CTL (0x27):</strong> Controla o acoplamento AC ou DC para os sinais de atividade e inatividade. Todos os eixos foram configurados com acoplamento AC (valor 0xFF), ajustando a detecção a variações rápidas de aceleração.</li>
        <li><strong>INT_ENABLE (0x2E):</strong> Habilita as interrupções para eventos de atividade e inatividade, o que permite ao sistema reagir quando o acelerômetro detecta movimento ou sua ausência.</li>
    </ul>
    <h2>Controle de Energia</h2>
    <p>
        A gestão de energia é feita pelo registrador <strong>POWER_CTL (0x2D)</strong>. O dispositivo é inicialmente colocado em modo de standby (espera), o que garante que nenhuma medição seja realizada até que todas as configurações estejam concluídas. Uma vez configurado, o modo de medição é ativado, permitindo que o ADXL345 comece a monitorar os movimentos.
    </p>
</div>
<div id="recursos-utilizados">
    <h1>Recursos</h1>
    <p>
        O projeto é implementado na placa de desenvolvimento Intel® DE1-SoC, uma plataforma versátil projetada para experimentos em organização de computadores e sistemas embarcados. 
        A arquitetura da DE1-SoC é composta por dois principais componentes: o Hard Processor System (HPS) e o FPGA, ambos integrados no chip Cyclone® V SoC.
    </p>
    <p>
        O HPS abriga um processador ARM Cortex A9 dual-core, que é responsável pelo processamento de dados, juntamente com uma porta de memória DDR3 e um conjunto de dispositivos periféricos. 
        O FPGA, por sua vez, implementa dois processadores Intel Nios® II, além de várias portas periféricas, como memória, módulos de temporização, áudio, vídeo, PS/2, conversão analógica-digital, 
        e portas paralelas conectadas a interruptores e luzes.
    </p>
    <p>
        Para o desenvolvimento do software, foi utilizada a linguagem de programação C, além do Visual Studio Code (VSCode), um editor de código-fonte que oferece suporte a várias linguagens de programação.
    </p>
</div>
