// ***************************************************************
//  XMLUtility   version:  1.0   ·  date: 10/10/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#include "atlbase.h"
#include "msxml2.h"
#include <time.h>

bool CreateXMLNode(CComPtr<IXMLDOMDocument> doc, CComPtr<IXMLDOMNode> node, TCHAR *nodeName, IXMLDOMNode **  newNode);
bool AddXMLAttribute(CComPtr<IXMLDOMNode> node, TCHAR * name, const TCHAR * value);
bool AddXMLText(CComPtr<IXMLDOMDocument> doc, CComPtr<IXMLDOMNode> node, TCHAR * text);
CComPtr<IXMLDOMText> CreateXMLText(CComPtr<IXMLDOMDocument> doc, CComPtr<IXMLDOMNode> parentNode, TCHAR* name);
void WriteXMLFile(const TCHAR* name, CComPtr<IXMLDOMDocument> pXMLDoc);
