# Implementando um Shell para estudo

O código contigo nesse repositório não está completo e serve apenas como espaço criativo para executar a prática de programação.

## Alguns recursos presentes atualmente

* Executar comandos com argumentos
* Suporte a redirect de output (stdout e stderr)
* Expansão de asteriscos para a lista de arquivos contigos na pasta
* Suporte a argumentos com espaços (envoltos em àspas)
* Histórico de comando digitados
* Reporte de alguns erros de sintaxe e semântica, bem como problemas ao abrir arquivos usados nos redirects
* Testes rudimentares para os recursos principais (feita com asserts customizados e um executor de testes básico)

## Referências

* Tutorial que guiou o processo explorativo: [https://brennan.io/2015/01/16/write-a-shell-in-c/](https://brennan.io/2015/01/16/write-a-shell-in-c/)
* [https://www.geeksforgeeks.org/fork-system-call/](https://www.geeksforgeeks.org/fork-system-call/)
* [https://www.includehelp.com/c/process-identification-pid_t-data-type.aspx](https://www.includehelp.com/c/process-identification-pid_t-data-type.aspx)
* [https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-waitpid-wait-specific-child-process-end](https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-waitpid-wait-specific-child-process-end)
* [https://medium.com/@nimeshneema/typedef-function-in-c-programming-language-968573e4cfc1](https://medium.com/@nimeshneema/typedef-function-in-c-programming-language-968573e4cfc1)
* [https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html](https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html)
* [https://c-for-dummies.com/blog/?p=3246](https://c-for-dummies.com/blog/?p=3246)
* [https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program](https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program)
* [https://stackoverflow.com/questions/69689586/dt-reg-undeclared-even-when-using-dirent-h-header-file-in-function](https://stackoverflow.com/questions/69689586/dt-reg-undeclared-even-when-using-dirent-h-header-file-in-function)
* [https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html](https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html)
* [https://stackoverflow.com/questions/8516823/redirecting-output-to-a-file-in-c](https://stackoverflow.com/questions/8516823/redirecting-output-to-a-file-in-c)
* [https://stackoverflow.com/questions/5517913/redirecting-stdout-to-file-after-a-fork](https://stackoverflow.com/questions/5517913/redirecting-stdout-to-file-after-a-fork)
* [https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html](https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html)
* [https://vt100.net/docs/vt100-ug/chapter3.html#ED](https://vt100.net/docs/vt100-ug/chapter3.html#ED)
* [https://en.wikipedia.org/wiki/VT100](https://en.wikipedia.org/wiki/VT100)
* [https://vt100.net/docs/vt100-ug/chapter3.html#CUP](https://vt100.net/docs/vt100-ug/chapter3.html#CUP)
* [https://stackoverflow.com/questions/2876357/determine-the-line-of-code-that-causes-a-segmentation-fault](https://stackoverflow.com/questions/2876357/determine-the-line-of-code-that-causes-a-segmentation-fault)
* [https://stackoverflow.com/questions/1079832/how-can-i-configure-my-makefile-for-debug-and-release-builds](https://stackoverflow.com/questions/1079832/how-can-i-configure-my-makefile-for-debug-and-release-builds)
* [https://stackoverflow.com/questions/12633039/shift-elements-in-array](https://stackoverflow.com/questions/12633039/shift-elements-in-array)
* [https://vt100.net/docs/vt100-ug/chapter3.html#DSR](https://vt100.net/docs/vt100-ug/chapter3.html#DSR)
* [https://vt100.net/docs/vt100-ug/chapter3.html#CPR](https://vt100.net/docs/vt100-ug/chapter3.html#CPR)
* [https://www.rozmichelle.com/pipes-forks-dups/](https://www.rozmichelle.com/pipes-forks-dups/)
* [https://www.geeksforgeeks.org/pipe-system-call/](https://www.geeksforgeeks.org/pipe-system-call/)
* [https://stackoverflow.com/questions/61114748/using-a-pipe-to-execute-cat-and-grep](https://stackoverflow.com/questions/61114748/using-a-pipe-to-execute-cat-and-grep)
* [https://www.geeksforgeeks.org/dup-dup2-linux-system-call/](https://www.geeksforgeeks.org/dup-dup2-linux-system-call/)
* [https://unix.stackexchange.com/questions/73669/what-are-the-characters-printed-when-altarrow-keys-are-pressed](https://unix.stackexchange.com/questions/73669/what-are-the-characters-printed-when-altarrow-keys-are-pressed)
* [https://stackoverflow.com/questions/276827/string-padding-in-c](https://stackoverflow.com/questions/276827/string-padding-in-c)
