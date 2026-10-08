#ifndef _CMIDIMANAGER
#define _CMIDIMANAGER

#include "Kyoto/Audio/CAudioHandle.hpp"
#include "Kyoto/SObjectTag.hpp"

#include "rstl/auto_ptr.hpp"
#include "rstl/reserved_vector.hpp"

class CInputStream;
class CMidiManager {
public:
  class CMidiWrapper {
  public:
    CMidiWrapper();
    const CAudioHandle& GetManagerHandle() const;
    const uint GetAudioSysHandle() const;
    const bool IsAvailable() const;

    void SetAvailable(const bool v);
    void SetAudioSysHandle(const uint handle);
    const short GetSongId() const;
    void SetMidiHandle(const CAudioHandle& handle);
    void SetSongId(const short id);

  private:
    uint mSysHandle;
    CAudioHandle mMidiHandle;
    short mSongId;
    bool mAvailable;
  };

  class CMidiData {
  public:
    CMidiData(CInputStream& in);

    const short GetSongId() const { return mSongId; }
    const short GetGroupId() const { return mGroupId; }
    CAssetId GetAGSCAssetId() const { return mAgscId; }
    uchar* GetData() const { return mData.get(); }

  private:
    short mSongId;
    short mGroupId;
    CAssetId mAgscId;
    rstl::auto_ptr< uchar > mData;
  };

  static CAudioHandle Play(const CMidiData&, unsigned short fadeTime, bool stopExisting,
                           short volume);
  static void Stop(const CAudioHandle&, unsigned short);
  static void StopAll();

  static CAudioHandle LocateHandle();

  static rstl::reserved_vector< CMidiWrapper, 3 > mMidiWrappers;
};

#endif // _CMIDIMANAGER
