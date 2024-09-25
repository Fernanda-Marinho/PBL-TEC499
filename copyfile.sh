#!/bin/bash

IP=$1
ARQUIVO=$2
OPCAO=$3

USUARIO_FONTE="aluno@10.0.0."
#DIRETORIO_FONTE="/home/aluno/TEC499/TP04/G03/MI-SistemasDigitais-Problema-1-master/accel"
DIRETORIO_FONTE="/home/aluno/TEC499/TP04/G03/tetris-leg"
DIRETORIO_DESTINO=$(pwd)

if [ "${OPCAO}" = "0" ]; then # pegar da placa pro meu PC
    scp ${USUARIO_FONTE}${IP}:${DIRETORIO_FONTE}/${ARQUIVO} ${DIRETORIO_DESTINO}
else # jogar do meu PC para a placa
    scp ${DIRETORIO_DESTINO}/${ARQUIVO} ${USUARIO_FONTE}${IP}:${DIRETORIO_FONTE}
fi

if [ $? -eq 0 ]; then
    echo "ok"
else
    echo "not ok"
fi
