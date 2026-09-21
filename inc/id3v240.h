#ifndef ID3V240_H
# define ID3V240_H

#include "typedef.h"

enum state {
  unset,
  bad,
  good,
};

enum txtenc {
  ascii = 0,
  ucs2 = 1,
  utf16be = 2,
  utf8 = 3,
  invalid_txtenc
};

/* TODO Maybe move to APIC file */
enum imgtype {
  other = 0,
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
