# Godot + Lua

Essa GDExtension tem como intenção ser uma ponte extremamente simples entre o GDScript e Lua, permitindo a execução de código Lua.

Essa extensão é muito limitada e possibilita apenas que o script Godot seja notificado quando um método é executado, quando a função "print" é chamada, e quando ocorrem erros de execução.
Ela também recria a máquina de estado do Lua toda a vez que um script é executado, o que pode ser prejudicial à performance.

Essa extensão provavelmente será incrementada no futuro, conforme a necessidade surge.

# Como compilar

Pra compilar a extensão, é necessário ter:

- Um compilador C++;
- A ferramenta de build `Scons`;

Na pasta raíz da extensão, rodar o comando

```bash
scons platform=%PLATFORM%
```

Onde *%PLATFORM%* corresponde ao sistema operacional desejado. As opções disponíveis são `windows`, `linux`, `maxcos`, `android` e `ios`. Para mais detalhes sobre as plataformas suportadas, consulte a [documentação sobre GDExtension do Godot](https://docs.godotengine.org/en/stable/tutorials/scripting/gdextension/index.html).

Após o termino da compilação (pode demorar um pouco), a extensão estará pronta para ser utilizada e os arquivos estarão dispostos na raíz do projeto.

# Como usar

Para utilizar essa extensão, é necessário que o arquivo `luaconsole.gdextension` e os arquivos `libluaconsole` estejam presentes na pasta `bin/luaconsole/` em relação à pasta raíz do projeto Godot.

Depois de posicionar os arquivos, ao abrir o projeto, será possível adicionar um node `LuaConsole`, o qual permitirá a interação com Lua através dos métodos que expõe.

## Métodos expostos:

- `bind_method`: Vincula um método de escuta do lado do Lua. Quando um script Lua chamar algum método vinculado, um sinal será emitido pela extensão.
- `run_script`: Executa o script Lua provido como argumento. Esse método não retorna nada.

## Sinais

- `error`: Sinal emitido quando ocorre um erro na execução do script. Fornece o argumento `message` com a mensagem de erro.
- `print`: Sinal emitido quando o script Lua chama a função `print`. Fornece o argumento `value` com o valor a ser exibido.
- `called_method`: Sinal emitido quando o script Lua chama algum dos métodos vinculados através do método `bind_method`. Fornece o argumento `method` contendo o nome do método.

# Futuro

Algumas coisas que desejo implementar no futuro:

- Retorno de parâmetros passados aos métodos vinculados;
- Permitir execução linha a linha do script, com notificação das linhas;
- Permitir breakpoints no código Lua;
  - Permitir inspeção de variáveis durante depuração; 
