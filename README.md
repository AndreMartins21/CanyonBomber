# CANYON BOMBER

### Autor: André Augusto Moreira Martins

<p>Trata-se do projeto final da disciplina PDS 1 (Programação e Desenvolvimento de Software I) ofertada pelo DCC-UFMG.</p>
<p>Objetivo: Utilizar os conhecimentos adquiridos em sala de aula acerca da Linguagem C, para fazer algo parecido com o jogo Canyon Bomber, publicado pela Atari.</p>


## Tecnologias utilizadas:
- Linguagem C
- Biblioteca [allegro5](https://liballeg.org/a5docs/trunk/)


## Como rodar o projeto?
- Para compilação dos códigos, basta rodar o comando `make` no terminal.
- PS: Fiz todo o projeto baseando-me na distro linux Ubuntu, impossibilitando de rodar em Windows / MacBook ou outras distos;


## Modo de vitória:
- Caso um jogador não acerte 3x nenhum alvo, perderá o jogo automaticamente;
- Caso acabe a quantidade de alvos ativos, o jogador com o maior `score` será o vencedor;

## Como jogar?
- A velocidade e direção das naves não é possível controlar;
- Para atirar com a nave1, basta apertar `a`;
- Para atirar com a nave2, basta apertar `space`;

## Funcionamento do jogo:
- Toda a lógica do jogo concentra-se no arquivo `main.c`, no método main. Para manter uma melhor organização na estrutura do código, implementei uma sequência de métodos com nomes claros e objetivos, que fazem parte da dinâmica do jogo, são eles:
- Inicialização de variáveis do Allegro;
- Criação das naves;
- Criação da matriz contendo os alvos;
- Enquanto o jogo estiver ativo, fará:
- Desenha cenário e grid;
- Atualiza valores das naves e do tiro de cada uma;
- Desenha nave1 e nave2;
- Escreve o placar na parte superior;
- Checa colisão do tiro com algum alvo;
- Checa se algum jogador venceu (Se a vida de algum jogador acabar, ou acabar os alvos na tela);
- Printa vencedor;
- Printa histórico de partidas e a última partida;
