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
  LEX_BOOL, LEX_BREAK, LEX_CONTINUE, LEX_DO, LEX_ELSE, LEX_FALSE, LEX_FOR, LEX_FUNCTION, LEX_IF, LEX_IN,                         /*10*/       
  LEX_NAN, LEX_NUMBER, LEX_NULLPTR, LEX_OBJECT, LEX_RETURN, LEX_STRING, LEX_TRUE, LEX_TYPEOF, 	                                 /*18*/
  LEX_UNDEFINED, LEX_VAR, LEX_WHILE, LEX_INT,                                                                                    /*22*/
  LEX_FIN,                                                                                                                       /*23*/
  LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_DOT, LEX_LPAREN, LEX_RPAREN, LEX_LQPAREN, LEX_RQPAREN, LEX_BEGIN, LEX_END,            /*33*/
  LEX_EQ, LEX_DEQ, LEX_TEQ, LEX_LSS, LEX_GTR, LEX_PLUS, LEX_PLUS_EQ, LEX_DPLUS, LEX_MINUS, LEX_MINUS_EQ, LEX_DMINUS,              /*44*/
  LEX_TIMES, LEX_TIMES_EQ, LEX_TIMES_SLASH, LEX_SLASH, LEX_SLASH_EQ, LEX_SLASH_TIMES, LEX_DSLASH, LEX_PERCENT, LEX_PERCENT_EQ,   /*53*/
  LEX_LEQ, LEX_NOT, LEX_NEQ, LEX_NDEQ, LEX_GEQ, LEX_OR, LEX_DPIPE, LEX_AND, LEX_DAMP,                                            /*62*/
  LEX_NUM,                                                                                                                       /*63*/
  LEX_STR_CONST,                                                                                                                 /*64*/                                                                       
  LEX_ID,                                                                                                                        /*65*/ 
  POLIZ_LABEL,                                                                                                                   /*66*/
  POLIZ_ADDRESS,                                                                                                                 /*67*/
  POLIZ_GO,                                                                                                                      /*68*/
  POLIZ_FGO,                                                                                                                     /*69*/
  POLIZ_LEFT_INC,                                                                                                                /*70*/
  POLIZ_RIGHT_INC,                                                                                                               /*71*/ 
  POLIZ_LEFT_DEC,                                                                                                                /*72*/
  POLIZ_RIGHT_DEC,                                                                                                               /*73*/
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
	string str;

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
	string get_str() const
	{
		return str;
	}
	void put_str(string s)
	{
		str=s;
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

const char* Scanner::TW[] = { "", "Boolean", "break", "continue", "do", "else", "false", "for", "function",
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
				else if (c == '#')
				{
					buf.push_back(c);
                    CS = COM3;
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
    if (l.t_lex <= 21)
        t = Scanner::TW[l.t_lex];
    else if (l.t_lex >= 23 && l.t_lex <= 62)
        t = Scanner::TD[l.t_lex - 23];
    else if (l.t_lex == 63)
        t = "NUMB";
    else if (l.t_lex == 64)
    {
        t = "STRING CONST";
        s << '(' << t << ',' << l.s_lex << ");" << endl;
        return s;
    }
    else if (l.t_lex == 65)
        t = TID[l.v_lex].get_name();
    else if (l.t_lex == 66)
        t = "Label";
    else if (l.t_lex == 67)
        t = "&";
    else if (l.t_lex == 68)
        t = "!";
    else if (l.t_lex == 69)
        t = "!F";
	else if (l.t_lex == 70)
        t = "+#";
	else if (l.t_lex == 71)
        t = "#+";
	else if (l.t_lex == 72)
        t = "-#";
	else if (l.t_lex == 73)
        t = "#-";
    else
        throw l;
    s << '(' << t << ',' << l.v_lex << ");" << endl;
    return s;
}



//////////////////////////  Класс Parser  /////////////////////////////////
template <class T, class T_EL>
void from_st(T & st, T_EL& i)
{
    i = st.top();
    st.pop();
}

class Parser
{
    Lex curr_lex;
    type_of_lex c_type;
    int c_val;
    Scanner scan;
    stack<int> st_int;
    stack<type_of_lex> st_lex;
	stack <int> labels_for_con;
	stack <int> labels_for_break;
    void D();
    void B();
    void S();
    void E();
    void E1();
    void T();
    void F();
	void FOR_PARAMS();
    void dec(type_of_lex type);
    void check_id();
    void check_op();
    void check_not();
    void eq_type(type_of_lex &);
    void eq_bool();
    void gl()
    {
        curr_lex = scan.get_lex();
        c_type = curr_lex.get_type();
        c_val = curr_lex.get_value();
		std::cout<<curr_lex<<'\n';
    }

public:
    vector<Lex> poliz;
    Parser(const char* program)
        : scan(program)
    {
    }
    void analyze();
};

void Parser::analyze()
{
	int abc = 0;
	gl();
    S();
    if (c_type != LEX_FIN)
        throw curr_lex;
    for (Lex l : poliz)
	{
        cout <<abc<<' '<< l;
		abc++;
	}
    cout << endl << "Yes!!!" << endl;
}

void Parser::B()
{
    if (c_type == LEX_BEGIN)
    {
        gl();
        S();
        if (c_type == LEX_END)
        {
			//std::cout<<'!'<<'\n';
            gl();
        }
        else
        {
            throw curr_lex;
        }
    }
    else
        throw curr_lex;
}

void Parser::S()
{
    int pl0, pl1, pl2, pl3;
    if (c_type == LEX_IF)
    {
        gl();
		if (c_type != LEX_LPAREN)
			throw curr_lex;
		else
		{
			gl();
			E();
			eq_bool();
			pl2 = poliz.size();
			poliz.push_back(Lex());
			poliz.push_back(Lex(POLIZ_FGO));
			if (c_type == LEX_RPAREN)
			{
				gl();
				B();
				pl3 = poliz.size();
				poliz.push_back(Lex());
				poliz.push_back(Lex(POLIZ_GO));
				if (c_type == LEX_ELSE)
				{
					poliz[pl2] = Lex(POLIZ_LABEL, poliz.size());
					gl();
					B();
					poliz[pl3] = Lex(POLIZ_LABEL, poliz.size());
				}
				else
				{
					poliz.pop_back();
					poliz.pop_back();
					poliz[pl2] = Lex(POLIZ_LABEL, poliz.size());
				}
			}
			else
				throw curr_lex;
		}
    } // end if
    else if (c_type == LEX_WHILE)
    {
        pl0 = poliz.size();
		labels_for_con.push(poliz.size());
		gl();
		if (c_type != LEX_LPAREN)
			throw curr_lex;
		else
		{
			gl();
			E();
			eq_bool();
			pl1 = poliz.size();
			poliz.push_back(Lex());
			poliz.push_back(Lex(POLIZ_FGO));
			if (c_type == LEX_RPAREN)
			{
				gl();
				B();
				poliz.push_back(Lex(POLIZ_LABEL, pl0));
				poliz.push_back(Lex(POLIZ_GO));
				poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
			}
			else
				throw curr_lex;
		}
		if (!labels_for_break.empty())
		{
			poliz[labels_for_break.top()] = Lex(POLIZ_LABEL, poliz.size());
			labels_for_break.pop();
		}
    } // end while
	else if (c_type == LEX_DO) 
	{
		pl0 = poliz.size();
        gl();
		B();
		//poliz.push_back(Lex(POLIZ_LABEL, pl0));
		//poliz.push_back(Lex(POLIZ_GO));
		labels_for_con.push(poliz.size());
        if (c_type == LEX_WHILE) 
		{
			gl();
            if (c_type == LEX_LPAREN) 
			{
				gl();
				E();
				eq_bool();
				pl1 = poliz.size()+4;
				poliz.push_back(Lex(POLIZ_LABEL, pl1)); // Нам известно количество лексем в полизе после...
				poliz.push_back(Lex(POLIZ_FGO));
				poliz.push_back(Lex(POLIZ_LABEL, pl0));
				poliz.push_back(Lex(POLIZ_GO));
                if (c_type != LEX_RPAREN)
                    throw curr_lex;
				gl();
				gl();
            }
            else
                throw curr_lex;
        }
        else
            throw curr_lex;
		if (!labels_for_break.empty())
		{
			poliz[labels_for_break.top()] = Lex(POLIZ_LABEL, poliz.size());
			labels_for_break.pop();
		}
    } //end do while
	else if (c_type == LEX_FOR) 
	{
        gl();
        if (c_type == LEX_LPAREN) 
		{
            gl();
			FOR_PARAMS();
			std::cout<<'!'<<'\n';
			pl0 = poliz.size();
			if (c_type == LEX_SEMICOLON) {
				gl();
				E();
				poliz.push_back(Lex());
				poliz.push_back(Lex(POLIZ_FGO));
				poliz.push_back(Lex());
				poliz.push_back(Lex(POLIZ_GO));
				pl1 = poliz.size();
				labels_for_con.push(poliz.size());
				if (c_type == LEX_SEMICOLON) {
					gl();
					FOR_PARAMS();
					poliz.push_back(Lex(POLIZ_LABEL, pl0));
					poliz.push_back(Lex(POLIZ_GO));
					poliz[pl1-2] = Lex(POLIZ_LABEL, poliz.size());
					if (c_type == LEX_RPAREN) {
						gl();
						B();
						poliz.push_back(Lex(POLIZ_LABEL, pl1));
						poliz.push_back(Lex(POLIZ_GO));
						poliz[pl1-4] = Lex(POLIZ_LABEL, poliz.size());
					}
					else
						throw curr_lex;
				}
				else
					throw curr_lex;
			}
			if (!labels_for_break.empty())
			{
				poliz[labels_for_break.top()] = Lex(POLIZ_LABEL, poliz.size());
				labels_for_break.pop();
			}
        }
        else
            throw curr_lex;
    } //end for
    else if (c_type == LEX_ID)
    {
		int l_v_index = curr_lex.get_value();
		type_of_lex new_val;
        check_id();
        poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
        gl();
        if (c_type == LEX_EQ || c_type == LEX_MINUS_EQ || c_type == LEX_PLUS_EQ ||
		c_type == LEX_TIMES_EQ || c_type == LEX_PERCENT_EQ || c_type == LEX_SLASH_EQ)
        {
			type_of_lex tmp=c_type;
            gl();
            E();
            eq_type(new_val);
			TID[l_v_index].put_type(new_val);
            poliz.push_back(Lex(tmp));
			if (c_type == LEX_SEMICOLON)
			{
				gl();
			}
			else
				throw curr_lex;
        }
		else if (c_type == LEX_DPLUS)
		{
			poliz.push_back(Lex(POLIZ_RIGHT_INC));
			gl();
			if (c_type!=LEX_SEMICOLON)
				throw curr_lex;
			gl();
		}
		else if (c_type == LEX_DMINUS)
		{
			poliz.push_back(Lex(POLIZ_RIGHT_DEC));
			gl();
			if (c_type!=LEX_SEMICOLON)
				throw curr_lex;
			gl();
		}
        else
            throw curr_lex;
    } // assign-end
	else if (c_type == LEX_CONTINUE)
	{
		if (labels_for_con.empty())
		{
			throw curr_lex;
		}
		int lab = labels_for_con.top();
		labels_for_con.pop();
		poliz.push_back(Lex(POLIZ_LABEL, lab));
		poliz.push_back(Lex(POLIZ_GO));
		gl();
		if (c_type != LEX_SEMICOLON)
		{
			throw curr_lex;
		}
		gl();
	}
	else if (c_type == LEX_BREAK)
	{
		std::cout<<"!\n";
		labels_for_break.push(poliz.size());
		poliz.push_back(Lex());
		poliz.push_back(Lex(POLIZ_GO));
		gl();
		if (c_type != LEX_SEMICOLON)
		{
			throw curr_lex;
		}
		gl();
	}
	else if (c_type == LEX_VAR)
	{
		gl();
		D();
	} // var-end
	else if (c_type == LEX_FIN)
	{
		return;
	}
	else if (c_type == LEX_END)
	{
		//std::cout<<"!!!!!!!!!!!\n";
		return;
	}
	if (c_type == LEX_DPLUS || c_type == LEX_DMINUS)
	{
		E();
		gl();
	}
	S();
}

void Parser::E()
{
	if (c_type == LEX_DPLUS)
	{
		gl();
		F();
		poliz.push_back(Lex(POLIZ_LEFT_INC));
	}
	else if (c_type == LEX_DMINUS)
	{
		gl();
		F();
		poliz.push_back(Lex(POLIZ_LEFT_DEC));
	}
	else
	{
		E1();
		if (c_type == LEX_DEQ || c_type == LEX_LSS || c_type == LEX_GTR || c_type == LEX_LEQ
			|| c_type == LEX_GEQ || c_type == LEX_NEQ)
		{
			st_lex.push(c_type);
			gl();
			E1();
			check_op();
		}
	}
}

void Parser::E1()
{
    T();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR)
    {
        st_lex.push(c_type);
        gl();
        T();
        check_op();
    }
}

void Parser::T()
{
    F();
    while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_AND || c_type == LEX_PERCENT)
    {
        st_lex.push(c_type);
        gl();
        F();
        check_op();
    }
}

void Parser::F()
{
    if (c_type == LEX_ID)
    {
        check_id();
        poliz.push_back(Lex(LEX_ID, c_val));
        gl();
    }
    else if (c_type == LEX_NUM)
    {
        st_lex.push(LEX_INT);
        poliz.push_back(curr_lex);
        gl();
    }
    else if (c_type == LEX_TRUE)
    {
        st_lex.push(LEX_BOOL);
        poliz.push_back(Lex(LEX_TRUE, 1));
        gl();
    }
    else if (c_type == LEX_FALSE)
    {
        st_lex.push(LEX_BOOL);
        poliz.push_back(Lex(LEX_FALSE, 0));
        gl();
    }
	else if (c_type == LEX_STR_CONST)
    {
        st_lex.push(LEX_STRING);
        poliz.push_back(Lex(curr_lex));
        gl();
    }
    else if (c_type == LEX_NOT)
    {
        gl();
        F();
        check_not();
    }
    else if (c_type == LEX_LPAREN)
    {
        gl();
        E();
        if (c_type == LEX_RPAREN)
            gl();
        else
            throw curr_lex;
    }
    else
        throw curr_lex;
}

void Parser::D()
{
    if (c_type != LEX_ID)
        throw curr_lex;
    else
    {
        st_int.push(c_val);
        gl();
        while (c_type == LEX_COMMA)
        {
            gl();
            if (c_type != LEX_ID)
                throw curr_lex;
            else
            {
                st_int.push(c_val);
                gl();
            }
        }
        if (c_type != LEX_SEMICOLON)
            throw curr_lex;
        else
        {
            dec(LEX_UNDEFINED);
            gl();
			S();
        }
    }
}

void Parser::FOR_PARAMS()
{
	int l_v_index = curr_lex.get_value();
	type_of_lex new_val;
	check_id();
	poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
	gl();
	if (c_type == LEX_EQ || c_type == LEX_MINUS_EQ || c_type == LEX_PLUS_EQ ||
		c_type == LEX_TIMES_EQ || c_type == LEX_PERCENT_EQ || c_type == LEX_SLASH_EQ)
	{
		gl();
		E();
		eq_type(new_val);
		TID[l_v_index].put_type(new_val);
		poliz.push_back(Lex(LEX_EQ));
	}
	else if (c_type == LEX_DPLUS)
	{
		poliz.push_back(Lex(POLIZ_RIGHT_INC));
		gl();
		if (c_type!=LEX_SEMICOLON)
			throw curr_lex;
		gl();
	}
	else if (c_type == LEX_DMINUS)
	{
		poliz.push_back(Lex(POLIZ_RIGHT_DEC));
		gl();
		if (c_type!=LEX_SEMICOLON)
			throw curr_lex;
		gl();
	}
	else
		throw curr_lex;
}
////////////////////////////////////////////////////////////////

void Parser::dec ( type_of_lex type ) {
  int i;
  while ( !st_int.empty()) {
    from_st(st_int, i);
    if ( TID[i].get_declare() ) 
      throw "twice";
    else {
      TID[i].put_declare();
      TID[i].put_type(type);
    }
  }
}
 
void Parser::check_id () {
  if ( TID[c_val].get_declare() )
    st_lex.push ( TID[c_val].get_type() );
  else 
    throw "not declared";
}
 
void Parser::check_op () {
  type_of_lex t1, t2, op, t = LEX_INT, r = LEX_BOOL;
 
  from_st(st_lex, t2);
  from_st(st_lex, op);
  from_st(st_lex, t1);
  if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH || op == LEX_PERCENT)
    r = LEX_INT;
  if (op == LEX_OR || op == LEX_AND)
    t = LEX_BOOL;
  if (t1 == t2  &&  t1 == t) 
    st_lex.push(r);
  else
    throw "wrong types are in operation";
  poliz.push_back (Lex (op) );
}
 
void Parser::check_not () {
  if (st_lex.top() != LEX_BOOL)
    throw "wrong type is in not";
  else  
    poliz.push_back (Lex (LEX_NOT));
}
 
void Parser::eq_type (type_of_lex & t1) {
  type_of_lex t2;
  from_st(st_lex, t2);
  from_st(st_lex, t1);
  if (t1 == LEX_UNDEFINED)
  {
	t1 = t2;
  }
  else if ( t1 != t2)
    throw "wrong types are in =";
}
 
void Parser::eq_bool () {
  if ( st_lex.top() != LEX_BOOL )
    throw "expression is not boolean";
  st_lex.pop();
}
//////////////////////////////////////////////////

class Interpretator
{
    Parser pars;

public:
    Interpretator(const char* program)
        : pars(program)
    {
    }
    void interpretation();
};

void Interpretator::interpretation()
{
    pars.analyze();

}

int main(int argc, char** argv)
{
    if (argc==1)
    {
        cout << "FILE ERROR!";
        return 0;
    }
    try
    {
        Interpretator I(argv[1]);
        I.interpretation();
        return 0;
    }
    catch (char c)
    {
        cout << "unexpected symbol " << c << endl;
        return 1;
    }
    catch (Lex l)
    {
        cout << "unexpected lexeme" << l << endl;
        return 1;
    }
    catch (const char* source)
    {
        cout << source << endl;
        return 1;
    }
}