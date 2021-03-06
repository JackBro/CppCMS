#include "dbixx.h"
#include <iostream>
using namespace dbixx;
using namespace std;

int main()
{
	try {
	session sql("sqlite3:dbname=test.db;sqlite3_dbdir=./");
	//session sql("mysql:dbname='cppcms';username=root;password='root';host=127.0.0.1;port=3306");
	//session sql("pgsql:dbname=cppcms;username=artik");

	
	sql<<"drop table if exists test",
		exec();
//	try {
//		sql<<"drop table test",
//			exec();	
//	}
//	catch(dbixx_error const &e) {
//	}

	//sql<<"create table test ( id integer primary key auto_increment not null,n integer, f real , t timestamp ,name text )",
	///	exec();
	sql<<"create table test ( id integer primary key autoincrement not null,n integer, f real , t timestamp ,name text )",
		exec();
	//sql<<"create table test ( id  serial  primary key not null ,n integer, f real , t timestamp ,name text )",
	//	exec();
	std::tm t;
	time_t tt;
	tt=time(NULL);
	t=*localtime(&tt);
	cout<<asctime(&t);

	
	int n;
	sql<<"insert into test(n,f,t,name) values(?,?,?,?)",
		10,3.1415926565,t,"Hello \'World\'",
		exec();
	//cout<<"ID:"<<sql.rowid()<<endl;
	cout<<"ID:"<<sql.rowid("test_id_seq")<<", Affected rows"<<sql.affected()<<endl;
	sql<<"insert into test(n,f,t,name) values(?,?,?,?)",
		use(10,true),use(3.1415926565,true),use(t,true),use("Hello \'World\'",true),
		exec();
	//cout<<"ID:"<<sql.rowid()<<endl;
	cout<<"ID:"<<sql.rowid("test_id_seq")<<", Affected rows"<<sql.affected()<<endl;

	row r;
	result res;
	sql<<"select id,n,f,t,name from test limit 10",
		res;
	cout<<"Rows:"<<res.rows()<<endl;
	cout<<"Cols:"<<res.cols()<<endl;
	n=0;
	while(res.next(r)){
		double f=-1;
		int id=-1,k=-1;
		std::tm atime={0};
		string name="nonset";
		r >> id >> k >> f >> atime >> name;
		cout <<r.get<int>(1) << ' '<<k <<' '<<f<<' '<<name<<' '<< asctime(&atime) << endl;
		cout<<"has "<<r.cols()<<" columns\n";
		n++;
	}

	sql<<"delete from test where 1<>0",
		exec();
	cout<<"Deleted "<<sql.affected()<<" rows\n";
	return 0;
	}
	catch(dbixx_error const  &e) {
		std::cerr<<"Error:"<<e.what()<<" for query:" << e.query() << std::endl;
	}
	catch(exception const  &e) {
		std::cerr<<"Error:"<<e.what()<<std::endl;
	}
}
