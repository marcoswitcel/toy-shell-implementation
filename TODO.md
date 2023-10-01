# Ideias não filtradas

* ler arquivo de configuração?
* executar comandos é um requisito, passar argumentos vai junto -- ok
* implementar suporte para comandos com àspas ao redor -- ok (embora ainda possua casos aonde pode apresentar erro, ambiguidade ou comportamento inapropriado)
* implementar o redirecionamento de output input -- parcielmente
* mais comandos builtin? -- em progresso
* globing e patter matching -- em progresso
* implementar limpeza de terminal com crtl + l -- ok
* fazer algum tutorial sobre interpretadores de AST para ter alguma base para implementar um interpretador aqui -- não iniciado
* acessar o último comando digitado apertando pra cima -- ok
* emitir caracteres coloridos -- ok


## Anotações 15/08/2023

Talvez eu dê um tempo desse projeto, talvez não, mas em todo caso vou deixar anotado aqui alguns pontos.
* Pensei em substituir a chamada da macro assert, dentro dos testes, por alguma macro que colete métricas e tenha uma API mais
intuitiva, por exemplo, aceitar uma mensagem que descreve qual é o teste sendo feito e possa ser retornado em caso de erro.
* No shell em si seria interessante deixar o arquivo "main.c" apenas com a chamada para inicializar o shell.
* Na rotina `shell_wait_command_input` tem bastante duplicidade e complexidade desnecessária.
* Mensurar o tempo de algumas funções [https://www.tutorialspoint.com/how-to-measure-time-taken-by-a-function-in-c](https://www.tutorialspoint.com/how-to-measure-time-taken-by-a-function-in-c)

# Bugs

* problema quando input e output iguais (aparentemente isso ocorre no bash também, é um alerta do próprio cat, possivelmente esse bug seja um alarme falso)
```bash
|>cat a.txt > a.txt
cat: a.txt: input file is output file
```
* unicode (utf8) ainda não é suportado, mas quando coloco um comando com caracteres especiais entra em loop
```bash
|>echo "olá joão" > output.txt
```
* segfault acontecendo ao tentar exibir mensagens de parse com erro -- FIXADO
```bash
|>echos
Internal: Processo filho não pode executar o programa alvo.
|>^L
```
* após rodar uma comando que resulte na mensagem  de erro interno acima, crt + l seguintes são emitidos como texto ao invés de ser processados. Acredito que alguma sequência de escape ficou pela mentade. -- FIXADO


## Onde parei

Onde parei: "Wrap up ...".
[https://brennan.io/2015/01/16/write-a-shell-in-c/](https://brennan.io/2015/01/16/write-a-shell-in-c/)
