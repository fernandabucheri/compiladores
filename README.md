# Compilador para linguagem C-

## Especificações

Módulos do compilador a serem construídos:
* Scanner (analisador léxico);
* Parser (analisador sintático);
* Gerenciador da tabela de símbolos;
* Analisador semântico (com atualização dos nós da árvore sintática);
* Gerador de código intermediário.

<br>

# Implementação 
Construiu-se todos os módulos acima citados. Utilizou-se como base o exemplo do livro "Compiladores: princípios e práticas" do autor Kenneth C. Louden e a [descrição do trabalho](https://github.com/fernandabucheri/compiladores/blob/master/Projeto%20da%20disciplina.pdf).

<br>

## Analisador léxico
Inicialmente construiu-se o módulo de análise léxica (ou scanner: sistema de varredura do compilador).

Utilizou-se a ferramenta _flex_. 

Para essa etapa é essencial criar o arquivo .l (que no nosso projeto chamamos de [cmenos.l](https://github.com/fernandabucheri/compiladores/blob/master/C%C3%B3digo/cmenos.l)). Nesse arquivo estão as palavras-chave, símbolos especiais e alguns marcadores, sendo que:

1. As palavras-chave da linguagem são as seguintes:
```
else if int return void while
```
Todas as palavras-chave são reservadas e devem ser escritas com caixa baixa.

2. Os símbolos especiais são os seguintes:
```
+ - * / < <= > >= == != = ; , () [] {} /* */
```

3. Há, ainda, os marcadores ID e NUM, definidos pelas expressões regulares a seguir:
```
ID = letra letra*
NUM = dígito dígito*
letra = a|..|z|A|..|Z
dígito = 0|..|9
```
Existe diferença entre caixa baixa e caixa alta.

4. Espaço em branco é composto por brancos, mudanças de linha e tabulações. O espaço em
branco é ignorado, exceto como separador de IDs, NUMs e palavras-chave.

5. Comentários são cercados pela notação usual de C ``` /*...*/. ``` Os comentários podem ser
colocados em qualquer lugar que possa ser ocupado por um espaço em branco (ou seja,
comentários não podem ser colocados dentro de tokens), e podem incluir mais de uma linha.
Comentários não podem ser aninhados.

<br>

## Analisador sintático e árvore de análise sintática

Posteriormente constrói-se o módulo de análise sintática (ou parser). Para a
construção do parser, utiliza-se a ferramenta _bison_. 

Para essa fase é essencial criar o arquivo .y (no nosso projeto [cmenos.y](https://github.com/fernandabucheri/compiladores/blob/master/C%C3%B3digo/cmenos.y)), pois ele é o responsável por construir a gramática em BNF para C-, tal qual:

```
1. programa → declaração-lista

2. declaração-lista → declaração-lista declaração | declaração

3. declaração → var-declaração | fun-declaração

4. var-declaração → tipo-especificador ID ; |
tipo-especificador ID [ NUM ] ;

5. tipo-especificador → int | void

6. fun-declaração → tipo-especificador ID( params ) composto-decl

7. params → param-lista | void

8. param-lista → param-lista,param | param

9. param → tipo-especificador ID | tipo-especificador ID []

10. composto-decl → { local-declarações statement-lista }

11. local-declarações → local-declarações var-declaração | vazio

12. statement-lista → statement-lista statement | vazio

13. statement → expressão-decl | composto-decl | seleção-decl |
iteração-decl | retorno-decl

14. expressão-decl → expressão ; | ;

15. seleção-decl → if ( expressão ) statement |
if ( expressão ) statement else statement

16. iteração-decl → while ( expressão ) statement

17. retorno-decl → return ; | return expressão ;

18. expressão → var = expressão | simples-expressão

19. var → ID | ID [expressão]

20. simples-expressão → soma-expressão relacional soma-expressão
| soma-expressão

21. relacional → <= | < | > | >= | == | !=

22. soma-expressão → soma-expressão soma termo | termo

23. soma → + | -

24. termo → termo mult fator | fator

25. mult → * | /

26. fator → ( expressão ) | var | ativação | NUM

27. ativação → ID ( args )

28. args → arg-lista | vazio

29. arg-lista → arg-lista,expressão | expressão
```

<br>

## Tabela de símbolos 
A tabela de símbolos está implementada no código [symtab.c](https://github.com/fernandabucheri/compiladores/blob/master/C%C3%B3digo/symtab.c). 

<br>

## Análise semântica
A análise semântica é feita através do código [analyze.c](https://github.com/fernandabucheri/compiladores/blob/master/C%C3%B3digo/analyze.c).

<br>

## Código intermediário
Nós optamos por gerar o código intermediário através do código de três endereços representado por quádruplas; 
As instruções para criá-lo estão no código [cgen.c](https://github.com/fernandabucheri/compiladores/blob/master/C%C3%B3digo/cgen.c). Já as funções que imprimem o código intermediário para o usuário estão definidas no [code.c](https://github.com/fernandabucheri/compiladores/blob/master/C%C3%B3digo/code.c).

<br>

## Para compilar e rodar
Digitar no terminal os seguintes comandos:

```
$ bison -d cmenos.y && flex cmenos.l && gcc -c *.c && gcc -o cmenos *.o -lfl

$ ./cmenos <teste>.cmenos
```

Sendo possível executar os seguintes testes:

→ Testes sem erros:
* teste1
* teste2

→ Testes com erros: 
* teste1Erro1 (com erro léxico)
* teste1Erro2 (com erro sintático)
* teste1Erro3 (com erros semânticos)
* teste2Erro1 (com erro léxico)
* teste2Erro2 (com erro sintático)
* teste2Erro3 (com erros semânticos)
