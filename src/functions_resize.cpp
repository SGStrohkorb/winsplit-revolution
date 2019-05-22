#include <Windows.h>

#include <psapi.h>
// For Vista and newer "extended" border compensation:
#include "Dwmapi-compat.h"

#include "auto_placement.h"
#include "dialog_fusion.h"
#include "functions_special.h"
#include "layout_manager.h"
#include "list_windows.h"
#include "multimonitor_move.h"

//=============================
// Resize window
//=============================
bool ResizeWindow(const int hotkey, bool fromKbd)
{
  HWND hwnd = ListWindows::ListWindow();
  bool flag_resizable = true;
  //((GetWindowLong(hwnd,GWL_STYLE)&WS_SIZEBOX)!=0);

  wxRect res = LayoutManager::GetInstance()->GetNext(hwnd, hotkey - 1);

  WINDOWPLACEMENT placement;
  GetWindowPlacement(hwnd, &placement);

  if (placement.showCmd == SW_SHOWMAXIMIZED) {
    placement.showCmd = SW_RESTORE;
    SetWindowPlacement(hwnd, &placement);
  }

  bool bMoveMouse = fromKbd & SettingsManager::Get().getMouseFollowWindow();
  if (bMoveMouse)
    StoreOrSetMousePosition(true, hwnd);

  int major;
  wxGetOsVersion(&major, NULL);

  // Windows Vista or newer.
  if (major >= 6) {
    RECT window_rect;
    GetWindowRect(hwnd, &window_rect);

    RECT ext_frame;
    HRESULT hr = DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &ext_frame, sizeof(RECT));
    if (SUCCEEDED(hr) && ext_frame.top != res.x || ext_frame.left != res.y ||
        ext_frame.bottom != res.GetBottom() || ext_frame.right != res.GetRight()) {

      LONG left_border = ext_frame.left - window_rect.left;
      LONG top_border = ext_frame.top - window_rect.top;
      LONG right_border = window_rect.right - ext_frame.right;
      LONG bottom_border = window_rect.bottom - ext_frame.bottom;

      res.SetX(res.x - left_border);
      res.SetY(res.y - top_border);
      res.SetWidth(res.width + left_border + right_border);
      res.SetHeight(res.height + top_border + bottom_border);
    }
  }

  SetWindowPos(hwnd,
               HWND_TOP,
               res.x,
               res.y,
               res.width,
               res.height,
               flag_resizable ? SWP_SHOWWINDOW : SWP_NOSIZE);

  if (bMoveMouse)
    StoreOrSetMousePosition(false, hwnd);

  return true;
}

void MoveToScreen(DIRECTION sens)
{
  HWND hwnd = GetForegroundWindow();
  if (!ListWindows::ValidateWindow(hwnd))
    return;

  MoveWindowToDirection(hwnd, sens);
}
