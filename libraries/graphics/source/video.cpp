#include "dos/graphics/video.hpp"

namespace DOS {
namespace Video {

    void initialize() {
        _setvideomode(VIDEO_MODE);
        _selectpalette(VIDEO_PALETTE);
    }

} // namespace Video
} // namespace DOS
