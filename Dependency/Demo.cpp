#include <iostream>
#include <string>
#include <initializer_list>

#include "Dependency.h"

using namespace std;

class Human
{
private:
	int _Age;
	string _Name;

public:
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
	DP_PROP_DECL(Age, int);
	DP_PROP_DECL(Name, string);
};

DP_PROP_INIT(HumanDp, Age, 0);
DP_PROP_INIT(HumanDp, Name, string(""));

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

class School : public DependencyObject
{
public:
	DP_PROP_DECL_A(StudentId, int);
};

DP_PROP_INIT(School, StudentId, 0);

int main()
{
	Human alice;
	alice.SetAge(12);
	alice.SetName("Alice");
	cout << alice.GetAge() << endl;
	cout << alice.GetName() << endl;

	HumanDp bob;
	bob.SetAge(14);
	bob.SetName("Bob");
	cout << bob.GetAge() << endl;
	cout << bob.GetName() << endl;

	HumanDp cathy;
	SetObjProperties(&cathy,
		{
			DpValPair(HumanDp::AgeProperty, new int(18)),
			DpValPair(HumanDp::NameProperty, new string("Cathy"))
		});
	cout << cathy.GetAge() << endl;
	cout << cathy.GetName() << endl;

	School::SetStudentId(&cathy, 42);
	cout << School::GetStudentId(&cathy) << endl;

	system("pause");
}