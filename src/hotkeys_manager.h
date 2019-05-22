#ifndef __HK_WINSPLIT_H__
#define __HK_WINSPLIT_H__

#include "minimize_restore.h"
#include "settingsmanager.h"

#include <wx/frame.h>

#include <vector>
#include <windows.h>

class TrayIcon;

enum HK_ID {
  HK_0 = 100,
  HK_1,
  HK_2,
  HK_3,
  HK_4,
  HK_5,
  HK_6,
  HK_7,
  HK_8,
  HK_9,
  HK_M,
  HK_F,
  HK_C,
  HK_LEFT,
  HK_RIGHT,
  HK_MINIMIZE,
  HK_MAXIMIZE,
  HK_ACTWNDTOOLS,
  HK_ALWAYSONTOP,
  HK_HMAXIMIZE,
  HK_VMAXIMIZE,
  HK_TOGGLEVNUMPAD
};

struct HotkeyStruct {
  unsigned int modifier1;
  unsigned int modifier2;
  unsigned int virtualKey;
  bool active;
  bool session;

  HotkeyStruct() {}

  HotkeyStruct(unsigned int mod1, unsigned int mod2, unsigned int vk, bool act, bool ses)
      : modifier1(mod1)
      , modifier2(mod2)
      , virtualKey(vk)
      , active(act)
      , session(ses)
  {
  }
};

class HotkeysManager : public wxFrame {
private:
  MinimizeRestore m_minimizeRestore;
  SettingsManager& m_options;
  TrayIcon* p_tray;

  std::vector<HotkeyStruct> vec_hotkey;

public:
  HotkeysManager(TrayIcon* tray);

  wxString Start();
  bool Stop();

  bool LoadData();
  bool SaveData();
  void SetDefaultData();
  int GetTaskIndex(wxString name);
  void SetVecHotkey(const std::vector<HotkeyStruct>& vec);
  std::vector<HotkeyStruct> GetCpyVector();

  WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
};

#endif // __HK_WINSPLIT_H__
