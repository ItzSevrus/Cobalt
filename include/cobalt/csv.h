/*

    Author: Sahil Gour

    Licensed Under: MIT LICENSE

*/

#pragma once
#include "cobalt.h"

Clist csvReadRow(CFile *file);
Clist csvReadRows(CFile *file);
Clist csvGetByCol(CFile *file, CString *col_name);

void csvHead(CFile *file);