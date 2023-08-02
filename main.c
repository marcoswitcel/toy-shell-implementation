#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <dirent.h>

// Desenvolvimento
#define DEBUG_INFO false

#include "./types.h"
#include "./list.h"
#include "./list.macro.h"
#include "./list.implementations.h"
#include "./parser.h"
#include "./builtins.c"


#define LINE_BUFFER_SIZE 1024

int shell_launch_process(char **args);

static inline void print_input_mark(const char *cstring)
{
  if (cstring)
  {
    printf("|>%s", cstring); // @note organizar reimpressão da marcação inicial
    return;
  }

  printf("|>");
}

static inline List_Of_Strings *get_all_files_for_dir(const char *path, List_Of_Strings *list_or_null, bool include_hidden)
{
  List_Of_Strings *list = list_or_null;
  if (list_or_null == NULL)
  {
    list = create_list_of_strings(128, 128);
  }

  DIR *current_dir = opendir(path);
  struct dirent *dir_entry;
  if (current_dir)
  {
    while((dir_entry = readdir(current_dir)))
    {
      if (dir_entry->d_type == DT_REG || dir_entry->d_type == DT_DIR)
      {
        if (include_hidden || dir_entry->d_name[0] != '.')
        {
          list_of_strings_push(list, copy((const char *) &dir_entry->d_name));
        }
      }
    }
    closedir(current_dir);
  }

  return list;
}

char *shell_wait_command_input(void)
{
  Buffer *buffer = create_buffer(LINE_BUFFER_SIZE, LINE_BUFFER_SIZE); // @note aqui para testar
  int c;

  print_input_mark(NULL);
  while (true)
  {
    c = getchar();

    if (c == EOF || c == '\n')
    {
      printf("\n");// @note deveria ser bufferizado do meu lado? por hora não está em "raw_mode" ainda, mas vai ficar.
      char *result = copy(buffer_ensure_null_terminated_view(buffer));

      if (DEBUG_INFO) printf("linha lida: [%s]\n", result); // @note apenas para debugar

      destroy_buffer(buffer);
      return result;
    }
    else if (c == FORM_FEED) // @note Crtl + L
    {
      builtin_clear(NULL);
      print_input_mark(buffer_ensure_null_terminated_view(buffer)); // @note organizar reimpressão da marcação inicial
    }
    else if (iscntrl(c))
    {
      if (c == BACKSPACE)
      {
        if (buffer_pop(buffer))
        {
          builtin_clear(NULL);
          print_input_mark(buffer_ensure_null_terminated_view(buffer)); // @note organizar reimpressão da marcação inicial
        }
      };
    }
    else
    {
      printf("%c", c);// @note deveria ser bufferizado do meu lado? por hora não está em "raw_mode" ainda, mas vai ficar.
      buffer_push(buffer, c); // @todo João, testar
    }
  }
}

char **shell_parse_command_into_args(const char *input_command ,const char **error)
{
  Parse_Context context = create_parse_context(input_command);
  List_Of_Strings *list_of_args = create_list_of_strings(1024, 1024);
  Sequence_Of_Tokens *tokens = parse(&context);

  if (DEBUG_INFO) printf("[[ tokens size: %d ]]\n", tokens->index);

  // Aqui a tokenização acaba e começa o análise léxica acaba e começa a análise sintática
  // após um token '>' deve sempre vir uma string, com o nome do arquivo, mas aí já é semântica
  bool has_redirec_token = false;
  bool redirect_expect_file_name = false;
  for (unsigned i = 0; i < tokens->index; i++)
  {
    Token token = tokens->sequence[i];

    if (token.type == STRING && token.data.string.cstring)
    {
      if (redirect_expect_file_name)
      {
        redirect_expect_file_name = false;
        // @todo João, aqui precisa consumir o nome do arquivo quando `redirect_expect_file_name` for `true`
      }
      else
      {
        list_of_strings_push(list_of_args, token.data.string.cstring);
      }
    }
    if (token.type == GLOBBING && token.data.globbing.cstring)
    {
      get_all_files_for_dir(".", list_of_args, false);
    }
    if (token.type == REDIRECT && token.data.redirect.cstring)
    {
      if (has_redirec_token)
      {
        *error = copy("Token > encontrado mais de uma vez");
      }
      has_redirec_token = true;
      redirect_expect_file_name = true;
    }
  }
  char **args = malloc((list_of_args->index + 1) * sizeof(char *));

  if (!args)
  {
    fprintf(stderr, "Internal: Erro de alocação");
    exit(EXIT_FAILURE);
  }

  for (unsigned i = 0; i < list_of_args->index; i++)
  {
    args[i] = (char *) list_of_args->data[i];
  }
  args[list_of_args->index] = NULL;

  // @note o bloco abaixo é apenas para visualizar o resultado
  if (DEBUG_INFO)
  {
    char **token = args;
    while (*token != NULL)
    {
      printf("argumento extraído: [%s]\n", *token);
      token++;
    }
  }
  return args;
}

#define SHELL_SPLIT_COMMAND_BUFFER_SIZE 64
#define SHELL_SPLIT_COMMAND_TOKEN_DELIMITER " \t\r\n\a"

// @note usar `shell_parse_command_into_args`
// @deprecated
char **shell_split_command_into_args(char *commands)
{
  int buffer_size = SHELL_SPLIT_COMMAND_BUFFER_SIZE;
  int position = 0;
  char **tokens = malloc(buffer_size * sizeof(char *));
  char *token;

  if (!tokens)
  {
    fprintf(stderr, "Internal: Erro de alocação");
    exit(EXIT_FAILURE);
  }

  /**
   * @note strtok retorna ponteiros para dentro da string provida, ele altera a string colocando '\0' aonde encontrar o delimitador provido.
   */
  token = strtok(commands, SHELL_SPLIT_COMMAND_TOKEN_DELIMITER);
  while (token != NULL)
  {
    tokens[position] = token;
    position++;

    if (position >= buffer_size)
    {
      buffer_size += SHELL_SPLIT_COMMAND_BUFFER_SIZE;
      tokens = realloc(tokens, buffer_size * sizeof(char *));
      if (!tokens)
      {
        fprintf(stderr, "Internal: Erro de alocação");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, SHELL_SPLIT_COMMAND_TOKEN_DELIMITER);
  }

  tokens[position] = NULL;
  // @note o bloco abaixo é apenas para visualizar o resultado
  if (DEBUG_INFO)
  {
    char **token = tokens;
    while (*token != NULL)
    {
      printf("argumento extraído: [%s]\n", *token);
      token++;
    }
  }
  return tokens;
}

int shell_launch_process(char **args)
{
  pid_t pid;
  int status;

  pid = fork();

  if (pid == 0)
  {
    // processo filho
    if (execvp(args[0], args) == -1)
    {
      printf("Internal: Processo filho não pode executar o programa alvo");
    }
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    printf("Internal: Processo filho não iniciou");
  }
  else
  {
    do
    {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

  }

  return 1;
}

Builtin_Function has_builtin_for(const char *cstring)
{
  for (int i = 0; i < number_of_builtins; i++)
  {
    if (strcmp(cstring, builtin_cstring[i]) == 0)
    {
      return builtin_func[i];
    }
  }

  return NULL;
}

int shell_execute_command(char **args)
{
  if (args[0] == NULL)
  {
    // @note De fato acontece?
    return 1; // comando vazio
  }

  Builtin_Function builtin_func = has_builtin_for(args[0]);
  if (builtin_func)
  {
    return builtin_func(args);
  }

  return shell_launch_process(args);
}

// @note Não tenho certeza de nomes, nem de estrutura ainda, mas vamos ver como flui.
void read_eval_shell_loop()
{
  char *readed_line;
  char **splitted_by_delimiter;
  int status = 0;

  do 
  {
    const char *error = NULL;
    readed_line = shell_wait_command_input();
    //splitted_by_delimiter = shell_split_command_into_args(readed_line);
    splitted_by_delimiter = shell_parse_command_into_args(readed_line, &error);
    // @note já consegue iniciar processos, por hora precisam ser com o caminho absoluto "/usr/bin/ls"
    // @note só precisava mudar para `execvp` para ele aceitar ls sem o caminho completo
    if (error == NULL)
    {
      status = shell_execute_command(splitted_by_delimiter);
    }
    else
    {
      printf("Erro ao executar o comando: %s", error);
      free((void *) error);
    }

    if (readed_line)
    {
      free(readed_line);
    }
    if (splitted_by_delimiter)
    {
      free(splitted_by_delimiter);
    }
    printf("\n");
    // @note loop infinito por enquanto
  } while (status);
}

static struct termios original_config;

void deactivate_raw_mode()
{
  if (DEBUG_INFO) printf("[[ deactivate_raw_mode ]] :: restaurando configurações de terminal.\n");
  tcsetattr(STDERR_FILENO, TCSAFLUSH, &original_config);
}

void activate_raw_mode()
{
  // @todo João, precisa restaurar antes de sair?
  tcgetattr(STDIN_FILENO, &original_config);
  atexit(deactivate_raw_mode);

  struct termios new_config = original_config;

  new_config.c_lflag &= ~(ECHO | ICANON); // sem echo e buffer de saída
  tcsetattr(STDERR_FILENO, TCSAFLUSH, &new_config);
}

int main(void)
{
  activate_raw_mode();
  // @note Aceitar argumentos pela linha de comando? que argumentos

  // @note talvez ler um arquivo de configuração? mas que configurações aceitar?

  // Read Eval Loop
  read_eval_shell_loop();

  // @note Cleanup se tiver algum para fazer

  return EXIT_SUCCESS;
}