#include "JsonLoader.h"
#include <tchar.h>
#include <windows.h>
const	char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
const   char* kObjKeyName[] = { "__type" , "classname" };
char	g_Buff[2048];

void string_replace(std::string& strBig, const std::string & strsrc, const std::string &strdst) 
{
	std::string::size_type pos = 0;
	while( (pos = strBig.find(strsrc, pos)) != string::npos)
	{
		strBig.replace(pos, strsrc.length(), strdst);
		pos += strdst.length();
	}
}


//��һ��ASCII�ļ�ת��ΪUTF8�ļ�
bool    ConvAsciiTxtToUTF8(const char* szAsciiFileName,const char* szUTF8TxtFile)
{
	FILE    *AsciiFile = fopen(szAsciiFileName,"rt");
	if( NULL == AsciiFile )
	{
		return false;
	}
	long    filesize = 0;
	if( fseek( AsciiFile,0,SEEK_END ))
	{
		return false;
	}
	filesize = ftell( AsciiFile );
	if( fseek( AsciiFile,0,SEEK_SET ) )
	{
		return false;
	}

	FILE    *UtfFile = fopen(szUTF8TxtFile,"wt,ccs=UTF-8");
	if( NULL == UtfFile )
	{
		return false;
	}    

	long    offset = 0;
	WCHAR   utfArr[4096];    

	while(1)
	{
		memset(g_Buff,'\0',2048);
		memset(utfArr,'\0',4096);
		int acual_count = fread( g_Buff,sizeof(char),2047,AsciiFile);

		if( acual_count <= 0 )        
			break;

		int nWCharSize = MultiByteToWideChar(CP_ACP,NULL,g_Buff,-1,utfArr,4096);
		if( 0 == nWCharSize)
		{
			return false;
		}
		
		fwrite(utfArr,  sizeof(wchar_t), nWCharSize, UtfFile);  
			
		offset += acual_count;
		if( offset >= filesize)
			break;
	}

	if( AsciiFile )
	{
		fclose( AsciiFile );
		AsciiFile = NULL ;    
	}

	if( UtfFile )
	{
		fclose(UtfFile);
		UtfFile = NULL;
	}

	return true;
}


//�����ȼ�ȡ�����������
int     GetObjectIndex(JsonLoader*	pLoader,vector<stJsonNode>& vChildVec)
{
	for(int i = 0 ; i < 2; i++)
	{
		vector<stJsonNode>::iterator tIter;
		for(tIter = vChildVec.begin(); tIter != vChildVec.end(); tIter++)
		{
			if(0 == strcmp(tIter->m_Name.c_str(),kObjKeyName[i]))
			{
				return pLoader->GetObjectDescIndex(tIter->m_Value);
			}
		}
	}
	return -1;
}
//�Ƚ���������
bool	stJsonAttribDesc::CompareAttribDesc(string& strName,bool bFastCompare)
{
	//=====================================================
	vector<string> m_strVec2;
	const	char*	pChar2 = m_Name.c_str();
	int		nLen2 = m_Name.size();
	char*   szMemory2 = new char[nLen2+1];
	strcpy(szMemory2,pChar2);
	char*   szFindStr2 = szMemory2 ;
	szFindStr2 = strtok(szMemory2,",");
	while(szFindStr2)
	{
		string str = szFindStr2 ;
		m_strVec2.push_back(str);

		szFindStr2 = strtok(NULL,",");
		if(!szFindStr2)break;
	}
	delete[] szMemory2;

	if(bFastCompare)
	{
		//=====================================================
		vector<string>::iterator tIter2;
		for(tIter2 = m_strVec2.begin(); tIter2 != m_strVec2.end(); tIter2++)
		{
			if( 0 == strcmp(strName.c_str(),tIter2->c_str()) )
			{
				return true;
			}

		}
	}
	else
	{
		//=====================================================
		vector<string> m_strVec1;
		const	char*	pChar = strName.c_str();
		int		nLen = strName.size();
		char*   szMemory = new char[nLen+1];
		strcpy(szMemory,pChar);
		char*   szFindStr = szMemory ;
		szFindStr = strtok(szMemory,",");
		while(szFindStr)
		{
			string str = szFindStr ;
			m_strVec1.push_back(str);

			szFindStr = strtok(NULL,",");
			if(!szFindStr)break;
		}
		delete[] szMemory;

		//=====================================================
		vector<string>::iterator tIter;
		for(tIter = m_strVec1.begin(); tIter != m_strVec1.end(); tIter++)
		{
			vector<string>::iterator tIter2;
			for(tIter2 = m_strVec2.begin(); tIter2 != m_strVec2.end(); tIter2++)
			{
				if( 0 == strcmp(tIter2->c_str(),tIter->c_str()) )
				{
					return true;
				}
			}
		}
	}
	
	return false;
}
//������������ļ�
void	stJsonAttribDesc::WriteToFile(FILE* hFile)
{
	//Type
	fwrite(&m_Type,sizeof(Type),1,hFile);

	//Name
	int  nLen = m_Name.size();
	fwrite(&nLen,sizeof(int),1,hFile);
	fwrite(m_Name.c_str(),sizeof(char),nLen,hFile);

	//DefaultValue
	nLen = m_DefaultValue.size();
	fwrite(&nLen,sizeof(int),1,hFile);
	fwrite(m_DefaultValue.c_str(),sizeof(char),nLen,hFile);
}
//������������ļ�
void	stJsonAttribDesc::WriteToFile_NoString(FILE* hFile,uint64_t& vWritePosition,uint64_t& vWriteSize)
{

	stExpJsonAttribDesc	tAttribDesc;
	tAttribDesc.m_Type = m_Type ;
	tAttribDesc.m_szName = vWriteSize;
	vWriteSize += m_Name.size()+1;
	tAttribDesc.m_szDefaultValue = vWriteSize;
	vWriteSize += m_DefaultValue.size()+1;

	vWritePosition	+= sizeof(stExpJsonAttribDesc);

	fwrite(&tAttribDesc,sizeof(stExpJsonAttribDesc),1,hFile);
}
//������������ļ�
void	stJsonAttribDesc::WriteToFile_String(FILE* hFile)
{
	int nLen = m_Name.size()+1;
	//Name
	fwrite(m_Name.c_str(),sizeof(char),nLen,hFile);
	//DefaultValue
	nLen = m_DefaultValue.size()+1;
	fwrite(m_DefaultValue.c_str(),sizeof(char),nLen,hFile);
}
//�Ӷ������ļ��ж�ȡ
char*	stJsonAttribDesc::ReadFromBuff(char* pBuff)
{
	//Type
	char*	pTempBuff = pBuff;
	memcpy(&m_Type,pTempBuff,sizeof(Type));
	pTempBuff += sizeof(Type);

	//Name
	int  nLen = 0;
	memcpy(&nLen,pTempBuff,sizeof(int));
	pTempBuff += sizeof(int);

	memcpy(g_Buff,pTempBuff,nLen);
	pTempBuff += nLen;
	g_Buff[nLen]='\0';

	m_Name = g_Buff;

	//DefaultValue
	memcpy(&nLen,pTempBuff,sizeof(int));
	pTempBuff += sizeof(int);

	memcpy(g_Buff,pTempBuff,nLen);
	pTempBuff += nLen;
	g_Buff[nLen]='\0';

	m_DefaultValue = g_Buff;

	return pTempBuff ;
}	

//ȡ�ýṹ��ռ�õĴ�С
int		stJsonAttribDesc::GetSize()
{
	int nSize = sizeof(m_Type);

	nSize += sizeof(int);
	nSize += m_Name.size();

	nSize += sizeof(int);
	nSize += m_DefaultValue.size();

	return nSize;
}

//������������
int		stJsonObjectDesc::FindAttribDesc(string& strName)
{
	vector<stJsonAttribDesc>::iterator tIter;


	//=====================================================
	vector<string> m_strVec2;
	const	char*	pChar2 = strName.c_str();
	int		nLen2 = strName.size();
	char*   szMemory2 = new char[nLen2+1];
	strcpy(szMemory2,pChar2);
	char*   szFindStr2 = szMemory2 ;
	szFindStr2 = strtok(szMemory2,",");
	while(szFindStr2)
	{
		string str = szFindStr2 ;
		m_strVec2.push_back(str);

		szFindStr2 = strtok(NULL,",");
		if(!szFindStr2)break;
	}
	delete[] szMemory2;

	vector<string>::iterator tStrIter;
	for(tStrIter = m_strVec2.begin();tStrIter != m_strVec2.end(); tStrIter++)
	{
		for(tIter = m_AttribVec.begin();tIter != m_AttribVec.end(); tIter++)
		{
			if( tIter->CompareAttribDesc(*tStrIter) )
			{
				strName = *tStrIter;
				return (tIter - m_AttribVec.begin());
			}
		}

	}
	return -1;
}
//������������ļ�
void	stJsonObjectDesc::WriteToFile(FILE* hFile)
{
	//Name
	int  nLen = m_Name.size();
	fwrite(&nLen,sizeof(int),1,hFile);
	fwrite(m_Name.c_str(),sizeof(char),nLen,hFile);
	//������������
	int nAttribNum = m_AttribVec.size();
	fwrite(&nAttribNum,sizeof(int),1,hFile);

	for(int i = 0 ; i < nAttribNum ; i++)
	{
		m_AttribVec[i].WriteToFile(hFile);
	}
}
//������������ļ�
void	stJsonObjectDesc::WriteToFile_Desc(FILE* hFile,uint64_t& vDescWritePosition,uint64_t& vAttribWritePosition,uint64_t& vWriteSize)
{

	stExpJsonObjectDesc	tObjectDesc;
	tObjectDesc.m_szName = vWriteSize ;
	tObjectDesc.m_nAttribNum = m_AttribVec.size();
	tObjectDesc.m_pAttribDescArray = vAttribWritePosition;

	vWriteSize			+= m_Name.size()+1;
	vDescWritePosition	+= sizeof(stExpJsonObjectDesc);
	vAttribWritePosition+= tObjectDesc.m_nAttribNum * sizeof(stExpJsonAttribDesc);

	fwrite(&tObjectDesc,sizeof(stExpJsonObjectDesc),1,hFile);
}
//������������ļ�
void	stJsonObjectDesc::WriteToFile_Attrib(FILE* hFile,uint64_t& vDescWritePosition,uint64_t& vWriteSize)
{
	int nAttribNum = m_AttribVec.size();
	for(int i = 0 ; i < nAttribNum ; i++)
	{
		m_AttribVec[i].WriteToFile_NoString(hFile,vDescWritePosition,vWriteSize);
	}
}
//������������ļ�
void	stJsonObjectDesc::WriteToFile_String(FILE* hFile)
{
	int nLen = m_Name.size()+1;
	//Name
	fwrite(m_Name.c_str(),sizeof(char),nLen,hFile);
}
//������������ļ�
void	stJsonObjectDesc::WriteToFile_AttribString(FILE* hFile)
{
	//������������
	int nAttribNum = m_AttribVec.size();
	for(int i = 0 ; i < nAttribNum ; i++)
	{
		m_AttribVec[i].WriteToFile_String(hFile);
	}
}
//�Ӷ������ļ��ж�ȡ
char*	stJsonObjectDesc::ReadFromBuff(char* pBuff)
{

	char*	pTempBuff = pBuff;
	//Name
	int  nLen = 0;
	memcpy(&nLen,pTempBuff,sizeof(int));
	pTempBuff += sizeof(int);

	memcpy(g_Buff,pTempBuff,nLen);
	pTempBuff += nLen;
	g_Buff[nLen]='\0';

	m_Name = g_Buff;

	int nAttribNum = 0;
	memcpy(&nAttribNum,pTempBuff,sizeof(int));
	pTempBuff += sizeof(int);
	for(int i = 0 ; i < nAttribNum ; i++)
	{
		stJsonAttribDesc	tAttribDesc;
		pTempBuff = tAttribDesc.ReadFromBuff(pTempBuff);
		m_AttribVec.push_back(tAttribDesc);
	}	

	return pTempBuff;
}

//����
stJsonNode::stJsonNode()
{
	m_Type		= kNullType ;
	m_ObjIndex	= -1;
	m_AttribIndex = -1;
	m_lChildMemAddr = 0;
}	

//��ӡ
void	stJsonNode::Print(void*	pFileName,int vLayer,bool bEndNode,bool bParentNodeIsArray)
{
	FILE*	pFile = NULL;
	if(pFileName)
	{
		if(vLayer == 0)
		{
			pFile = fopen((const char*)pFileName,"wt");
			if(!pFile)return ;
			fprintf(pFile,"{\n");
		}
		else
		{
			pFile = (FILE*)pFileName;
		}
	}


	if(pFile)
	{
		for(int i = 0 ; i < vLayer ; i++)
		{
			fprintf(pFile," ");
		}
	}
	else
	{	
		for(int i = 0 ; i < vLayer ; i++)
		{
			printf(" ");
		}

	}

	bool bThieNodeIsArray = false;

	switch(m_Type)
	{
	case kNullType:
		{
			if(m_Name.size() > 0)
			{
				if(pFile)
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\":null\n",m_Name.c_str());
					}
					else
					{
						fprintf(pFile,"\"%s\":null,\n",m_Name.c_str());
					}
				}
				else
				{
					printf("%s:null\n",m_Name.c_str());
				}

			}

		}
		break;
	case kFalseType:
		{
			if(pFile)
			{
				if(bEndNode)
				{
					fprintf(pFile,"\"%s\":false\n",m_Name.c_str());
				}
				else
				{
					fprintf(pFile,"\"%s\":false,\n",m_Name.c_str());
				}

			}
			else
			{
				printf("%s:false\n",m_Name.c_str());
			}
		}
		break;
	case kTrueType:
		{
			if(pFile)
			{
				if(bEndNode)
				{
					fprintf(pFile,"\"%s\":true\n",m_Name.c_str());
				}
				else
				{
					fprintf(pFile,"\"%s\":true,\n",m_Name.c_str());
				}

			}
			else
			{
				printf("%s:true\n",m_Name.c_str());
			}

		}
		break;
	case kObjectType:
		{
			if(pFile)
			{
				if(false == m_Name.empty() && false == bParentNodeIsArray)
				{
					fprintf(pFile,"\"%s\":\n",m_Name.c_str());
				}

				vLayer++;
				for(int i = 0 ; i < vLayer; i++)
				{
					fprintf(pFile," ");
				}
				fprintf(pFile,"{\n");
			}
			else
			{
				printf("%s:\n",m_Name.c_str());

				vLayer++;
				for(int i = 0 ; i < vLayer; i++)
				{
					printf(" ");
				}
				printf("{\n");
			}

		}
		break;
	case kArrayType:
		{
			if(pFile)
			{
				if(false == m_Name.empty())
				{
					fprintf(pFile,"\"%s\":[\n",m_Name.c_str());
				}
				vLayer++;

			}
			else
			{
				printf("%s:\n",m_Name.c_str());
				vLayer++;
				for(int i = 0 ; i < vLayer; i++)
				{
					printf(" ");
				}
				printf("[\n");
			}

			bThieNodeIsArray = true;
		}
		break;
	case kStringType:
		{
			string strResult = m_Value;
			string_replace(strResult,"\\","\\\\");

			if(pFile)
			{
				if(false == m_Name.empty())
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\":\"%s\"\n",m_Name.c_str(),strResult.c_str());
					}
					else
					{
						fprintf(pFile,"\"%s\":\"%s\",\n",m_Name.c_str(),strResult.c_str());
					}
				}
				else
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\"\n",strResult.c_str());
					}
					else
					{
						fprintf(pFile,"\"%s\",\n",strResult.c_str());
					}
				}
				

			}
			else
			{
				printf("%s:String:%s\n",m_Name.c_str(),strResult.c_str());
			}

		}
		break;
	case kNumberType:
		{
			if(pFile)
			{
				if(false == m_Name.empty())
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\":%s\n",m_Name.c_str(),m_Value.c_str());
					}
					else
					{
						fprintf(pFile,"\"%s\":%s,\n",m_Name.c_str(),m_Value.c_str());
					}
				}
				else
				{
					if(bEndNode)
					{
						fprintf(pFile,"%s\n",m_Value.c_str());
					}
					else
					{
						fprintf(pFile,"%s,\n",m_Value.c_str());
					}
				}

			}
			else
			{
				printf("%s:Number:%s\n",m_Name.c_str(),m_Value.c_str());
			}

		}
		break;
	}



	vector<stJsonNode>::iterator tIter;
	for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
	{
		if(tIter == m_ChildVec.end()-1)
		{
			tIter->Print(pFile,vLayer+1,true,bThieNodeIsArray);
		}
		else
		{
			tIter->Print(pFile,vLayer+1,false,bThieNodeIsArray);
		}

	}

	switch(m_Type)
	{
	case kObjectType:
		{
			if(pFile)
			{
				for(int i = 0 ; i < vLayer; i++)
				{
					fprintf(pFile," ");
				}
				if(bEndNode)
				{
					fprintf(pFile,"}\n");
				}
				else
				{
					fprintf(pFile,"},\n");
				}

			}
			else
			{
				for(int i = 0 ; i < vLayer; i++)
				{
					printf(" ");
				}
				printf("}\n");
			}

		}
		break;
	case kArrayType:
		{
			if(pFile)
			{
				for(int i = 0 ; i < vLayer; i++)
				{
					fprintf(pFile," ");
				}
				if(bEndNode)
				{
					fprintf(pFile,"]\n");
				}
				else
				{
					fprintf(pFile,"],\n");
				}

			}
			else
			{
				for(int i = 0 ; i < vLayer; i++)
				{
					printf(" ");
				}
				printf("]\n");
			}

		}
		break;
	}

	if(vLayer == 0&&pFile)
	{
		fprintf(pFile,"}\n");
		fclose(pFile);
/*
		char szFileName[_MAX_PATH];
		sprintf(szFileName,"%s.utf8",pFileName);
		if(true == ConvAsciiTxtToUTF8((const char*)pFileName,szFileName))
		{
			DeleteFile((const char*)pFileName);
			rename(szFileName,(const char*)pFileName);
		}
*/
	}
}

//���ⲿ�����ȡ���뵱ǰƥ��Ľڵ�
stJsonNode	stJsonNode::GetGoodJsonNode(JsonLoader*		pJson,bool bIsRoot,const char* szParentArrayName)
{
	stJsonNode	tNewJsonNode;
	tNewJsonNode.m_Type  = m_Type;
	tNewJsonNode.m_Name  = m_Name;
	tNewJsonNode.m_Value = m_Value;
	tNewJsonNode.m_ObjIndex = -1;
	tNewJsonNode.m_AttribIndex = -1;
	if(false == bIsRoot)
	{
		tNewJsonNode.m_ObjIndex  = GetObjectIndex(pJson,m_ChildVec);
		
		vector<stJsonNode>::iterator tIter;
		/*for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
		{
			if(0 == strcmp(tIter->m_Name.c_str(),"classname"))
			{
				tNewJsonNode.m_ObjIndex  = pJson->GetObjectDescIndex(tIter->m_Value);
				break;
			}
			if(0 == strcmp(tIter->m_Name.c_str(),"__type"))
			{
				tNewJsonNode.m_ObjIndex  = pJson->GetObjectDescIndex(tIter->m_Value);
				break;
			}
		}
		*/
		if( tNewJsonNode.m_ObjIndex  < 0 && kObjectType == m_Type)
		{
			if(m_Name.size() > 0)
			{
				tNewJsonNode.m_ObjIndex  = pJson->GetObjectDescIndex(m_Name);
				tNewJsonNode.m_Name  = m_Name;
			}
			else if(szParentArrayName)
			{
				string strParentArrayName = szParentArrayName;
				tNewJsonNode.m_ObjIndex  = pJson->GetObjectDescIndex(strParentArrayName);
				tNewJsonNode.m_Name  = szParentArrayName;
			}

		}
		if( tNewJsonNode.m_ObjIndex  >= 0 )
		{
			
			stJsonObjectDesc&	vObjectDesc = pJson->GetObjectDesc(tNewJsonNode.m_ObjIndex );


			for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
			{

				int	nIndex = vObjectDesc.FindAttribDesc(tIter->m_Name);
				if(nIndex >= 0)
				{

					bool bComType = (tIter->m_Type == vObjectDesc.m_AttribVec[nIndex].m_Type)?true:false;

					if(tIter->m_Type == kFalseType && vObjectDesc.m_AttribVec[nIndex].m_Type == kTrueType)bComType = true;
					if(tIter->m_Type == kTrueType && vObjectDesc.m_AttribVec[nIndex].m_Type == kFalseType)bComType = true;
					if(tIter->m_Type == kNullType && vObjectDesc.m_AttribVec[nIndex].m_Type == kStringType )
					{
						tIter->m_Type = kStringType;
						tIter->m_Value="null";
						bComType = true;
					}
					if(tIter->m_Type == kObjectType && vObjectDesc.m_AttribVec[nIndex].m_Type == kNullType)bComType = true;
					if(tIter->m_Type == kArrayType && vObjectDesc.m_AttribVec[nIndex].m_Type == kNullType)bComType = true;

					//if(tIter->m_Type == kStringType && vObjectDesc.m_AttribVec[nIndex].m_Type == kNullType )bComType = true;
					if(true == bComType)
					{

						if(tIter->m_Type == kObjectType)
						{
							stJsonNode	tNewObjectJsonNode = tIter->GetGoodJsonNode(pJson,false);
							tNewJsonNode.m_ChildVec.push_back(tNewObjectJsonNode);
							continue ;

						}
						if(tIter->m_Type == kArrayType)
						{
							stJsonNode	tNewArrayJsonNode = tIter->GetGoodJsonNode(pJson,false);
							tNewJsonNode.m_ChildVec.push_back(tNewArrayJsonNode);
							continue ;
						}

						stJsonNode	tNewChildJsonNode;
						tNewChildJsonNode.m_Type  = tIter->m_Type;
						tNewChildJsonNode.m_Name  = tIter->m_Name;
						tNewChildJsonNode.m_Value = tIter->m_Value;
						tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
						tNewChildJsonNode.m_AttribIndex = nIndex;
						tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
						continue ;
					}
					else if(0 == strcmp(tIter->m_Name.c_str(),"classname"))
					{
						stJsonNode	tNewChildJsonNode;
						tNewChildJsonNode.m_Type  = tIter->m_Type;
						tNewChildJsonNode.m_Name  = tIter->m_Name;
						tNewChildJsonNode.m_Value = tIter->m_Value;
						tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
						tNewChildJsonNode.m_AttribIndex = nIndex;
						tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
						continue ;
					}
					else if(0 == strcmp(tIter->m_Name.c_str(),"__type"))
					{
						stJsonNode	tNewChildJsonNode;
						tNewChildJsonNode.m_Type  = tIter->m_Type;
						tNewChildJsonNode.m_Name  = tIter->m_Name;
						tNewChildJsonNode.m_Value = tIter->m_Value;
						tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
						tNewChildJsonNode.m_AttribIndex = nIndex;
						tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
						continue ;
					}
					else if(tIter->m_Type == kNullType && (vObjectDesc.m_AttribVec[nIndex].m_Type == kObjectType||vObjectDesc.m_AttribVec[nIndex].m_Type == kArrayType))
					{
						stJsonNode	tNewChildJsonNode;
						tNewChildJsonNode.m_Type  = tIter->m_Type;
						tNewChildJsonNode.m_Name  = tIter->m_Name;
						tNewChildJsonNode.m_Value = tIter->m_Value;
						tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
						tNewChildJsonNode.m_AttribIndex = nIndex;
						tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
						continue ;
					}
				}
				else if(0 == strcmp(tIter->m_Name.c_str(),"children"))
				{
					stJsonNode	tNewArrayJsonNode = tIter->GetGoodJsonNode(pJson,false);
					tNewJsonNode.m_ChildVec.push_back(tNewArrayJsonNode);
					continue ;
				}
				else if(tIter->m_Type == kObjectType)
				{
					int	nAttribIndex = pJson->GetObjectDescIndex(tIter->m_Name);
					if( nAttribIndex >= 0 )
					{
						stJsonNode	tNewObjectJsonNode = tIter->GetGoodJsonNode(pJson,false);
						tNewJsonNode.m_ChildVec.push_back(tNewObjectJsonNode);
						continue ;
					}
				}
			}
			//��ȱʧ�������Բ���
			vector<stJsonAttribDesc>::iterator tIter2;
			for(tIter2 = vObjectDesc.m_AttribVec.begin(); tIter2 != vObjectDesc.m_AttribVec.end(); tIter2++)
			{
				bool bFind = false;
				vector<stJsonNode>::iterator tIter3;
				for(tIter3 = m_ChildVec.begin(); tIter3 != m_ChildVec.end(); tIter3++)
				{
					if(0 == strcmp(tIter3->m_Name.c_str(),tIter2->m_Name.c_str()))
					{
						bFind = true;
						break;
					}
				}

				if(false == bFind)
				{
					stJsonNode	tNewChildJsonNode;
					tNewChildJsonNode.m_Type  = tIter2->m_Type;
					tNewChildJsonNode.m_Name  = tIter2->m_Name;
					tNewChildJsonNode.m_Value = tIter2->m_DefaultValue;
					tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
					tNewChildJsonNode.m_AttribIndex = int(tIter2 - vObjectDesc.m_AttribVec.begin());
					tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
				}
			}

		}
		//else if(0 == strcmp(m_Name.c_str(),"children"))
		//{
		//	for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
		//	{
		//		if(tIter->m_Type == kObjectType)
		//		{
		//			stJsonNode	tNewObjectJsonNode = tIter->GetGoodJsonNode(pJson,false);
		//			tNewJsonNode.m_ChildVec.push_back(tNewObjectJsonNode);
		//			continue ;

		//		}
		//		if(tIter->m_Type == kArrayType)
		//		{
		//			stJsonNode	tNewArrayJsonNode = tIter->GetGoodJsonNode(pJson,false);
		//			tNewJsonNode.m_ChildVec.push_back(tNewArrayJsonNode);
		//			continue ;
		//		}
		//	}
		//}
		else if(m_Type == kArrayType)
		{
			for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
			{
				if(tIter->m_Type == kObjectType)
				{
					stJsonNode	tNewObjectJsonNode = tIter->GetGoodJsonNode(pJson,false,m_Name.c_str());
					tNewJsonNode.m_ChildVec.push_back(tNewObjectJsonNode);
					continue ;

				}
				if(tIter->m_Type == kArrayType)
				{
					stJsonNode	tNewArrayJsonNode = tIter->GetGoodJsonNode(pJson,false);
					tNewJsonNode.m_ChildVec.push_back(tNewArrayJsonNode);
					continue ;
				}

				stJsonNode	tNewChildJsonNode;
				tNewChildJsonNode.m_Type  = tIter->m_Type;
				tNewChildJsonNode.m_Name  = tIter->m_Name;
				tNewChildJsonNode.m_Value = tIter->m_Value;
				tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
				tNewChildJsonNode.m_AttribIndex = int(tIter - m_ChildVec.begin());
				tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
				continue ;
			}
		}
	}
	else
	{
		tNewJsonNode.m_ObjIndex = 0;
		stJsonObjectDesc&	vObjectDesc = pJson->GetObjectDesc(tNewJsonNode.m_ObjIndex);

		vector<stJsonNode>::iterator tIter;
		for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
		{

			int	nIndex = vObjectDesc.FindAttribDesc(tIter->m_Name);
			if(nIndex >= 0)
			{			
				bool bComType = (tIter->m_Type == vObjectDesc.m_AttribVec[nIndex].m_Type)?true:false;
				if(tIter->m_Type == kFalseType && vObjectDesc.m_AttribVec[nIndex].m_Type == kTrueType)bComType = true;
				if(tIter->m_Type == kTrueType && vObjectDesc.m_AttribVec[nIndex].m_Type == kFalseType)bComType = true;
				if(tIter->m_Type == kNullType && vObjectDesc.m_AttribVec[nIndex].m_Type == kStringType )
				{
					tIter->m_Type = kStringType;
					tIter->m_Value="null";
					bComType = true;
				}
				if(tIter->m_Type == kObjectType && vObjectDesc.m_AttribVec[nIndex].m_Type == kNullType)bComType = true;
				if(tIter->m_Type == kArrayType && vObjectDesc.m_AttribVec[nIndex].m_Type == kNullType)bComType = true;

				//if(tIter->m_Type == kStringType && vObjectDesc.m_AttribVec[nIndex].m_Type == kNullType )bComType = true;
				if(bComType)
				{
					if(tIter->m_Type == kObjectType)
					{
						stJsonNode	tNewObjectJsonNode = tIter->GetGoodJsonNode(pJson,false);
						tNewJsonNode.m_ChildVec.push_back(tNewObjectJsonNode);
						continue ;

					}
					if(tIter->m_Type == kArrayType)
					{
						stJsonNode	tNewArrayJsonNode = tIter->GetGoodJsonNode(pJson,false);
						tNewJsonNode.m_ChildVec.push_back(tNewArrayJsonNode);
						continue ;
					}

					stJsonNode	tNewChildJsonNode;
					tNewChildJsonNode.m_Type  = tIter->m_Type;
					tNewChildJsonNode.m_Name  = tIter->m_Name;
					tNewChildJsonNode.m_Value = tIter->m_Value;
					tNewChildJsonNode.m_ObjIndex = 0;
					tNewChildJsonNode.m_AttribIndex = nIndex;
					tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
					continue ;
				}
				else if(0 == strcmp(tIter->m_Name.c_str(),"classname"))
				{
					stJsonNode	tNewChildJsonNode;
					tNewChildJsonNode.m_Type  = tIter->m_Type;
					tNewChildJsonNode.m_Name  = tIter->m_Name;
					tNewChildJsonNode.m_Value = tIter->m_Value;
					tNewChildJsonNode.m_ObjIndex = 0;
					tNewChildJsonNode.m_AttribIndex = nIndex;
					tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
					continue ;
				}
				else if(0 == strcmp(tIter->m_Name.c_str(),"__type"))
				{
					stJsonNode	tNewChildJsonNode;
					tNewChildJsonNode.m_Type  = tIter->m_Type;
					tNewChildJsonNode.m_Name  = tIter->m_Name;
					tNewChildJsonNode.m_Value = tIter->m_Value;
					tNewChildJsonNode.m_ObjIndex = 0;
					tNewChildJsonNode.m_AttribIndex = nIndex;
					tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
					continue ;
				}
				else if(tIter->m_Type == kNullType && (vObjectDesc.m_AttribVec[nIndex].m_Type == kObjectType||vObjectDesc.m_AttribVec[nIndex].m_Type == kArrayType))
				{
					stJsonNode	tNewChildJsonNode;
					tNewChildJsonNode.m_Type  = tIter->m_Type;
					tNewChildJsonNode.m_Name  = tIter->m_Name;
					tNewChildJsonNode.m_Value = tIter->m_Value;
					tNewChildJsonNode.m_ObjIndex = tNewJsonNode.m_ObjIndex;
					tNewChildJsonNode.m_AttribIndex = nIndex;
					tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
					continue ;
				}
			}
			else if(0 == strcmp(tIter->m_Name.c_str(),"children"))
			{
				stJsonNode	tNewArrayJsonNode = tIter->GetGoodJsonNode(pJson,false);
				tNewJsonNode.m_ChildVec.push_back(tNewArrayJsonNode);
				continue ;
			}
		}

		//��ȱʧ�������Բ���
		vector<stJsonAttribDesc>::iterator tIter2;
		for(tIter2 = vObjectDesc.m_AttribVec.begin(); tIter2 != vObjectDesc.m_AttribVec.end(); tIter2++)
		{
			bool bFind = false;
			vector<stJsonNode>::iterator tIter3;
			for(tIter3 = m_ChildVec.begin(); tIter3 != m_ChildVec.end(); tIter3++)
			{
				if(0 == strcmp(tIter3->m_Name.c_str(),tIter2->m_Name.c_str()))
				{
					bFind = true;
					break;
				}
			}

			if(false == bFind)
			{
				stJsonNode	tNewChildJsonNode;
				tNewChildJsonNode.m_Type  = tIter2->m_Type;
				tNewChildJsonNode.m_Name  = tIter2->m_Name;
				tNewChildJsonNode.m_Value = tIter2->m_DefaultValue;
				tNewChildJsonNode.m_ObjIndex = 0;
				tNewChildJsonNode.m_AttribIndex = tIter2 - vObjectDesc.m_AttribVec.begin();
				tNewJsonNode.m_ChildVec.push_back(tNewChildJsonNode);
			}
		}

	}

	

	return tNewJsonNode ;
}

//�����ӽ��
int		stJsonNode::FindChild(const char* szName)
{
	int nIndex = 0;
	vector<stJsonNode>::iterator tIter;
	for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
	{
		if( 0 == strcmp ( tIter->m_Name.c_str() , szName ))
		{
			return nIndex ;
		}
		nIndex++;
	}
	return -1;
}
//������������ļ�
void	stJsonNode::WriteToFile(JsonLoader*		pJson,FILE* hFile,bool bIsRoot)
{
	if( m_Type == kObjectType || bIsRoot)
	{
		//Obj Index
		int    n_UsedObjectIndex  = pJson->GetUsedObjectDescIndex(m_ObjIndex);
		fwrite(&n_UsedObjectIndex,sizeof(int),1,hFile);

		if(n_UsedObjectIndex >= 0)
		{
			int nAttribNum = m_ChildVec.size();
			fwrite(&nAttribNum,sizeof(int),1,hFile);
			vector<stJsonNode>::iterator tIter;
			for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
			{
				tIter->WriteToFile(pJson,hFile,false);
			}
		}

	}
	else if( m_Type == kArrayType )
	{
		int nChildNum = m_ChildVec.size();
		fwrite(&nChildNum,sizeof(int),1,hFile);
		if(false == m_ChildVec.empty())
		{

			int  nLen = m_Name.size();
			fwrite(&nLen,sizeof(int),1,hFile);
			fwrite(m_Name.c_str(),sizeof(char),nLen,hFile);

			//�����ʹ洢һ��
			fwrite(&m_ChildVec.front().m_Type,sizeof(Type),1,hFile);

			vector<stJsonNode>::iterator tIter;
			for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
			{
				tIter->WriteToFile(pJson,hFile,false);
			}
		}

	}
	else 
	{
		if(m_Type == kNullType)return ;
		//if(m_Type == kFalseType)return ;
		//if(m_Type == kTrueType)return ;

		int  nLen = m_Value.size();
		fwrite(&nLen,sizeof(int),1,hFile);
		fwrite(m_Value.c_str(),sizeof(char),nLen,hFile);
	}
}

//������������ļ�
void	stJsonNode::WriteToFile_NoString(JsonLoader*		pJson,uint64_t& vStringSize,FILE* hFile)
{
	
	stExpJsonNode	tWriteNode;
	tWriteNode.m_ObjIndex = m_ObjIndex ;
	tWriteNode.m_AttribIndex = m_AttribIndex ;
	tWriteNode.m_szValue = vStringSize ;
	vStringSize += m_Value.size()+1;
	tWriteNode.m_ChildNum = m_ChildVec.size();
	tWriteNode.m_ChildArray = m_lChildMemAddr;

	fwrite(&tWriteNode,sizeof(stExpJsonNode),1,hFile);

}
//������������ļ�
void	stJsonNode::WriteChildToFile_NoString(JsonLoader*		pJson,uint64_t& vStringSize,FILE* hFile)
{
	vector<stJsonNode>::iterator tIter;
	for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
	{
		tIter->WriteToFile_NoString(pJson,vStringSize,hFile);
	}
}
//������������ļ�
void	stJsonNode::WriteToFile_String(JsonLoader*		pJson,FILE* hFile)
{
	
	if(m_ObjIndex >= 0)
	{
		if(m_AttribIndex >= 0)
		{
			int  nLen = m_Value.size()+1;
			fwrite(m_Value.c_str(),sizeof(char),nLen,hFile);
		}
		else
		{
			stJsonObjectDesc	tObjectDesc = pJson->GetObjectDesc(m_ObjIndex);
			if(0 == strcmp(m_Name.c_str(),tObjectDesc.m_Name.c_str()))
			{
				int  nLen = m_Value.size()+1;
				fwrite(m_Value.c_str(),sizeof(char),nLen,hFile);
			}
			else
			{
				int  nLen = m_Name.size()+1;
				fwrite(m_Name.c_str(),sizeof(char),nLen,hFile);
			}
		}
	}
	else
	{

		if(m_AttribIndex >= 0)
		{
			//�������ͨ����Ԫ�أ����ӽ�������ַ��ʾ����
			int  nLen = m_Name.size()+1;
			fwrite(m_Name.c_str(),sizeof(char),nLen,hFile);
			nLen = m_Value.size()+1;
			fwrite(m_Value.c_str(),sizeof(char),nLen,hFile);
		}
		else
		{
			//��������飬�洢����
			int  nLen = m_Name.size()+1;
			fwrite(m_Name.c_str(),sizeof(char),nLen,hFile);
		}

	}

}
//������������ļ�
void	stJsonNode::WriteChildToFile_String(JsonLoader*		pJson,FILE* hFile)
{
	vector<stJsonNode>::iterator tIter;
	for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
	{
		tIter->WriteToFile_String(pJson,hFile);
	}
}
//�Ӷ������ļ��ж�ȡ
char*	stJsonNode::ReadFromBuff(JsonLoader*		pJson,char* pBuff,bool bIsArray,stJsonObjectDesc* pJsonObjDesc,int vAttribIndex)
{
	char*	pTempBuff = pBuff;
	if( vAttribIndex == -1 )
	{
		if(bIsArray)
		{
			int nObjNum = 0;
			memcpy(&nObjNum,pTempBuff,sizeof(int));
			pTempBuff += sizeof(int);

			if( nObjNum > 0 )
			{
				int  nLen = 0;
				memcpy(&nLen,pTempBuff,sizeof(int));
				pTempBuff += sizeof(int);

				memcpy(g_Buff,pTempBuff,nLen);
				pTempBuff += nLen;
				g_Buff[nLen]='\0';

				m_Name = g_Buff;

				Type	tType;
				memcpy(&tType,pTempBuff,sizeof(Type));
				pTempBuff += sizeof(Type);

				for(int i = 0 ; i < nObjNum ; i++)
				{
					stJsonNode	tAttrib;
					pTempBuff = tAttrib.ReadFromBuff(pJson,pTempBuff,true,NULL,i);
					tAttrib.m_Type = tType;
					m_ChildVec.push_back(tAttrib);
				}	
			}

		}
		else
		{
			//Obj Index
			memcpy(&m_ObjIndex,pTempBuff,sizeof(int));
			pTempBuff += sizeof(int);


			if( m_ObjIndex >= 0 )
			{
				stJsonObjectDesc&	tJsonObjDesc = pJson->GetObjectDesc(m_ObjIndex);

				int nAttribNum = 0;
				memcpy(&nAttribNum,pTempBuff,sizeof(int));
				pTempBuff += sizeof(int);

				for(int i = 0 ; i < nAttribNum ; i++)
				{
					stJsonNode	tAttrib;
					pTempBuff = tAttrib.ReadFromBuff(pJson,pTempBuff,false,&tJsonObjDesc,i);
					m_ChildVec.push_back(tAttrib);
				}	
			}

		}
	}
	else
	{
		if(bIsArray)
		{
			int  nLen = 0;
			memcpy(&nLen,pTempBuff,sizeof(int));
			pTempBuff += sizeof(int);

			memcpy(g_Buff,pTempBuff,nLen);
			pTempBuff += nLen;
			g_Buff[nLen]='\0';

			m_Value = g_Buff;
		}
		else if(pJsonObjDesc)
		{
			m_Type = pJsonObjDesc->m_AttribVec[vAttribIndex].m_Type ;
			m_Name = pJsonObjDesc->m_AttribVec[vAttribIndex].m_Name ;

			if( m_Type == kArrayType )
			{

				int nObjNum = 0;
				memcpy(&nObjNum,pTempBuff,sizeof(int));
				pTempBuff += sizeof(int);

				if( nObjNum > 0)
				{
					int  nLen = 0;
					memcpy(&nLen,pTempBuff,sizeof(int));
					pTempBuff += sizeof(int);

					memcpy(g_Buff,pTempBuff,nLen);
					pTempBuff += nLen;
					g_Buff[nLen]='\0';

					m_Name = g_Buff;

					Type	tType;	
					memcpy(&tType,pTempBuff,sizeof(Type));
					pTempBuff += sizeof(Type);

					if(kObjectType == tType)
					{
						for(int i = 0 ; i < nObjNum ; i++)
						{
							stJsonNode	tAttrib;
							pTempBuff = tAttrib.ReadFromBuff(pJson,pTempBuff,false,NULL,-1);
							tAttrib.m_Type = tType;
							m_ChildVec.push_back(tAttrib);
						}	
					}
					else if(kArrayType == tType)
					{
						for(int i = 0 ; i < nObjNum ; i++)
						{
							stJsonNode	tAttrib;
							pTempBuff = tAttrib.ReadFromBuff(pJson,pTempBuff,true,NULL,-1);
							tAttrib.m_Type = tType;
							m_ChildVec.push_back(tAttrib);
						}	
					}
					else 
					{
						for(int i = 0 ; i < nObjNum ; i++)
						{
							stJsonNode	tAttrib;
							pTempBuff = tAttrib.ReadFromBuff(pJson,pTempBuff,true,NULL,i);
							tAttrib.m_Type = tType;
							m_ChildVec.push_back(tAttrib);
						}	
					}

				}

			}
			else if( m_Type == kObjectType )
			{
				memcpy(&m_ObjIndex,pTempBuff,sizeof(int));
				pTempBuff += sizeof(int);


				if( m_ObjIndex >= 0 )
				{
					stJsonObjectDesc&	tJsonObjDesc = pJson->GetObjectDesc(m_ObjIndex);

					int nAttribNum = 0;
					memcpy(&nAttribNum,pTempBuff,sizeof(int));
					pTempBuff += sizeof(int);

					for(int i = 0 ; i < nAttribNum ; i++)
					{
						stJsonNode	tAttrib;
						pTempBuff = tAttrib.ReadFromBuff(pJson,pTempBuff,false,&tJsonObjDesc,i);
						m_ChildVec.push_back(tAttrib);
					}	
				}

			}
			else
			{

				if(m_Type == kNullType)
				{
					return pTempBuff;
				}
				/*
				if(m_Type == kFalseType)
				{
					return pTempBuff;
				}
				if(m_Type == kTrueType)
				{
					return pTempBuff;
				}
				*/

				int  nLen = 0;
				memcpy(&nLen,pTempBuff,sizeof(int));
				pTempBuff += sizeof(int);

				memcpy(g_Buff,pTempBuff,nLen);
				pTempBuff += nLen;
				g_Buff[nLen]='\0';

				m_Value = g_Buff;

				if(m_Type == kFalseType)
				{
					if(0 == strcmp(g_Buff,"1"))
					{
						m_Type = kTrueType	;
					}
				}
				else if(m_Type == kTrueType)
				{
					if(0 == strcmp(g_Buff,"0"))
					{
						m_Type = kFalseType;	
					}
				}
			}
		}
	}

	return pTempBuff;
}
//ȡ�����е��ӽ�㣬��ڵ�
int		stJsonNode::GetAllJsonNodes(vector<stJsonNode*>&  vAllChildVec)
{
	int nChildCount = 0;
	//���е�
	if(false == m_ChildVec.empty())
	{
		nChildCount += m_ChildVec.size();
		vector<stJsonNode>::iterator tIter;
		for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
		{
			if(false == tIter->m_ChildVec.empty())
			{
				vAllChildVec.push_back(&(*tIter));
			}
		}

		for(tIter = m_ChildVec.begin(); tIter != m_ChildVec.end(); tIter++)
		{
			nChildCount += tIter->GetAllJsonNodes(vAllChildVec);
		}
	}

	return nChildCount;
}
//ȡ����
Type	stExpJsonNode::GetType(JsonLoader*		pJson)
{
	Type	tType = kObjectType;
	if(m_ObjIndex >= 0)
	{
		stExpJsonObjectDesc*	tpJsonObjectDesc = pJson->GetJsonObjectDesc_Fast();
		if( m_AttribIndex >= 0 )
		{
			stExpJsonAttribDesc* tpAttribDescArray = (stExpJsonAttribDesc*) tpJsonObjectDesc[m_ObjIndex].m_pAttribDescArray;
			tType = tpAttribDescArray[m_AttribIndex].m_Type;

			if(kFalseType == tType || kTrueType == tType)
			{
				char* szValue = (char*)m_szValue;
				if(szValue[0] == '0')
				{
					return kFalseType;
				}
				else
				{
					return kTrueType;
				}
			}

		}
		else
		{
			tType = kObjectType;
		}
	}
	else
	{
		if(m_AttribIndex >= 0)
		{
			tType   = (Type)m_ChildNum;

			if(kFalseType == tType || kTrueType == tType)
			{
				char* szValue = (char*)m_szValue;
				if(szValue[0] == '0')
				{
					return kFalseType;
				}
				else
				{
					return kTrueType;
				}
			}
		}
		else
		{
			tType = kArrayType;
		}
	}
	return tType;
}
//ȡ����
char*	stExpJsonNode::GetName(JsonLoader*		pJson)
{
	char*   szName  = NULL ;
	if(m_ObjIndex >= 0)
	{
		stExpJsonObjectDesc*	tpJsonObjectDesc = pJson->GetJsonObjectDesc_Fast();
		if( m_AttribIndex >= 0 )
		{
			stExpJsonAttribDesc* tpAttribDescArray = (stExpJsonAttribDesc*) tpJsonObjectDesc[m_ObjIndex].m_pAttribDescArray;
			szName = (char*)tpAttribDescArray[m_AttribIndex].m_szName;
		}
		else
		{
			//��������������Ʋ�ͬ������ʵֵ����
			char* szValue = (char*)m_szValue;
			if(szValue[0])
			{
				//����
				szName = (char*)m_szValue;
			}
			else
			{
				//�ṹ
				szName = (char*)tpJsonObjectDesc[m_ObjIndex].m_szName;
			}
		}
	}
	else
	{
		if(m_AttribIndex >= 0)
		{
			char*   pStringAddr = (char*)pJson->GetJsonObjectDesc_Fast() + pJson->GetFileHeader_Fast()->m_lStringMemAddr ;
			szName  = m_ChildArray + pStringAddr;	
		}
		else
		{
			szName = (char*)m_szValue;
		}
	}
	return szName ;
}
//ȡֵ
char*	stExpJsonNode::GetValue()
{
	return (char*)m_szValue;
}
//ȡ���ӽ������
int		stExpJsonNode::GetChildNum()
{
	return m_ChildNum;
}
//ȡ���ӽ������
stExpJsonNode*		stExpJsonNode::GetChildArray()
{
	return (stExpJsonNode*)m_ChildArray;
}
//�ؽ�
void	stExpJsonNode::ReBuild(char* pJsonNodeAddr,char* pStringMemoryAddr)
{
	m_szValue = m_szValue + (uint64_t)pStringMemoryAddr;
	//�������������飬�����ӽ��
	if( -1 == m_AttribIndex )
	{
		if(m_ChildNum > 0)
		{
			m_ChildArray = m_ChildArray + (uint64_t)pJsonNodeAddr;

			stExpJsonNode* tpChildArray = (stExpJsonNode*)m_ChildArray;
			for(int i = 0 ; i < m_ChildNum ; i++)
			{
				tpChildArray[i].ReBuild(pJsonNodeAddr,pStringMemoryAddr);
			}
		}
	}

}
//��ӡ
void	stExpJsonNode::Print(JsonLoader*		pJson,void*	pFileName,int vLayer,bool bEndNode,bool bParentNodeIsArray)
{
	FILE*	pFile = NULL;
	if(pFileName)
	{
		if(vLayer == 0)
		{
			pFile = fopen((const char*)pFileName,"wt");
			if(!pFile)return ;

		}
		else
		{
			pFile = (FILE*)pFileName;
		}
	}


	if(pFile)
	{
		for(int i = 0 ; i < vLayer ; i++)
		{
			fprintf(pFile," ");
		}
	}
	else
	{	
		for(int i = 0 ; i < vLayer ; i++)
		{
			printf(" ");
		}

	}

	bool bThieNodeIsArray = false;

	Type	tType = kObjectType;
	char*   szName  = NULL ;
	char*   szValue = NULL;
	if(m_ObjIndex >= 0)
	{
		szValue = (char*)m_szValue;
		stExpJsonObjectDesc*	tpJsonObjectDesc = pJson->GetJsonObjectDesc_Fast();
		if( m_AttribIndex >= 0 )
		{
			stExpJsonAttribDesc*	tpAttribDescArray = (stExpJsonAttribDesc*)tpJsonObjectDesc[m_ObjIndex].m_pAttribDescArray;
			tType = tpAttribDescArray[m_AttribIndex].m_Type;
			szName = (char*)tpAttribDescArray[m_AttribIndex].m_szName;

			//�����Ojbect������Ԫ�أ���ΪNULL
			if( m_ChildNum == 0 && tType == kObjectType)
			{
				tType = kNullType;
			}

		}
		else
		{
			

			tType = kObjectType;
			//��������������Ʋ�ͬ������ʵֵ����
			char* szValue = (char*)m_szValue;
			if(szValue[0])
			{
				//����
				szName = (char*)m_szValue;
			}
			else
			{
				//�ṹ
				szName = (char*)tpJsonObjectDesc[m_ObjIndex].m_szName;
			}
		}
	}
	else
	{
		if(m_AttribIndex >= 0)
		{
			char*   pStringAddr = (char*)pJson->GetJsonObjectDesc_Fast() + pJson->GetFileHeader_Fast()->m_lStringMemAddr ;
			tType   = (Type)m_ChildNum;
			szName  = m_ChildArray + pStringAddr;	
			szValue = (char*)m_szValue;
		}
		else
		{

			tType = kArrayType;
			szName = (char*)m_szValue;
			szValue = (char*)m_szValue;
		}
	}


	switch(tType)
	{
	case kNullType:
		{

			if(szName && strlen(szName) > 0)
			{
				if(pFile)
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\":null\n",szName);
					}
					else
					{
						fprintf(pFile,"\"%s\":null,\n",szName);
					}
				}
				else
				{
					printf("%s:null\n",szName);
				}

			}
		}
		break;
	case kFalseType:
	case kTrueType:
		{
			if(szValue[0] == '0')
			{
				if(szName && strlen(szName) > 0)
				{
					if(pFile)
					{
						if(bEndNode)
						{
							fprintf(pFile,"\"%s\":false\n",szName);
						}
						else
						{
							fprintf(pFile,"\"%s\":false,\n",szName);
						}

					}
					else
					{
						printf("%s:false\n",szName);
					}
				}
			}
			else
			{
				if(szName && strlen(szName) > 0)
				{
					if(pFile)
					{
						if(bEndNode)
						{
							fprintf(pFile,"\"%s\":true\n",szName);
						}
						else
						{
							fprintf(pFile,"\"%s\":true,\n",szName);
						}

					}
					else
					{
						printf("%s:true\n",szName);
					}
				}
			}

		}
		break;
	case kObjectType:
		{
			
			if(pFile)
			{
				if((szName && strlen(szName) > 0)&& false == bParentNodeIsArray)
				{
					fprintf(pFile,"\"%s\" : { \n",szName);
				}
				else
				{
					if( vLayer > 0 )
					{
						for(int i = 0 ; i < vLayer+2; i++)
						{
							fprintf(pFile," ");
						}
					}

					fprintf(pFile,"{\n");
				}

			}
			else
			{

				if(szName && strlen(szName) > 0)
				{
					printf("%s: {\n",szName);
				}
				else
				{
					if( vLayer > 0 )
					{
						for(int i = 0 ; i < vLayer+2; i++)
						{
							fprintf(pFile," ");
						}
					}
					printf("{\n");
				}

			}
		}
		break;
	case kArrayType:
		{
			if(pFile)
			{
				if(szName && strlen(szName) > 0)
				{
					if(m_ChildNum > 0)
					{
						fprintf(pFile,"\"%s\": [\n",szName);
					}
					else
					{
						fprintf(pFile,"\"%s\": [",szName);
					}

				}
			}
			else
			{
				if(szName && strlen(szName) > 0)
				{
					printf("%s: [\n",szName);
				}
			}

			bThieNodeIsArray = true;
		}
		break;
	case kStringType:
		{
			if(pFile)
			{
				if(szName && strlen(szName) > 0)
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\":\"%s\"\n",szName,szValue);
					}
					else
					{
						fprintf(pFile,"\"%s\":\"%s\",\n",szName,szValue);
					}
				}
				else
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\"\n",szValue);
					}
					else
					{
						fprintf(pFile,"\"%s\",\n",szValue);
					}
				}
			}
			else
			{
				if(szName && strlen(szName) > 0)
				{
					printf("%s:String:%s\n",szName,szValue);
				}
				else
				{
					printf("String:%s\n",szValue);
				}
			}

		}
		break;
	case kNumberType:
		{
			if(pFile)
			{
				if(szName && strlen(szName) > 0)
				{
					if(bEndNode)
					{
						fprintf(pFile,"\"%s\":%s\n",szName,szValue);
					}
					else
					{
						fprintf(pFile,"\"%s\":%s,\n",szName,szValue);
					}
				}
				else
				{
					if(bEndNode)
					{
						fprintf(pFile,"%s\n",szValue);
					}
					else
					{
						fprintf(pFile,"%s,\n",szValue);
					}
				}

			}
			else
			{
				if(szName && strlen(szName) > 0)
				{
					printf("%s:Number:%s\n",szName,szValue);
				}
				else
				{
					printf("Number:%s\n",szValue);
				}
			}

		}
		break;
	}

	if( kObjectType == tType || kArrayType == tType)
	{
		stExpJsonNode* tpChildArray = (stExpJsonNode*)m_ChildArray;

		for(int i = 0 ; i < m_ChildNum ; i++)
		{
			if(i == m_ChildNum-1)
			{
				tpChildArray[i].Print(pJson,pFile,vLayer+1,true,bThieNodeIsArray);
			}
			else
			{
				tpChildArray[i].Print(pJson,pFile,vLayer+1,false,bThieNodeIsArray);
			}

		}
	}

	switch(tType)
	{
	case kObjectType:
		{
			if(pFile)
			{
				if( vLayer > 0 )
				{
					for(int i = 0 ; i < vLayer+2; i++)
					{
						fprintf(pFile," ");
					}
				}
				if(bEndNode||0==vLayer)
				{
					fprintf(pFile,"}\n");
				}
				else
				{
					fprintf(pFile,"},\n");
				}

			}
			else
			{
				if( vLayer > 0 )
				{
					for(int i = 0 ; i < vLayer+2; i++)
					{
						fprintf(pFile," ");
					}
				}
				printf("}\n");
			}

		}
		break;
	case kArrayType:
		{
			if(pFile)
			{
				if( vLayer > 0 && m_ChildNum > 0)
				{
					for(int i = 0 ; i < vLayer+2; i++)
					{
						fprintf(pFile," ");
					}
				}

				if(bEndNode||0==vLayer)
				{
					fprintf(pFile,"]\n");
				}
				else
				{
					fprintf(pFile,"],\n");
				}

			}
			else
			{
				if( vLayer > 0  && m_ChildNum > 0)
				{
					for(int i = 0 ; i < vLayer+2; i++)
					{
						fprintf(pFile," ");
					}
				}
				printf("]\n");
			}

		}
		break;
	}

	if(vLayer == 0&&pFile)
	{
		fclose(pFile);
/*
		char szFileName[_MAX_PATH];
		sprintf(szFileName,"%s.utf8",pFileName);
		if(true == ConvAsciiTxtToUTF8((const char*)pFileName,szFileName))
		{
			DeleteFile((const char*)pFileName);
			rename(szFileName,(const char*)pFileName);
		}
*/
	}
}
//����
CJsonNodeExport::CJsonNodeExport()
{
	m_nJsonNodeCount = 0;
	m_pJsonNodeArray = NULL;
	m_nFillIndex = 0;
}
//����
CJsonNodeExport::~CJsonNodeExport()
{
	stExpJsonNode* pJsonNodeArray = (stExpJsonNode* )m_pJsonNodeArray;
	if(pJsonNodeArray)
	{
		delete[] pJsonNodeArray;
		pJsonNodeArray = NULL;
	}
}
//����
bool	CJsonNodeExport::CreateMemory(int vCount)
{
	stExpJsonNode* pJsonNodeArray = (stExpJsonNode* )m_pJsonNodeArray;
	if(pJsonNodeArray)
	{
		delete[] pJsonNodeArray;
		pJsonNodeArray = NULL;
	}
	m_nJsonNodeCount = vCount ;
	pJsonNodeArray= new stExpJsonNode[vCount];
	m_nFillIndex = 0;
	if(!pJsonNodeArray)return false;
	m_pJsonNodeArray = (uint64_t)pJsonNodeArray;
	return true;
}
//���
void	CJsonNodeExport::ResetFill()
{
	m_nFillIndex = 0;
}
//�������
int		CJsonNodeExport::FillRootDate(JsonLoader*		pJson,stJsonNode*	pJsonNode,uint64_t& vStringDataAddr,bool bCopyData)
{
	stExpJsonNode* pJsonNodeArray = (stExpJsonNode* )m_pJsonNodeArray;
	if(bCopyData)
	{
		//pJsonNodeArray[0].m_Type			= pJsonNode->m_Type;
		pJsonNodeArray[0].m_ObjIndex		= pJsonNode->m_ObjIndex;
		pJsonNodeArray[0].m_AttribIndex		= pJsonNode->m_AttribIndex;
		//m_pJsonNodeArray[0].m_szName		= (char*)vStringDataAddr;
		//vStringDataAddr += pJsonNode->m_Name.size() + 1;
		pJsonNodeArray[0].m_ChildNum		= pJsonNode->m_ChildVec.size();
		pJsonNodeArray[0].m_ChildArray		= pJsonNode->m_lChildMemAddr;

		if(pJsonNode->m_ObjIndex >=0)
		{

			if(pJsonNodeArray[0].m_AttribIndex >= 0)
			{
				pJsonNodeArray[0].m_szValue = vStringDataAddr;
				vStringDataAddr += pJsonNode->m_Value.size() + 1;
			}
			else
			{
				stJsonObjectDesc	tObjectDesc = pJson->GetObjectDesc(pJsonNode->m_ObjIndex);
				if(0 == strcmp(pJsonNode->m_Name.c_str(),tObjectDesc.m_Name.c_str()))
				{
					pJsonNodeArray[0].m_szValue = vStringDataAddr;
					vStringDataAddr += pJsonNode->m_Value.size() + 1;
				}
				else
				{
					pJsonNodeArray[0].m_szValue = vStringDataAddr;
					vStringDataAddr += pJsonNode->m_Name.size() + 1;
				}
			}
		}
		else
		{

			if(pJsonNodeArray[0].m_AttribIndex >= 0)
			{
				//�������ͨ����Ԫ�أ����ӽ��������ʾ����,���ӽ�������ַ��ʾ����
				pJsonNodeArray[0].m_ChildNum = pJsonNode->m_Type;
				pJsonNodeArray[0].m_ChildArray = vStringDataAddr;
				vStringDataAddr += pJsonNode->m_Name.size() + 1;
				pJsonNodeArray[0].m_szValue = vStringDataAddr;
				vStringDataAddr += pJsonNode->m_Value.size() + 1;
			}
			else
			{
				//��������飬��Value������

				pJsonNodeArray[0].m_szValue = vStringDataAddr;
				vStringDataAddr += pJsonNode->m_Name.size() + 1;
			}

		}

	}
	else
	{
		pJsonNode->m_lChildMemAddr = sizeof(stExpJsonNode);
	}

	m_nFillIndex ++ ;
	return m_nFillIndex ;
}
//���
int		CJsonNodeExport::FillDate(JsonLoader*		pJson,stJsonNode*	pJsonNode,uint64_t& vStringDataAddr,bool bCopyData)
{
	int		nChildCount = pJsonNode->m_ChildVec.size() ;

	if(bCopyData)
	{
		stExpJsonNode* pJsonNodeArray = (stExpJsonNode* )m_pJsonNodeArray;

		int nDestIndex = 0;
		for(int i = 0 ; i < nChildCount ; i++)
		{
			nDestIndex = m_nFillIndex + i;
			//pJsonNodeArray[nDestIndex].m_Type = pJsonNode->m_ChildVec[i].m_Type;
			pJsonNodeArray[nDestIndex].m_ObjIndex = pJsonNode->m_ChildVec[i].m_ObjIndex;
			pJsonNodeArray[nDestIndex].m_AttribIndex = pJsonNode->m_ChildVec[i].m_AttribIndex;
			//pJsonNodeArray[nDestIndex].m_szName = (char*)vStringDataAddr;
			//vStringDataAddr += pJsonNode->m_ChildVec[i].m_Name.size() + 1;
			pJsonNodeArray[nDestIndex].m_ChildNum = pJsonNode->m_ChildVec[i].m_ChildVec.size();
			pJsonNodeArray[nDestIndex].m_ChildArray = pJsonNode->m_ChildVec[i].m_lChildMemAddr;

			if(pJsonNodeArray[nDestIndex].m_ObjIndex >= 0)
			{
				if(pJsonNodeArray[nDestIndex].m_AttribIndex >= 0)
				{
					pJsonNodeArray[nDestIndex].m_szValue = vStringDataAddr;
					vStringDataAddr += pJsonNode->m_ChildVec[i].m_Value.size() + 1;
				}
				else
				{
					stJsonObjectDesc	tObjectDesc = pJson->GetObjectDesc(pJsonNodeArray[nDestIndex].m_ObjIndex);
					if(0 == strcmp(pJsonNode->m_ChildVec[i].m_Name.c_str(),tObjectDesc.m_Name.c_str()))
					{
						pJsonNodeArray[nDestIndex].m_szValue = vStringDataAddr;
						vStringDataAddr += pJsonNode->m_ChildVec[i].m_Value.size() + 1;
					}
					else
					{
						pJsonNodeArray[nDestIndex].m_szValue = vStringDataAddr;
						vStringDataAddr += pJsonNode->m_ChildVec[i].m_Name.size() + 1;
					}
				}

			}
			else
			{
				if(pJsonNodeArray[nDestIndex].m_AttribIndex >= 0)
				{
					//�������ͨ����Ԫ�أ����ӽ��������ʾ����,���ӽ�������ַ��ʾ����
					pJsonNodeArray[nDestIndex].m_ChildNum = pJsonNode->m_ChildVec[i].m_Type;
					pJsonNodeArray[nDestIndex].m_ChildArray = vStringDataAddr;
					vStringDataAddr += pJsonNode->m_ChildVec[i].m_Name.size() + 1;
					pJsonNodeArray[nDestIndex].m_szValue = vStringDataAddr;
					vStringDataAddr += pJsonNode->m_ChildVec[i].m_Value.size() + 1;

				}
				else
				{
					//��������飬��Value������
					pJsonNodeArray[nDestIndex].m_szValue = vStringDataAddr;
					vStringDataAddr += pJsonNode->m_ChildVec[i].m_Name.size() + 1;
				}
			}

		}
	}
	else
	{
		pJsonNode->m_lChildMemAddr = m_nFillIndex * sizeof(stExpJsonNode);
	}

	m_nFillIndex += nChildCount ;
	return m_nFillIndex ;
}

//ȡ���ڴ�
stExpJsonNode*		CJsonNodeExport::GetMemory()
{
	return (stExpJsonNode*)m_pJsonNodeArray ;
}
//����
JsonLoader::JsonLoader()
{
	m_pRootNode_Fast = NULL;
	m_pObjectDescArray_Fast = NULL;
}
//����
JsonLoader::~JsonLoader()
{

}

//����
bool	JsonLoader::ReadCocoJsonBuff(const char* pJsonBuff)
{
	std::string clearData(pJsonBuff);
	size_t pos = clearData.rfind("}");
	clearData = clearData.substr(0, pos+1);

	rapidjson::Document jsonDict;

	jsonDict.Parse<0>(clearData.c_str());
	if(jsonDict.HasParseError())
	{
		printf("GetParseError : %s\n",jsonDict.GetParseError());
		return false;
	}

	stJsonObjectDesc		tNewRootObject;

	for (rapidjson::Document::ConstMemberIterator itr = jsonDict.MemberonBegin(); itr != jsonDict.MemberonEnd(); ++itr)
	{
		const char*	szName  = itr->name.GetString();
		string	strName = szName;

		stJsonAttribDesc	tJsonAttrib;
		tJsonAttrib.m_Name = strName ;
		tJsonAttrib.m_Type = itr->value.GetType();
		tNewRootObject.m_AttribVec.push_back(tJsonAttrib);

		if(itr->value.IsNull())
		{
			AddJsonNode(kNullType,strName,"",m_RootNode);
			continue ;
		}
		if(itr->value.IsArray())
		{
			stJsonNode&	tNewJsonNode = AddJsonNode(kArrayType,strName,"",m_RootNode);
			for (rapidjson::SizeType i = 0; i < itr->value.Size(); i++)
			{
				const rapidjson::Value &val = itr->value[i]; 
				AddJsonNode_ArrayItem(val,tNewJsonNode);
			}
			continue ;
		}
		if(itr->value.IsBool())
		{
			if(itr->value.IsFalse())
			{
				AddJsonNode(kFalseType,strName,"0",m_RootNode);
			}
			if(itr->value.IsTrue())
			{
				AddJsonNode(kTrueType,strName,"1",m_RootNode);
			}
			continue ;
		}

		if(itr->value.IsNumber())
		{
			
			if(itr->value.IsInt())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetInt());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,m_RootNode);
				continue;
			}
			if(itr->value.IsInt64())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetInt64());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,m_RootNode);
				continue;
			}
			if(itr->value.IsUint())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetUint());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,m_RootNode);
				continue ;
			}
			if(itr->value.IsUint64())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetUint64());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,m_RootNode);
				continue ;
			}
			if(itr->value.IsDouble())
			{
				char  szText[100];
				sprintf(szText,"%.4f",itr->value.GetDouble());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,m_RootNode);
				continue ;
			}
			continue ;
		}
		if(itr->value.IsObject())
		{
			//�����
			stJsonNode&	tNewJsonNode = AddJsonNode(kObjectType,strName,"",m_RootNode);
			AddJsonNode_Object(itr->value,tNewJsonNode);
			continue ;
		}
		if(itr->value.IsString())
		{
			string	strText = itr->value.GetString() ;
			AddJsonNode(kStringType,strName,strText,m_RootNode);
			continue ;
		}

	}

	if(m_ObjectDescVec.empty())
	{
		m_ObjectDescVec.push_back(tNewRootObject);
	}
	else
	{
		m_ObjectDescVec.insert(m_ObjectDescVec.begin(),tNewRootObject);
	}

	return true;
}
//��ӡJSON
void	JsonLoader::PrintAllJsonNode()
{
	m_RootNode.Print();
}	
//д��������ļ�
bool	JsonLoader::WriteCocoBinBuff(const char* pBinFileName,stJsonNode&	vNewJsonNode)
{

	FILE* hFile = fopen(pBinFileName,"wb");
	if(hFile)
	{
		//ͷ��Ϣ
		stCocoFileHeader	tCocoFileHeader;
		strcpy(tCocoFileHeader.m_FileDesc,"Cocostudio-UI");
		strcpy(tCocoFileHeader.m_Version,"V1.0.1");
		tCocoFileHeader.m_ObjectCount = 0;
		//������
		vector<stJsonObjectDesc>::iterator tIter;
		for(tIter = m_ObjectDescVec.begin();tIter != m_ObjectDescVec.end(); tIter++)
		{
			if(tIter->m_HasNode)
			{
				tCocoFileHeader.m_ObjectCount++;
			}
		}
		fwrite(&tCocoFileHeader,sizeof(stCocoFileHeader),1,hFile);
		//������
		for(tIter = m_ObjectDescVec.begin();tIter != m_ObjectDescVec.end(); tIter++)
		{
			if(tIter->m_HasNode)
			{
				tIter->WriteToFile(hFile);
			}
		}

		vNewJsonNode.WriteToFile(this,hFile);
		fclose(hFile);
	}
	return true;

}
//�Ӷ������ļ��ж�ȡ
bool	JsonLoader::ReadCocoBinBuff(char* pBinBuff)
{
	//Type
	char*	pTempBuff = pBinBuff;

	//ͷ��Ϣ
	stCocoFileHeader	tCocoFileHeader;

	memcpy(tCocoFileHeader.m_FileDesc,pTempBuff,sizeof(tCocoFileHeader.m_FileDesc));
	pTempBuff += sizeof(tCocoFileHeader.m_FileDesc);

	memcpy(tCocoFileHeader.m_Version,pTempBuff,sizeof(tCocoFileHeader.m_Version));
	pTempBuff += sizeof(tCocoFileHeader.m_Version);

	memcpy(&tCocoFileHeader.m_ObjectCount,pTempBuff,sizeof(tCocoFileHeader.m_ObjectCount));
	pTempBuff += sizeof(tCocoFileHeader.m_ObjectCount);

	//������
	for(int i = 0 ; i < tCocoFileHeader.m_ObjectCount ; i++)
	{
		stJsonObjectDesc	tJsonObjDesc;
		pTempBuff = tJsonObjDesc.ReadFromBuff(pTempBuff);
		m_ObjectDescVec.push_back(tJsonObjDesc);
	}

	if(false == m_ObjectDescVec.empty())
	{
		m_RootNode.ReadFromBuff(this,pTempBuff,false,&m_ObjectDescVec[0],-1);
	}

	return true;
}
//д��������ļ�
bool	JsonLoader::WriteCocoBinBuff_Fast(const char* pBinFileName,stJsonNode&	vNewJsonNode)
{

	FILE* hFile = fopen(pBinFileName,"wb");
	if(hFile)
	{
		//ͷ��Ϣ
		stCocoFileHeader_Fast	tCocoFileHeader;
		tCocoFileHeader.m_nFirstUsed = 1;
		strcpy(tCocoFileHeader.m_FileDesc,"Cocostudio-UI");
		strcpy(tCocoFileHeader.m_Version,"V1.0.1");
		tCocoFileHeader.m_ObjectCount    = 0;
		tCocoFileHeader.m_lAttribMemAddr = 0;
		tCocoFileHeader.m_JsonNodeMemAddr= 0;
		tCocoFileHeader.m_lStringMemAddr = 0;
		
		//������
		vector<stJsonObjectDesc>::iterator tIter;
		for(tIter = m_ObjectDescVec.begin();tIter != m_ObjectDescVec.end(); tIter++)
		{
			if(tIter->m_HasNode)
			{
				tCocoFileHeader.m_ObjectCount++;
			}
		}
		fwrite(&tCocoFileHeader,sizeof(stCocoFileHeader_Fast),1,hFile);

		//
		uint64_t lDescSize	= 0;
		uint64_t lAttribSize	= 0;
		uint64_t lStringSize	= 0;
		//������,����ͳһ�ṹ��
		for(tIter = m_ObjectDescVec.begin();tIter != m_ObjectDescVec.end(); tIter++)
		{
			if(tIter->m_HasNode)
			{
				tIter->WriteToFile_Desc(hFile,lDescSize,lAttribSize,lStringSize);
			}
		}

		tCocoFileHeader.m_lAttribMemAddr = lDescSize;
		for(tIter = m_ObjectDescVec.begin();tIter != m_ObjectDescVec.end(); tIter++)
		{
			if(tIter->m_HasNode)
			{
				tIter->WriteToFile_Attrib(hFile,lDescSize,lStringSize);
			}
		}
		tCocoFileHeader.m_JsonNodeMemAddr = lDescSize;

		vector<stJsonNode*>  tAllChildVec;

		stJsonNode	tRootJsonNode;
		tRootJsonNode.m_ChildVec.push_back(vNewJsonNode);
		int		nChildCount = tRootJsonNode.GetAllJsonNodes(tAllChildVec);

		CJsonNodeExport	tJsonNodeExport;
		tJsonNodeExport.CreateMemory(nChildCount);
		vector<stJsonNode*>::iterator tChildIter;

		tJsonNodeExport.FillRootDate(this,&vNewJsonNode,lStringSize,false);
		for(tChildIter = tAllChildVec.begin();tChildIter != tAllChildVec.end(); tChildIter++)
		{
			tJsonNodeExport.FillDate(this,(*tChildIter),lStringSize,false);
		}

		tJsonNodeExport.ResetFill();
		tJsonNodeExport.FillRootDate(this,&vNewJsonNode,lStringSize,true);
		for(tChildIter = tAllChildVec.begin();tChildIter != tAllChildVec.end(); tChildIter++)
		{
			tJsonNodeExport.FillDate(this,(*tChildIter),lStringSize,true);
		}
	
		stExpJsonNode*	tpExpJsonNodeMemory = tJsonNodeExport.GetMemory();

		fwrite(tpExpJsonNodeMemory,sizeof(stExpJsonNode),nChildCount,hFile);
	
		tCocoFileHeader.m_lStringMemAddr = tCocoFileHeader.m_JsonNodeMemAddr + sizeof(stExpJsonNode) * nChildCount;
		//�����࣬�����ַ���
		for(tIter = m_ObjectDescVec.begin();tIter != m_ObjectDescVec.end(); tIter++)
		{
			if(tIter->m_HasNode)
			{
				tIter->WriteToFile_String(hFile);
			}
		}
		//�����࣬�����ַ���
		for(tIter = m_ObjectDescVec.begin();tIter != m_ObjectDescVec.end(); tIter++)
		{
			if(tIter->m_HasNode)
			{
				tIter->WriteToFile_AttribString(hFile);
			}
		}
		vNewJsonNode.WriteToFile_String(this,hFile);
		for(tChildIter = tAllChildVec.begin();tChildIter != tAllChildVec.end(); tChildIter++)
		{
			(*tChildIter)->WriteChildToFile_String(this,hFile);
		}

		//��дһ��
		fseek(hFile,0,SEEK_SET);
		fwrite(&tCocoFileHeader,sizeof(stCocoFileHeader_Fast),1,hFile);
		fclose(hFile);
	}
	return true;

}
//�Ӷ������ļ��ж�ȡ
bool	JsonLoader::ReadCocoBinBuff_Fast(char* pBinBuff)
{
	//Type
	char*	pTempBuff = pBinBuff;

	//ͷ��Ϣ
	m_pFileHeader = (stCocoFileHeader_Fast*)pTempBuff;
	pTempBuff += sizeof(stCocoFileHeader_Fast);

	//������
	char*   pStartAddr = pTempBuff;
	m_pObjectDescArray_Fast = (stExpJsonObjectDesc*)pStartAddr;

	char*   pAttrAddr = pStartAddr + m_pFileHeader->m_lAttribMemAddr ;
	char*	pJsonMemAddr = pStartAddr + m_pFileHeader->m_JsonNodeMemAddr;
	char*   pStringAddr = pStartAddr + m_pFileHeader->m_lStringMemAddr ;

	for(int i = 0 ; i < m_pFileHeader->m_ObjectCount ; i++)
	{
		m_pObjectDescArray_Fast[i].ReBuild(pAttrAddr,pStringAddr)	;
	}

	m_pRootNode_Fast = (stExpJsonNode*)pJsonMemAddr;
	m_pRootNode_Fast->ReBuild(pJsonMemAddr,pStringAddr);
	return true;
}
//ȡ�ø����
stJsonNode&	JsonLoader::GetRootJsonNode()
{
	return m_RootNode;
}
//����һ�����
stJsonNode&		JsonLoader::AddJsonNode(Type vType,string strName,string strValue,stJsonNode& vParentNode)
{
	stJsonNode	tNewJsonNode;
	tNewJsonNode.m_Type = vType ;
	tNewJsonNode.m_Name = strName ;
	tNewJsonNode.m_Value= strValue ;
	tNewJsonNode.m_ObjIndex = -1;
    tNewJsonNode.m_AttribIndex = -1;
	vParentNode.m_ChildVec.push_back(tNewJsonNode);
	return vParentNode.m_ChildVec.back();
}
//����һ�����
stJsonNode&		JsonLoader::AddJsonNode_ArrayItem(const rapidjson::Value &	vValue,stJsonNode& vParentNode)
{
	if(vValue.IsNull())
	{
		return AddJsonNode(kNullType,"","",vParentNode);
	}
	if(vValue.IsArray())
	{
		stJsonNode&	tNewJsonNode = AddJsonNode(kArrayType,"","",vParentNode);
		for (rapidjson::SizeType i = 0; i < vValue.Size(); i++)
		{
			const rapidjson::Value &val = vValue[i]; 
			AddJsonNode_ArrayItem(val,tNewJsonNode);
		}
		return tNewJsonNode;
	}
	if(vValue.IsBool())
	{
		if(vValue.IsFalse())
		{
			return AddJsonNode(kFalseType,"","0",vParentNode);
		}

		return	AddJsonNode(kTrueType,"","1",vParentNode);

	}

	if(vValue.IsNumber())
	{
		
		if(vValue.IsInt())
		{
			char  szText[100];
			sprintf(szText,"%d",vValue.GetInt());
			string	strText = szText ;
			return AddJsonNode(kNumberType,"",strText,vParentNode);
		}
		if(vValue.IsInt64())
		{
			char  szText[100];
			sprintf(szText,"%d",vValue.GetInt64());
			string	strText = szText ;
			return AddJsonNode(kNumberType,"",strText,vParentNode);
		}
		if(vValue.IsUint())
		{
			char  szText[100];
			sprintf(szText,"%d",vValue.GetUint());
			string	strText = szText ;
			return AddJsonNode(kNumberType,"",strText,vParentNode);
		}
		if(vValue.IsUint64())
		{
			char  szText[100];
			sprintf(szText,"%d",vValue.GetUint64());
			string	strText = szText ;
			return AddJsonNode(kNumberType,"",strText,vParentNode);
		}

		char  szText[100];
		sprintf(szText,"%.4f",vValue.GetDouble());
		string	strText = szText ;
		return AddJsonNode(kNumberType,"",strText,vParentNode);
	}
	if(vValue.IsObject())
	{
		stJsonNode&	tNewJsonNode = AddJsonNode(kObjectType,"","",vParentNode);
		//��������
		if(tNewJsonNode.m_Name.size() == 0)
		{
			AddObjectDesc(vParentNode.m_Name,vValue);
			tNewJsonNode.m_Name = vParentNode.m_Name;
		}
		AddJsonNode_Object(vValue,tNewJsonNode);
		return tNewJsonNode;
	}

	string	strText = vValue.GetString() ;
	return	AddJsonNode(kStringType,"",strText,vParentNode);
}
//����һ�����
stJsonNode&		JsonLoader::AddJsonNode_Object(const rapidjson::Value &	vValue,stJsonNode& vParentNode)
{
	bool  bHasObjectDesc = false;

	for (rapidjson::Value::ConstMemberIterator itr = vValue.MemberonBegin(); itr != vValue.MemberonEnd(); ++itr)
	{
		const char*	szName  = itr->name.GetString();
		string	strName = szName;
		if(itr->value.IsNull())
		{
			stJsonNode&	tNewJsonNode = AddJsonNode(kNullType,strName,"",vParentNode);

			if(false == bHasObjectDesc && 0 == strcmp(szName,"classname") )
			{
				if(vParentNode.m_Name.size() == 0)
				{
					AddObjectDesc("",vValue);
					bHasObjectDesc = true;
				}
				else
				{
					AddObjectDesc(vParentNode.m_Name,vValue);
					bHasObjectDesc = true;
				}

			}
			continue ;
		}
		if(itr->value.IsArray())
		{
			stJsonNode&	tNewJsonNode = AddJsonNode(kArrayType,strName,"",vParentNode);
			for (rapidjson::SizeType i = 0; i < itr->value.Size(); i++)
			{
				const rapidjson::Value &val = itr->value[i]; 
				AddJsonNode_ArrayItem(val,tNewJsonNode);
			}
			continue ;
		}
		if(itr->value.IsBool())
		{
			if(itr->value.IsFalse())
			{
				AddJsonNode(kFalseType,strName,"0",vParentNode);
			}
			if(itr->value.IsTrue())
			{
				AddJsonNode(kTrueType,strName,"1",vParentNode);
			}
			continue ;
		}

		if(itr->value.IsNumber())
		{

			if(itr->value.IsInt())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetInt());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,vParentNode);
				continue;
			}
			if(itr->value.IsInt64())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetInt64());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,vParentNode);
				continue;
			}
			if(itr->value.IsUint())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetUint());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,vParentNode);
				continue ;
			}
			if(itr->value.IsUint64())
			{
				char  szText[100];
				sprintf(szText,"%d",itr->value.GetUint64());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,vParentNode);
				continue ;
			}
			if(itr->value.IsDouble())
			{
				char  szText[100];
				sprintf(szText,"%.4f",itr->value.GetDouble());
				string	strText = szText ;
				AddJsonNode(kNumberType,strName,strText,vParentNode);
				continue ;
			}
			continue ;
		}
		if(itr->value.IsObject())
		{
			stJsonNode&	tNewJsonNode = AddJsonNode(kObjectType,strName,"",vParentNode);
			AddJsonNode_Object(itr->value,tNewJsonNode);
			continue ;
		}
		if(itr->value.IsString())
		{
			string	strText = itr->value.GetString() ;
			stJsonNode&	tNewJsonNode = AddJsonNode(kStringType,strName,strText,vParentNode);
			if(false == bHasObjectDesc)
			{
				if(0 == strcmp(szName,"classname"))
				{
					AddObjectDesc(strText,vValue);
					bHasObjectDesc = true;
				}
				if(0 == strcmp(szName,"__type"))
				{
					AddObjectDesc(strText,vValue);
					bHasObjectDesc = true;
				}
			}

			continue ;
		}
	}
	if(false == bHasObjectDesc && vParentNode.m_Name.size() > 0)
	{
		AddObjectDesc(vParentNode.m_Name,vValue);
		bHasObjectDesc = true;
	}

	return vParentNode;
}

//��¼һ������
void	JsonLoader::AddObjectDesc(string strName,const rapidjson::Value &	vValue)
{
	//��������
	vector<stJsonObjectDesc>::iterator tIter;
	for(tIter = m_ObjectDescVec.begin() ; tIter != m_ObjectDescVec.end() ;  tIter++)
	{
		if(0 == strcmp(tIter->m_Name.c_str(),strName.c_str()))
		{

			unsigned int nSize = vValue.MemberonEnd() - vValue.MemberonBegin();
			unsigned int nAttribSize = tIter->m_AttribVec.size();
			if( nSize > nAttribSize )
			{
				for (rapidjson::Value::ConstMemberIterator itr = vValue.MemberonBegin(); itr != vValue.MemberonEnd(); ++itr)
				{
					const char*	szAttribName  = itr->name.GetString();
					vector<stJsonAttribDesc>::iterator tAttribIter;
					bool bFind = false;
					for(tAttribIter = tIter->m_AttribVec.begin();tAttribIter != tIter->m_AttribVec.end(); tAttribIter++)
					{
						if( 0 == strcmp(szAttribName,tAttribIter->m_Name.c_str()))
						{
							/*
							if(tAttribIter->m_Type != itr->value.GetType())
							{
							tAttribIter->m_Type = itr->value.GetType();
							}
							*/
							bFind = true;
							break;
						}
					}

					if(false == bFind)
					{
						stJsonAttribDesc tAttribDesc;
						tAttribDesc.m_Name = szAttribName ;
						tAttribDesc.m_DefaultValue = "";
						tAttribDesc.m_Type = itr->value.GetType();
						tIter->m_AttribVec.push_back(tAttribDesc);

						if( tAttribDesc.m_Type == kObjectType )
						{
							AddObjectDesc(tAttribDesc.m_Name,itr->value);
						}

						if( tAttribDesc.m_Type == kArrayType && itr->value.Empty() == false)
						{
							rapidjson::SizeType i = 0;
							const rapidjson::Value &val = itr->value[i]; 
							if( val.GetType() == kObjectType )
							{
								AddObjectDesc(tAttribDesc.m_Name,val);
							}
						}
					}
				}	
			}
			return ;

		}
	}

	if(strName.empty() == false)
	{
		const	char*	pChar = strName.c_str();
		int		nLen = strName.size();
		char*   szMemory = new char[nLen+1];
		strcpy(szMemory,pChar);
		char*   szFindStr = szMemory ;
		szFindStr = strtok(szFindStr,",");
		while(szFindStr)
		{
			stJsonObjectDesc	tNewObject;
			tNewObject.m_Name  = szFindStr ;
			for (rapidjson::Value::ConstMemberIterator itr = vValue.MemberonBegin(); itr != vValue.MemberonEnd(); ++itr)
			{
				const char*	szAttribName  = itr->name.GetString();
				/*
				if(0 == strcmp(szAttribName,"children"))
				{
					continue ;
				}
				*/
				string	strAttribName = szAttribName;
				stJsonAttribDesc	tJsonAttrib;
				tJsonAttrib.m_Name = strAttribName ;
				tJsonAttrib.m_Type = itr->value.GetType();
				tJsonAttrib.m_DefaultValue = GetValueString(itr->value);
				tNewObject.m_AttribVec.push_back(tJsonAttrib);
			}
			m_ObjectDescVec.push_back(tNewObject);

			szFindStr = strtok(NULL,",");
			if(!szFindStr)break;
		}
		delete[] szMemory;
	}

}
//��ѯ��Ӧ������������������
int		JsonLoader::GetObjectDescIndex(string& strName)
{

	const	char*	pChar = strName.c_str();
	int		nLen = strName.size();
	char*   szMemory = new char[nLen+1];
	strcpy(szMemory,pChar);
	char*   szFindStr = szMemory ;
	szFindStr = strtok(szFindStr,",");
	while(szFindStr)
	{
		//��������
		vector<stJsonObjectDesc>::iterator tIter;
		for(tIter = m_ObjectDescVec.begin() ; tIter != m_ObjectDescVec.end() ;  tIter++)
		{
			if(0 == strcmp(tIter->m_Name.c_str(),szFindStr))
			{
				strName = szFindStr;
				delete[] szMemory;
				return int(tIter - m_ObjectDescVec.begin());
			}
		}

		szFindStr = strtok(NULL,",");
		if(!szFindStr)break;
	}
	delete[] szMemory;
	return -1;
}
//ȡ����Ӧ��������
stJsonObjectDesc&	JsonLoader::GetObjectDesc(int vIndex)
{
	return m_ObjectDescVec[vIndex];
}
//ȡ�õ���ʱ����Ľ�ʹ�õ�������
int		JsonLoader::GetUsedObjectDescIndex(int vIndex)
{
	//��������
	int  nObjectIndex = -1;
	int  nUsedObjectIndex = -1;
	vector<stJsonObjectDesc>::iterator tIter;
	for(tIter = m_ObjectDescVec.begin() ; tIter != m_ObjectDescVec.end() ;  tIter++)
	{
		if(tIter->m_HasNode)
		{
			nUsedObjectIndex++;
		}

		nObjectIndex++;

		if(vIndex == nObjectIndex)
		{
			return nUsedObjectIndex;
		}
	}

	return nUsedObjectIndex;
}
//ȡ��һ��ֵ����ַ���
string	JsonLoader::GetValueString(const rapidjson::Value &	vValue)
{
	string	strResult = "";
	switch(vValue.GetType())
	{
	case kNullType:
		{
			strResult = "";
		}
		break;
	case kFalseType:
		{
			strResult = "0";
		}
		break;
	case kTrueType:
		{
			strResult = "1";
		}
		break;
	case kObjectType:
		{
			strResult = "";
		}
		break;
	case kArrayType:
		{
			strResult = "";
		}
		break;
	case kStringType:
		{
			strResult = vValue.GetString() ;
		}
		break;
	case kNumberType:
		{
			if(vValue.IsInt())
			{
				char  szText[100];
				sprintf(szText,"%d",vValue.GetInt());
				strResult = szText ;
			}
			if(vValue.IsInt64())
			{
				char  szText[100];
				sprintf(szText,"%d",vValue.GetInt64());
				strResult = szText ;
			}
			if(vValue.IsUint())
			{
				char  szText[100];
				sprintf(szText,"%d",vValue.GetUint());
				strResult = szText ;
			}
			if(vValue.IsUint64())
			{
				char  szText[100];
				sprintf(szText,"%d",vValue.GetUint64());
				strResult = szText ;
			}
			if(vValue.IsDouble())
			{
				char  szText[100];
				sprintf(szText,"%.4f",vValue.GetDouble());
				strResult = szText ;
			}
		}
		break;
	}
	return strResult;
}

