#include "opc_constants.h"

opcPart getPart(opcContainer *container, C_TEXT &path) {
	
	opcPart part = OPC_PART_INVALID;
	
	if(path.getUTF16Length()) {
		
		CUTF8String target;
		path.copyUTF8String(&target);
		part = opcPartFind(container, _X(target.c_str()), NULL, 0);
	}
	
	return part;
}

const xmlChar *getRelationType(opcContainer *container, opcPart source, opcRelation rel) {

	const xmlChar *prefix = NULL;
	opc_uint32_t counter = -1;
	const xmlChar *type = NULL;
	
	opcRelationGetInformation(container, source, rel, &prefix, &counter, &type);
	
	return type;
}

const xmlChar *getRelationPrefixSource(opcContainer *container, opcPart part, opcRelation rel, CUTF8String &prefix, CUTF8String &source) {
	
	const xmlChar *p = NULL;
	opc_uint32_t counter = -1;
	const xmlChar *type = NULL;
	
	opcRelationGetInformation(container, part, rel, &p, &counter, &type);
	
	prefix = CUTF8String((const uint8_t *)p);
	
	if (OPC_RELATION_INVALID != counter) {
		std::stringstream ss;
		ss << counter;
		prefix += (const uint8_t *)ss.str().c_str();
	}
	
	if(part)
		source = CUTF8String((const uint8_t *)part);

	return type;	
}
