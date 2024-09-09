# Projeto 1 de MO601 - Simulador RISC-V

Veja a especificação detalhada do projeto na [página da disciplina](https://www.ic.unicamp.br/~rodolfo/mo601/projeto1)

## Identificação do Grupo

- RA: 290345
- Nome: Telles René Silva Soares

## Descrição do Projeto

- Descrição geral do seu projeto:

O projeto consiste na implementação de um simulador básico do processador RISC-V, mais especificamente da versão RV32IM, que corresponde à arquitetura de 32 bits com suporte a instruções básicas de aritmética e operações de multiplicação.
O objetivo principal é compreender melhor a operação de processadores, focando na simulação de instruções e na análise de desempenho do processador.

- Descrição do seu ambiente de desenvolvimento:
  
Virtual Box para gerar Máquina Virtual com Ubuntu 22 e GCC
RISC-V GNU Compiler Toolchain para de códigos C gerar Assembly e Binários para o RV32IM.
https://github.com/riscv-collab/riscv-gnu-toolchain
Também usei o compilador ANSI-C LabWindows/CVI para testes no PC com Windows 10.

- Descrição do seu algoritmo de simulação
  
A aplicação carrega arquivo com código binário em vetor de instruções.
Aloca recursos necessário para simulação do Core (RV32IM) e ringbuffer para salvar informações para o log conforme requisitos do projeto. Atualmente buffer esta fixado em 100 instruções para log, mas valor pode ser configurado.
Rotina executa instrução por instrução, passando pelos processos básicos de load, decode e execute, copiando dados relevantes para o log no ringbuffer:
Load: carrega instrução da memória;
Decode: decodifica os bits da instrução para os respectivos parâmentros da instrução: opcode, rd, rs1, rs2, funct3 e funct7;
Execute: de acordo com uma máscara ou comparação entre opcode, func3 e func7 converge na função e a executa; 
Foi adotado escrita em buffer para posterior escrita em arquivo devido melhor performance da cópia dos dados em memória pré alocada, ao invés de descarregar em disco durante execução da simulação.
Após todas as rotinas do código serem executadas, todas as instruções (eg 100) salvas no ringbuffer serão formatadas conforme requisito do projeto e gravadas no arquivo "log.txt".
Os scripts "rv32im_asm2bin.sh" e "rv32im_c2bin.sh" foram criados para compilar código Assembly e C para binário para o RV32IM. Note que mesmo compilando em C o assembly é gerado para ajudar no estudo e entendimento da simulação.


- Descrição de como você testou seu projeto
  
Os códigos do ACStone (031.add, 032.add, 033.add, 051.mul, 052.mul, 053.mul e 054.mul) foram compilados utilizando o script "rv32im_c2bin.sh" (por exemplo: ./rv32im_c2bin.sh 031.add, obs: não colocar a extenção ".c").
E rodados no simulador com o comando "./riscv_sim *,bin"

- Considerações gerais sobre seu aprendizado nesse projeto
O desenvolvimento de um simulador de processador RISC-V, que segue a filosofia "Reduced Instruction Set Computer", foi fundamental para o entendimento dos conceitos de arquitetura de computadores, sistemas embarcados e simulação.
A simulação de um processador proporcionou uma visão prática de como as instruções de máquina são interpretadas e executadas pelo hardware, assim como desafios como a contagem de ciclos, encapsulamento de memória e análise de logs.
