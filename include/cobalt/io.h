/*

    Author: Sahil Gour

    Licensed Under: MIT LICENSE

*/

#pragma once

#include "cobalt.h"

CFile fileOpen(const char *filepath, const char *mode);
CString fileRead(CFile *file);
bool resetCursor(CFile *file);
Clist fileReadLines(CFile *file);
CString fileReadLine(CFile *file);
bool fileClose(CFile *file);