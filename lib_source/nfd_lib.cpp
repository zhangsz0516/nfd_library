// copyright (c) 2019-2020 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "nfd_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

LIB_SOURCE_EXPORT int NFD_CreateScanHandle()
{  
    return NFD_lib().createHandle();
}

LIB_SOURCE_EXPORT char *NFD_ScanFileA(int nHandle, char *pszFileName, unsigned int nFlags)
{
    return NFD_lib().scanFileA(nHandle,pszFileName,nFlags);
}

LIB_SOURCE_EXPORT wchar_t *NFD_ScanFileW(int nHandle, wchar_t *pwszFileName, unsigned int nFlags)
{
    return NFD_lib().scanFileW(nHandle,pwszFileName,nFlags);
}

LIB_SOURCE_EXPORT bool NFD_CloseScanHandle(int nHandle)
{
    return NFD_lib().closeHandle(nHandle);
}

#ifdef __cplusplus
}
#endif

NFD_lib::NFD_lib()
{

}

quint64 NFD_lib::nCurrentHandle=1;
QMap<quint64,char *> NFD_lib::mapHandles={};

int NFD_lib::createHandle()
{
    int nResult=0;

    while(mapHandles.contains(nCurrentHandle))
    {
        nCurrentHandle++;
    }

    nResult=nCurrentHandle;

    mapHandles.insert(nCurrentHandle,0);

    nCurrentHandle++;

    if(nCurrentHandle==0)
    {
        nCurrentHandle++;
    }

    return nResult;
}

char *NFD_lib::scanFileA(int nHandle, char *pszFileName, unsigned int nFlags)
{
    QString sResult=_scanFile(pszFileName,nFlags);

    int nSize=sResult.size()+1;

    char *pMemory=new char[nSize];

    XBinary::_copyMemory(pMemory,sResult.toLatin1().data(),nSize);

    closeHandle(nHandle);

    getMapHandles()->insert(nHandle,pMemory);

    return pMemory;
}

wchar_t *NFD_lib::scanFileW(int nHandle, wchar_t *pwszFileName, unsigned int nFlags)
{
    QString sResult=_scanFile(QString::fromWCharArray(pwszFileName,-1),nFlags);

    int nSize=(sResult.size()+1)*2;

    char *pMemory=new char[nSize];

    sResult.toWCharArray((wchar_t *)pMemory);

    closeHandle(nHandle);

    getMapHandles()->insert(nHandle,pMemory);

    return (wchar_t *)pMemory;
}

bool NFD_lib::closeHandle(int nHandle)
{
    bool bResult=false;

    if(mapHandles.contains(nHandle))
    {
        char *pMemory=mapHandles.value(nHandle);

        mapHandles.remove(nHandle);

        if(pMemory)
        {
            delete [] pMemory;
        }

        bResult=true;
    }

    return bResult;
}

QMap<quint64, char *> *NFD_lib::getMapHandles()
{
    return &(mapHandles);
}

QString NFD_lib::_scanFile(QString sFileName, quint32 nFlags)
{
    SpecAbstract::SCAN_OPTIONS options={};

    if(nFlags&SF_DEEPSCAN)
    {
        options.bDeepScan=true;
    }

    if(nFlags&SF_RECURSIVE)
    {
        options.bRecursiveScan=true;
    }

    if(nFlags&SF_HEURISTICSCAN)
    {
        options.bHeuristicScan=true;
    }

    if(nFlags&SF_RESULTASJSON)
    {
        options.bResultAsJSON=true;
    }
    else if(nFlags&SF_RESULTASXML)
    {
        options.bResultAsXML=true;
    }

    SpecAbstract::SCAN_RESULT scanResult=StaticScan::processFile(sFileName,&options);

    StaticScanItemModel model(&scanResult.listRecords);

    return model.toString(&options);
}
