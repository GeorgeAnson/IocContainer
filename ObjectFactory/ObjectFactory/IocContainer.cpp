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
简单Ioc容器开发

部分基础功能缺陷：
	1.暂不支持接口与派生类的关系配置
	2.Any类，创建智能指针函数——decay移除引用和cv符函数，可优化
	3.Ioc容器统一入口类，待完善
*/
class IocContainer:NonCopyable
{
public:
	IocContainer(void){}
	~IocContainer(void){}

	//注册需要创建的对象的构造函数，需要传入一个唯一的标识，以便在后面的创建对象是方便查找
	template<class T, typename Depend, typename... Args>
	typename std::enable_if<!std::is_base_of<T, Depend>::value>::type RegisterType(const string& strKey)
	{
		//通过闭包擦除类型
		function<T*(Args...)> function_ = [](Args...args){return new T(new Depend(args...)); };
		RegisterType(strKey, function_);
	}

	//根据唯一的标识去查找对应的构造器，并创建对象
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

	//创建智能指针对象
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
		//通过Any类型擦除不同类型构造器
		this->m_creatorMap.emplace(strKey, constructor);
	}
private:
	unordered_map<string, Any> m_creatorMap;
};

//Any类型擦除测试
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

//IoC容器测试
//基类
class Base
{
public:
	virtual void func(){}
	virtual ~Base(){}
};

//子类B
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

//子类C
class DerivedC :public Base
{

};

//Ioc调用通用基本入口类，待完善
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

//Ioc容器测试--调用函数
void TestIoc()
{
	IocContainer ioc;

	//复杂自定义类型测试
	ioc.RegisterType<A, DerivedC>("A");
	auto c = ioc.ResolveShared<A>("A");

	//简单Ioc容器，测试基本类型与自定义类型混用
	ioc.RegisterType<A, DerivedB, int, double>("C");
	auto b = ioc.ResolveShared<A>("C",1,3.14);
	
	//此处可以改进，隐藏Shared指针的出现
	b->m_ptr->func();
}

int main()
{	
	Test();
	TestIoc();
	return 0;
}