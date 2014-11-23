#include "IGFaceDetectorClient.h"
#include <Ice/Ice.h>
#include <IGFaceDetectorIce.h>

using namespace IGFaceDetector;
using namespace Ice;

extern "C"
{
CommunicatorPtr g_pCom = NULL;

void iceClientDetectFaces(const wchar_t* pcwPictureId, const wchar_t* pcwPictureName, int rotationIndex)
{
	PropertiesPtr pProp = createProperties();
	pProp->load ("config.client");
	InitializationData initData;
	initData.properties = pProp;
	if (!g_pCom)
		g_pCom = initialize (initData);
	IGFaceDetectorIcePrx pFaceDetector = IGFaceDetectorIcePrx::uncheckedCast(g_pCom->propertyToProxy("IGFaceDetectorIce.Proxy"));
    if(!pFaceDetector)
        return;

	try{
		pFaceDetector->faceDetection (pcwPictureId, pcwPictureName, rotationIndex, true);
	}
	catch(const Ice::NotRegisteredException& exc){
		printf ("%s", exc.what());
	}
}

}