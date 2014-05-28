#pragma once

class TX_Item
{
private:
protected:
public:
	TX_Item()
		{}

	virtual ~TX_Item() {}
	virtual const TiXmlNode * ParseNode(const TiXmlNode * node, const char * name, const TiXmlNode * prev = NULL) = 0;
	virtual bool Integrate() {return true;}
	virtual bool IsIntegrated() const {return true;}
	virtual bool Validate() const {return IsIntegrated();}
	virtual bool IsValid() const {return IsIntegrated();}

	virtual bool IsDefault() const {return false;}
	virtual void WriteNode(TiXmlNode * node, const char * name, bool suppress_defaults = true) const = 0;
};


class TX_Long : public TX_Item
{
private:
	long Value;
protected:
public:

	TX_Long( long def = 666 )
		:Value( def )
		{}

	virtual ~TX_Long() {}

	virtual const TiXmlNode * ParseNode( const TiXmlNode * node, const char * name, const TiXmlNode * prev = NULL )
	{
		const TiXmlNode* mynode = node->IterateChildren( name, prev );
		if ( mynode )
		{
			Value = GetLong( mynode, 777 );
		}
		return mynode;
	}

	virtual void WriteNode(TiXmlNode * node, const char * name, bool suppress_defaults = true) const
	{
		AddLong( node, name, Value );
		if (suppress_defaults) //suppress_defaults does nothing
			return;
	}

	operator long () const
	{
		return Value;
	}
};

class TX_String : public TX_Item
{
private:
	bool bDefault;
	std::string Value;
protected:
public:

	TX_String( const char *def = "" )
		:bDefault( true )
		,Value( def )
		{}

	virtual ~TX_String() {}

	virtual bool IsDefault() const { return bDefault; }

	virtual const TiXmlNode * ParseNode( const TiXmlNode * node, const char * name, const TiXmlNode * prev = NULL )
	{
		const TiXmlNode* mynode = node->IterateChildren( name, prev );
		if ( mynode )
		{
			const char * str = GetString( mynode );

			if ( Value != str )
			{
				Value = str;
				bDefault = false;
			}
		}
		return mynode;
	}

	virtual void WriteNode(TiXmlNode * node, const char * name, bool suppress_defaults = true) const
	{
		if ( !suppress_defaults || !IsDefault() )
			AddString( node, name, Value.data() );
	}

	operator const char * ()
	{
		return Value.data();
	}
};

struct ItemOffset
{
	const char * name;
	int offset;
};


template
	<typename T
	,char const *UsualName
	>
class TX_Structure : public TX_Item
{
	static const ItemOffset Itemz[];
	static const T * Exzample;
	 
public:
	static char const*Namex; 

	TX_Structure()
		{}

	virtual ~TX_Structure() {}

	virtual bool IsDefault() const
	{
		for ( int i = 0; Itemz[i].name; i++ )
		{
			if ( !((TX_Item *)((char *)this + Itemz[i].offset))->IsDefault() )
				return false;
		}
		return true;
	}

	virtual bool Integrate()
	{
		bool ret = true;
		for ( int i = 0; Itemz[i].name; i++ )
		{
			//try integrating everything 
			if ( !((TX_Item *)((char *)this + Itemz[i].offset))->Integrate() )
				ret = false;
		}
		return ret;
	}

	virtual bool IsIntegrated() const
	{
		for ( int i = 0; Itemz[i].name; i++ )
		{
			if ( !((TX_Item *)((char *)this + Itemz[i].offset))->IsIntegrated() )
				return false;
		}
		return true;
	}

	virtual const TiXmlNode * ParseNode( const TiXmlNode * node, char const * name = UsualName, const TiXmlNode * prev = NULL )
	{
		const TiXmlNode* mynode = node->IterateChildren( name, prev);
		if ( mynode )
		{
			for ( int i = 0; Itemz[i].name; i++ )
			{
				((TX_Item *)((char *)this + Itemz[i].offset))->ParseNode(mynode, Itemz[i].name);
			}
		}
		return mynode;
	}

	virtual void WriteNode(TiXmlNode * node, char const * name = UsualName, bool suppress_defaults = true) const
	{
		if (!suppress_defaults || !IsDefault())
		{
			TiXmlElement * mynode = new TiXmlElement(name);
			for (int i = 0; Itemz[i].name; i++)
			{
				((TX_Item *)((char *)this + Itemz[i].offset))->WriteNode(mynode, Itemz[i].name, suppress_defaults);
			}
			node->LinkEndChild(mynode);
		}
	}
};


template
	<typename T
	,char const *UsualName
	>
char const* TX_Structure<T,UsualName>::Namex = UsualName; 

template
	<typename T
	,char const *UsualName
	>
const T * TX_Structure<T,UsualName>::Exzample = NULL;


template
	<typename ElemType
	,char const *UsualName
	>
class TX_Vector : public TX_Item
{
public:
	static char const*Namex; 
	std::vector<ElemType *> m_vec;

	TX_Vector()
		{}

	virtual ~TX_Vector() 
		{Clear();}

	void Clear()
		{for (int elem = m_vec.size()-1;elem >= 0; elem--) delete (m_vec[elem]); 
		m_vec.clear();}

	virtual bool IsDefault() const
		{return false;}

	virtual const TiXmlNode * ParseNode(const TiXmlNode * node, const char * name = UsualName, const TiXmlNode * prev = NULL)
	{
		Clear();
		const TiXmlNode* mynode = node->IterateChildren( name, prev);
		if (mynode)
		{
			const TiXmlNode* childnode = NULL;
			do
			{			
				ElemType * child = new (ElemType);
				childnode = child->ParseNode(mynode,child->Namex,childnode);
				if(childnode)
					m_vec.push_back(child);
				else
					delete (child);
			}
			while (childnode);
		}
		return mynode;
	}

	virtual void WriteNode(TiXmlNode * node, const char * name, bool suppress_defaults = true) const
	{
		if (!suppress_defaults || !IsDefault())
		{
			TiXmlElement * mynode = new TiXmlElement(name);
			for (int i = 0; i<m_vec.size(); i++)
			{
				m_vec[i]->WriteNode(mynode, m_vec[i]->Namex, suppress_defaults);
			}
			node->LinkEndChild(mynode);
		}
	}
};

template
	<typename ElemType
	,char const *UsualName
	>
char const* TX_Vector<ElemType,UsualName>::Namex = UsualName; 


#define DECLARE_TX_LOADER(The_Class,The_Template) \
	class The_Class; \
	extern const char The_Class##Name[]; \
	typedef The_Template<The_Class, The_Class##Name> The_Class##Loader; \

#define DEFINE_CLASS_NAME(The_Class) \
	extern const char The_Class##Name[] = #The_Class ; \


#define BEGIN_TX_LOAD_TABLE(The_Class) \
	const ItemOffset The_Class##Loader::Itemz[] = \
	{ \


#define TX_ITEM(The_Item) \
			{#The_Item,((char *)(&Exzample->The_Item) - (char *)(Exzample))}, \


#define END_TX_LOAD_TABLE() \
		{NULL,0} \
	}; \

