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
#include "rapidjson.h"
#include "document.h"

using namespace std;
using namespace rapidjson;
//const	char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
class	JsonLoader;
class   stJsonNode;
//�ڵ���������
struct  stJsonAttribDesc
{
	Type				m_Type;
	string				m_Name;
	string				m_DefaultValue;
public:
	//������������ļ�
	void				WriteToFile(FILE* hFile);
	//�Ӷ������ļ��ж�ȡ
	char*				ReadFromBuff(char* pBuff);
};

//JSON��ڵ�
struct  stJsonObjectDesc
{
	char								m_MD5[32];			//32�ַ�MD5���Ӧ
	string								m_Name;				//�ڵ�����
	vector<stJsonAttribDesc>			m_AttribVec;		//��������
	vector<stJsonNode*>					m_MemoryAddrVec;	//��Ӧ�ڴ��ַ
public:
	//������������
	int					FindAttribDesc(string& strName);
	//������������ļ�
	void				WriteToFile(FILE* hFile);
	//�Ӷ������ļ��ж�ȡ
	char*				ReadFromBuff(char* pBuff);
};


//JSON�ڵ�����
struct  stJsonNode
{
	Type				m_Type;
	string				m_Name;
	string				m_Value;
	vector<stJsonNode>	m_ChildVec;
public:
	//����
	stJsonNode();
	//��ӡ
	void				Print(void*	pFileName = NULL,int vLayer = 0,bool bEndNode = false);
	//���ⲿ�����ȡ���뵱ǰƥ��Ľڵ�
	stJsonNode			GetGoodJsonNode(JsonLoader*		pJson,bool bIsRoot = true);
	//�����ӽ��
	int					FindChild(const char* szName);
	//������������ļ�
	void				WriteToFile(FILE* hFile);
	//�Ӷ������ļ��ж�ȡ
	char*				ReadFromBuff(char* pBuff);
	//ȡ�ó��������ƽṹ��ռ�õĴ�С
	int					GetPackedSize();

};


//������ͷ�ļ�
struct		stCocoFileHeader
{
	char	m_FileDesc[32];
	char	m_Version[32];
	int		m_ObjectCount;
};


//JsonLoader
class JsonLoader
{
	//�����
	stJsonNode					m_RootNode;
	//��������
	vector<stJsonObjectDesc>	m_ObjectDescVec;
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
	//д��������ļ�
	bool					WriteCocoBinBuff(const char* pBinFileName,stJsonNode&	vNewJsonNode);
	//�Ӷ������ļ��ж�ȡ
	bool					ReadCocoBinBuff(char* pBinBuff);
	//ȡ�ø����
	stJsonNode&				GetRootJsonNode();
	//��ѯ��Ӧ����������	��������
	int						GetObjectDescIndex(string& strName);
	//ȡ����Ӧ��������
	stJsonObjectDesc&		GetObjectDesc(int vIndex);	
	//������е��������ϵ����ݽڵ�
	void					CleanObjectMemoryAddr();
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

#endif