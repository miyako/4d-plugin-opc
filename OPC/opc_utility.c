#include "opc_utility.h"

// ---------------------------------- OPC Utility ---------------------------------

void OPC_Document_type(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_LONGINT Param1;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	OPC_DOC_TYPE t = (OPC_DOC_TYPE)Param1.getIntValue();
	
	switch (t) {
		case OPC_DOCX:
			returnValue.setUTF8String(OPC_DOCX_TYPE, OPC_DOCX_TYPE_LEN);
			break;
			
		case OPC_XLSX:
			returnValue.setUTF8String(OPC_XLSX_TYPE, OPC_XLSX_TYPE_LEN);
			break;
			
		case OPC_PPTX:
			returnValue.setUTF8String(OPC_PPTX_TYPE, OPC_PPTX_TYPE_LEN);
			break;
			
		default:
			break;
	}
	
	returnValue.setReturn(pResult);
}

void OPC_Relation_type(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_LONGINT Param1;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	
	OPC_RELATION_TYPE t = (OPC_RELATION_TYPE)Param1.getIntValue();
	
	switch (t) {
		case OPC_IMAGE:
			returnValue.setUTF8String(OPC_IMAGE_RELATION, OPC_IMAGE_RELATION_LEN);
			break;
			
		case OPC_STYLES:
			returnValue.setUTF8String(OPC_STYLES_RELATION, OPC_STYLES_RELATION_LEN);
			break;			
			
		case OPC_STYLES_WITH_EFFECTS:
			returnValue.setUTF8String(OPC_STYLES_WITH_EFFECTS_RELATION, OPC_STYLES_WITH_EFFECTS_RELATION_LEN);
			break;	
			
		case OPC_SETTINGS:
			returnValue.setUTF8String(OPC_SETTINGS_RELATION, OPC_SETTINGS_RELATION_LEN);
			break;				

		case OPC_WEB_SETTINGS:
			returnValue.setUTF8String(OPC_WEB_SETTINGS_RELATION, OPC_WEB_SETTINGS_RELATION_LEN);
			break;	
			
		case OPC_FONT_TABLE:
			returnValue.setUTF8String(OPC_FONT_TABLE_RELATION, OPC_FONT_TABLE_RELATION_LEN);
			break;	
			
		case OPC_THEME:
			returnValue.setUTF8String(OPC_THEME_RELATION, OPC_THEME_RELATION_LEN);
			break;	
			
		default:
			break;
	}
	
	
	returnValue.setReturn(pResult);
}