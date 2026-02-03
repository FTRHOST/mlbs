#pragma once

#include "Utils.h"
#include "Utils/jniStuff.h"
#include "Utils/Unity/ByNameModding/Tools.h"
#include "Utils/Unity/ByNameModding/fake_dlfcn.h"
#include "Utils/Unity/ByNameModding/Il2Cpp.h"
#include "Utils/MonoString.h"

int glWidth, glHeight;

/*
const-string v0, "MEOW"
invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
*/
