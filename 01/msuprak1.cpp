#include <iostream>
#include <cmath>

using namespace std;

class Matrix 
{
	int m, n;
	double **mtx;
public:
	int get_m () {return m;}
	int get_n () {return n;}
	Matrix (int size1, int size2)
	{
		m=size1;
		n=size2;
		mtx=(double**)malloc(sizeof(double*)*m);
		for (int i=0; i<m; i++)
		{
			mtx[i]=(double*)malloc(sizeof(double)*n);
		}
	}
	Matrix (const Matrix & cp)
	{
		m=cp.m;
		n=cp.n;
		mtx=(double**)malloc(sizeof(double*)*m);
		for (int i=0; i<m; i++)
		{
			mtx[i]=(double*)malloc(sizeof(double)*n);
		}
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<n; j++)
			{
				mtx[i][j]=cp.mtx[i][j];
			}
		}
	}
	void MtxRead ()
	{
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<n; j++)
			{
				cin>>mtx[i][j];
			}
		}
	}
	void MtxPrint ()
	{
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<n; j++)
			{
				cout<<mtx[i][j]<<' ';
			}
		cout<<endl;
		}
	}
	Matrix operator+(Matrix op2)
	{
		Matrix res(m,n);
		if (m!=op2.m || n!=op2.n)
		{
			cout<<"Size Error!"<<endl;
			return res;
		}
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<n; j++)
			{
				res.mtx[i][j]=mtx[i][j]+op2.mtx[i][j];
			}
		}
		return res;
	}
	Matrix & operator+=(const Matrix & op2) //Почему именно такие типы?
	{
		if (m!=op2.m || n!=op2.n)
		{
			cout<<"Size Error!"<<endl;
		}
		else
		{
			for (int i=0; i<m; i++)
			{
				for (int j=0; j<n; j++)
				{
					mtx[i][j]+=op2.mtx[i][j];
				}
			}
		}
	}
	Matrix operator-(Matrix op2)
	{
		Matrix res(m,n);
		if (m!=op2.m || n!=op2.n)
		{
			cout<<"Size Error!"<<endl;
			return res;
		}
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<n; j++)
			{
				res.mtx[i][j]=mtx[i][j]-op2.mtx[i][j];
			}
		}
		return res;
	}
	Matrix & operator-=(const Matrix & op2)
	{
		if (m!=op2.m || n!=op2.n)
		{
			cout<<"Size Error!"<<endl;
		}
		else
		{
			for (int i=0; i<m; i++)
			{
				for (int j=0; j<n; j++)
				{
					mtx[i][j]-=op2.mtx[i][j];
				}
			}
		}
	}
	Matrix operator*(Matrix op2)
	{
		Matrix res(m,op2.n);
		if (n!=op2.m)
		{
			cout<<"Size Error!"<<endl;
			return res;
		}
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<op2.n; j++)
			{
				res.mtx[i][j]=0;
				for (int k=0; k<n; k++)
				{
					res.mtx[i][j]+=mtx[i][k]*op2.mtx[k][j];
				}
			}
		}
		return res;
	}
	Matrix & operator*=(const Matrix & op2)
	{
		if (n!=op2.m)
		{
			cout<<"Size Error!"<<endl;
		}
		else
		{
			Matrix res(m,op2.n);
			for (int i=0; i<m; i++)
			{
				for (int j=0; j<op2.n; j++)
				{
					res.mtx[i][j]=0;
					for (int k=0; k<n; k++)
					{
						res.mtx[i][j]+=mtx[i][k]*op2.mtx[k][j];
					}
				}
			}
			for (int i=0; i<m; i++)
			{
				for (int j=0; j<op2.n; j++)
				{
					mtx[i][j]=res.mtx[i][j];
				}
			}
		}
	}
	Matrix operator*(double p)
	{
		Matrix res(m,n);
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<n; j++)
			{
				res.mtx[i][j]=mtx[i][j]*p;
			}
		}
		return res;
	}
	Matrix & operator*=(double p)
	{
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<n; j++)
			{
				mtx[i][j]*=p;
			}
		}
	}
	Matrix operator/(Matrix op2)
	{
		Matrix res(m,op2.n);
		if (n!=op2.m || op2.n!=op2.m)
		{
			cout<<"Size Error!"<<endl;
			return res;
		}
		op2.Reverse();
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<op2.n; j++)
			{
				res.mtx[i][j]=0;
				for (int k=0; k<n; k++)
				{
					res.mtx[i][j]+=mtx[i][k]*op2.mtx[k][j];
				}
			}
		}
		op2.Reverse();
		return res;
	}
	
	Matrix & operator/=(Matrix & op2)
	{
		if (n!=op2.m || op2.m!=op2.n)
		{
			cout<<"Size Error!"<<endl;
		}
		else
		{
			Matrix res(m,op2.n);
			op2.Reverse();
			for (int i=0; i<m; i++)
			{
				for (int j=0; j<op2.n; j++)
				{
					res.mtx[i][j]=0;
					for (int k=0; k<n; k++)
					{
						res.mtx[i][j]+=mtx[i][k]*op2.mtx[k][j];
					}
				}
			}
			for (int i=0; i<m; i++)
			{
				for (int j=0; j<op2.n; j++)
				{
					mtx[i][j]=res.mtx[i][j];
				}
			}
			op2.Reverse();
		}
	}
	Matrix operator/(double p)
	{
		Matrix res(m,n);
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<n; j++)
			{
				res.mtx[i][j]=mtx[i][j]/p;
			}
		}
		return res;
	}
	Matrix & operator/=(double p)
	{
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<n; j++)
			{
				mtx[i][j]/=p;
			}
		}
	}
	bool operator==(Matrix op2)
	{
		bool res=1;
		if (m!=op2.m || n!=op2.n)
		{
			return 0;
		}
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<n; j++)
			{
				res=res*(mtx[i][j]==op2.mtx[i][j]);
			}
		}
		return res;
	}

	bool operator!=(Matrix op2)
	{
		bool res=1;
		if (m!=op2.m || n!=op2.n)
		{
			return 0;
		}
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<n; j++)
			{
				res=res*(mtx[i][j]==op2.mtx[i][j]);
			}
		}
		return !res;
	}
	
	Matrix Nmatr(int y, int x)
	{
		int k, fl=0;
		Matrix res(m-1,n-1);
		for (int i = 0; i < n; ++i)
		{
			if (i==y)
			{
				fl=1;
				continue;
			}
			k=0;
			for (int j = 0; j < n; ++j)
			{
				if (j == x)
				{
					continue;
				}
				res.mtx[i - fl][k] = mtx[i][j];
				k++;
			}
		}
		return res;
	}
	
	double Det()
	{
		if (m!=n)
		{
			cout<<"Size Error!"<<endl;
		}
		else
		{
			double ans=0;
			int sig = 1;
			if (n == 1)
				return mtx[0][0];
			if (n == 2)
				return (mtx[1][1] * mtx[0][0]) - (mtx[0][1] * mtx[1][0]);
			for (int i = 0; i < n; ++i)
			{
				ans += (sig * mtx[0][i] * (this->Nmatr(0,i)).Det());
				sig *= -1;
			}
			return ans;
		}
	}
	
	void Reverse ()
	{
		int sig;
		double ans;
		if (this->Det()==0 || m!=n)
		{
			cout<<"No Reverse Matrix"<<endl;
		}
		else
		{
			Matrix res(m, n);
			for (int i=0; i<m; i++)
			{
				for (int j=0; j<n; j++)
				{
					if ((i+j)%2) sig=-1;
					else sig=1;
					ans=(this->Nmatr(j,i)).Det();
					if (ans==0) res.mtx[i][j]=0;
					else res.mtx[i][j]=sig*ans;
				}
			}
			res/=(this->Det());
			for (int i=0; i<m; i++)
			{
				for (int j=0; j<n; j++)
				{
					mtx[i][j]=res.mtx[i][j];
				}
			}
		}
	}		
	
	void Trans ()
	{
		double swap;
		if (m!=n)
		{
			cout<<"Size error!"<<endl;
		}
		for (int i=0; i<m; i++)
		{
			for (int j=i; j<n; j++)
			{
				swap=mtx[i][j];
				mtx[i][j]=mtx[j][i];
				mtx[j][i]=swap;
			}
		}
	}
	
	void Pow(int p)
	{
		if (m!=n)
		{
			cout<<"Size Error!"<<endl;
		}
		else
		{
			Matrix res(m,n);
			if (p==1)
			{
				return;
			}
			if (p==-1)
			{
				this->Reverse();
				return;
			}
			for (int i=0; i<m; i++)
			{
				for (int j=0; j<n; j++)
				{
					if (i==j) res.mtx[i][j]=mtx[i][j];
					else res.mtx[i][j]=mtx[i][j];
				}
			}
			for (int i=0; i<m; i++)
			{
				for (int j=0; j<n; j++)
				{
					if (i==j) mtx[i][j]=1;
					else mtx[i][j]=0;
				}
			}
			if (p<-1)
			{
				res.Reverse();
				p=p*(-1);
			}
			if (p!=0)
			{
				for (int i=0; i<p; i++)
				{
					(*this)*=res;
				}
			}
		}
	}
	
	double Norma ()
	{
		double res=0;
		for (int i=0; i<m; i++)
		{
			for (int j=0; j<n; j++)
			{
				res+=(mtx[i][j]*mtx[i][j]);
			}
		}
		return sqrt(res);
	}
	
	void Type ()
	{
		int fldiag=1, fldt=1, fllt=1, flnull=1, flone=1;
		Matrix Tr();
		if(m==n) 
		{
			cout<<"Square"<<endl;
			for (int i=0; i<m; i++)
			{
				for (int j=0; j<n; j++)
				{
					if (i<j) 
					{
						if (mtx[i][j]!=0)
						{
							fldiag=0; 
							fllt=0;
							flnull=0;
						}
					}					
					else if (i>j) 
					{
						if (mtx[i][j]!=0)
						{
							fldiag=0; 
							fldt=0;
							flnull=0;
						}
					}
					else if (i==j) 
					{
						if (mtx[i][j]!=0)
						{
							flnull=0;
						}
					}
				}
			}
			if (fldiag) 
			{
				cout<<"Diagonal"<<endl;
				for (int i=0; i<n; i++)
				{
					if (mtx[i][i]!=1) flone=0;
				}
				if (flone) cout<<"Single matrix"<<endl;
			}
			if (fldt) cout<<"Lower triangular"<<endl;
			if (fllt) cout<<"Upper triangular"<<endl;
			if (flnull) cout<<"Zero"<<endl;
			Matrix res(*this);
			res.Trans();
			if ((*this)==res) cout<<"Symmetrical"<<endl;
		}
		else
		{
			cout<<"Rectangular"<<endl;
			for (int i=0; i<m; i++)
			{
				for (int j=0; j<n; j++)
				{
					if (mtx[i][j]!=0)
					{
						flnull=0;
					}
				}
			}
			if (flnull) cout<<"Zero"<<endl;
		}
	}
	~Matrix()
	{
		for (int i=0; i<m; i++)
		{
			free(mtx[i]);
		}
		free(mtx);
	}
};
	
int main()
{
	int k, m, n, p;
	double x;
	cout<<"Enter Size a:"<<endl;
	cin>>m>>n;
	Matrix a(m,n);
	cout<<"Enter Size b:"<<endl;
	cin>>m>>n;
	Matrix b(m,n);
	Matrix res(3,3);
	cout<<"Enter key:"<<endl
	<<"1 - Input Matrix a"<<endl
	<<"2 - Input Matrix b"<<endl
	<<"3 - res=a+b"<<endl
	<<"4 - a+=b"<<endl
	<<"5 - res=a-b"<<endl
	<<"6 - a-=b"<<endl
	<<"7 - res=a*b"<<endl
	<<"8 - a*=b"<<endl
	<<"9 - res=(double)*a"<<endl
	<<"10 - a*=(double)"<<endl
	<<"11 - res=a/b"<<endl
	<<"12 - a/=b"<<endl
	<<"13 - res=a/(double)"<<endl
	<<"14 - a/=(double)"<<endl
	<<"15 - a==b?"<<endl
	<<"16 - a!=b?"<<endl
	<<"17 - a^(-1)"<<endl
	<<"18 - a^T"<<endl
	<<"19 - a^(int)"<<endl
	<<"20 - det(a)"<<endl
	<<"21 - ||a||"<<endl
	<<"22 - type a"<<endl
	<<"23 - Print a"<<endl
	<<"24 - Print b"<<endl
	<<"0 - Exit"<<endl
	<<"Key:";
	cin>>k;
	while (k)
	{
		if (k==1)
		{
			a.MtxRead();
		}
		else if (k==2)
		{
			b.MtxRead();
		}
		else if (k==3)
		{
			(a+b).MtxPrint();
		}
		else if (k==4)
		{
			a+=b;
		}
		else if (k==5)
		{
			(a-b).MtxPrint();
		}
		else if (k==6)
		{
			a-=b;
		}
		else if (k==7)
		{
			(a*b).MtxPrint();
		}
		else if (k==8)
		{
			a*=b;
		}
		else if (k==9)
		{
			cout<<"Enter number:"<<endl;
			cin>>x;
			(a*x).MtxPrint();
		}
		else if (k==10)
		{
			cout<<"Enter number:"<<endl;
			cin>>x;
			a*=x;
		}
		else if (k==11)
		{
			(a/b).MtxPrint();
		}
		else if (k==12)
		{
			a/=b;
		}
		else if (k==13)
		{
			cout<<"Enter number:"<<endl;
			cin>>x;
			(a/x).MtxPrint();
		}
		else if (k==14)
		{
			cout<<"Enter number:"<<endl;
			cin>>x;
			a/=x;
		}
		else if (k==15)
		{
			cout<<(a==b)<<endl;
		}
		else if (k==16)
		{
			cout<<(a!=b)<<endl;
		}
		else if (k==17)
		{
			a.Reverse();
		}
		else if (k==18)
		{
			a.Trans();
		}
		else if (k==19)
		{
			cout<<"Enter number:"<<endl;
			cin>>p;
			a.Pow(p);
		}
		else if (k==20)
		{
			cout<<a.Det()<<endl;
		}
		else if (k==21)
		{
			cout<<a.Norma()<<endl;
		}
		else if (k==22)
		{
			a.Type();
		}
		else if (k==23)
		{
			a.MtxPrint();
		}
		else if (k==24)
		{
			b.MtxPrint();
		}
		cout<<"Key:";
		cin>>k;
	}
	return 0;
}