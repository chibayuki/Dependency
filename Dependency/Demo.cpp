/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Copyright © 2021 chibayuki@foxmail.com

Dependency.Dependency
Version 21.2.17.0000

This file is part of Dependency
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <iostream>
#include <string>
#include <initializer_list>

#include "Dependency.h"

using namespace std;

class Human // 作为对照，这是一个封装了一组字段以及 Get、Set 方法的普通类
{
private:
	int _Age;
	string _Name;

public:
	Human() : _Age(0), _Name("") {}

	int GetAge() const
	{
		return _Age;
	}
	void SetAge(int value)
	{
		_Age = value;
	}

	string GetName() const
	{
		return _Name;
	}
	void SetName(string value)
	{
		_Name = value;
	}
};

class HumanDp : public DependencyObject
{
public:
	DP_PROP_DECL(Age, int); // 定义依赖属性 AgeProperty 以及访问器方法 GetAge、SetAge
	DP_PROP_DECL(Name, string); // 定义依赖属性 NameProperty 以及访问器方法 GetName、SetName
};

DP_PROP_INIT(HumanDp, Age, 0); // 为 AgeProperty 属性设定初值
DP_PROP_INIT(HumanDp, Name, string("")); // 为 NameProperty 属性设定初值

// 示例 1 展示依赖对象、依赖属性的定义与使用
void Demo1()
{
	Human alice;
	alice.SetAge(12);
	alice.SetName("Alice");
	cout << alice.GetAge() << endl; // Output: 12
	cout << alice.GetName() << endl; // Output: Alice

	HumanDp bob;
	bob.SetAge(14);
	bob.SetName("Bob");
	cout << bob.GetAge() << endl; // Output: 14
	cout << bob.GetName() << endl; // Output: Bob
}

class School : public DependencyObject
{
public:
	DP_PROP_DECL_A(StudentId, int); // 定义附加属性 StudentIdProperty 以及访问器方法 GetStudentId、SetStudentId
};

DP_PROP_INIT(School, StudentId, 0); // 为 StudentIdProperty 属性设定初值

// 示例 2 展示附加属性的定义与使用
void Demo2()
{
	HumanDp cathy;
	School::SetStudentId(&cathy, 42); // 将 School 的 StudentIdProperty 属性附加到 HumanDp 对象 cathy
	cout << School::GetStudentId(&cathy) << endl; // Output: 42
}

using DpValPair = pair<const DependencyProperty*, const void*>;

void SetObjProperties(DependencyObject* obj, const initializer_list<DpValPair>& values)
{
	if (obj != nullptr)
	{
		for (auto& val : values)
		{
			obj->SetValue(val.first, val.second);
		}
	}
}

// 示例 3 展示依赖对象、依赖属性的多态性使用
void Demo3()
{
	HumanDp david;
	SetObjProperties(&david,
		{
			DpValPair(HumanDp::AgeProperty, new int(18)),
			DpValPair(HumanDp::NameProperty, new string("David"))
		}); // 可以像这样通过一致的方式为不同类型的依赖对象的若干个不同的依赖属性赋值
	cout << david.GetAge() << endl; // Output: 18
	cout << david.GetName() << endl; // Output: David
}

int main()
{
	Demo1();
	Demo2();
	Demo3();

	system("pause");
}