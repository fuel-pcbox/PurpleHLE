#pragma once

#include "..\types.h"
#include "stdio.h"
#include <sstream>
#include <wx/wx.h>

void ARMdisassemble(std::stringstream &ss, u32 address, u32 op);
void THUMBdisassemble(std::stringstream &ss, u32 address, u32 op32);

wxString ArmDisasm(u32 code, u32 address, u8 thumb);