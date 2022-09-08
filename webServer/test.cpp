#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <vector>
#include <queue>
#include <deque>
#include <set>
#include <memory>
#include <queue>

using namespace std;
typedef long long ll;
typedef pair<int, int> pii;
const int N = 2e5 + 10;
const int mod = 1e9 + 7;
const double eps = 1e-9;
typedef long long ll;
using pii = pair<int, int>;
//ip 192.168.29.135 -> integer

class base {
public:
	base() {
		cout << "base default ctor\n";
	}

	virtual void work() {
		cout << "base work function\n";
	}

	virtual ~base() {
		cout << "base default dtor\n";
	}

private:

};

class drived : public base {
public:
	//using base::work;

	drived() {
		cout << "drived default ctor\n";
	}

	void work() {
		cout << "drived work function\n";
	}

	// void parent() {
	// 	__super::work();
	// }

	~drived() {
		cout << "drived default dtor\n";
	}

private:
	int data;
};

void middleNumber(int a[], int n) {
	priority_queue<int, vector<int>, greater<int> > q1;
	priority_queue<int, vector<int>, less<int> >q2;

	for (int i = 0; i < n; ++i) {
		q1.push(a[i]);
		if (q1.size() - 1 > q2.size()) {
			q2.push(q1.top());
			q1.pop();
		}

		while (q1.size() && q2.size() && q1.top() < q2.top()) {
			//swap(q1.top(), q2.top());//返回值是一个常量，不能修改
			int x = q1.top();
			int y = q2.top();
			q1.pop();
			q2.pop();
			q1.push(y);
			q2.push(x);
		}
	}

	if (q1.size() > q2.size()) cout << q1.top() << endl;
	else cout << (q1.top() + q2.top()) / 2 << endl;

	while (!q1.empty()) {
		cout << q1.top() << " ";
		q1.pop();
	}cout << endl;

	while (!q2.empty()) {
		cout << q2.top() << " ";
		q2.pop();
	}cout << endl;
}

void testSort() {

	int a[11] = { 0, 19,38,65,97,76,13,27,49 ,43, 34 };
	//QuickSort<int>(a, 0, 9);

	//B = new int[10];
	//MergeSort<int>(a, 0, 9);

	/*for (int i = 1; i <= 1e9; i *= 10) {
		BaseSort(a, 10, i);
		print(a);
	}*/

	//InsertSort(a, 10);


	//ShellSort(a, 10);

	//BubbleSort(a, 10);

	//SelectSort(a, 10);

	//HeapSort(a, 10);

	/*srand((unsigned int)time(0));
	int n = 12;
	bool book[12];
	for (int i = 0; i < n; ++i) {
		if (rand() & 1) {
			book[i] = i;
		}
		else {
			book[i] = 0;
		}
	}

	for (int i = 0; i < n; ++i) {
		cout << boolalpha << book[i] << " ";
	}cout << endl;

	for (int i = 0; i < n; ++i) {
		cout << boolalpha << (bool)(book[i]^true) << " ";
	}cout << endl;


	char str[] = "abcdafahil";
	int ret = strspn(str, "cadb");
	cout << ret << endl;*/
}

void testCast() {
	base* d1 = new drived;
	d1->base::work();

	drived* d2 = new drived;
	d2->base::work();
	//d2->parent();
	d2->work();
	static_cast<base*>(d1)->work();
	base* b1 = new base;
	if ((b1 = dynamic_cast<drived*>(b1)) != nullptr) {
		b1->work();
	}
	else {
		cout << "cast failed\n";
	}

	union {
		unsigned short num;
		char bytes[2];
	}t;

	t.num = 0x0102;
	int* data = new int(0x01020304);
	int x = 0x01020305;
	if (t.bytes[0] == 0x01) {//高位在低地址部分，大端，便于阅读
		cout << "大端模式" << endl;
	}
	else {					 //高位在高地址部分，小端，便于机器处理
		cout << "小端模式" << endl;
	}
	cout << (int)t.bytes[0] << "\n" << (int)t.bytes[1] << endl;

	cout << (int)*reinterpret_cast<char*>(data) << endl;
	cout << (int)static_cast<char>(x) << endl;

	//cout << boolalpha << (typeid(d1) == typeid(d2)) << endl;

	const int* k = new int(5);
	int* p = const_cast<int*>(k);
	//int* q = k; //const int* 不能初始化int*
	*p = 10;
	cout << "k = " << *k << endl;

	int* y = new int(4);

	//区别？
	//const int* z = y;
	const int* z = const_cast<const int*>(y);

	//*z = 8;
	*y = 6;
	cout << "y = " << *y << endl;
}

int func(void);
int func(int x);
int func(double x, char y);
int func(int x, double y, char z);
//void func(int x);//无法重载仅按返回值区分的函数

class A {
public:
	void func(void);
	virtual void func(int);
};

class B : public A{
public: 
	void func();
	void func(int);
};

int main() {

	func();
	func(5);
	func(2.5, 'k');
	func(2, 3.14, 'd');

	A a;
	B b;
	a.func();
	a.func(1);
	b.func();
	b.func(1);

	//"int __cdecl func(void)" (? func@@YAHXZ)
	//"int __cdecl func(int)" (? func@@YAHH@Z)
	//"int __cdecl func(double,char)" (? func@@YAHND@Z)
	//"int __cdecl func(int,double,char)" (? func@@YAHHND@Z)

	//"public: void __thiscall A::func(void)" (? func@A@@QAEXXZ)
	//"public: virtual void __thiscall A::func(int)" (? func@A@@UAEXH@Z)
	//"public: void __thiscall B::func(void)" (? func@B@@QAEXXZ)
	//"public: virtual void __thiscall B::func(int)" (? func@B@@UAEXH@Z)

	return 0;
}
