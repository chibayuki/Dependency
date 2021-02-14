#include <iostream>
#include <string>
#include <initializer_list>

#include "Dependency.h"

using namespace std;

class Student
{
private:
	int _Id;
	string _Name;

public:
	int GetId() const
	{
		return _Id;
	}
	void SetId(int id)
	{
		_Id = id;
	}

	string GetName() const
	{
		return _Name;
	}
	void SetName(string name)
	{
		_Name = name;
	}
};

class StudentDp : public DependencyObject
{
	DP_OBJ(StudentDp)

public:
	virtual ~StudentDp()
	{
		DP_OBJ_DTOR(StudentDp);
	}

	DP_PROP_DECL(StudentDp, Id, int);
	DP_PROP_DECL(StudentDp, Name, string);
};

DP_PROP_INIT(StudentDp, Id, 0);
DP_PROP_INIT(StudentDp, Name, string(""));

using DpValPair = pair<DependencyProperty, const void*>;

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

int main()
{
	Student s1;
	s1.SetId(7);
	s1.SetName("Alice");
	cout << s1.GetId() << endl;
	cout << s1.GetName() << endl;

	StudentDp s2;
	s2.SetId(42);
	s2.SetName("Bob");
	cout << s2.GetId() << endl;
	cout << s2.GetName() << endl;

	StudentDp s3;
	SetObjProperties(&s3,
		{
			DpValPair(StudentDp::IdProperty, new int(1024)),
			DpValPair(StudentDp::NameProperty, new string("Cathy"))
		});
	cout << s3.GetId() << endl;
	cout << s3.GetName() << endl;

	system("pause");
}