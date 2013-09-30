#include "opc_relation.h"

// --------------------------------- OPC Relation ---------------------------------

void OPC_Create_relation(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_TEXT Param3;
	C_TEXT Param4;
	C_LONGINT Param5;
	C_TEXT Param6;	
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);
	Param4.fromParamAtIndex(pParams, 4);
	Param5.fromParamAtIndex(pParams, 5);

	if(Param4.getUTF16Length()) {
		
		if(Param5.getIntValue() > 0) {
			
			CUTF8String path, src, dst, type, name;
			
			if(!Param6.getUTF16Length()) {
				name = DEFAULT_RELATION_PREFIX;
			}else{
				Param6.copyUTF8String(&name);
			}
				
			Param1.copyPath(&path);
			Param2.copyUTF8String(&src);
			Param3.copyUTF8String(&dst);	
			Param4.copyUTF8String(&type);	
			
			std::stringstream ss;
			ss << Param5.getIntValue();
			name +=	(const uint8_t *)ss.str().c_str();
		
			opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_WRITE, NULL, NULL);
			
			if(c) {
				
				opcPart srcPart = opcPartFind(c, _X(src.c_str()), NULL, 0);
				
				if (OPC_PART_INVALID != srcPart) {
					
					opcPart dstPart = opcPartFind(c, _X(dst.c_str()), NULL, 0);
					
					if (OPC_PART_INVALID != dstPart) {
						
						signed int status = (signed int)opcRelationAdd(c, srcPart, (const xmlChar *)name.c_str(), dstPart, (const xmlChar *)type.c_str());
		
						if((status) == -1) {
							returnValue.setIntValue(OPC_ERROR_INVALID_RELATION_ID);
						}else{
							returnValue.setIntValue(status);
						}
						
					}else{returnValue.setIntValue(OPC_ERROR_INVALID_RESOURCE_ID);}	
					
				}else{returnValue.setIntValue(OPC_ERROR_INVALID_RESOURCE_ID);}	
				
				opcContainerClose(c, OPC_CLOSE_NOW);
				
			}else{returnValue.setIntValue(OPC_ERROR_INVALID_PATH);}	
			
		}else{returnValue.setIntValue(OPC_ERROR_INVALID_RELATION_ID);}
		   
	}else{returnValue.setIntValue(OPC_ERROR_INVALID_RELATION_TYPE);}	   
	   
	returnValue.setReturn(pResult);
}

void OPC_Remove_relation(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_LONGINT Param3;
	C_TEXT Param4;	
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);
	Param4.fromParamAtIndex(pParams, 4);
	
	if(Param3.getIntValue() > 0) {
		
		CUTF8String path, type, name;
		
		if(!Param4.getUTF16Length()) {
			name = DEFAULT_RELATION_PREFIX;
		}else{
			Param4.copyUTF8String(&name);
		}
		
		Param1.copyPath(&path);
		Param2.copyUTF8String(&type);
		
		std::stringstream ss;
		ss << Param3.getIntValue();
		name +=	(const uint8_t *)ss.str().c_str();
		
		opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_WRITE, NULL, NULL);
		
		if(c) {
			
			opcPart part = opcPartFind(c, _X(type.c_str()), NULL, 0);
			
			if (OPC_PART_INVALID != part) {
			
				if (OPC_RELATION_INVALID != opcRelationFind(c, part, (const xmlChar *)name.c_str(), NULL)) {
					returnValue.setIntValue(opcRelationDelete(c, part, (const xmlChar *)name.c_str(), NULL));
				}else{
					returnValue.setIntValue(OPC_ERROR_INVALID_RELATION_ID);
				}
				
			}else{returnValue.setIntValue(OPC_ERROR_INVALID_RESOURCE_ID);}	
			
			opcContainerClose(c, OPC_CLOSE_TRIM);
			
		}else{returnValue.setIntValue(OPC_ERROR_INVALID_PATH);}	
		
	}else{returnValue.setIntValue(OPC_ERROR_INVALID_RELATION_ID);}
	
	returnValue.setReturn(pResult);
}

void OPC_Create_external_relation(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_TEXT Param3;
	C_TEXT Param4;
	C_LONGINT Param5;
	C_TEXT Param6;	
	C_LONGINT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);
	Param4.fromParamAtIndex(pParams, 4);
	Param5.fromParamAtIndex(pParams, 5);
	
	if(Param4.getUTF16Length()) {
		
		if(Param5.getIntValue() > 0) {
			
			CUTF8String path, src, dst, type, name;
			
			if(!Param6.getUTF16Length()) {
				name = DEFAULT_RELATION_PREFIX;
			}else{
				Param6.copyUTF8String(&name);
			}
			
			Param1.copyPath(&path);
			Param2.copyUTF8String(&src);
			Param3.copyUTF8String(&dst);	
			Param4.copyUTF8String(&type);	
			
			std::stringstream ss;
			ss << Param5.getIntValue();
			name +=	(const uint8_t *)ss.str().c_str();
			
			opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_WRITE, NULL, NULL);
			
			if(c) {
				
				opcPart srcPart = opcPartFind(c, _X(src.c_str()), NULL, 0);
				
				if (OPC_PART_INVALID != srcPart) {
				
					opcPart dstPart = opcPartFind(c, _X(dst.c_str()), NULL, 0);
					
					if (OPC_PART_INVALID != dstPart) {
					
						signed int status = (signed int)opcRelationAddExternal(c, srcPart, (const xmlChar *)name.c_str(), dstPart, (const xmlChar *)type.c_str());
						
						if((status) == -1) {
							returnValue.setIntValue(OPC_ERROR_INVALID_RELATION_ID);
						}else{
							returnValue.setIntValue(status);
						}
						
					}else{returnValue.setIntValue(OPC_ERROR_INVALID_RESOURCE_ID);}
				
				}else{returnValue.setIntValue(OPC_ERROR_INVALID_RESOURCE_ID);}
				
			}else{returnValue.setIntValue(OPC_ERROR_INVALID_PATH);}	
			
		}else{returnValue.setIntValue(OPC_ERROR_INVALID_RELATION_ID);}
	
	}else{returnValue.setIntValue(OPC_ERROR_INVALID_RELATION_TYPE);}
	
	returnValue.setReturn(pResult);
}

#pragma mark -

static void traverseRelation(opcContainer *c, 
							 opcPart source, 
							 ARRAY_TEXT &srcs,
							 ARRAY_TEXT &dsts,
							 ARRAY_TEXT &names, 
							 ARRAY_TEXT &types) {
	
    for(opcRelation rel = opcRelationFirst(c, source);OPC_RELATION_INVALID != rel;rel=opcRelationNext(c, source, rel)) {
		
		const xmlChar *target = opcRelationGetExternalTarget(c, source, rel);
		
		if (!target) {
			
			//is internal
			opcPart part = opcRelationGetInternalTarget(c, source, rel);
			
			if (OPC_PART_INVALID != part) {

				CUTF8String n, s, t, p;
				
				t = CUTF8String((const uint8_t *)getRelationPrefixSource(c, source, rel, n, s));	
				p = CUTF8String((const uint8_t *)part);	
			
				srcs.appendUTF8String(&s);
				dsts.appendUTF8String(&p);
				names.appendUTF8String(&n);
				types.appendUTF8String(&t);
					
			}
				
			traverseRelation(c, part, srcs, dsts, names, types);
			
		}else{
			
			//is external
			CUTF8String n, s, t, p;
									
			t = CUTF8String((const uint8_t *)getRelationPrefixSource(c, source, rel, n, s));	
			p = CUTF8String((const uint8_t *)target);	
			
			srcs.appendUTF8String(&s);
			dsts.appendUTF8String(&p);
			names.appendUTF8String(&n);
			types.appendUTF8String(&t);

		}
    }
}

#pragma mark -

void OPC_RELATION_LIST(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	ARRAY_TEXT Param3;
	ARRAY_TEXT Param4;
	ARRAY_TEXT Param5;
	ARRAY_TEXT Param6;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	CUTF8String path;
	Param1.copyPath(&path);
	
	opcContainer *c = opcContainerOpen(_X(path.c_str()), OPC_OPEN_READ_ONLY, NULL, NULL);
	
	if (c) {
		
		Param3.setSize(1);	
		Param4.setSize(1);
		Param5.setSize(1);
		Param6.setSize(1);
		
		opcPart part = getPart(c, Param2);
				
		traverseRelation(c, part, Param3, Param4, Param5, Param6);
		
		opcContainerClose(c, OPC_CLOSE_NOW);
		
	}
	
	Param3.toParamAtIndex(pParams, 3);
	Param4.toParamAtIndex(pParams, 4);
	Param5.toParamAtIndex(pParams, 5);	
	Param6.toParamAtIndex(pParams, 6);		
}