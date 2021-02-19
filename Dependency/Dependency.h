/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Copyright © 2021 chibayuki@foxmail.com

Dependency.Dependency
Version 21.2.19.0000

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
	DependencyProperty() = delete;
	~DependencyProperty() = default;
	DependencyProperty(const DependencyProperty&) = delete;
	DependencyProperty(DependencyProperty&&) = delete;
	DependencyProperty& operator=(const DependencyProperty&) = delete;
	DependencyProperty& operator=(DependencyProperty&&) = delete;

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
	static DependencyProperty* Register(const type_info* ti, const string& propertyName, const void* defaultValue)
	{
		DependencyProperty* dp = new DependencyProperty(ti, propertyName);
		__DefaultValues.insert(__DpValPair(dp, defaultValue));
		return dp;
	}
};

__DpValTable DependencyProperty::__DefaultValues;

class DependencyObject
{
private:
	__DpValTable* __ObjValues;

public:
	DependencyObject() : __ObjValues(nullptr) {}
	virtual ~DependencyObject()
	{
		if (__ObjValues != nullptr)
		{
			for (auto& val : *__ObjValues)
			{
				if (val.second != nullptr)
				{
					delete val.second;
				}
			}

			delete __ObjValues;
		}
	}

	const void* GetValue(const DependencyProperty* const dp) const
	{
		if (__ObjValues != nullptr)
		{
			auto it = __ObjValues->find(dp);

			if (it != __ObjValues->end())
			{
				return it->second;
			}
		}

		return DependencyProperty::__GetDefaultValue(dp);
	}
	void SetValue(const DependencyProperty* const dp, const void* value)
	{
		if (__ObjValues == nullptr)
		{
			__ObjValues = new __DpValTable();
			__ObjValues->insert(__DpValPair(dp, value));
		}
		else
		{
			auto it = __ObjValues->find(dp);

			if (it != __ObjValues->end())
			{
				if (it->second != nullptr)
				{
					delete it->second;
				}

				it->second = value;
			}
			else
			{
				__ObjValues->insert(__DpValPair(dp, value));
			}
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
static const DependencyProperty* const PropertyName##Property; \
using PropertyName##Property##_t = PropertyType

#define DP_PROP_DECL_A(PropertyName, PropertyType) \
static PropertyType Get##PropertyName(DependencyObject* obj) \
{ \
	return *(PropertyType*)GetValue(obj, PropertyName##Property); \
} \
static void Set##PropertyName(DependencyObject* obj, const PropertyType& value) \
{ \
	SetValue(obj, PropertyName##Property, new PropertyType(value)); \
} \
static const DependencyProperty* const PropertyName##Property; \
using PropertyName##Property##_t = PropertyType

#define DP_PROP_INIT(Class, PropertyName, DefaultValue) \
const DependencyProperty* const Class::PropertyName##Property = \
	DependencyProperty::Register(&typeid(Class), #PropertyName, new Class::PropertyName##Property##_t(DefaultValue))