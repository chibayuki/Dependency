/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Copyright © 2021 chibayuki@foxmail.com

Dependency.Dependency
Version 21.2.17.0000

This file is part of Dependency
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include <string>
#include <map>

using namespace std;

class DependencyProperty;
class DependencyObject;

using __DpValTable = map<const DependencyProperty*, const void*>;
using __DpValPair = pair<const DependencyProperty*, const void*>;

class DependencyProperty
{
	friend class DependencyObject;

private:
	const type_info* _Ti;
	string _PropertyName;

	DependencyProperty(const type_info* ti, const string& propertyName) : _Ti(ti), _PropertyName(propertyName) {}
	DependencyProperty() {}
	~DependencyProperty() {}
	DependencyProperty(const DependencyProperty&) {}
	DependencyProperty& operator=(const DependencyProperty&) {}

	static __DpValTable __DefaultValues;

	static const void* __GetDefaultValue(const DependencyProperty* const dp)
	{
		auto it = __DefaultValues.find(dp);
		if (it != __DefaultValues.end())
		{
			return it->second;
		}

		return nullptr;
	}

public:
	template<typename T>
	static DependencyProperty* RegisterProperty(const type_info* ti, const string& propertyName, const T& defaultValue)
	{
		DependencyProperty* dp = new DependencyProperty(ti, propertyName);
		__DefaultValues.insert(__DpValPair(dp, new T(defaultValue)));
		return dp;
	}
};

__DpValTable DependencyProperty::__DefaultValues;

struct __DpMetaData
{
	__DpValTable DefaultValues;
	map<DependencyObject*, __DpValTable*> ObjValues;
};

class DependencyObject
{
private:
	__DpValTable __ObjValues;

public:
	virtual ~DependencyObject()
	{
		for (auto& val : __ObjValues)
		{
			if (val.second != nullptr)
			{
				delete val.second;
			}
		}

		__ObjValues.clear();
	}

	const void* GetValue(const DependencyProperty* const dp) const
	{
		auto it = __ObjValues.find(dp);
		if (it != __ObjValues.end())
		{
			return it->second;
		}

		return DependencyProperty::__GetDefaultValue(dp);
	}
	void SetValue(const DependencyProperty* const dp, const void* value)
	{
		auto it = __ObjValues.find(dp);
		if (it != __ObjValues.end())
		{
			delete it->second;
			it->second = value;
		}
		else
		{
			__ObjValues.insert(__DpValPair(dp, value));
		}
	}

	static const void* GetValue(DependencyObject* obj, const DependencyProperty* const dp)
	{
		return obj->GetValue(dp);
	}
	static void SetValue(DependencyObject* obj, const DependencyProperty* const dp, const void* value)
	{
		obj->SetValue(dp, value);
	}
};

#define DP_PROP_DECL(PropertyName, PropertyType) \
PropertyType Get##PropertyName() const \
{ \
	return *(PropertyType*)GetValue(PropertyName##Property); \
} \
void Set##PropertyName(const PropertyType& value) \
{ \
	SetValue(PropertyName##Property, new PropertyType(value)); \
} \
static const DependencyProperty* const PropertyName##Property

#define DP_PROP_DECL_A(PropertyName, PropertyType) \
static PropertyType Get##PropertyName(DependencyObject* obj) \
{ \
	return *(PropertyType*)GetValue(obj, PropertyName##Property); \
} \
static void Set##PropertyName(DependencyObject* obj, const PropertyType& value) \
{ \
	SetValue(obj, PropertyName##Property, new PropertyType(value)); \
} \
static const DependencyProperty* const PropertyName##Property

#define nameof(obj) #obj

#define DP_PROP_INIT(Class, PropertyName, DefaultValue) \
const DependencyProperty* const Class::PropertyName##Property = \
	DependencyProperty::RegisterProperty(&typeid(Class), nameof(PropertyName##Property), DefaultValue)