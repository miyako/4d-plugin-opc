#include "opc_document.h"

// --------------------------------- OPC Document ---------------------------------

void OPC_Get_document_type(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	CUTF8String path;
	Param1.copyPath(&path);
	
	opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_ONLY, NULL, NULL);
	
	if(c) {
		
		opcRelation rel = opcRelationFind(c, OPC_PART_INVALID, NULL, _X("http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument"));
		
		if (OPC_RELATION_INVALID != rel) {
			
			opcPart main = opcRelationGetInternalTarget(c, OPC_PART_INVALID, rel);
			
			if (OPC_PART_INVALID != main) {
				
				const xmlChar *type = opcPartGetType(c, main);
				
				CUTF8String t = CUTF8String((const uint8_t *)type);
				returnValue.setUTF8String(&t);
				
			}
			
		}
		
		opcContainerClose(c, OPC_CLOSE_NOW);
		
	}
	
	returnValue.setReturn(pResult);
}

void OPC_RESOURCE_TYPE_LIST(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	ARRAY_TEXT Param2;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	CUTF8String path;
	Param1.copyPath(&path);
	
	opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_ONLY, NULL, NULL);
	
	if(c) {
		
		Param2.setSize(1);

		for(const xmlChar *type = opcContentTypeFirst(c);
			NULL != type;
			type = opcContentTypeNext(c, type)) {
				CUTF8String t = CUTF8String((const uint8_t *)type);
				Param2.appendUTF8String(&t);
		}
		
		opcContainerClose(c, OPC_CLOSE_NOW);
		
	}
	
	Param2.toParamAtIndex(pParams, 2);
}

void OPC_EXTENSION_LIST(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	ARRAY_TEXT Param2;
	ARRAY_TEXT Param3;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	CUTF8String path;
	Param1.copyPath(&path);
	
	opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_ONLY, NULL, NULL);
	
	if(c) {
		
		Param2.setSize(1);
		Param3.setSize(1);
		
		for(const xmlChar *ext = opcExtensionFirst(c);
			NULL != ext;
			ext = opcExtensionNext(c, ext)) {
				CUTF8String e = CUTF8String((const uint8_t *)ext);
				Param2.appendUTF8String(&e);
			
				const xmlChar *type = opcExtensionGetType(c, ext);
				CUTF8String t = CUTF8String((const uint8_t *)type);
				Param3.appendUTF8String(&t);
			
		}
		
		opcContainerClose(c, OPC_CLOSE_NOW);
		
	}
	
	Param2.toParamAtIndex(pParams, 2);
	Param3.toParamAtIndex(pParams, 3);
}

void OPC_Register_resource_type(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_TEXT Param3;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);
	
	CUTF8String path;
	Param1.copyPath(&path);
	
	opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_WRITE, NULL, NULL);
	
	if(c) {
	
		CUTF8String ext, type;	
		
		Param2.copyUTF8String(&ext);
		Param3.copyUTF8String(&type);		
			
		CUTF8String r = CUTF8String((const uint8_t *)opcExtensionRegister(c, (const xmlChar *)ext.c_str(), (const xmlChar *)type.c_str()));
		returnValue.setUTF8String(&r);
		
		opcContainerClose(c, OPC_CLOSE_NOW);
		
	}	
	
	returnValue.setReturn(pResult);
}

void OPC_EXTERNAL_TARGET_LIST(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	ARRAY_TEXT Param2;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	CUTF8String path;
	Param1.copyPath(&path);
	
	opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_ONLY, NULL, NULL);
	
	if(c) {
		
		Param2.setSize(1);
		
		for(const xmlChar *target = opcExternalTargetFirst(c);
			NULL != target;
			target = opcExternalTargetNext(c, target)) {
			CUTF8String t = CUTF8String((const uint8_t *)target);
			Param2.appendUTF8String(&t);
		}
		
		opcContainerClose(c, OPC_CLOSE_NOW);
		
	}
	
	Param2.toParamAtIndex(pParams, 2);
}

void OPC_RELATION_TYPE_LIST(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	ARRAY_TEXT Param2;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	CUTF8String path;
	Param1.copyPath(&path);
	
	opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_ONLY, NULL, NULL);
	
	if(c) {
		
		Param2.setSize(1);
		
		for(const xmlChar *type = opcRelationTypeFirst(c);
			NULL != type;
			type = opcRelationTypeNext(c, type)) {
			CUTF8String t = CUTF8String((const uint8_t *)type);
			Param2.appendUTF8String(&t);
		}
		
		opcContainerClose(c, OPC_CLOSE_NOW);
		
	}
	
	Param2.toParamAtIndex(pParams, 2);
}