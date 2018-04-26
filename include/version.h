#ifndef _QT_TOX_VERSION_H_
#define _QT_TOX_VERSION_H_

#include "tox.h"

namespace QtTox
{

static constexpr uint32_t VersionMajor = TOX_VERSION_MAJOR;
static constexpr uint32_t VersionMinor = TOX_VERSION_MINOR;
static constexpr uint32_t VersionPatch = TOX_VERSION_PATCH;

static constexpr bool isAPICompatiable(uint32_t major, uint32_t minor, uint32_t patch)
{
    return (
            (VersionMajor > 0 && VersionMajor == major) &&
            (VersionMinor > minor ||
             (VersionMinor == minor && VersionPatch >= patch)
            )
           ) || (
            (VersionMajor == 0 && major == 0) &&
            (
             (
              (VersionMinor > 0 && VersionMinor == minor) &&
              (VersionPatch >= patch)
             ) || (
                 (VersionMinor == 0 && minor == 0) &&
                 (VersionPatch == patch)
             )
            )
           );
}

}

#endif // _QT_TOX_VERSION_H_
