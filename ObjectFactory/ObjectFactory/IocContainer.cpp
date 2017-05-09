#include<iostream>
#include<functional>
#include<unordered_map>
#include<map>
#include<memory>
#include<string>
#include"Any.h"
#include"NonCopyable.h"
using namespace std;

/*
��Ioc��������

���ֻ�������ȱ�ݣ�
	1.�ݲ�֧�ֽӿ���������Ĺ�ϵ����
	2.Any�࣬��������ָ�뺯������decay�Ƴ����ú�cv�����������Ż�
	3.Ioc����ͳһ����࣬������
*/
class IocContainer:NonCopyable
{
public:
	IocContainer(void){}
	~IocContainer(void){}

	//ע����Ҫ�����Ķ���Ĺ��캯������Ҫ����һ��Ψһ�ı�ʶ���Ա��ں���Ĵ��������Ƿ������
	template<class T, typename Depend, typename... Args>
	typename std::enable_if<!std::is_base_of<T, Depend>::value>::type RegisterType(const string& strKey)
	{
		//ͨ���հ���������
		function<T*(Args...)> function_ = [](Args...args){return new T(new Depend(args...)); };
		RegisterType(strKey, function_);
	}

	//����Ψһ�ı�ʶȥ���Ҷ�Ӧ�Ĺ�����������������
	template<class T, typename... Args>
	T* Resolve(const string& strKey, Args...args)
	{
		if (this->m_creatorMap.find(strKey) == this->m_creatorMap.end())
		{
			return nullptr;
		}
		Any resolver = this->m_creatorMap[strKey];
		function<T*(Args...)> function_ = resolver.AnyCast<function<T*(Args...)> >();
		return function_(args...);
	}

	//��������ָ�����
	template<class T, typename... Args>
	shared_ptr<T> ResolveShared(const string& strKey, Args... args)
	{
		T* ptr = Resolve<T>(strKey, args...);
		return shared_ptr<T>(ptr);
	}

private:
	void RegisterType(const string& strKey, Any constructor)
	{
		if (this->m_creatorMap.find(strKey) != this->m_creatorMap.end())
		{
			throw invalid_argument("this key has already exist!");
		}
		//ͨ��Any���Ͳ�����ͬ���͹�����
		this->m_creatorMap.emplace(strKey, constructor);
	}
private:
	unordered_map<string, Any> m_creatorMap;
};

//Any���Ͳ�������
void Test()
{
	Any n;
	auto r = n.IsNull();
	std::cout << r << std::endl;
	std::string s1 = "hello";
	n = s1;
	n.AnyCast<std::string>();
	std::cout << n.Is<int>() << std::endl;
	Any n1 = 1;
	std::cout << n1.Is<int>() << std::endl;
}

//IoC��������
//����
class Base
{
public:
	virtual void func(){}
	virtual ~Base(){}
};

//����B
class DerivedB:public Base
{
public:
	DerivedB(int a, double b) :a_(a), b_(b){}
	void func() override
	{
		cout << a_ + b_ << endl;
	}
private:
	int a_;
	double b_;
};

//����C
class DerivedC :public Base
{

};

//Ioc����ͨ�û�������࣬������
class A
{
public:
	A(Base * ptr) :m_ptr(ptr){}

	~A()
	{
		if (m_ptr != nullptr)
		{
			delete m_ptr;
			m_ptr = nullptr;
		}
	}
	Base * m_ptr;
};

//Ioc��������--���ú���
void TestIoc()
{
	IocContainer ioc;

	//�����Զ������Ͳ���
	ioc.RegisterType<A, DerivedC>("A");
	auto c = ioc.ResolveShared<A>("A");

	//��Ioc���������Ի����������Զ������ͻ���
	ioc.RegisterType<A, DerivedB, int, double>("C");
	auto b = ioc.ResolveShared<A>("C",1,3.14);
	
	//�˴����ԸĽ�������Sharedָ��ĳ���
	b->m_ptr->func();
}

int main()
{	
	Test();
	TestIoc();
	return 0;
}