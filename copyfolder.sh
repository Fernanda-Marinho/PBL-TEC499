#!/bin/bash

IP=$1
PASTA=$2
OPCAO=$3

USUARIO_FONTE="aluno@10.0.0."
DIRETORIO_FONTE="/home/aluno/TEC499/TP04/G03"
DIRETORIO_DESTINO=$(pwd)

if [ "${OPCAO}" = "0" ]; then #da placa pro PC
    scp -r ${USUARIO_FONTE}${IP}:${DIRETORIO_FONTE}/${PASTA} ${DIRETORIO_DESTINO}
else #do PC para a placa
    scp -r ${DIRETORIO_DESTINO}/${PASTA} ${USUARIO_FONTE}${IP}:${DIRETORIO_FONTE}
fi

if [ $? -eq 0 ]; then
    echo "ok"
else
    echo "not ok"
fi
