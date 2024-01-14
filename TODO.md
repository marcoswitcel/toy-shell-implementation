# Ideias não filtradas

* executar comandos é um requisito, passar argumentos vai junto -- ok
* implementar limpeza de terminal com crtl + l -- ok
* acessar o último comando digitado apertando pra cima -- ok
* emitir caracteres coloridos -- ok
* implementar atalhos como ctrl-a e ctrl-e pra navegar do início ao final do input -- ok
* crtl + d para fechar -- ok
* ctrl+c para interromper digitação? -- ok
* Implementar redirect de stdin (PIPE) -- ok
* Implementar e testar redirect do stdout e stderror antes de seguir com as PIPEs de stdin -- ok
* implementar uma função de ordenação e aplicar na lista de diretórios retornada pela expansão do asterísco -- ok
* implementar o redirecionamento de output input -- ok
* implementar pipe de um processo pra outro -- ok
* suporte a crtl+arrow_left e crtl+arrow_right -- ok
* incluir a expressão "$?" (permite consultar status do último comando) -- ok
* não permito enviar ctrl-c do terminal para um programa rodando, por exemplo: depois de iniciar o comando "tail -f a.txt" no terminal, não consigo mais interrompê-lo. -- ok
* implementar suporte para comandos com àspas ao redor -- ok (embora ainda possua casos aonde pode apresentar erro, ambiguidade ou comportamento inapropriado)
* ler arquivo de configuração?
* mais comandos builtin? -- em progresso
* globing e patter matching -- em progresso
* fazer algum tutorial sobre interpretadores de AST para ter alguma base para implementar um interpretador aqui -- não iniciado
* implementar mais suporte a cores (cores no erros?)
* suporte tab com autocomplete
* * Elaborando o ponto acima: Apenas habilitar a duplicação do STDIN não resolve, preciso desabilitar as flags pra não emitir os sinais senão o processo filho também não recebe, no caso preciso restaurar o terminal ao estado "normal" e depois voltar ao modo canônico no final. Desta forma o ctrl-c consegue chegar ao processo filho, porém o processo pai também encerra, o que posso fazer é cadastrar um handler vazio pro ctrl-c e pedir pra não encerrar. O problema é que não gostei dessa solução, tem muitas partes que precisam funcionar em conjunto, pelo menos é o que parece. Preciso tentar simplificar essa solução.
Posso criar um stdin separado para o processo filho e duplicar meu input pra lá? posso mandar apenas ctrl-c incialmente? ou posso continuar usando o mesmo para os dois só acertar as flags e os handlers
    https://stackoverflow.com/questions/4217037/catch-ctrl-c-in-c
    https://stackoverflow.com/questions/34036642/reading-from-stdin-by-multiple-processes
* implementar suporte a inputs de multiplas linhas, permitindo navegar e reportando erros de forma clara
* uma funcionalidade interessante para os testes seria poder agrupar testes em alguma estrura de "subcategoria"
* permitir trocar o símbolo de "input", se for possível trocar durante execução, melhor ainda
* adicionar builtin que permite trocar em tempo de execução parâmetros do terminal
* adicionar suporte a busca na history, exemplo: 2! execute comando 2


## Anotações 15/08/2023

Talvez eu dê um tempo desse projeto, talvez não, mas em todo caso vou deixar anotado aqui alguns pontos.
* Pensei em substituir a chamada da macro assert, dentro dos testes, por alguma macro que colete métricas e tenha uma API mais
intuitiva, por exemplo, aceitar uma mensagem que descreve qual é o teste sendo feito e possa ser retornado em caso de erro.
* No shell em si seria interessante deixar o arquivo "main.c" apenas com a chamada para inicializar o shell. -- ok, mas preciso novamente tirar coisa de lá
* Na rotina `shell_wait_command_input` tem bastante duplicidade e complexidade desnecessária. -- ok, parcialmente ajustado e monitorando
* Mensurar o tempo de algumas funções [https://www.tutorialspoint.com/how-to-measure-time-taken-by-a-function-in-c](https://www.tutorialspoint.com/how-to-measure-time-taken-by-a-function-in-c) -- em progresso para fins de prática

## Anotações 06/10/2023

# Bugs

* problema quando input e output iguais (aparentemente isso ocorre no bash também, é um alerta do próprio cat, possivelmente esse bug seja um alarme falso. Interessante é que no bash o comando cat com um redirect simples geralmente funciona, esse só falha quando é um redirect de append, e só se o arquivo já existir e não estiver vazio, aparentemente)
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
* não está processando corretamente && depois de uma pipe -- FIXADO
```bash
|>echo "teste com &&" && echo "super mario" | grep "super" > teste-complexo.txt && echo "depois"
teste com &&
|>^C
```
* se o usuário digitar apenas um token reservado e tentar rodar a aplicação não reporta o erro correto -- OK
```bash
|>   &&
  -----^
  Problema: Nenhum comando encontrado, possivelmente por se tratar de uma linha apenas com espaços.
```
* se o usuário digitar apenas "&&" e dar um espaço o input passa na tokenização mas não gera um nó válido, infelizmente o sistema tenta executar esse nó possivelmente contendo valores nulos e crasha -- OK
```bash
|>   && 
main: src/./shell.c:729: shell_execute_command: Assertion `false && "Não deveria ser atingido nunca"' failed.

```


## Onde parei

* Onde parei: "Wrap up ..." [https://brennan.io/2015/01/16/write-a-shell-in-c/](https://brennan.io/2015/01/16/write-a-shell-in-c/)
* Ler https://www.rozmichelle.com/pipes-forks-dups/#pipelines -- em progresso (Parei no subtítulo: Running Commands in a Pipeline)