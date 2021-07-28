//--------------------字节流扩展函数--------------------//

CHAR* MbsCull(CHAR* String, CHAR cDelete){
	CHAR *pSrcChar, *pDstChar;

	for(pSrcChar = pDstChar = String; *pSrcChar != '\0'; ++pSrcChar)
		if(*pSrcChar != cDelete) *pDstChar++ = *pSrcChar;
	*pDstChar = '\0';

	return String;
}
CHAR* MbsCull(CHAR* String, CHAR* szDelete){
	CHAR **arrTarget;
	CHAR *pSrcChar, *pDstChar;
	UPTR StrSize, SubSize;
	UPTR CopySize, SubCount;

	//计算字符串长度

	StrSize = strlen(String);
	SubSize = strlen(szDelete);
	arrTarget = new CHAR*[StrSize / SubSize];

	//查找所有子串地址

	pSrcChar = String;
	for(SubCount = 0; TRUE; ++SubCount){
		arrTarget[SubCount] = strstr(pSrcChar, szDelete);
		if(!arrTarget[SubCount]) break;
		pSrcChar = arrTarget[SubCount] + SubSize;
	}

	if(SubCount == 0) goto done;
	else arrTarget[SubCount] = &String[StrSize];

	//覆盖第一个子串

	pDstChar = arrTarget[0];
	pSrcChar = pDstChar + SubSize;
	CopySize = arrTarget[1] - pSrcChar;
	MemCopy(pDstChar, pSrcChar, CopySize);

	//覆盖其它子串

	for(UINT i = 1; i < SubCount; ++i){
		pDstChar += CopySize;
		pSrcChar = arrTarget[i] + SubSize;
		CopySize = arrTarget[i + 1] - pSrcChar;
		MemCopy(pDstChar, pSrcChar, CopySize);
	}
	*(pDstChar + CopySize) = '\0';

	//返回字符串地址

done:
	delete[] arrTarget;
	return String;
};

CHAR* MbsMerge(CHAR* Output, CHAR* String0, CHAR* String1){
	UPTR Size0 = strlen(String0);
	UPTR Size1 = strlen(String1) + 1;

	if(!Output) Output = new CHAR[Size0 + Size1];
	MemCopy(Output, String0, Size0);
	MemCopy(&Output[Size0], String1, Size1);

	return Output;
}
WCHR* MbsMergeW(WCHR* Output, CHAR* String0, CHAR* String1){
	if(!Output) Output = new WCHR[strlen(String0) + strlen(String1) + 1];

	UINT Index0, Index1;
	for(Index0 = 0; String0[Index0]; ++Index0)
		Output[Index0] = String0[Index0];
	for(Index1 = 0; String1[Index1]; ++Index1, ++Index0)
		Output[Index0] = String1[Index1];
	Output[Index0] = L'\0';

	return Output;
}

WCHR* MbsToWcs(WCHR* Output, CHAR* String, out UINT* pSize = NULL){
	UPTR Size = strlen(String);
	if(pSize) *pSize = (UINT)Size;
	if(!Output) Output = new WCHR[++Size];
	mbstowcs(Output, String, Size);

	return Output;
}
WCHR* MbsToUcs(WCHR* Output, CHAR* String, out UINT* pSize = NULL){
	UINT Size = MultiByteToWideChar(CP_ACP, 0x0, String, -1, NULL, 0);
	if(pSize) *pSize = Size - 1;
	if(!Output) Output = new WCHR[Size];
	MultiByteToWideChar(CP_ACP, 0x0, String, -1, Output, Size);

	return Output;
}

//----------------------------------------//


//--------------------宽字符串扩展函数--------------------//

WCHR* WcsCull(WCHR* String, WCHR cDelete){
	WCHR *pSrcChar, *pDstChar;
	for(pSrcChar = pDstChar = String; *pSrcChar != '\0'; ++pSrcChar)
		if(*pSrcChar != cDelete) *pDstChar++ = *pSrcChar;
	*pDstChar = '\0';

	return String;
}
WCHR* WcsCull(WCHR* String, WCHR* szDelete){
	WCHR **arrTarget;
	WCHR *pDstChar, *pSrcChar;
	UPTR StrSize, SubSize;
	UPTR CopySize, SubCount;

	//计算字符串长度

	StrSize = wcslen(String);
	SubSize = wcslen(szDelete);
	arrTarget = new WCHR*[StrSize / SubSize];

	//查找所有子串地址

	pSrcChar = String;
	for(SubCount = 0; TRUE; ++SubCount){
		arrTarget[SubCount] = wcsstr(pSrcChar, szDelete);
		if(!arrTarget[SubCount]) break;
		pSrcChar = arrTarget[SubCount] + SubSize;
	}

	if(SubCount == 0) goto done;
	else arrTarget[SubCount] = &String[StrSize];

	//覆盖第一个子串

	pDstChar = arrTarget[0];
	pSrcChar = pDstChar + SubSize;
	CopySize = arrTarget[1] - pSrcChar;
	MemCopy(pDstChar, pSrcChar, CopySize << 1);

	//覆盖其它子串

	for(UINT i = 1; i < SubCount; ++i){
		pDstChar += CopySize;
		pSrcChar = arrTarget[i] + SubSize;
		CopySize = arrTarget[i + 1] - pSrcChar;
		MemCopy(pDstChar, pSrcChar, CopySize << 1);
	}
	*(pDstChar + CopySize) = L'\0';

done:
	delete[] arrTarget;
	return String;
}

WCHR* WcsMerge(WCHR* Output, WCHR* String0, WCHR* String1){
	UPTR Size0 = wcslen(String0);
	UPTR Size1 = wcslen(String1) + 1;

	if(!Output) Output = new WCHR[Size0 + Size1];
	MemCopy(Output, String0, Size0 << 1);
	MemCopy(&Output[Size0], String1, Size1 << 1);

	return Output;
}
CHAR* WcsMergeA(CHAR* Output, WCHR* String0, WCHR* String1){
	if(!Output) Output = new CHAR[wcslen(String0) + wcslen(String1) + 1];

	UINT Index0, Index1;
	for(Index0 = 0; String0[Index0]; ++Index0)
		Output[Index0] = (CHAR)String0[Index0];
	for(Index1 = 0; String1[Index1]; ++Index1, ++Index0)
		Output[Index0] = (CHAR)String1[Index1];
	Output[Index0] = '\0';

	return Output;
}

CHAR* WcsToMbs(CHAR* Output, WCHR* String, out UINT* pSize = NULL){
	UPTR Size = wcslen(String);
	if(pSize) *pSize = (UINT)Size;
	if(!Output) Output = new CHAR[++Size];
	wcstombs(Output, String, Size);

	return Output;
}
CHAR* UcsToMbs(CHAR* Output, WCHR* String, out UINT* pSize = NULL){
	UINT Size = WideCharToMultiByte(CP_ACP, 0x0, String, -1, NULL, 0, NULL, NULL);
	if(pSize) *pSize = Size - 1;
	if(!Output) Output = new CHAR[Size];
	WideCharToMultiByte(CP_ACP, 0x0, String, -1, Output, Size, NULL, NULL);

	return Output;
}

//----------------------------------------//


//--------------------字符串字面量--------------------//

#define TXT __TEXT

//----------------------------------------//


//--------------------通用字符串函数--------------------//

#define StrLen _tcslen
#define StrNLen _tcsnlen

#define StrSet _tcsset
#define StrNSet _tcsnset

#define StrCpy _tcscpy
#define StrNCpy _tcsncpy

#define StrCat _tcscat
#define StrNCat _tcsncat

#define StrCmp _tcscmp
#define StrNCmp _tcsncmp
#define StrICmp _tcsicmp
#define StrNICmp _tcsnicmp

#define StrLwr _tcslwr
#define StrUpr _tcsupr
#define StrRev _tcsrev
#define StrDup _tcsdup

#define StrChr _tcschr
#define StrRChr _tcsrchr
#define StrStr _tcsstr
#define StrPBrk _tcspbrk
#define StrSpn _tcsspn
#define StrCSpn _tcscspn

#define StrTok _tcstok

//----------------------------------------//


//--------------------字节流函数--------------------//

#define MbsLen strlen
#define MbsNLen strnlen

#define MbsSet strset
#define MbsNSet strnset

#define MbsCpy strcpy
#define MbsNCpy strncpy

#define MbsCat strcat
#define MbsNCat strncat

#define MbsCmp strcmp
#define MbsNCmp strncmp
#define MbsICmp stricmp
#define MbsNICmp strnicmp

#define MbsLwr strlwr
#define MbsUpr strupr
#define MbsRev strrev
#define MbsDup strdup

#define MbsChr strchr
#define MbsRChr strrchr
#define MbsStr strstr
#define MbsPBrk strpbrk
#define MbsSpn strspn
#define MbsCSpn strcspn

#define MbsTok strtok

//----------------------------------------//


//--------------------宽字符串函数--------------------//

#define WcsLen wcslen
#define WcsNLen wcsnlen

#define WcsSet wcsset
#define WcsNSet wcsnset

#define WcsCpy wcscpy
#define WcsNCpy wcsncpy

#define WcsCat wcscat
#define WcsNCat wcsncat

#define WcsCmp wcscmp
#define WcsNCmp wcsncmp
#define WcsICmp wcsicmp
#define WcsNICmp wcsnicmp

#define WcsLwr wcslwr
#define WcsUpr wcsupr
#define WcsRev wcsrev
#define WcsDup wcsdup

#define WcsChr wcschr
#define WcsRChr wcsrchr
#define WcsStr wcsstr
#define WcsPBrk wcspbrk
#define WcsSpn wcsspn
#define WcsCSpn wcscspn

#define WcsTok wcstok

//----------------------------------------//