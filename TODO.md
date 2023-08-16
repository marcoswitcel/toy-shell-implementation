# Ideias não filtradas

* ler arquivo de configuração?
* executar comandos é um requisito, passar argumentos vai junto
* implementar suporte para comandos com àspas ao redor
* implementar o redirecionamento de output input
* mais comandos builtin?
* globing e patter matching
* implementar limpeza de terminal com crtl + l
* fazer algum tutorial sobre interpretadores de AST para ter alguma base para implementar um interpretador aqui


## Anotações 15/08/2023

Talvez eu dê um tempo desse projeto, talvez não, mas em todo caso vou deixar anotado aqui alguns pontos.
* Pensei em substituir a chamada da macro assert, dentro dos testes, por alguma macro que colete métricas e tenha uma API mais
intuitiva, por exemplo, aceitar uma mensagem que descreve qual é o teste sendo feito e possa ser retornado em caso de erro.
* No shell em si seria interessante deixar o arquivo "main.c" apenas com a chamada para inicializar o shell.

# Bugs

* problema quando input e output iguais
```bash
|>cat a.txt > a.txt
cat: a.txt: input file is output file
```

## Onde parei

Onde parei: "Wrap up ...".
[https://brennan.io/2015/01/16/write-a-shell-in-c/](https://brennan.io/2015/01/16/write-a-shell-in-c/)
