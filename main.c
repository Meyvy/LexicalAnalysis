#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define LIMIT 256        // defined limit for string and keywords and identifiers
#define NUM_LIMIT 25     // limist for numbers which are 64 bit
#define NONE "$NO_VALUE" // the defualt no value for tokens that have no value

unsigned int LINE = 1;   // to keep track of lines
typedef enum Token_Type  // token types
{
    COLON,
    SEMI_COLON,
    IDENTIFIER,
    KEY_WORD,
    INT,
    CHAR,
    STR,
    BOOL,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVISION,
    POWER,
    REMAINDER,
    EQUAL,
    ASSIGN,
    NOT,
    NOT_EQUAL,
    SMALLER,
    GREATER,
    AND,
    OR,
    INCREMENT,
    DECREMENT,
    COMMA,
    SMALLER_EQUAL,
    GREATER_EQUAL,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_CURLY_BRACKET,
    RIGHT_CURLY_BRACKET,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
} Token_Type;

typedef union Value // value for tokens
{
    unsigned long long int number;
    char *string;
} Value;

typedef struct Token // struct for token
{
    Token_Type token_type;
    Value token_value;
    unsigned int line;
} Token;

const char *TOKEN_DESCRIBTION[] = // describtion for tokens
    {
        "colon", "semicolon", "identifier", "key word", "number", "character", "string", "boolean", "plus", "minus", "multiply", "division", "power",
        "remainder", "equal", "assign", "not", "not equal", "smaller than", "greater than", "and", "or", "increment", "decrement", "comma",
        "left bracket", "right bracket", "left curly bracket", "right curly bracket", "left paranthesis", "right paranthesis",

        "smaller than or equal", "greater than or equal"};

const char *RESERVED[] = // reserved words
    {
        "array", "boolean", "char", "else", "false", "for", "function", "if",
        "integer", "print", "return", "string", "true", "void", "while"};

int is_reserved(char *buffer) // is reserved
{
    int n = sizeof(RESERVED) / sizeof(RESERVED[0]);
    for (int i = 0; i < n; i++)
    {
        if (!strcmp(buffer, RESERVED[i]))
        {
            return 1;
        }
    }
    return 0;
}

short int check_symbol(char a) // check for symbol
{
    if (a == '(' || a == ')' || a == '{' || a == '}' || a == '[' || a == ']' || a == '-' || a == '+' || a == '*' || a == '/' || a == '^' || a == '<' || a == '>' || a == '!' || a == '&' || a == '|' || a == '%' || a == ',' || a == ':' || a == ';' || a == '=')
    {
        return 1;
    }
    return 0;
}

unsigned short int is_bool(char *buffer) // check for bool
{
    if (!strcmp("true", buffer))
    {
        return 1;
    }
    else if (!strcmp("false", buffer))
    {
        return 0;
    }
    else
    {
        return 2;
    }
}

void error() // for error checking
{
    printf("invalid input!");
    exit(0);
}

void print_token(Token *token) // prints tokens
{
    printf("[type:%s,", TOKEN_DESCRIBTION[token->token_type]);
    if (token->token_type == INT || token->token_type == BOOL)
    {
        printf("value:%llu,]", token->token_value.number);
    }
    else
    {
        printf("value:%s,", token->token_value.string);
    }
    printf("line:%u]\n", token->line);
}

void skip_white_space(FILE *fp) // ignores whitespace
{
    char c;
    while (1)
    {
        c = fgetc(fp);
        if (c == '\t' || c == ' ' || c == '\n')
        {
            if (c == '\n')
                LINE++;
            continue;
        }
        else
        {
            ungetc(c, fp);
            break;
        }
    }
}

short int skip_comment(FILE *fp) // ignore comments
{
    char c;
    c = fgetc(fp);
    if (c == '/')
    {
        c = fgetc(fp);
        while (c != '\n')
        {
            if (c == EOF)
            {
                ungetc(c, fp);
                return 1;
            }
            c = fgetc(fp);
        }
        LINE++;
    }
    else if (c == '*')
    {
        while (1)
        {
            c = fgetc(fp);
            if (c == '\n')
            {
                LINE++;
            }
            if (c == EOF)
            {
                ungetc(c, fp);
                return 1;
            }
            if (c == '*')
            {
                c = fgetc(fp);
                if (c == '/')
                {
                    return 1;
                }
                else
                {
                    ungetc(c, fp);
                    continue;
                }
            }
        }
    }
    else
    {
        ungetc(c, fp);
        ungetc('/', fp);
        return 0;
    }
}

void strip_all(FILE *fp) // strip all white spaces and comments
{
    char c;
    while (1)
    {
        c = fgetc(fp);
        if (c == '\t' || c == ' ' || c == '\n')
        {
            if (c == '\n')
                LINE++;
            skip_white_space(fp);
        }
        else if (c == '/')
        {
            if (!skip_comment(fp))
            {
                return;
            }
        }
        else
        {
            ungetc(c, fp);
            return;
        }
    }
}

void get_num(FILE *fp, Value *token_value) // create a number token
{
    char buffer[NUM_LIMIT];
    memset((void *)buffer, '\0', sizeof(buffer));
    char c;
    short unsigned int i = 0;
    while (1)
    {
        c = fgetc(fp);
        if (i >= NUM_LIMIT)
        {
            error();
        }
        if (isdigit(c))
        {
            buffer[i++] = c;
        }
        else if (isalpha(c) || c == '_')
        {
            error();
        }
        else
        {
            ungetc(c, fp);
            break;
        }
    }
    token_value->number = strtoull(buffer, NULL, 10);
}

void get_identifier_keyword_bool(FILE *fp, Token_Type *token, Value *token_value)
{
    char buffer[LIMIT];
    memset((void *)buffer, '\0', sizeof(buffer));
    char c;
    short unsigned int i = 0;
    while (1)
    {
        c = fgetc(fp);
        if (i >= LIMIT)
        {
            error();
        }
        if (c == '_' || isdigit(c) || isalpha(c))
        {
            buffer[i++] = c;
            continue;
        }
        else
        {
            ungetc(c, fp);
            break;
        }
    }
    i = is_bool(buffer);
    if (i != 2)
    {
        *token = BOOL;
        token_value->number = i;
        return;
    }
    token_value->string = (char *)malloc(sizeof(char) * strlen(buffer));
    strcpy(token_value->string, buffer);
    if (is_reserved(buffer))
    {
        *token = KEY_WORD;
    }
    else
    {
        *token = IDENTIFIER;
    }
}

void get_char_str(FILE *fp, Token_Type *token, Value *token_value)
{
    char buffer[LIMIT];
    memset((void *)buffer, '\0', sizeof(buffer));
    char c;
    char d;
    unsigned short int i = 0;
    c = fgetc(fp);
    buffer[i++] = c;
    if (c == '\'')
    {
        *token = CHAR;
        c = fgetc(fp);
        if (c == '\'')
        {
            error();
        }
        if (c == EOF)
        {
            error();
        }
        if (c == '\\')
        {
            buffer[i++] = c;
            c = fgetc(fp);
            if (c == '\\' || c == '0' || c == 'n' || c == '\'')
            {
                buffer[i++] = c;
            }
            else
            {
                error();
            }
        }
        buffer[i++] = c;
        c = fgetc(fp);
        if (c != '\'')
        {
            error();
        }
        buffer[i++] = c;
        token_value->string = (char *)malloc(sizeof(char) * strlen(buffer));
        strcpy(token_value->string, buffer);
    }
    else
    {
        *token = STR;
        while (1)
        {
            c = fgetc(fp);
            if (c == EOF || c == '\n')
            {
                error();
            }
            if (i >= LIMIT)
            {
                error();
            }
            buffer[i++] = c;
            if (c == '"')
            {
                break;
            }
        }
        token_value->string = (char *)malloc(sizeof(char) * i);
        strcpy(token_value->string, buffer);
    }
}

void get_symbol(FILE *fp, Token_Type *token) // get symbol
{
    char c;
    c = fgetc(fp);
    if (c == '(')
    {
        *token = LEFT_PARENTHESIS;
    }
    else if (c == ')')
    {
        *token = LEFT_PARENTHESIS;
    }
    else if (c == '{')
    {
        *token = LEFT_CURLY_BRACKET;
    }
    else if (c == ':')
    {
        *token = COLON;
    }
    else if (c == ';')
    {
        *token = SEMI_COLON;
    }
    else if (c == ',')
    {
        *token = COMMA;
    }
    else if (c == '}')
    {
        *token = RIGHT_CURLY_BRACKET;
    }
    else if (c == '[')
    {
        *token = LEFT_BRACKET;
    }
    else if (c == ']')
    {
        *token = RIGHT_BRACKET;
    }
    else if (c == '*')
    {
        *token = MULTIPLY;
    }
    else if (c == '/')
    {
        *token = DIVISION;
    }
    else if (c == '^')
    {
        *token = POWER;
    }
    else if (c == '%')
    {
        *token = REMAINDER;
    }
    else if (c == '=')
    {
        c = fgetc(fp);
        if (c == '=')
        {
            *token = EQUAL;
        }
        else
        {
            ungetc(c, fp);
            *token = ASSIGN;
        }
    }
    else if (c == '+')
    {
        c = fgetc(fp);
        if (c == '+')
        {
            *token = INCREMENT;
        }
        else
        {
            ungetc(c, fp);
            *token = PLUS;
        }
    }
    else if (c == '-')
    {
        c = fgetc(fp);
        if (c == '-')
        {
            *token = DECREMENT;
        }
        else
        {
            ungetc(c, fp);
            *token = MINUS;
        }
    }
    else if (c == '!')
    {
        c = fgetc(fp);
        if (c == '=')
        {
            *token = NOT_EQUAL;
        }
        else
        {
            ungetc(c, fp);
            *token = NOT;
        }
    }
    else if (c == '<')
    {
        c = fgetc(fp);
        if (c == '=')
        {
            *token = SMALLER_EQUAL;
        }
        else
        {
            ungetc(c, fp);
            *token = SMALLER;
        }
    }
    else if (c == '>')
    {
        c = fgetc(fp);
        if (c == '=')
        {
            *token = GREATER_EQUAL;
        }
        else
        {
            ungetc(c, fp);
            *token = GREATER;
        }
    }
    else if (c == '&')
    {
        c = fgetc(fp);
        if (c == '&')
        {
            *token = AND;
        }
        else
        {
            error();
        }
    }
    else if (c == '|')
    {
        c = fgetc(fp);
        if (c == '|')
        {
            *token = OR;
        }
        else
        {
            error();
        }
    }
}

unsigned short int next_token(FILE *fp, Token_Type *token, Value *token_value) // get next token
{
    strip_all(fp);
    char c;
    c = fgetc(fp);
    if (c == EOF)
    {
        return 0;
    }
    else if (isdigit(c))
    {
        ungetc(c, fp);
        (*token) = INT;
        get_num(fp, token_value);
        return 1;
    }
    else if (c == '_' || isalpha(c))
    {
        ungetc(c, fp);
        get_identifier_keyword_bool(fp, token, token_value);
        return 1;
    }
    else if (c == '\'' || c == '"')
    {
        ungetc(c, fp);
        get_char_str(fp, token, token_value);
        return 1;
    }
    else if (check_symbol(c))
    {
        ungetc(c, fp);
        get_symbol(fp, token);
        token_value->string = (char *)malloc(sizeof(char) * strlen(NONE));
        strcpy(token_value->string, NONE);
        return 1;
    }
    else
    {
        error();
    }
}

Token **get_tokens(const char *address, int *n) // get all the tokens
{
    int size = 512;
    FILE *fp = fopen(address, "r");
    Token **con = (Token **)malloc(sizeof(Token *) * size);
    Token **temp;
    unsigned int i = 0;
    Token_Type token;
    Value token_value;
    int check;
    while (!feof(fp))
    {
        check = next_token(fp, &token, &token_value);
        if (check)
        {
            con[i] = (Token *)malloc(sizeof(Token));
            con[i]->token_type = token;
            con[i]->token_value = token_value;
            con[i]->line = LINE;
            i++;
            if (i >= size)
            {
                size *= 2;
                temp = (Token **)malloc(sizeof(Token *) * size);
                for (int j = 0; j < i; j++)
                {
                    temp[j] = con[j];
                }
                free(con);
                con = temp;
            }
        }
    }
    *n = i;
    return con;
}

int main()
{
    int m;
    char* file_name="test.txt";
    Token **mmd = get_tokens(file_name, &m);
    for (size_t i = 0; i < m; i++)
    {
        print_token(mmd[i]);
    }
}