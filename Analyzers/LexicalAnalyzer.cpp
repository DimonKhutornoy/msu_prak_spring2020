#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>
#include <algorithm>
 
using namespace std;
 
enum type_of_lex {
  LEX_NULL,                                                                                                                      /*00*/
  LEX_BOOL, LEX_BREAK, LEX_CONTINUE, LEX_DO, LEX_ELSE, LEX_FALSE, LEX_FUNCTION, LEX_IF, LEX_IN,                                  /*09*/       
  LEX_NAN, LEX_NUMBER, LEX_NULLPTR, LEX_OBJECT, LEX_RETURN, LEX_STRING, LEX_TRUE, LEX_TYPEOF, 	                                 /*17*/
  LEX_UNDEFINED, LEX_VAR, LEX_WHILE,                                                                                             /*20*/
  LEX_FIN,                                                                                                                       /*21*/
  LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_LQPAREN, LEX_RQPAREN, LEX_BEGIN, LEX_END,         /*31*/
  LEX_EQ, LEX_DEQ, LEX_TEQ, LEX_LSS, LEX_GTR, LEX_PLUS, LEX_PLUSEQ, LEX_DPLUS, LEX_MINUS, LEX_MINUS_EQ, LEX_DMINUS,              /*42*/
  LEX_TIMES, LEX_TIMES_EQ, LEX_TIMES_SLASH, LEX_SLASH, LEX_SLASH_EQ, LEX_SLASH_TIMES, LEX_DSLASH, LEX_PERCENT, LEX_PERCENT_EQ,   /*51*/
  LEX_LEQ, LEX_NOT, LEX_NEQ, LEX_NDEQ, LEX_GEQ, LEX_OR, LEX_DPIPE, LEX_AND, LEX_DAMP,                                            /*60*/
  LEX_NUM,                                                                                                                       /*61*/
  LEX_STR_CONST,                                                                                                                 /*62*/                                                                       
  LEX_ID,                                                                                                                        /*63*/ 
  POLIZ_LABEL,                                                                                                                   /*64*/
  POLIZ_ADDRESS,                                                                                                                 /*65*/
  POLIZ_GO,                                                                                                                      /*66*/
  POLIZ_FGO                                                                                                                      /*67*/
};
 

/////////////////////////  Класс Lex  //////////////////////////

class Lex
{
    type_of_lex t_lex;
    int v_lex;
    string s_lex;

public:
    Lex(type_of_lex t = LEX_NULL, int v = 0, string s = "")
        : t_lex(t)
        , v_lex(v)
        , s_lex(s)
    {
    }
    type_of_lex get_type() const
    {
        return t_lex;
    }
    int get_value() const
    {
        return v_lex;
    }
    string get_str() const
    {
        return s_lex;
    }
    friend ostream& operator<<(ostream& s, Lex l);
};

/////////////////////  Класс Ident  ////////////////////////////

class Ident
{
    string name;
    bool declare;
    type_of_lex type;
    bool assign;
    int value;

public:
    Ident()
    {
        declare = false;
        assign = false;
    }
    bool operator==(const string& s) const
    {
        return name == s;
    }
    Ident(const string n)
    {
        name = n;
        declare = false;
        assign = false;
    }
    string get_name() const
    {
        return name;
    }
    bool get_declare() const
    {
        return declare;
    }
    void put_declare()
    {
        declare = true;
    }
    type_of_lex get_type() const
    {
        return type;
    }
    void put_type(type_of_lex t)
    {
        type = t;
    }
    bool get_assign() const
    {
        return assign;
    }
    void put_assign()
    {
        assign = true;
    }
    int get_value() const
    {
        return value;
    }
    void put_value(int v)
    {
        value = v;
    }
};

//////////////////////  TID  ///////////////////////

vector<Ident> TID;

int put(const string& buf)
{
    vector<Ident>::iterator k;

    if ((k = find(TID.begin(), TID.end(), buf)) != TID.end())
        return k - TID.begin();
    TID.push_back(Ident(buf));
    return TID.size() - 1;
}

/////////////////////////////////////////////////////////////////

class Scanner
{
    FILE* fp;
    char c;
    int look(const string buf, const char** list)
    {
        int i = 0;
        while (list[i])
        {
            if (buf == list[i])
                return i;
            ++i;
        }
        return 0;
    }
    void gc()
    {
        c = fgetc(fp);
    }

public:
    static const char *TW[], *TD[];
    Scanner() = default;
    Scanner(const char* program)
    {
        fp = fopen(program, "r");
    }
    Lex get_lex();
};

const char* Scanner::TW[] = { "", "Boolean", "break", "continue", "do", "else", "false", "function",
    "if", "in", "NaN", "Number", "null", "Object", "return", "String", "true", "typeof",
    "undefined", "var", "while", NULL };

const char* Scanner::TD[] = { "@", ";", ",", ":", ".", "(", ")", "[", "]", "{", "}", "=", "==",
    "===", "<", ">", "+", "+=", "++", "-", "-=", "--", "*", "*=", "*/", "/", "/=", "/*", "//", "%",
    "%=", "<=", "!", "!=", "!==", ">=", "|", "||", "&", "&&", NULL };

Lex Scanner::get_lex()
{
    enum state
    {
        H,
        IDENT,
        NUMB,
        COM,
        COM2,
        COM3,
        SLSH,
        ALE,
        ALE2,
        PLUS,
        MINUS,
        AMP,
        PIPE,
        QUOTE
    };
    int d, j;
    string buf;
    state CS = H;
    do
    {
        gc();
        switch (CS)
        {
            case H:
                if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
                    ;
                else if (isalpha(c))
                {
                    buf.push_back(c);
                    CS = IDENT;
                }
                else if (isdigit(c))
                {
                    d = c - '0';
                    CS = NUMB;
                }
                else if (c == '/')
                {
                    buf.push_back(c);
                    CS = SLSH;
                }
                else if (c == '!' || c == '=')
                {
                    buf.push_back(c);
                    CS = ALE;
                }
                else if (c == '*' || c == '<' || c == '>' || c == '%')
                {
                    buf.push_back(c);
                    CS = ALE2;
                }
                else if (c == '+')
                {
                    buf.push_back(c);
                    CS = PLUS;
                }
                else if (c == '-')
                {
                    buf.push_back(c);
                    CS = MINUS;
                }
                else if (c == '&')
                {
                    buf.push_back(c);
                    CS = AMP;
                }
                else if (c == '|')
                {
                    buf.push_back(c);
                    CS = PIPE;
                }
                else if (c == '"')
                {
                    CS = QUOTE;
                }
                else if (c == '@')
                {
                    return Lex(LEX_FIN);
                }
                else
                {
                    buf.push_back(c);
                    if ((j = look(buf, TD)))
                    {
                        return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                    }
                    else
                        throw c;
                }
                break;
            case IDENT:
                if (isalpha(c) || isdigit(c))
                {
                    buf.push_back(c);
                }
                else
                {
                    ungetc(c, fp);
                    if ((j = look(buf, TW)))
                    {
                        return Lex((type_of_lex)j, j);
                    }
                    else
                    {
                        j = put(buf);
                        return Lex(LEX_ID, j);
                    }
                }
                break;
            case NUMB:
                if (isdigit(c))
                {
                    d = d * 10 + (c - '0');
                }
                else if (isalpha(c))
                {
                    throw c;
                }
                else
                {
                    ungetc(c, fp);
                    return Lex(LEX_NUM, d);
                }
                break;

            case SLSH:
                if (c == '*')
                {
                    buf.pop_back();
                    CS = COM;
                }
                else if (c == '=')
                {
                    buf.push_back(c);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                else if (c == '/')
                {
                    buf.pop_back();
                    CS = COM3;
                }
                else
                {
                    ungetc(c, fp);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                break;

            case COM:
                if (c == '*')
                {
                    CS = COM2;
                }
                else if (c == '@')
                    throw c;
                break;

            case COM2:
                if (c == '/')
                {
                    CS = H;
                }
                else if (c == '@')
                    throw c;
                else
                    CS = COM;
                break;

            case COM3:
                if (c == '\n')
                    CS = H;
                else if (c == '@')
                    throw c;
                break;

            case ALE:
                if (c == '=')
                {
                    buf.push_back(c);
                    CS = ALE2;
                }
                else
                {
                    ungetc(c, fp);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                break;

            case ALE2:
                if (c == '=')
                {
                    buf.push_back(c);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                else
                {
                    ungetc(c, fp);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                break;

            case PLUS:
                if (c == '=' || c == '+')
                {
                    buf.push_back(c);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                else
                {
                    ungetc(c, fp);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                break;

            case MINUS:
                if (c == '=' || c == '-')
                {
                    buf.push_back(c);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                else
                {
                    ungetc(c, fp);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                break;
            case AMP:
                if (c == '&')
                {
                    buf.push_back(c);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                else
                {
                    ungetc(c, fp);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                break;
            case PIPE:
                if (c == '|')
                {
                    buf.push_back(c);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                else
                {
                    ungetc(c, fp);
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                break;
            case QUOTE:
                if (c == '"')
                {
                    string res = "";
                    for (size_t i = 0; i < buf.size(); i++)
                    {
                        res += buf[i];
                    }
                    return Lex(LEX_STR_CONST, 0, res);
                }
                else if (c == '@')
                    throw c;

                buf.push_back(c);
                break;
        } // end switch
    } while (true);
}

ostream& operator<<(ostream& s, Lex l)
{
    string t;
    if (l.t_lex <= 20)
        t = Scanner::TW[l.t_lex];
    else if (l.t_lex >= 21 && l.t_lex <= 60)
        t = Scanner::TD[l.t_lex - 21];
    else if (l.t_lex == 61)
        t = "NUMB";
    else if (l.t_lex == 62)
    {
        t = "STRING CONST";
        s << '(' << t << ',' << l.s_lex << ");" << endl;
        return s;
    }
    else if (l.t_lex == 63)
        t = TID[l.v_lex].get_name();
    else if (l.t_lex == 64)
        t = "Label";
    else if (l.t_lex == 65)
        t = "Addr";
    else if (l.t_lex == 66)
        t = "!";
    else if (l.t_lex == 67)
        t = "!F";
    else
        throw l;
    s << '(' << t << ',' << l.v_lex << ");" << endl;
    return s;
}

int main(int argc, char** argv)
{
    Scanner s;
    if (argv)
        s = Scanner(argv[1]);
    else
    {
        cout << "FILE ERROR!";
        return 0;
    }
    Lex a;
    while (1)
    {
        try
        {
            a = s.get_lex();
        }
        catch (char c)
        {
            cout << "ERROR: " << c;
            return 0;
        }
        if (a.get_type() == LEX_FIN)
        {
            break;
        }
        cout << a;
    }
    return 0;
}