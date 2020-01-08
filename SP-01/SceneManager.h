#pragma once

#include "main.h"
HRESULT InitScene();
void UpdateScene();
void ChangeScene();
void DrawScene();
void EndScene();

void TransitionForNewScene(bool bTransitionIn);