#include "opc_resource.h"

// --------------------------------- OPC Resource ---------------------------------

#pragma mark -

static void traverseInternal(opcContainer *c, 
							 opcPart source, 
							 ARRAY_TEXT &paths, 
							 ARRAY_TEXT &types) {
	
    for(opcRelation rel = opcRelationFirst(c, source);OPC_RELATION_INVALID != rel;rel=opcRelationNext(c, source, rel)) {
		
		opcPart part = opcRelationGetInternalTarget(c, source, rel);
		
		if (OPC_PART_INVALID != part) {
						
			CUTF8String p = CUTF8String((const uint8_t *)part);		
			CUTF8String t = CUTF8String((const uint8_t *)opcPartGetType(c, part));
			
			paths.appendUTF8String(&p);
			types.appendUTF8String(&t);
			
			traverseInternal(c, part, paths, types);
			
		}
    }
}

static void traverseExternal(opcContainer *c, 
							 opcPart source, 
							 ARRAY_TEXT &paths, 
							 ARRAY_TEXT &types) {
	
    for(opcRelation rel = opcRelationFirst(c, source);OPC_RELATION_INVALID != rel;rel=opcRelationNext(c, source, rel)) {
		
		const xmlChar *target = opcRelationGetExternalTarget(c, source, rel);
		
		if (!target) {
			
			//is internal
			opcPart part = opcRelationGetInternalTarget(c, source, rel);
			
			if (OPC_PART_INVALID != part) 
				traverseExternal(c, part, paths, types);
				
		}else{
			
			//is external
			CUTF8String p = CUTF8String((const uint8_t *)target);			
			CUTF8String t = CUTF8String((const uint8_t *)getRelationType(c, source, rel));	
			
			paths.appendUTF8String(&p);
			types.appendUTF8String(&t);
			
		}
    }
}

#pragma mark -

void OPC_Get_resource_size(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	CUTF8String path, type;
	
	Param1.copyPath(&path);
	Param2.copyUTF8String(&type);
	
	opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_ONLY, NULL, NULL);
	
	if(c) {
		
		opcPart part = getPart(c, Param2);
		
		if (OPC_PART_INVALID != part) {
			
			returnValue.setIntValue(opcPartGetSize(c, part));
			
		}else{returnValue.setIntValue(OPC_ERROR_INVALID_RESOURCE_ID);}	
		
		opcContainerClose(c, OPC_CLOSE_NOW);
		
	}else{returnValue.setIntValue(OPC_ERROR_INVALID_PATH);}	
	
	returnValue.setReturn(pResult);
}

#pragma mark -

void OPC_Delete_resource(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	CUTF8String path, type;
	
	Param1.copyPath(&path);
	Param2.copyUTF8String(&type);
	
	opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_WRITE, NULL, NULL);
	
	if(c) {
		
		opcPart part = getPart(c, Param2);
		
		if (OPC_PART_INVALID != part) {
			
			returnValue.setIntValue(opcPartDelete(c, part));
			opcContainerClose(c, OPC_CLOSE_TRIM);
			
		}else{
			
			returnValue.setIntValue(OPC_ERROR_INVALID_RESOURCE_ID);
			opcContainerClose(c, OPC_CLOSE_NOW);
			
		}	
		
	}else{returnValue.setIntValue(-1);}	
	
	returnValue.setReturn(pResult);
}

#pragma mark -

void OPC_Get_resource(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_BLOB Param3;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	CUTF8String path, type;
	
	Param1.copyPath(&path);
	Param2.copyUTF8String(&type);
	
	opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_ONLY, NULL, NULL);
	
	if(c) {
		
		opcPart part = getPart(c, Param2);
		
		if (OPC_PART_INVALID != part) {
			
			opcContainerInputStream *stream = opcContainerOpenInputStream(c, part);
			
			if (stream) {
				
				int ret = 0;
				opc_uint8_t buf[7096];
				
				while((ret = opcContainerReadInputStream(stream, buf, sizeof(buf))) > 0) {
					Param3.addBytes((const uint8_t *)buf, ret);
				}
				
				opcContainerCloseInputStream(stream);
			}
			
		}else{returnValue.setIntValue(OPC_ERROR_INVALID_RESOURCE_ID);}	
		
		opcContainerClose(c, OPC_CLOSE_NOW);
		
	}else{returnValue.setIntValue(OPC_ERROR_INVALID_PATH);}	
	
	Param3.toParamAtIndex(pParams, 3);
	returnValue.setReturn(pResult);
}

void OPC_Set_resource(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_TEXT Param3;
	C_BLOB Param4;
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);
	Param4.fromParamAtIndex(pParams, 4);

	CUTF8String path, name, type;
	
	Param1.copyPath(&path);
	Param2.copyUTF8String(&name);
	Param3.copyUTF8String(&type);
	
	if ((Param3.getUTF16Length())) {
	
		opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_WRITE, NULL, NULL);
		
		if(c) {
			
			opcPart part = opcPartCreate(c, _X(name.c_str()), _X(type.c_str()), 0); 
			
			if (OPC_PART_INVALID != part) {	
				
				opcContainerOutputStream *stream = opcContainerCreateOutputStream(c, part, OPC_COMPRESSIONOPTION_NONE);
				opcContainerWriteOutputStream(stream, Param4.getBytesPtr(), Param4.getBytesLength());
				opcContainerCloseOutputStream(stream); 
				
			}else{returnValue.setIntValue(OPC_ERROR_INVALID_RESOURCE_ID);}	
			
			opcContainerClose(c, OPC_CLOSE_NOW);
			
		}else{returnValue.setIntValue(OPC_ERROR_INVALID_PATH);}	
	
	}else{returnValue.setIntValue(OPC_ERROR_INVALID_RESOURCE_ID);}	 
	
	returnValue.setReturn(pResult);
}

#pragma mark -

void OPC_RESOURCE_LIST(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	ARRAY_TEXT Param3;
	ARRAY_TEXT Param4;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	CUTF8String path;
	Param1.copyPath(&path);
	
	opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_ONLY, NULL, NULL);
	
	if (c) {
		
		Param3.setSize(1);	
		Param4.setSize(1);
		
		opcPart part = getPart(c, Param2);
		
		traverseInternal(c, part, Param3, Param4);
		
		opcContainerClose(c, OPC_CLOSE_NOW);
		
	}
	
	Param3.toParamAtIndex(pParams, 3);
	Param4.toParamAtIndex(pParams, 4);
}

void OPC_EXTERNAL_RESOURCE_LIST(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	ARRAY_TEXT Param3;
	ARRAY_TEXT Param4;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	CUTF8String path;
	Param1.copyPath(&path);
	
	opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_ONLY, NULL, NULL);
	
	if (c) {
			
		Param3.setSize(1);	
		Param4.setSize(1);
		
		opcPart part = getPart(c, Param2);
		
		traverseExternal(c, part, Param3, Param4);
		
		opcContainerClose(c, OPC_CLOSE_NOW);
		
	}
	
	Param3.toParamAtIndex(pParams, 3);
	Param4.toParamAtIndex(pParams, 4);
}
