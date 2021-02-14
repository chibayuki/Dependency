#pragma once

#include <string>
#include <map>

using namespace std;

class DependencyObject;

class DependencyProperty
{
	friend class DependencyObject;

private:
	const type_info* _Ti;
	string _PropertyName;
	size_t _Hash;

	DependencyProperty(const type_info* ti, const string& propertyName) :
		_Ti(ti),
		_PropertyName(propertyName)
	{
		_Hash = _Ti->hash_code() ^ hash<string>()(_PropertyName);
	}

public:
	size_t GetHashCode() const
	{
		return _Hash;
	}
};

static bool operator==(const DependencyProperty& left, const DependencyProperty& right)
{
	return left.GetHashCode() == right.GetHashCode();
}
static bool operator!=(const DependencyProperty& left, const DependencyProperty& right)
{
	return left.GetHashCode() != right.GetHashCode();
}
static bool operator<(const DependencyProperty& left, const DependencyProperty& right)
{
	return left.GetHashCode() < right.GetHashCode();
}
static bool operator>(const DependencyProperty& left, const DependencyProperty& right)
{
	return left.GetHashCode() > right.GetHashCode();
}
static bool operator<=(const DependencyProperty& left, const DependencyProperty& right)
{
	return left.GetHashCode() <= right.GetHashCode();
}
static bool operator>=(const DependencyProperty& left, const DependencyProperty& right)
{
	return left.GetHashCode() >= right.GetHashCode();
}

using __DpValTable = map<DependencyProperty, const void*>;

struct __DpMetaData
{
	__DpValTable DefaultValues;
	map<DependencyObject*, __DpValTable*> ObjValues;
};

class DependencyObject
{
public:
	virtual ~DependencyObject() {}
	virtual const void* GetValue(const DependencyProperty& dp) = 0;
	virtual void SetValue(const DependencyProperty& dp, const void* value) = 0;

	template<typename T>
	static DependencyProperty RegisterProperty(const type_info* ti, const string& propertyName, const T& defaultValue)
	{
		DependencyProperty dp = DependencyProperty(ti, propertyName);
		__RegDefaultValue(ti, dp, new T(defaultValue));
		return dp;
	}

private:
	static map<const type_info*, __DpMetaData*> __DpMDTables;

protected:
	static __DpMetaData& __GetOrRegDpMetaData(const type_info* ti)
	{
		auto it = __DpMDTables.find(ti);
		if (it != __DpMDTables.end())
		{
			return *(it->second);
		}
		else
		{
			__DpMetaData* md = new __DpMetaData();
			__DpMDTables.insert(pair<const type_info*, __DpMetaData*>(ti, md));
			return *md;
		}
	}

	static void __RegDefaultValue(const type_info* ti, const DependencyProperty& dp, const void* value)
	{
		__DpMetaData& md = __GetOrRegDpMetaData(ti);

		auto it = md.DefaultValues.find(dp);
		if (it != md.DefaultValues.end())
		{
			delete it->second;
			it->second = value;
		}
		else
		{
			md.DefaultValues.insert(pair<DependencyProperty, const void*>(dp, value));
		}
	}
	static const void* __GetDefaultValue(const type_info* ti, const DependencyProperty& dp)
	{
		__DpMetaData& md = __GetOrRegDpMetaData(ti);

		auto it = md.DefaultValues.find(dp);
		if (it != md.DefaultValues.end())
		{
			return it->second;
		}

		return nullptr;
	}

	static __DpValTable* __GetObjValueTable(const type_info* ti, DependencyObject* obj)
	{
		__DpMetaData& md = __GetOrRegDpMetaData(ti);

		auto it = md.ObjValues.find(obj);
		if (it != md.ObjValues.end())
		{
			return it->second;
		}

		return nullptr;
	}
	static __DpValTable* __GetOrRegObjValueTable(const type_info* ti, DependencyObject* obj)
	{
		__DpMetaData& md = __GetOrRegDpMetaData(ti);

		auto it = md.ObjValues.find(obj);
		if (it != md.ObjValues.end())
		{
			return it->second;
		}
		else
		{
			__DpValTable* p = new __DpValTable();
			md.ObjValues.insert(pair<DependencyObject*, __DpValTable*>(obj, p));
			return p;
		}
	}

	static void __UnregObjValueTable(const type_info* ti, DependencyObject* obj)
	{
		__DpMetaData& md = __GetOrRegDpMetaData(ti);

		auto it = md.ObjValues.find(obj);
		if (it != md.ObjValues.end())
		{
			md.ObjValues.erase(it);
		}
	}

	static const void* __GetObjValue(const type_info* ti, DependencyObject* obj, const DependencyProperty& dp)
	{
		__DpValTable* p = __GetObjValueTable(ti, obj);
		if (p != nullptr)
		{
			auto it = p->find(dp);
			if (it != p->end())
			{
				return it->second;
			}
		}

		return __GetDefaultValue(ti, dp);
	}
	static void __SetObjValue(const type_info* ti, DependencyObject* obj, const DependencyProperty& dp, const void* value)
	{
		__DpValTable* p = __GetOrRegObjValueTable(ti, obj);
		if (p != nullptr)
		{
			auto it = p->find(dp);
			if (it != p->end())
			{
				delete it->second;
				it->second = value;
			}
			else
			{
				p->insert(pair<DependencyProperty, const void*>(dp, value));
			}
		}
	}

};

map<const type_info*, __DpMetaData*> DependencyObject::__DpMDTables;

#define nameof(obj) #obj

#define DP_OBJ(Class) \
public: \
	virtual const void* GetValue(const DependencyProperty& dp) \
	{ \
		return __GetObjValue(&typeid(Class), this, dp); \
	} \
	virtual void SetValue(const DependencyProperty& dp, const void* value) \
	{ \
		__SetObjValue(&typeid(Class), this, dp, value); \
	}

#define DP_OBJ_DTOR(Class) \
__UnregObjValueTable(&typeid(Class), this)

#define DP_PROP_DECL(Class, PropertyName, PropertyType) \
PropertyType& Get##PropertyName() \
{ \
	return *(PropertyType*)GetValue(PropertyName##Property); \
} \
void Set##PropertyName(const PropertyType& value) \
{ \
	SetValue(PropertyName##Property, new PropertyType(value)); \
} \
static DependencyProperty PropertyName##Property

#define DP_PROP_INIT(Class, PropertyName, DefaultValue) \
DependencyProperty Class::PropertyName##Property = \
	DependencyObject::RegisterProperty(&typeid(Class), nameof(PropertyName##Property), DefaultValue)