// GDIPlusHelper.cpp: implementation of the CGDIPlusHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImageEx.h"
#include <process.h>

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//
//

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::ImageEx
// 
// DESCRIPTION:	Constructor for constructing images from a file
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
ImageEx::ImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement) : Image(filename, useEmbeddedColorManagement)
{
	Initialize();

	m_bIsInitialized = true;

	TestForAnimatedGIF();
}

ImageEx::ImageEx(BYTE* pBuff, int nSize)
{
	Initialize();

	m_bIsInitialized = true;

	LoadFromBuffer(pBuff, nSize);

	lastResult = DllExports::GdipLoadImageFromStreamICM(m_pStream, &nativeImage);

	TestForAnimatedGIF();
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::~ImageEx
// 
// DESCRIPTION:	Free up fresources
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
ImageEx::~ImageEx()
{
	Destroy();
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	InitAnimation
// 
// DESCRIPTION:	Prepare animated GIF
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::InitAnimation(HWND hWnd/*, CRect rect*/)
{

	m_hWnd = hWnd;
	//m_rect = rect;

	if (!m_bIsInitialized)
	{
		TRACE(_T("GIF not initialized\n"));
		return false;
	};

	if (IsAnimatedGIF())
	{
		if (m_hThread == NULL)
		{
		
			unsigned int nTID = 0;

			m_hThread = (HANDLE) _beginthreadex( NULL, 0, _ThreadAnimationProc, this, CREATE_SUSPENDED,&nTID);
			
			if (!m_hThread)
			{
				TRACE(_T("Couldn't start a GIF animation thread\n"));
				return true;
			} 
			else 
				ResumeThread(m_hThread);
		}
	} 

	return false;	

}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	LoadFromBuffer
// 
// DESCRIPTION:	Helper function to copy phyical memory from buffer a IStream
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::LoadFromBuffer(BYTE* pBuff, int nSize)
{
	bool bResult = false;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	if (hGlobal)
	{
		void* pData = GlobalLock(hGlobal);
		if (pData)
			memcpy(pData, pBuff, nSize);
		
		GlobalUnlock(hGlobal);

		if (CreateStreamOnHGlobal(hGlobal, TRUE, &m_pStream) == S_OK)
			bResult = true;

	}

	return bResult;
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	GetSize
// 
// DESCRIPTION:	Returns Width and Height object
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
CSize ImageEx::GetSize()
{
	return CSize(GetWidth(), GetHeight());
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	TestForAnimatedGIF
// 
// DESCRIPTION:	Check GIF/Image for avialability of animation
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::TestForAnimatedGIF()
{
   UINT count = 0;
   count = GetFrameDimensionsCount();
   GUID* pDimensionIDs = new GUID[count];

   // Get the list of frame dimensions from the Image object.
   GetFrameDimensionsList(pDimensionIDs, count);

   // Get the number of frames in the first dimension.
   m_nFrameCount = GetFrameCount(&pDimensionIDs[0]);

	// Assume that the image has a property item of type PropertyItemEquipMake.
	// Get the size of that property item.
   int nSize = GetPropertyItemSize(PropertyTagFrameDelay);

   // Allocate a buffer to receive the property item.
   m_pPropertyItem = (PropertyItem*) malloc(nSize);

   GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);

   
   delete  pDimensionIDs;

   return m_nFrameCount > 1;
}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::Initialize
// 
// DESCRIPTION:	Common function called from Constructors
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void ImageEx::Initialize()
{
	m_pStream = NULL;
	m_nFramePosition = 0;
	m_nFrameCount = 0;
	m_pStream = NULL;
	lastResult = InvalidParameter;
	m_hThread = NULL;
	m_bIsInitialized = false;
	m_pPropertyItem = NULL;
	
	m_bPause = false;

	m_hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	m_hPause = CreateEvent(NULL,TRUE,TRUE,NULL);

}


////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::_ThreadAnimationProc
// 
// DESCRIPTION:	Thread to draw animated gifs
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
UINT WINAPI ImageEx::_ThreadAnimationProc(LPVOID pParam)
{
	ASSERT(pParam);
	ImageEx *pImage = reinterpret_cast<ImageEx *> (pParam);
	pImage->ThreadAnimation();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::ThreadAnimation
// 
// DESCRIPTION:	Helper function
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void ImageEx::ThreadAnimation()
{
	m_nFramePosition = 0;

	bool bExit = false;
	while (bExit == false)
	{
		bExit = DrawFrameGIF();
	}
}

////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::DrawFrameGIF
// 
// DESCRIPTION:	
// 
// RETURNS:	
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::DrawFrameGIF()
{

	::WaitForSingleObject(m_hPause, INFINITE);

	GUID   pageGuid = FrameDimensionTime;

	long hmWidth = GetWidth();
	long hmHeight = GetHeight();

	HDC hDC = GetDC(m_hWnd);
	if (hDC)
	{
		Graphics graphics(hDC);
		graphics.DrawImage(this, 0, 0, hmWidth, hmHeight);
		ReleaseDC(m_hWnd, hDC);
	}

	SelectActiveFrame(&pageGuid, m_nFramePosition++);		
	
	if (m_nFramePosition == m_nFrameCount)
		m_nFramePosition = 0;


	long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;

	DWORD dwErr = WaitForSingleObject(m_hExitEvent, lPause);

	return dwErr == WAIT_OBJECT_0;


}



////////////////////////////////////////////////////////////////////////////////
// 
// FUNCTION:	ImageEx::SetPause
// 
// DESCRIPTION:	Toggle Pause state of GIF
// 
// RETURNS:		
// 
// NOTES:		
// 
// MODIFICATIONS:
// 
// Name				Date		Version		Comments
// N T ALMOND       29012002	1.0			Origin
// 
////////////////////////////////////////////////////////////////////////////////
void ImageEx::SetPause(bool bPause)
{
	if (!IsAnimatedGIF())
		return;

	if (bPause && !m_bPause)
	{
		::ResetEvent(m_hPause);
	}
	else
	{

		if (m_bPause && !bPause)
		{
			::SetEvent(m_hPause);
		}
	}

	m_bPause = bPause;
}


void ImageEx::Destroy()
{
	
	if (m_hThread)
	{
		// If pause un pause
		SetPause(false);

		SetEvent(m_hExitEvent);
		WaitForSingleObject(m_hThread, INFINITE);
	}

	CloseHandle(m_hThread);
	CloseHandle(m_hExitEvent);
	CloseHandle(m_hPause);

	free(m_pPropertyItem);

	m_pPropertyItem = NULL;
	m_hThread = NULL;
	m_hExitEvent = NULL;
	m_hPause = NULL;

	if (m_pStream)
		m_pStream->Release();

}

} // namespace DuiLib