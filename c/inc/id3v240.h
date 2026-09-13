#ifndef ID3V240_H
# define ID3V240_H

#include "typedef.h"

enum state {
  unset,
  bad,
  good,
};

enum txtenc {
  ascii,
  ucs2,
  utf16be,
  utf8,
  invalid_txtenc
};

enum imgtype {
  other,
  icon32x32,
  icon_other,
  cover_front,
  cover_back,
  leaflet,
  media,
  lead_artist,
  artist,
  conductor,
  band,
  composer,
  lyricist,
  location,
  during_recording,
  during_performance,
  video_capture,
  fish,
  illustration,
  band_logo,
  studio_logo
};

#endif
