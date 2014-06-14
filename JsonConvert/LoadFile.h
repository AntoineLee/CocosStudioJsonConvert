#ifndef _LOADFILE_H
#define _LOADFILE_H

#include <stdio.h>
#include <windows.h>

// ��ȡһ�����ݿ�
inline void		ReadMemoryData(BYTE* pReadData, int nMaxSize, DWORD& uOffSize, void* pOutData, int nReadSize)
{
	if (uOffSize + nReadSize <= 0 || uOffSize + nReadSize > nMaxSize)
	{
		// �쳣�׳�
		throw;
	}
	memcpy(pOutData, &pReadData[uOffSize], nReadSize);
	uOffSize += nReadSize;
}

// ȡ��ĳһ�ļ���С��
bool GetFileSize( const char *szFile, size_t *p_szFile );
// �����ļ����ݡ�
bool LoadFile( const char *szFile, void *pBuf, size_t szBuf, size_t *p_szLoaded, unsigned int flag = 0 );

#endif