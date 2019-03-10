#include "orx.h"
#include "imgui/imgui.h"

static void *orxImGui_Allocate(size_t _sSize, void *)
{
  return orxMemory_Allocate(_sSize, orxMEMORY_TYPE_TEMP);
}

static void orxImGui_Free(void *_pData, void *)
{
  orxMemory_Free(_pData);
}

static void orxImGui_BeginFrame()
{
  ImGuiIO &io = ImGui::GetIO();

  io.DeltaTime = orxClock_GetInfo(orxClock_FindFirst(-1.0f, orxCLOCK_TYPE_CORE))->fDT;

  orxDisplay_GetScreenSize(&io.DisplaySize.x, &io.DisplaySize.y);
  orxVECTOR vFramebufferSize;
  orxConfig_PushSection(orxDISPLAY_KZ_CONFIG_SECTION);
  orxConfig_GetVector(orxDISPLAY_KZ_CONFIG_FRAMEBUFFER_SIZE, &vFramebufferSize);
  orxConfig_PopSection();
  io.DisplayFramebufferScale.x = vFramebufferSize.fX / io.DisplaySize.x;
  io.DisplayFramebufferScale.y = vFramebufferSize.fY / io.DisplaySize.y;

  orxVECTOR vMousePos;
  orxMouse_GetPosition(&vMousePos);
  io.MousePos.x = vMousePos.fX;
  io.MousePos.y = vMousePos.fY;
  io.MouseWheel = orxMouse_GetWheelDelta();
  for(int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
  {
    io.MouseDown[i] = orxMouse_IsButtonPressed((orxMOUSE_BUTTON)i) ? true : false;
  }

  for(int i = 0; i < orxKEYBOARD_KEY_NUMBER; i++)
  {
    io.KeysDown[i] = orxKeyboard_IsKeyPressed((orxKEYBOARD_KEY)i) ? true : false;
  }
  io.KeyCtrl  = (orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_LCTRL) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_RCTRL)) ? true : false;
  io.KeyShift = (orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_LSHIFT) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_RSHIFT)) ? true : false;
  io.KeyAlt   = (orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_LALT) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_RALT)) ? true : false;
  io.KeySuper = (orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_LSYSTEM) || orxKeyboard_IsKeyPressed(orxKEYBOARD_KEY_RSYSTEM)) ? true : false;
  io.AddInputCharactersUTF8(orxKeyboard_ReadString());

  io.NavInputs[ImGuiNavInput_Activate]    = orxJoystick_IsButtonPressed(orxJOYSTICK_BUTTON_A_1) ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_Cancel]      = orxJoystick_IsButtonPressed(orxJOYSTICK_BUTTON_B_1) ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_Input]       = orxJoystick_IsButtonPressed(orxJOYSTICK_BUTTON_Y_1) ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_Menu]        = orxJoystick_IsButtonPressed(orxJOYSTICK_BUTTON_X_1) ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_DpadLeft]    = orxJoystick_IsButtonPressed(orxJOYSTICK_BUTTON_LEFT_1) ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_DpadRight]   = orxJoystick_IsButtonPressed(orxJOYSTICK_BUTTON_RIGHT_1) ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_DpadUp]      = orxJoystick_IsButtonPressed(orxJOYSTICK_BUTTON_UP_1) ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_DpadDown]    = orxJoystick_IsButtonPressed(orxJOYSTICK_BUTTON_DOWN_1) ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_LStickLeft]  = orxJoystick_GetAxisValue(orxJOYSTICK_AXIS_LX_1) <= -0.3f ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_LStickRight] = orxJoystick_GetAxisValue(orxJOYSTICK_AXIS_LX_1) >= 0.3f ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_LStickUp]    = orxJoystick_GetAxisValue(orxJOYSTICK_AXIS_LY_1) <= -0.3f ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_LStickDown]  = orxJoystick_GetAxisValue(orxJOYSTICK_AXIS_LY_1) >= 0.3f ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_FocusPrev]   = orxJoystick_IsButtonPressed(orxJOYSTICK_BUTTON_LBUMPER_1) ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_FocusNext]   = orxJoystick_IsButtonPressed(orxJOYSTICK_BUTTON_RBUMPER_1) ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_TweakSlow]   = orxJoystick_GetAxisValue(orxJOYSTICK_AXIS_LTRIGGER_1) >= 0.3f ? 1.0f : 0.0f;
  io.NavInputs[ImGuiNavInput_TweakFast]   = orxJoystick_GetAxisValue(orxJOYSTICK_AXIS_RTRIGGER_1) >= 0.3f ? 1.0f : 0.0f;

  ImGui::NewFrame();

  orxKeyboard_Show(io.WantTextInput);
  if(io.WantSetMousePos)
  {
    orxVECTOR vMousePos = {io.MousePos.x, io.MousePos.y};
    orxMouse_SetPosition(&vMousePos);
  }

  //! TODO : Handle capture outputs
}

static void orxImGui_EndFrame()
{
  ImGui::EndFrame();
  ImGui::Render();
  ImDrawData *pstDrawData = ImGui::GetDrawData();

  orxBITMAP * pstScreen = orxDisplay_GetScreenBitmap();
  orxDisplay_SetDestinationBitmaps(&pstScreen, 1);

  orxDISPLAY_MESH stMesh = {};
  stMesh.ePrimitive = orxDISPLAY_PRIMITIVE_TRIANGLES;

  for(int i = 0; i < pstDrawData->CmdListsCount; i++)
  {
    ImDrawList *pstDrawList = pstDrawData->CmdLists[i];
    stMesh.astVertexList = (orxDISPLAY_VERTEX *)pstDrawList->VtxBuffer.Data;
    stMesh.u32VertexNumber = pstDrawList->VtxBuffer.size();
    stMesh.au16IndexList = pstDrawList->IdxBuffer.Data;

    for(int j = 0; j < pstDrawList->CmdBuffer.Size; j++)
    {
      ImDrawCmd &rstCommand = pstDrawList->CmdBuffer[j];

      if(rstCommand.UserCallback)
      {
        rstCommand.UserCallback(pstDrawList, &rstCommand);
      }
      else
      {
        stMesh.u32IndexNumber = rstCommand.ElemCount;
        orxDisplay_SetBitmapClipping(orxNULL, orxF2U(rstCommand.ClipRect.x), orxF2U(rstCommand.ClipRect.y), orxF2U(rstCommand.ClipRect.z), orxF2U(rstCommand.ClipRect.w));
        orxDisplay_DrawMesh(&stMesh, (orxBITMAP *)rstCommand.TextureId, orxDISPLAY_SMOOTHING_ON, orxDISPLAY_BLEND_MODE_ALPHA);
      }
      stMesh.au16IndexList += rstCommand.ElemCount;
    }
  }
}

static orxSTATUS orxFASTCALL orxImGui_NewFrame(const orxEVENT *_pstEvent)
{
  orxImGui_EndFrame();
  orxImGui_BeginFrame();

  return orxSTATUS_SUCCESS;
}

orxSTATUS orxFASTCALL orxImGui_Init()
{
  ImGui::SetAllocatorFunctions(&orxImGui_Allocate, &orxImGui_Free);
  ImGui::CreateContext();
  ImGuiIO& io                     = ImGui::GetIO();
  io.BackendFlags                 = ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasGamepad;
  io.ConfigFlags                  = ImGuiConfigFlags_None;
  io.KeyMap[ImGuiKey_Tab]         = orxKEYBOARD_KEY_TAB;
  io.KeyMap[ImGuiKey_LeftArrow]   = orxKEYBOARD_KEY_LEFT;
  io.KeyMap[ImGuiKey_RightArrow]  = orxKEYBOARD_KEY_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow]     = orxKEYBOARD_KEY_UP;
  io.KeyMap[ImGuiKey_DownArrow]   = orxKEYBOARD_KEY_DOWN;
  io.KeyMap[ImGuiKey_PageUp]      = orxKEYBOARD_KEY_PAGE_UP;
  io.KeyMap[ImGuiKey_PageDown]    = orxKEYBOARD_KEY_PAGE_DOWN;
  io.KeyMap[ImGuiKey_Home]        = orxKEYBOARD_KEY_HOME;
  io.KeyMap[ImGuiKey_End]         = orxKEYBOARD_KEY_END;
  io.KeyMap[ImGuiKey_Insert]      = orxKEYBOARD_KEY_INSERT;
  io.KeyMap[ImGuiKey_Delete]      = orxKEYBOARD_KEY_DELETE;
  io.KeyMap[ImGuiKey_Backspace]   = orxKEYBOARD_KEY_BACKSPACE;
  io.KeyMap[ImGuiKey_Space]       = orxKEYBOARD_KEY_SPACE;
  io.KeyMap[ImGuiKey_Enter]       = orxKEYBOARD_KEY_ENTER;
  io.KeyMap[ImGuiKey_Escape]      = orxKEYBOARD_KEY_ESCAPE;
  io.KeyMap[ImGuiKey_A]           = orxKEYBOARD_KEY_A;
  io.KeyMap[ImGuiKey_C]           = orxKEYBOARD_KEY_C;
  io.KeyMap[ImGuiKey_V]           = orxKEYBOARD_KEY_V;
  io.KeyMap[ImGuiKey_X]           = orxKEYBOARD_KEY_X;
  io.KeyMap[ImGuiKey_Y]           = orxKEYBOARD_KEY_Y;
  io.KeyMap[ImGuiKey_Z]           = orxKEYBOARD_KEY_Z;

  int iWidth, iHeight;
  unsigned char *pcPixels;
  io.Fonts->GetTexDataAsRGBA32(&pcPixels, &iWidth, &iHeight);
  orxBITMAP *pstBitmap = orxDisplay_CreateBitmap(iWidth, iHeight);
  orxDisplay_SetBitmapData(pstBitmap, pcPixels, iWidth * iHeight * 4);
  io.Fonts->TexID = (void *)pstBitmap;

  ImGui::StyleColorsDark();

  orxEvent_AddHandler(orxEVENT_TYPE_RENDER, &orxImGui_NewFrame);
  orxEvent_SetHandlerIDFlags(&orxImGui_NewFrame, orxEVENT_TYPE_RENDER, orxNULL, orxEVENT_GET_FLAG(orxRENDER_EVENT_STOP), orxEVENT_KU32_MASK_ID_ALL);

  orxImGui_BeginFrame();

  return orxSTATUS_SUCCESS;
}

void orxFASTCALL orxImGui_Exit()
{
  orxDisplay_DeleteBitmap((orxBITMAP *)ImGui::GetIO().Fonts->TexID);
  ImGui::DestroyContext();
}
