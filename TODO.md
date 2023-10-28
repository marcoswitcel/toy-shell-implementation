# Ideias não filtradas

* executar comandos é um requisito, passar argumentos vai junto -- ok
* implementar limpeza de terminal com crtl + l -- ok
* acessar o último comando digitado apertando pra cima -- ok
* emitir caracteres coloridos -- ok
* ler arquivo de configuração?
* implementar suporte para comandos com àspas ao redor -- ok (embora ainda possua casos aonde pode apresentar erro, ambiguidade ou comportamento inapropriado)
* implementar o redirecionamento de output input -- parcialmente
* mais comandos builtin? -- em progresso
* globing e patter matching -- em progresso
* fazer algum tutorial sobre interpretadores de AST para ter alguma base para implementar um interpretador aqui -- não iniciado
* implementar pipe de um processo pra outro (ler https://www.rozmichelle.com/pipes-forks-dups/#pipelines) -- em progresso
* implementar mais suporte a cores (cores no erros?)
* suporte a crtl+arrow_left e crtl+arrow_right
* suporte tab com autocomplete
* crtl + d para fechar -- ok
* incluir a expressão "$?" (permite consultar status do último comando)
* implementar atalhos como ctrl-a e ctrl-e pra navegar do início ao final do input -- ok
* ctrl+c para interromper digitação?
* não permito enviar ctrl-c do terminal para um programa rodando, por exemplo: depois de iniciar o comando "tail -f a.txt" no terminal, não consigo mais interrompê-lo.


## Anotações 15/08/2023

Talvez eu dê um tempo desse projeto, talvez não, mas em todo caso vou deixar anotado aqui alguns pontos.
* Pensei em substituir a chamada da macro assert, dentro dos testes, por alguma macro que colete métricas e tenha uma API mais
intuitiva, por exemplo, aceitar uma mensagem que descreve qual é o teste sendo feito e possa ser retornado em caso de erro.
* No shell em si seria interessante deixar o arquivo "main.c" apenas com a chamada para inicializar o shell. -- ok, mas preciso novamente tirar coisa de lá
* Na rotina `shell_wait_command_input` tem bastante duplicidade e complexidade desnecessária. -- ok, parcialemente ajustado e monitorando
* Mensurar o tempo de algumas funções [https://www.tutorialspoint.com/how-to-measure-time-taken-by-a-function-in-c](https://www.tutorialspoint.com/how-to-measure-time-taken-by-a-function-in-c) -- em progresso para fins de prática

## Anotações 06/10/2023
* Implementar e testar redirect do stdout e stderror antes de seguir com as PIPEs de stdin -- em progresso
* Implementar redirect de stdin (PIPE)

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
