#include "dos/graphics/video.hpp"

namespace DOS {
namespace Video {

    void initialize() {
        _setvideomode(MODE);
        _selectpalette(PALETTE);
    }

} // namespace Video
} // namespace DOS
