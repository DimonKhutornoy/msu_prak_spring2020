#include <iostream>
#include <cstring>
#include <cstdio>
#include <vector>

using namespace std;

class shop
{
	char* name;
	char* street;
	
public:
	shop(char* n, char* st) 
	{
		name = new char[strlen(n) + 1];
		strcpy(name, n);
		street = new char[strlen(st) + 1];
		strcpy(street, st);
	}

	shop(const shop& op) 
	{
		delete [] name;
		name = new char[strlen(op.name) + 1];
		strcpy(name, op.name);
		delete [] street;
		street = new char[strlen(op.street) + 1];
		strcpy(street, op.street);
	}
	
	char* getname() const 
	{
		return name;
	}

	char* getstreet() const 
	{
		return street;
	}
	shop& operator=(const shop& op) 
	{
		delete [] name;
		name = new char[strlen(op.name) + 1];
		strcpy(name, op.name);
		delete [] street;
		street = new char[strlen(op.street) + 1];
		strcpy(street, op.street);
	}

	virtual void print() const = 0;

	virtual ~shop() 
	{
		delete [] name;
		delete [] street;
	}
};

class product: public shop 
{
	char* name;
	double primePrice;
	double cost;
	int count;
	
public:
	static double potentialProfit;
	static double generalProductDamages;
	static vector <product> products;
	
	product(char* shopn, char* st, char* productn, double p, double cst, int cnt): shop(shopn, st) 
	{
		name = new char[strlen(productn) + 1];
		strcpy(name, productn);
		primePrice=p;
		cost = cst;
		count = cnt;
		potentialProfit+=(count*cost);
		generalProductDamages+=(count*primePrice);
		products.push_back(*this);
	}

	product(const product& op): shop(op.getname(), op.getstreet())
	{
		name = new char[strlen(op.name) + 1];
		strcpy(name, op.name);
		primePrice=op.primePrice;
		cost=op.cost;
		count=op.count;
	}

	product& operator= (const product& op) 
	{
		delete [] name;
		shop::operator=(op);
		name = new char[strlen(op.name) + 1];
		strcpy(name, op.name);
		//potentialProfit-=(count*cost);
		//generalProductDamages-=(count*primePrice);
		primePrice=op.primePrice;
		cost=op.cost;
		count=op.count;
		//potentialProfit+=(count*cost);
		//generalProductDamages+=(count*primePrice);
	}

	char * getprname() const 
	{
		return name;
	}

	bool getprimePrice() const 
	{
		return primePrice;
	}
	
	double getcost() const
	{
		return cost;
	}
	
	int getcount () const
	{
		return count;
	}
	
	virtual void print() const
	{
		cout << "Shop: " << this->getname()<< endl;
		cout << "Street: " << this->getstreet()<< endl;
		cout << "product: " << name<<endl;
		cout << "prime price: " << primePrice << endl;
		cout << "cost: " << cost <<endl;
		cout << "count: " << count << endl;
	}
	virtual ~product() 
	{
		delete [] name;
	};
};

double product::potentialProfit=0;
double product::generalProductDamages=0;
vector <product> product::products;


class person: public shop 
{
	char* name;
	char* position;
	double salary;
	
public:
	static double generalPersonalDamages;
	static vector <person> persons;
	
	person(char* shopn, char* st, char* persn, char* pos, double slr): shop(shopn, st) 
	{
		name = new char[strlen(persn) + 1];
		strcpy(name, persn);
		position = new char[strlen(pos) + 1];
		strcpy(position, pos);
		salary = slr;
		generalPersonalDamages+=salary;
		persons.push_back(*this);
	}

	person(const person& op): shop(op.getname(), op.getstreet())
	{
		name = new char[strlen(op.name) + 1];
		strcpy(name, op.name);
		delete [] position;
		position=new char[strlen(op.position) + 1];
		strcpy(position, op.position);
		salary=op.salary;
	}

	person& operator= (const person& op) 
	{
		shop::operator=(op);
		delete [] name;
		name = new char[strlen(op.name) + 1];
		strcpy(name, op.name);
		delete [] position;
		position=new char[strlen(op.position) + 1];
		strcpy(position, op.position);
		//generalPersonalDamages-=salary;
		salary=op.salary;
		//generalPersonalDamages+=salary;
	}

	char * getpername() const 
	{
		return name;
	}

	bool getposition() const 
	{
		return position;
	}
	
	double getsalary() const
	{
		return salary;
	}

	virtual void print() const
	{
		cout << "Shop: " << this->getname()<< endl;
		cout << "Street: " << this->getstreet()<< endl;
		cout << "Person name: " << name<<endl;
		cout << "Position: " << position << endl;
		cout << "salary: " << salary <<endl;
	}

	virtual ~person() 
	{
		delete [] name;
		delete [] position;
	};
};

double person::generalPersonalDamages=0;
vector <person> person::persons;

char * readstr()
{
	char c;
	int i=0;
	char*res=(char*)malloc(sizeof(char));
	while ((c=getchar())!='\n')
	{
		res=(char*)realloc(res, sizeof(char)*(i+2));
		res[i]=c;
		i++;
	}
	res[i]='\0';
	return res;
}

void menu()
{
	cout<<"Enter key:"<<endl
	<<"1 - enter product"<< endl
	<<"2 - enter personal"<<endl
	<<"3 - print shop info"<<endl
	<<"0 - exit"<<endl;
}

void cinProd(char **n, char ** s, char ** np, double & pr, double & cst, int & cnt)
{
	cout<<"Enter shop name: ";
	*n=readstr();
	cout<<"Enter shop street: ";
	*s=readstr();
	cout<<"Enter product name: ";
	*np=readstr();
	cout<<"Enter prime price: ";
	cin>>pr;
	cout<<"Enter cost: ";
	cin>>cst;
	cout<<"Enter count: ";
	cin>>cnt;
}

void cinPers(char **n, char ** s, char ** np, char ** pos, double & cst)
{
	cout<<"Enter shop name: ";
	*n=readstr();
	cout<<"Enter shop street: ";
	*s=readstr();
	cout<<"Enter person name: ";
	*np=readstr();
	cout<<"Enter person position: ";
	*pos=readstr();
	cout<<"Enter salary: ";
	cin>>cst;
}

void getinfo()
{
	cout<<"potential profit: "<<product::potentialProfit<<endl
	<<"General product damages: "<<product::generalProductDamages<<endl
	<<"General personal damages: "<<person::generalPersonalDamages<<endl<<endl;
	cout<<"Products:"<<endl;
	for (int i=0; i<product::products.size(); i++)
	{
		product::products[i].print();
		cout<<endl;
	}
	cout<<"Persons:"<<endl;
	for (int i=0; i<person::persons.size(); i++)
	{
		person::persons[i].print();
		cout<<endl;
	}
}

template <class T> ostream& operator<<(ostream& out, const T& c) 
{
	c.print();
}

int main() 
{
	int k=1;
	char* n;
	char* s;
	char* np;
	char*pos;
	double prime, cst;
	int cnt;
	while (k)
	{
		menu();
		cin>>k;
		getchar();
		if(k==1)
		{
			cinProd(&n, &s, &np, prime, cst, cnt);
			product a(n, s, np, prime, cst, cnt);
			delete [] n;
			delete [] s;
			delete [] np;
			//cout<<a;
		}
		if (k==2)
		{
			cinPers(&n, &s, &np, &pos, cst);
			person a(n,s,np,pos,cst);
			delete [] n;
			delete [] s;
			delete [] np;
			delete [] pos;
			//cout<<a;
		}
		if (k==3)
		{
			getinfo();
		}
	}
	product::products.clear();
	person::persons.clear();
	return 0;
}
