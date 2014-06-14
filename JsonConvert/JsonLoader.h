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
	//������������ļ�
	void				WriteToFile(FILE* hFile);
	//������������ļ�
	void				WriteToFile_NoString(FILE* hFile,uint64_t& vWritePosition,uint64_t& vWriteSize);
	//������������ļ�
	void				WriteToFile_String(FILE* hFile);
	//�Ӷ������ļ��ж�ȡ
	char*				ReadFromBuff(char* pBuff);
	//�Ӷ������ļ��ж�ȡ
	char*				ReadFromBuff(char* pBuff,uint64_t& vWritePosition);
	//ȡ�ýṹ��ռ�õĴ�С
	int					GetSize();
};

//JSON�������ڵ�
struct  stJsonObjectDesc
{
	string								m_Name;					//�ڵ�����
	vector<stJsonAttribDesc>			m_AttribVec;			//��������
	bool								m_HasNode;				//������
public:
	//������������
	int					FindAttribDesc(string& strName);
	//������������ļ�
	void				WriteToFile(FILE* hFile);
	//������������ļ�
	void				WriteToFile_Desc(FILE* hFile,uint64_t& vDescPosition,uint64_t& vAttribPosition,uint64_t& vStringSize);
	//������������ļ�
	void				WriteToFile_Attrib(FILE* hFile,uint64_t& vDescWritePosition,uint64_t& vWriteSize);
	//������������ļ�
	void				WriteToFile_String(FILE* hFile);
	//������������ļ�
	void				WriteToFile_AttribString(FILE* hFile);
	//�Ӷ������ļ��ж�ȡ
	char*				ReadFromBuff(char* pBuff);
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
	//������������ļ�
	void				WriteToFile(JsonLoader*		pJson,FILE* hFile,bool bIsRoot = true);
	//������������ļ�
	void				WriteToFile_NoString(JsonLoader*		pJson,uint64_t& vStringSize,FILE* hFile);
	//������������ļ�
	void				WriteChildToFile_NoString(JsonLoader*		pJson,uint64_t& vStringSize,FILE* hFile);
	//������������ļ�
	void				WriteToFile_String(JsonLoader*		pJson,FILE* hFile);
	//������������ļ�
	void				WriteChildToFile_String(JsonLoader*		pJson,FILE* hFile);
	//�Ӷ������ļ��ж�ȡ
	char*				ReadFromBuff(JsonLoader*		pJson,char* pBuff,bool bIsArray,stJsonObjectDesc* pJsonObjDesc,int vAttribIndex);

	//ȡ�����е��ӽ�㣬��ڵ�
	int					GetAllJsonNodes(vector<stJsonNode*>&  vAllChildVec);

};

//�����ýڵ���������
struct  stExpJsonAttribDesc
{
	Type								m_Type;					//��Ӧ������
	uint64_t/*char*		*/				m_szName;				//��Ӧ������
	uint64_t/*char*		*/				m_szDefaultValue;		//��Ӧ��Ĭ��ֵ
public:
	//�ؽ�
	void	ReBuild(char* pStringMemoryAddr)
	{
		m_szName = m_szName + (uint64_t)pStringMemoryAddr;
		m_szDefaultValue = m_szDefaultValue + (uint64_t)pStringMemoryAddr;
	}
}
;
//������JSON�������ڵ�
struct  stExpJsonObjectDesc
{
	uint32_t							m_nAttribNum;			//��������
	uint64_t/*char*		*/				m_szName;				//��Ӧ�������ַ�����λ��,Ĭ��Ϊ0
	uint64_t/*stExpJsonAttribDesc*	*/	m_pAttribDescArray;		//��������
public:
	//�ؽ�
	void	ReBuild(char* pAttribMemoryAddr,char* pStringMemoryAddr)
	{
		m_szName = m_szName + (uint64_t)pStringMemoryAddr;
		m_pAttribDescArray = m_pAttribDescArray + (uint64_t)pAttribMemoryAddr;
		stExpJsonAttribDesc* tpAttribDescArray = (stExpJsonAttribDesc*)m_pAttribDescArray;
		for(int i = 0 ; i < m_nAttribNum ; i++)
		{
			tpAttribDescArray[i].ReBuild(pStringMemoryAddr);
		}
	}

};


//JSON�ڵ�����
struct  stExpJsonNode
{
friend class CJsonNodeExport;
friend struct stJsonNode;
protected:
	int32_t							m_ObjIndex;				//��Ӧ����������
	int32_t							m_AttribIndex;			//���Ե���������
	uint32_t						m_ChildNum;				//�ӽڵ�����
	uint64_t/*char*		*/			m_szValue;				//��Ӧ��Ĭ��ֵ
	uint64_t/*stExpJsonNode*	*/	m_ChildArray;			//��Ӧ���ӽڵ�����
public:

	//ȡ����
	Type				GetType(JsonLoader*		pJson);
	//ȡ����
	char*				GetName(JsonLoader*		pJson);
	//ȡֵ
	char*				GetValue();
	//ȡ���ӽ������
	int					GetChildNum();
	//ȡ���ӽ������
	stExpJsonNode*		GetChildArray();
public:
	//�ؽ�
	inline  void	ReBuild(char* pJsonNodeAddr,char* pStringMemoryAddr);
	//��ӡ
	void	Print(JsonLoader*		pJson,void*	pFileName = NULL,int vLayer = 0,bool bEndNode = false,bool bParentNodeIsArray = false);
}
;

//JsonNode������
class	CJsonNodeExport
{
	uint32_t						m_nJsonNodeCount;
	uint32_t						m_nFillIndex;
	uint64_t/*stExpJsonNode* */		m_pJsonNodeArray;
public:
	//����
	CJsonNodeExport();
	//����
	~CJsonNodeExport();
public:
	//����
	bool				CreateMemory(int vCount);
	//���
	void				ResetFill();
	//�������
	int					FillRootDate(JsonLoader*		pJson,stJsonNode*	pJsonNode,uint64_t& vStringDataAddr,bool bCopyData);
	//���
	int					FillDate(JsonLoader*		pJson,stJsonNode*	pJsonNode,uint64_t& vStringDataAddr,bool bCopyData);
	//ȡ���ڴ�
	stExpJsonNode*		GetMemory();
};

//������ͷ�ļ�
struct		stCocoFileHeader
{
	char			m_FileDesc[32];
	char			m_Version[32];
	int				m_ObjectCount;
};
;
 
//������ͷ�ļ�
struct		stCocoFileHeader_Fast
{
	char			m_FileDesc[32];
	char			m_Version[32];
	uint32_t		m_nFirstUsed;
	uint32_t		m_ObjectCount;
	uint64_t		m_lAttribMemAddr;
	uint64_t		m_JsonNodeMemAddr;
	uint64_t		m_lStringMemAddr;
	
};

//JsonLoader
class JsonLoader
{
	//�����
	stJsonNode					m_RootNode;
	//��������
	vector<stJsonObjectDesc>	m_ObjectDescVec;

	//ͷ��Ϣ
	stCocoFileHeader_Fast*		m_pFileHeader;
	//�����
	stExpJsonNode*				m_pRootNode_Fast;
	//��������
	stExpJsonObjectDesc*		m_pObjectDescArray_Fast;
	//

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
	//�����������ļ�
	bool					WriteCocoBinBuff(const char* pBinFileName,stJsonNode&	vNewJsonNode);
	//�Ӷ������ļ��ж�ȡ
	bool					ReadCocoBinBuff(char* pBinBuff);

	//�����������ļ�
	bool					WriteCocoBinBuff_Fast(const char* pBinFileName,stJsonNode&	vNewJsonNode);
	//�Ӷ������ļ��ж�ȡ
	bool					ReadCocoBinBuff_Fast(char* pBinBuff);


	//ȡ�ø����
	stJsonNode&				GetRootJsonNode();
	//��ѯ��Ӧ����������	��������
	int						GetObjectDescIndex(string& strName);
	//ȡ����Ӧ��������
	stJsonObjectDesc&		GetObjectDesc(int vIndex);	
	//ȡ�õ���ʱ����Ľ�ʹ�õ�������
	int						GetUsedObjectDescIndex(int vIndex);

	//ͷ��Ϣ
	stCocoFileHeader_Fast*	GetFileHeader_Fast(){return m_pFileHeader;}
	//�����
	stExpJsonNode*			GetJsonNode_Fast(){return	m_pRootNode_Fast;}
	//��������
	stExpJsonObjectDesc*	GetJsonObjectDesc_Fast(){return	m_pObjectDescArray_Fast;}
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