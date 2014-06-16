#ifndef _JSONLOADER_H
#define _JSONLOADER_H
//=========================================================================
//File:JsonLoader.h
//Author: Bianan@chukong-inc.com
//Desc:����JSON�ĵ����ļ�����
//=========================================================================

#include <stdio.h>
#include <vector>
#include <cstdio>
#include "stdint.h"
#include "rapidjson.h"
#include "document.h"
#pragma pack (4)
using namespace std;
using namespace rapidjson;
//const	char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
class	JsonLoader;
struct   stJsonNode;
//�ڵ���������
struct  stJsonAttribDesc
{
	Type				m_Type;
	string				m_Name;
	string				m_DefaultValue;
public:
	//�Ƚ���������
	bool				CompareAttribDesc(string& strName,bool bFastCompare = true);
	//ȡ�ýṹ��ռ�õĴ�С
	int					GetSize();
};

//JSON�������ڵ�
struct  stJsonObjectDesc
{
	vector<string>						m_NameVec;				//�ڵ���������
	vector<stJsonAttribDesc>			m_AttribVec;			//��������
	bool								m_HasNode;				//������
public:
	//������������
	int					FindAttribDesc(string& strName,int& nNameIndex);
};


//JSON�ڵ�����
struct  stJsonNode
{
	Type				m_Type;
	string				m_Name;
	string				m_Value;
	int32_t				m_ObjIndex;
	int32_t				m_AttribIndex;
	uint64_t			m_lChildMemAddr;
	vector<stJsonNode>	m_ChildVec;
public:
	//����
	stJsonNode();
	//��ӡ
	void				Print(void*	pFileName = NULL,int vLayer = 0,bool bEndNode = false,bool bParentNodeIsArray = false);
	//���ⲿ�����ȡ���뵱ǰƥ��Ľڵ�
	stJsonNode			GetGoodJsonNode(JsonLoader*		pJson,bool bIsRoot = true,const char* szParentArrayName = NULL);
	//�����ӽ��
	int					FindChild(const char* szName);
	//ȡ�����е��ӽ�㣬��ڵ�
	int					GetAllJsonNodes(vector<stJsonNode*>&  vAllChildVec);

};

//JsonLoader
class JsonLoader
{
	//�����
	stJsonNode					m_RootNode;
	//��������
	vector<stJsonObjectDesc>	m_ObjectDescVec;

	//�汾
	string						m_strVersion;

public:
	//����
	JsonLoader();
	//����
	~JsonLoader();

public:
	//����JSON
	bool					ReadCocoJsonBuff(const char* pJsonBuff);
	//��ӡJSON
	void					PrintAllJsonNode();

	//ȡ�ø����
	stJsonNode&				GetRootJsonNode();
	//��ѯ��Ӧ����������	��������
	int						GetObjectDescIndex(string& strName);
	//ȡ����Ӧ��������
	stJsonObjectDesc&		GetObjectDesc(int vIndex);	
	//ȡ�õ���ʱ����Ľ�ʹ�õ�������
	int						GetUsedObjectDescIndex(int vIndex);
	//ȡ�ð汾
	const string&			GetVersion(){return m_strVersion;}
private:
	//����һ�����
	stJsonNode&				AddJsonNode(Type vType,string strName,string strValue,stJsonNode& vParentNode);
	//����һ�����
	stJsonNode&				AddJsonNode_ArrayItem(const rapidjson::Value &	vValue,stJsonNode& vParentNode);
	//����һ�����
	stJsonNode&				AddJsonNode_Object(const rapidjson::Value &	vValue,stJsonNode& vParentNode);

private:
	//��¼һ������
	void					AddObjectDesc(string strName,const rapidjson::Value &	vValue);
	//ȡ��һ��ֵ����ַ���
	string					GetValueString(const rapidjson::Value &	vValue);
			
};
#pragma pack ()
#endif